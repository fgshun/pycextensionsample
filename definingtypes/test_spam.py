import pytest
import spam


def test_spam():
    a = spam.Spam(3)
    assert a.spam('spam') == 'spamspamspam'

    a.multiplier = 5
    assert a.spam('spam') == 'spamspamspamspamspam'

    a.multiplier = 'spam'
    with pytest.raises(TypeError):
        a.spam('spam')


def test_del():
    a = spam.Spam(3)
    del a.multiplier
    with pytest.raises(AttributeError):
        a.spam('spam')

