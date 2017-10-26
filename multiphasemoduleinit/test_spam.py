import spam

def test():
    assert issubclass(spam.error, Exception)
    assert spam.zero == 0
    assert spam.one == 1
    assert spam.spam == "spamspamspam"
