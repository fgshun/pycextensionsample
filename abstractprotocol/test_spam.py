import os
import pytest
import spam


def test_getattr():
    assert spam.getattr(spam, 'getattr') is spam.getattr
    with pytest.raises(AttributeError):
        spam.getattr(spam, 'ham')


def test_callmethod():
    assert spam.callmethod() == os.getcwd()


def test_getitem():
    t = ('spam', 'ham', 'eggs')
    assert spam.getitem(t, 0) == 'spam'
    assert spam.getitem(t, 1) == 'ham'
    assert spam.getitem(t, 2) == 'eggs'
    with pytest.raises(IndexError):
        spam.getitem(t, 3)

    d = {'spam': 'ham'}
    assert spam.getitem(d, 'spam') == 'ham'
    with pytest.raises(KeyError):
        spam.getitem(d, 'eggs')


def test_getitem_from_seq():
    t = ('spam', 'ham', 'eggs')
    assert spam.getitem_from_seq(t, 0) == 'spam'
    assert spam.getitem_from_seq(t, 1) == 'ham'
    assert spam.getitem_from_seq(t, 2) == 'eggs'
    with pytest.raises(IndexError):
        spam.getitem_from_seq(t, 3)

    d = {'spam': 'ham'}
    with pytest.raises(TypeError):
        spam.getitem_from_seq(d, 'spam')
    with pytest.raises(TypeError):
        spam.getitem_from_seq(d, 'eggs')


def test_useiter():
    s = 'spam'
    h = hash('s') ^ hash('p') ^ hash('a') ^ hash('m')
    assert spam.useiter(s) == h
