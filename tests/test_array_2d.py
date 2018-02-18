import psyche as ps


def test_array_literal_integral():
    expr = ps.Expr([list(range(-5, 6)) for _ in range(-5, 6)])
    ps.testing.assert_same_as_np(expr)


def test_array_literal_non_integral():
    expr = ps.Expr([[n + 0.5 for n in range(-5, 6)] for _ in range(-5, 6)])
    ps.testing.assert_same_as_np(expr)


def test_add_broadcast_scalar():
    a = ps.Expr([[n + 0.5 for n in range(-5, 6)] for _ in range(-5, 6)])
    expr = a + 1.2
    ps.testing.assert_same_as_np(expr, decimal=5)


def test_add_broadcast_row():
    a = ps.Expr([[n + 0.5 for n in range(-5, 6)] for _ in range(-5, 6)])
    row = ps.Expr([n + 0.5 for n in range(-5, 6)])
    expr = a + row
    ps.testing.assert_same_as_np(expr, decimal=5)


def test_add_broadcast_column():
    a = ps.Expr([[n + 0.5 for n in range(-5, 6)] for _ in range(-5, 6)])
    column = ps.Expr([[n + 0.5] for n in range(-5, 6)])
    expr = a + column
    ps.testing.assert_same_as_np(expr, decimal=5)


def test_add_elementwise():
    a = ps.Expr([[n + 0.5 for n in range(-5, 6)] for _ in range(-5, 6)])
    expr = a + a
    ps.testing.assert_same_as_np(expr, decimal=5)


def test_sub_broadcast_scalar():
    a = ps.Expr([[n + 0.5 for n in range(-5, 6)] for _ in range(-5, 6)])
    expr = a - 1.2
    ps.testing.assert_same_as_np(expr, decimal=5)


def test_sub_broadcast_row():
    a = ps.Expr([[n + 0.5 for n in range(-5, 6)] for _ in range(-5, 6)])
    row = ps.Expr([n + 0.3 for n in range(-5, 6)])
    expr = a - row
    ps.testing.assert_same_as_np(expr, decimal=5)


def test_sub_broadcast_column():
    a = ps.Expr([[n + 0.5 for n in range(-5, 6)] for _ in range(-5, 6)])
    column = ps.Expr([[n + 0.3] for n in range(-5, 6)])
    expr = a - column
    ps.testing.assert_same_as_np(expr, decimal=5)


def test_sub_elementwise():
    a = ps.Expr([[n + 0.5 for n in range(-5, 6)] for _ in range(-5, 6)])
    b = ps.Expr([[n + 0.3 for n in range(-5, 6)] for _ in range(-5, 6)])
    expr = a - b
    ps.testing.assert_same_as_np(expr, decimal=5)


def test_mul_broadcast_scalar():
    a = ps.Expr([[n + 0.5 for n in range(-5, 6)] for _ in range(-5, 6)])
    expr = a * 1.2
    ps.testing.assert_same_as_np(expr, decimal=4)


def test_mul_broadcast_row():
    a = ps.Expr([[n + 0.5 for n in range(-5, 6)] for _ in range(-5, 6)])
    row = ps.Expr([n + 0.3 for n in range(-5, 6)])
    expr = a * row
    ps.testing.assert_same_as_np(expr, decimal=4)


def test_mul_broadcast_column():
    a = ps.Expr([[n + 0.5 for n in range(-5, 6)] for _ in range(-5, 6)])
    column = ps.Expr([[n + 0.3] for n in range(-5, 6)])
    expr = a * column
    ps.testing.assert_same_as_np(expr, decimal=4)


def test_mul_elementwise():
    a = ps.Expr([[n + 0.5 for n in range(-5, 6)] for _ in range(-5, 6)])
    b = ps.Expr([[n + 0.3 for n in range(-5, 6)] for _ in range(-5, 6)])
    expr = a * b
    ps.testing.assert_same_as_np(expr, decimal=4)


def test_div_broadcast_scalar():
    a = ps.Expr([[n + 0.5 for n in range(-5, 6)] for _ in range(-5, 6)])
    expr = a / 1.2
    ps.testing.assert_same_as_np(expr, decimal=3)


def test_div_broadcast_row():
    a = ps.Expr([[n + 0.5 for n in range(-5, 6)] for _ in range(-5, 6)])
    row = ps.Expr([n + 0.3 for n in range(-5, 6)])
    expr = a / row
    ps.testing.assert_same_as_np(expr, decimal=3)


def test_div_broadcast_column():
    a = ps.Expr([[n + 0.5 for n in range(-5, 6)] for _ in range(-5, 6)])
    column = ps.Expr([[n + 0.3] for n in range(-5, 6)])
    expr = a / column
    ps.testing.assert_same_as_np(expr, decimal=3)


def test_div_elementwise():
    a = ps.Expr([[n + 0.5 for n in range(-5, 6)] for _ in range(-5, 6)])
    b = ps.Expr([[n + 0.3 for n in range(-5, 6)] for _ in range(-5, 6)])
    expr = a / b
    ps.testing.assert_same_as_np(expr, decimal=3)


def test_exp():
    a = ps.Expr([[n + 0.5 for n in range(-5, 6)] for _ in range(-5, 6)])
    expr = a.exp()
    ps.testing.assert_same_as_np(expr, decimal=2)


def test_neg():
    a = ps.Expr([[n + 0.5 for n in range(-5, 6)] for _ in range(-5, 6)])
    expr = -a
    ps.testing.assert_same_as_np(expr)


def test_dot_square():
    a = ps.Expr([[n + 0.5 for n in range(-5, 6)] for _ in range(-5, 6)])
    b = ps.Expr([[n + 0.3 for n in range(-5, 6)] for _ in range(-5, 6)])
    expr = a.dot(b)
    ps.testing.assert_same_as_np(expr, decimal=4)


def test_dot_not_square():
    a = ps.Expr([[n + 0.5 for n in range(-5, 3)] for _ in range(-5, 6)])
    b = ps.Expr([[n + 0.3 for n in range(-5, 4)] for _ in range(-5, 3)])
    expr = a.dot(b)
    ps.testing.assert_same_as_np(expr, decimal=4)


def test_T_square():
    a = ps.Expr([[n + 0.5 for n in range(-5, 6)] for _ in range(-5, 6)])
    expr = a.T
    ps.testing.assert_same_as_np(expr, decimal=4)


def test_T_not_square():
    a = ps.Expr([[n + 0.5 for n in range(-5, 3)] for _ in range(-5, 6)])
    expr = a.T
    ps.testing.assert_same_as_np(expr, decimal=4)
