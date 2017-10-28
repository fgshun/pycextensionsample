import array
import spam
import pytest


def test_use_readonly_buffer():
    a = b'abc'
    assert spam.use_readonly_buffer(a) == sum(a)

    b = bytearray(b'abc')
    assert spam.use_readonly_buffer(b) == sum(b)

    c = array.array('B', [0, 1, 2])
    assert spam.use_readonly_buffer(c) == sum(c)

    d = array.array('L', [2017, 10, 29])
    assert spam.use_readonly_buffer(d) != sum(d)
    assert spam.use_readonly_buffer(d) == sum(d.tobytes())

    with pytest.raises(TypeError):
        spam.use_readonly_buffer(None)


def test_use_writable_buffer():
    a = bytearray(b'abc')
    spam.use_writable_buffer(a)
    assert a == b'bcd'

    with pytest.raises(TypeError):
        spam.use_writable_buffer(None)

    with pytest.raises(TypeError):
        spam.use_writable_buffer(b'def')


def test_bufferproc():
    b = b'a'
    s = spam.Spam(b)

    assert s.export_count == 0
    v = memoryview(s)
    assert s.export_count == 1

    assert v.tobytes() == b
    assert s.b == v[0] == b[0]

    assert spam.use_readonly_buffer(s) == b[0]

    s.b += 1
    assert s.b == v[0] == b[0] + 1


    v[0] += 1
    assert s.b == v[0] == b[0] + 2


    spam.use_writable_buffer(s)
    assert s.b == v[0] == b[0] + 3
