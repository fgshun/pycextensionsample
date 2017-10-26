import spam


def test_parse_int():
    assert spam.parse_int(0) == 0


def test_parse_str():
    s = "spam"
    assert spam.parse_str(s) == len(s)


def test_parse_byte():
    b = b"spam"
    assert spam.parse_byte(b) == len(b)


def test_parse_keywords():
    _spam = object()
    _ham = object()

    a, b = spam.parse_keywords(a=_spam, b=_ham)
    assert _spam is a
    assert _ham is b
