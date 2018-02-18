import psyche as ps


def test_integral_literal():
    for n in range(-5, 6):
        expr = ps.Expr(n)
        ps.testing.assert_same_as_np(expr, err_msg=f'n={n}')


def test_non_integral():
    for n in range(-5, 6):
        expr = ps.Expr(n + 0.5)
        ps.testing.assert_same_as_np(expr, err_msg=f'n={n}')


def test_add():
    for n in -1, 0, 1:
        expr = ps.Expr(n) + 5.5
        ps.testing.assert_same_as_np(expr, err_msg=f'n={n}')


def test_sub():
    for n in -1, 0, 1:
        expr = ps.Expr(n) - 5.5
        ps.testing.assert_same_as_np(expr, err_msg=f'n={n}')


def test_mul():
    for n in -1, 0, 1:
        expr = ps.Expr(n) * 5.5
        ps.testing.assert_same_as_np(expr, err_msg=f'n={n}')


def test_div():
    for n in -1, 0, 1:
        expr = ps.Expr(n) / 5.5
        ps.testing.assert_same_as_np(
            expr,
            decimal=4,  # div is pretty lossy at 16b radix
            err_msg=f'n={n}',
        )


def test_exp():
    for n in -1.5, -1, 0, 1, 1.5:
        expr = ps.Expr(n).exp()
        ps.testing.assert_same_as_np(
            expr,
            decimal=4,  # exp is pretty lossy at 16b radix
            err_msg=f'n={n}'
        )


def test_neg():
    for n in -1, 0, 1:
        expr = -ps.Expr(n)
        ps.testing.assert_same_as_np(expr, err_msg=f'n={n}')


def test_dot():
    for n in -1, 0, 1:
        expr = ps.Expr(n).dot(5.5)
        ps.testing.assert_same_as_np(expr, err_msg=f'n={n}')
