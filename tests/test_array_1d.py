import psyche as ps


def test_array_literal_integral():
    expr = ps.Expr(list(range(-5, 6)))
    ps.testing.assert_same_as_np(expr)


def test_array_literal_non_integral():
    expr = ps.Expr([n + 0.5 for n in range(-5, 6)])
    ps.testing.assert_same_as_np(expr)


def test_add_broadcast():
    expr = ps.Expr([n + 0.5 for n in range(-5, 6)]) + 1.2
    ps.testing.assert_same_as_np(expr, decimal=5)


def test_add_elementwise():
    a = ps.Expr([n + 0.5 for n in range(-5, 6)])
    expr = a + a
    ps.testing.assert_same_as_np(expr, decimal=5)


def test_sub_broadcast():
    expr = ps.Expr([n + 0.5 for n in range(-5, 6)]) - 1.2
    ps.testing.assert_same_as_np(expr, decimal=5)


def test_sub_elementwise():
    a = ps.Expr([n + 0.5 for n in range(-5, 6)])
    b = [n + 0.3 for n in reversed(range(-5, 6))]
    expr = a - b
    ps.testing.assert_same_as_np(expr, decimal=5)


def test_mul_broadcast():
    expr = ps.Expr([n + 0.5 for n in range(-5, 6)]) * 1.2
    ps.testing.assert_same_as_np(expr, decimal=4)


def test_mul_elementwise():
    a = ps.Expr([n + 0.5 for n in range(-5, 6)])
    b = [n + 0.3 for n in reversed(range(-5, 6))]
    expr = a * b
    ps.testing.assert_same_as_np(expr, decimal=4)


def test_div_broadcast():
    expr = ps.Expr([n + 0.5 for n in range(-5, 6)]) / 1.2
    ps.testing.assert_same_as_np(expr, decimal=4)


def test_div_elementwise():
    a = ps.Expr([n + 0.5 for n in range(-5, 6)])
    b = [n + 0.3 for n in reversed(range(-5, 6))]
    expr = a / b
    ps.testing.assert_same_as_np(expr, decimal=4)


def test_exp():
    expr = ps.Expr([-1.5, -1, 0, 1, 1.5]).exp()
    ps.testing.assert_same_as_np(expr, decimal=4)


def test_neg():
    expr = -ps.Expr([-1.5, -1, 0, 1, 1.5])
    ps.testing.assert_same_as_np(expr, decimal=4)


def test_dot_broadcast():
    expr = ps.Expr([n + 0.5 for n in range(-5, 6)]).dot(1.2)
    ps.testing.assert_same_as_np(expr, decimal=4)


def test_dot():
    a = ps.Expr([n + 0.5 for n in range(-5, 6)])
    b = [n + 0.3 for n in reversed(range(-5, 6))]
    expr = a.dot(b)
    ps.testing.assert_same_as_np(expr, decimal=4)
