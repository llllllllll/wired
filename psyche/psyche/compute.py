from functools import partial
import math
import os
import subprocess
import tempfile

import numpy as np

from .compiler import compile as ps_compile


def evaluate_numpy(expr):
    """Evaluate an expression using the numpy backend.

    Parameters
    ----------
    expr : ps.Expr
        The expression to evaluate.

    Returns
    -------
    result : array-like
        The result of the expression.
    """
    ns = {
        'array': np.array,
        'dot': np.dot,
        'exp': np.exp,
        'ndim': np.ndim,
        'np': np,
        'product': partial(np.product, axis=0),
        'shape': np.shape,
        'sum': partial(np.sum, axis=0),
    }
    exec(ps_compile('numpy', expr), ns, ns)
    return ns['result']


class CompilationFailure(Exception):
    def __init__(self, source, gcc_stderr):
        self._source = source
        self._gcc_stderr = gcc_stderr

    def __str__(self):
        decoded = self._gcc_stderr.decode('ascii', 'ignore')
        source_lines = self._source.splitlines()
        ndigits = int(math.log(len(source_lines), 10)) + 1
        source_with_lineno = '\n'.join(
            f'{n:-{ndigits}}: {line}' for n, line in enumerate(source_lines, 1)
        )
        return f'\n{source_with_lineno}\n\ncompiler message:\n{decoded}'


def evaluate_wired(expr, include_dir=None):
    """Evaluate an expression using the wired backend.

    Parameters
    ----------
    expr : ps.Expr
        The expression to evaluate.
    include_dir : path-like, optional
        Extra include directory.

    Returns
    -------
    result : array-like
        The result of the expression.
    """
    args = ['g++', '-std=gnu++17', '-x', 'c++', '-fno-diagnostics-color']
    if include_dir is not None:
        args.append('-I' + os.fspath(include_dir))
    args.extend(['-o', '/dev/stdout', '-'])

    source = ps_compile('wired', expr, full_executable=True)
    with tempfile.NamedTemporaryFile(mode='w+b', delete=False) as f:
        p = subprocess.run(
            args,
            pass_fds=(f.fileno(),),
            input=source.encode('ascii'),
            stdout=f,
            stderr=subprocess.PIPE,
        )
        if p.returncode:
            raise CompilationFailure(source, p.stderr)

        # close the file to chmod and run it
        f.close()

        os.chmod(f.name, 0o700)
        p = subprocess.run(
            [f.name],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )

    p.check_returncode()

    stdout = p.stdout.decode('ascii')
    out = np.array(eval(stdout), dtype='float64')
    if out.ndim == 0:
        out = out.item()
    return out


def evaluate(backend, expr, **kwargs):
    """Evaluate an expression.

    Parameters
    ----------
    backend : {'wired', 'numpy'}
        The backend to use when evaluating ``expr``.
    expr : ps.Expr
        The expression to evaluate.
    **kwargs
        Forwarded to the underlying evaluation function.

    Returns
    -------
    result : array-like
        The result of the expression.
    """
    if backend == 'numpy':
        return evaluate_numpy(expr, **kwargs)
    elif backend == 'wired':
        return evaluate_wired(expr, **kwargs)

    raise ValueError(f'unknown backend {backend}')
