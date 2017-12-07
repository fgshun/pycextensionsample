import pytest
import spam


def test_parse_int():
    assert spam.parse_int(0) == 0


def test_parse_str():
    s = "spam"
    assert spam.parse_str(s) == len(s)


def test_parse_byte():
    b = b"spam"
    assert spam.parse_byte(b) == len(b)


def test_optional_args():
    assert spam.optional_args(1) == (1, 0)
    assert spam.optional_args(2, 3) == (2, 3)


def test_parse_keywords():
    _spam = object()
    _ham = object()

    a, b = spam.parse_keywords(a=_spam, b=_ham)
    assert _spam is a
    assert _ham is b


def test_use_converter():
    assert spam.use_converter(b'spam') == b'spam'
    assert spam.use_converter(bytearray(b'spam')) == b'spam'
    with pytest.raises(TypeError):
        spam.use_converter('spam')
