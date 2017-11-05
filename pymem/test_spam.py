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
