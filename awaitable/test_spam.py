import pytest

import spam


def test_ham():
    seq = ('spam', 'ham', 'eggs')
    value = 'spamspamspam'
    iter_ = iter(seq)
    ins = spam.Ham(iter_, value)
    assert ins.iter is iter_
    assert value is ins.value

    assert next(ins) == seq[0]
    assert next(ins) == seq[1]
    assert next(ins) == seq[2]
    with pytest.raises(StopIteration) as exc_info:
        next(ins)
    assert exc_info.value.value == value


async def spam_await(awaitable):
    return await awaitable


def test_spam():
    seq = ('spam', 'ham', 'eggs')
    value = 'spamspamspam'
    ins = spam.Spam(seq, value)
    assert seq is ins.seq
    assert value is ins.value

    co = spam_await(ins)
    assert co.send(None) == seq[0]
    assert co.send(None) == seq[1]
    assert co.send(None) == seq[2]
    with pytest.raises(StopIteration) as exc_info:
        co.send(None)
    assert exc_info.value.value == value


def test_spam_send_value():
    seq = ('spam', 'ham', 'eggs')
    value = 'spamspamspam'
    ins = spam.Spam(seq, value)

    co = spam_await(ins)
    assert co.send(None) == seq[0]
    assert co.send(2) == seq[1] * 2


def test_spam_throw():
    seq = ('spam', 'ham', 'eggs')
    value = 'spamspamspam'
    ins = spam.Spam(seq, value)
    co = spam_await(ins)
    assert co.send(None) == seq[0]
    assert co.throw(ValueError) == seq[1]
    with pytest.raises(Exception) as exc_info:
        co.throw(Exception, 'err')
    assert exc_info.value.args[0] == 'err'
