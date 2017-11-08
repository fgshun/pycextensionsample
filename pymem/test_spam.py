import random
import pytest
import spam

SEQUENCE_LENGTH = 100


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


@pytest.fixture(scope='module')
def sequence():
    random.seed(20171105)
    return tuple(random.randrange(-1000, 1000) for i in range(SEQUENCE_LENGTH))


def test_insert_search2(sequence):
    b = spam.BinaryTree()
    for v in sequence:
        b.insert(v)
    assert list(b) == sorted(sequence)
    for v in sequence:
        assert b.search(v)
    assert 0 not in sequence
    assert not b.search(0)


@pytest.mark.parametrize('i', range(SEQUENCE_LENGTH))
def test_delete2(sequence, i):
    random.seed(20171105)
    a = spam.BinaryTree()
    for v in sequence:
        a.insert(v)

    a.delete(sequence[i])
    seq = list(sequence)
    seq.pop(i)
    assert list(a) == sorted(seq)
