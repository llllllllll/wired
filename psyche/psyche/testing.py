import os

import numpy as np

import psyche as ps


def assert_same_as_np(expr, **kwargs):
    """Evaluate an expression with numpy and wired and assert that they produce
    the same result.

    Parameters
    ----------
    expr : ps.Expr
        The expression to evaluate.
    **kwargs
        Forwarded to :func:`~numpy.testing.assert_almost_equal`.

    Raises
    ------
    AssertionError
        Raised then ``expr`` produces different results with numpy and wired.
    """
    expected = ps.evaluate('numpy', expr)
    actual = ps.evaluate(
        'wired',
        expr,
        include_dir=os.environ.get('WIRED_INCLUDE_PATH'),
    )

    np.testing.assert_almost_equal(
        actual,
        expected,
        **kwargs,
    )
