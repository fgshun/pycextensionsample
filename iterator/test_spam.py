import spam


def test_iterator_cls():
    iterable = spam.Spam(b'spam')
    iterator =  iter(iterable)
    assert iterator
    assert list(iterator) == list(b'spam')
