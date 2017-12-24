import pytest
import spam

def test_add():
    a = spam.Spam(1)
    b = spam.Spam(2)

    c = a + b
    assert c.v == 3

    with pytest.raises(TypeError):
        a + "spam"

def test_undefined_protocol():
    a = spam.Spam(1)
    b = spam.Spam(2)

    with pytest.raises(TypeError):
        c = a - b
