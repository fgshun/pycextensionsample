import io

import myzstd


def test_simple():
    b = b'spam' * 100000
    c = myzstd.compress(b)
    assert len(b) > len(c)

    d = myzstd.decompress(c)
    assert b == d

def test_compression_level():
    b = b'spam' * 100000
    c = myzstd.compress(b, 10)
    assert len(b) > len(c)

    d = myzstd.decompress(c)
    assert b == d

def test_stream():
    src = io.BytesIO(b'spam' * 100000)
    compressed = io.BytesIO()
    myzstd.compress_stream(src, compressed)
    assert len(src.getvalue()) > len(compressed.getvalue()) > 0

    decompressed = io.BytesIO()
    compressed.seek(0)
    myzstd.decompress_stream(compressed, decompressed)
    assert src.getvalue() == decompressed.getvalue()
