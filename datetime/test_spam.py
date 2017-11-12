import datetime
import pytest
import spam


def test_date20170930():
    assert spam.date20170930() == datetime.date(2017, 9, 30)


def test_extract_fields():
    dt = datetime.datetime(2017, 11, 12, 13, 4, 5, 6)
    d = dt.date()
    t = dt.time()

    assert spam.extract_fields_fromdate(dt) == 20171112
    assert spam.extract_fields_fromdate(d) == 20171112
    with pytest.raises(TypeError):
        spam.extract_fields_fromdate(t)

    assert spam.extract_fields_fromtime(dt) == (130405, 6)
    with pytest.raises(TypeError):
        assert spam.extract_fields_fromtime(d)
    assert spam.extract_fields_fromtime(t) == (130405, 6)
