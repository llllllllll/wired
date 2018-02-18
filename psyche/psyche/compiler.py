from collections import OrderedDict
from operator import attrgetter
from pathlib import Path


def _format_numpy(statements):
    try:
        template = _format_numpy._template
    except AttributeError:
        with open(Path(__file__).parent / '_numpy_template.inc') as f:
            _format_numpy._template = template = f.read()

    return template.replace(
        '# PSYCHE-GENERATED-CODE',
        ('\n' + ' ' * 4).join(statements),
    )


def _format_wired(statements):
    try:
        template = _format_wired._template
    except AttributeError:
        with open(Path(__file__).parent / '_wired_template.inc') as f:
            _format_wired._template = template = f.read()

    return template.replace(
        '// PSYCHE-GENERATED-CODE',
        '\n'.join(statements),
    )


def make_name(instance, counters):
    type_ = type(instance)
    count = counters.setdefault(type_, 0)
    counters[type_] += 1

    return f'{type_.__name__.lower()}_{count}'


def compile(backend, expr, *, fbits=16, full_executable=False):
    """Compile an expression for the given backend.

    Parameters
    ----------
    backend : {'wired', 'numpy'}
        The backend to compile for.
    expr : ps.Expr
        The expression to compile.
    fbits : int, optional
        The fbits to pass to ``wired::fixed``.
    full_executable : bool, optional
        Generate code to produce a standalone executable

    Returns
    -------
    backend_source_code : str
        The source code to feed to the given backend.
    """
    counters = {}
    namespace = OrderedDict(reversed([
        (node, make_name(node, counters))
        for node in expr._tree.subnodes()
    ]))

    # rename the last value to 'result'
    node, _ = namespace.popitem(last=True)
    namespace[node] = 'result'

    statement = attrgetter(f'{backend}_statement')

    if backend == 'numpy':
        format_statements = _format_numpy
    elif backend == 'wired':
        format_statements = _format_wired
    else:
        raise ValueError(f'unknown backend {backend}')

    if not full_executable:
        format_statements = '\n'.join

    return format_statements(
        statement(node)(
            fbits,
            target,
            *(namespace[child] for child in node.children),
        )
        for node, target in namespace.items()
    )
