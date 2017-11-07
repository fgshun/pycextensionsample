import random
import pytest
import spam


def test_insert_search():
    a = spam.BinaryTree()
    assert not a.search(0)

    a.insert(0)
    a.insert(-1)
    a.insert(1)

    assert list(a) == [-1, 0, 1]
    assert a.search(-1)
    assert a.search(0)
    assert a.search(1)
    assert not a.search(2)


    random.seed(20171105)
    b = spam.BinaryTree()
    seq = [random.randrange(-1000, 1000) for i in range(100)]
    for v in seq:
        b.insert(v)
    assert list(b) == sorted(seq)
    for v in seq:
        assert b.search(v)
    assert 0 not in seq
    assert not b.search(0)


def test_delete():
    a = spam.BinaryTree()
    a.insert(0)
    a.insert(6)
    a.insert(3)
    a.insert(7)
    a.insert(2)
    a.insert(4)
    a.insert(5)

    assert list(a) == [0, 2, 3, 4, 5, 6, 7]

    a.delete(6)
    assert list(a) == [0, 2, 3, 4, 5, 7]
