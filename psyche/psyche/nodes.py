import abc

import numpy as np

from .utils import flat_repr


class _cxx_code:
    def __init__(self, code):
        self._code = code

    def __getattr__(self, attr):
        return type(self)(f'{self._code}::{attr}')

    def __call__(self, *args):
        return type(self)(f'{self._code}<{", ".join(map(str, args))}>')

    def store_type(self, name):
        return f'using {name} = {self._code};'

    def store_value(self, name, fbits):
        return f'using {name} = wired::from_integral<{self._code}, {fbits}>;'

    def __str__(self):
        return self._code


_wired = _cxx_code('wired')


class Node(abc.ABC):
    def __eq__(self, other):
        if not (isinstance(other, type(self)) or
                isinstance(self, type(other))):
            return NotImplemented

        return self.children == other.children

    def __hash__(self):
        return hash((type(self), self.children))

    def subnodes(self):
        yield self
        for child in self.children:
            yield from child.subnodes()

    @property
    @abc.abstractmethod
    def children(self):
        raise NotImplementedError('children')

    @abc.abstractmethod
    def numpy_statement(self, fbits, target, *children):
        raise NotImplementedError('numpy_statement')

    @abc.abstractmethod
    def wired_statement(self, fbits, target, *children):
        raise NotImplementedError('wired_statement')

    def __repr__(self):
        return f'{type(self).__name__}({", ".join(map(repr, self.children))})'


class Literal(Node):
    children = ()

    def __init__(self, value):
        self._value = value = np.array(value, dtype='float64')
        value.flags.writeable = False

        self._hash = None

    def __hash__(self):
        hash_ = self._hash
        if hash_ is not None:
            return hash_

        self._hash = hash_ = hash(self._value.tobytes())
        return hash_

    def __eq__(self, other):
        if not (isinstance(other, type(self)) or
                isinstance(self, type(other))):
            return NotImplemented

        return self._value == other._value

    def numpy_statement(self, fbits, target):
        value = self._value
        if value.ndim == 0:
            value = value.item()

        return f'{target} = {flat_repr(value)}'

    def _convert_scalar(self, value, fbits):
        int_value = int(value)
        if value == int_value:
            return _wired.from_integral(int_value, fbits)

        return _wired.fixed(int(value * (1 << fbits)), fbits)

    def _convert_array(self, value, fbits):
        return _wired.array(*(self._convert(v, fbits) for v in value))

    def _convert(self, value, fbits):
        if value.ndim == 0:
            return self._convert_scalar(value, fbits)

        return self._convert_array(value, fbits)

    def wired_statement(self, fbits, target):
        return self._convert(self._value, fbits).store_type(target)

    def __repr__(self):
        value = self._value
        if value.ndim == 0:
            value = self._value.item()

        return f'{type(self).__name__}({flat_repr(value)})'


class Op(Node):
    @property
    @abc.abstractmethod
    def name(self):
        raise NotImplementedError('name')

    @property
    @abc.abstractmethod
    def op(self):
        raise NotImplementedError('op')

    @classmethod
    def define(cls, name, op=None):
        if op is None:
            op = name

        return type(name.title(), (cls,), {'name': name, 'op': op})


class UnOp(Op):
    def __init__(self, operand):
        self._operand = operand

    @property
    def children(self):
        return self._operand,

    def numpy_statement(self, fbits, target, operand):
        if self.op.isidentifier():
            rhs = f'{self.op}({operand})'
        else:
            rhs = f'{self.op}{operand}'

        return f'{target} = {rhs}'

    def wired_statement(self, fbits, target, operand):
        return getattr(_wired, self.name)(operand).store_type(target)


Neg = UnOp.define('neg', '-')
Exp = UnOp.define('exp')
Sum = UnOp.define('sum')
Prod = UnOp.define('product')
Shape = UnOp.define('shape')


class NDim(UnOp):
    name = op = 'ndim'

    def wired_statement(self, fbits, target, operand):
        return _wired.ndim(operand).store_value(target, fbits)


class T(UnOp):
    name = op = 'T'

    def numpy_statement(self, fbits, target, operand):
        return f'{target} = {operand}.T'


class BinOp(Op):
    def __init__(self, lhs, rhs):
        self._lhs = lhs
        self._rhs = rhs

    @property
    def children(self):
        return self._lhs, self._rhs

    def numpy_statement(self, fbits, target, lhs, rhs):
        if self.op.isidentifier():
            value = f'{self.op}({lhs}, {rhs})'
        else:
            value = f'{lhs} {self.op} {rhs}'

        return f'{target} = {value}'

    def wired_statement(self, fbits, target, lhs, rhs):
        return getattr(_wired, self.name)(lhs, rhs).store_type(target)


Add = BinOp.define('add', '+')
Sub = BinOp.define('sub', '-')
Mul = BinOp.define('mul', '*')
Div = BinOp.define('div', '/')
Dot = BinOp.define('dot')
