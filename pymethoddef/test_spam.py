import spam


def test_noargs():
    assert spam.noargs() is None


def test_varargs():
    args = (object(),)
    assert spam.varargs(*args) is args


def test_keywords():
    args = (object(),)
    kwargs = {'spam': object()}
    a, k = spam.keywords(*args, **kwargs)
    assert args is a
    assert kwargs is k


def test_o():
    a = object()
    assert spam.o(a) is a


