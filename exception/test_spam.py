import pytest
import spam


def test_raise_spamerror():
    with pytest.raises(spam.error):
        spam.raise_spamerror()


def test_div():
    assert spam.div(6, 3) == 2

    # TypeError
    assert spam.div("ham", "eggs") == "spam"

    with pytest.raises(ZeroDivisionError):
        spam.div(1, 0)


def test_getitem():
    assert spam.getitem("abc", 0) == "a"

    # IndexError
    assert spam.getitem("abc", 3) == "spam"

    # TypeError
    with pytest.raises(spam.error) as excinfo:
        spam.getitem(None, "spam")
    assert isinstance(excinfo.value.__context__, TypeError)
    assert isinstance(excinfo.value.__cause__, TypeError)

    # KeyError
    with pytest.raises(spam.error) as excinfo:
        spam.getitem({"a": "b"}, "c")
    assert isinstance(excinfo.value.__context__, KeyError)
    assert not excinfo.value.__cause__

