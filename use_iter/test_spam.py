import pytest

import spam

def test_use_iter():
    L = list('spam')
    assert spam.use_iterable(L) is None
    assert spam.use_iterable(iter(L)) is None
    with pytest.raises(TypeError):
        spam.use_iterable(None)

    with pytest.raises(TypeError):
        spam.use_iterator(L)
    assert spam.use_iterator(iter(L)) is None
    with pytest.raises(TypeError):
        spam.use_iterator(None)
