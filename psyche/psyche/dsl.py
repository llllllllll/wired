from functools import wraps

from . import nodes


def _wrap_arg(ob):
    if isinstance(ob, Expr):
        ob = ob._tree
    elif not isinstance(ob, nodes.Node):
        ob = nodes.Literal(ob)

    return ob


def wrap_args(f):
    """Decorator which wraps all of the arguments as Nodes.
    """
    @wraps(f)
    def wrapper(self, *args):
        return f(self, *map(_wrap_arg, args))

    return wrapper


class Expr:
    def __init__(self, tree):
        self._tree = _wrap_arg(tree)

    def __repr__(self):
        return f'{type(self).__name__}({self._tree!r})'

    def _op(name, *, dunder=False, node_type=None, reflect=False):
        if node_type is None:
            node_type = getattr(nodes, name.title())

        @wrap_args
        def binop(self, *args):
            return type(self)(node_type(self._tree, *args))

        if dunder:
            binop.__name__ = f'__{name}__'
        else:
            binop.__name__ = name

        if not reflect:
            return binop

        @wrap_args
        def rbinop(self, other):
            return type(self)(node_type(other, self._tree))

        if dunder:
            rbinop.__name__ = f'__r{name}__'

        return binop, rbinop

    for op in 'sum', 'prod', 'dot', 'exp':
        _f = _op(op)
        locals()[_f.__name__] = _f
        del _f
        del op

    ndim = property(_op('ndim', node_type=nodes.NDim))

    for op in 'T', 'shape':
        _f = _op(op)
        locals()[_f.__name__] = property(_f)
        del _f
        del op

    for op in ('neg',):  # might add more ops later
        _f = _op(op, dunder=True)
        locals()[_f.__name__] = _f
        del _f
        del op

    for op in 'add', 'sub', 'mul':
        _f, _r = _op(op, dunder=True, reflect=True)
        locals()[_f.__name__] = _f
        locals()[_r.__name__] = _r
        del _f
        del _r
        del op

    __truediv__ = _op('div')
