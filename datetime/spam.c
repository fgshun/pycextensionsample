#include <Python.h>
#include <datetime.h>

static PyObject *
spam_date20170930(PyObject *self, PyObject *args)
{
    return PyDate_FromDate(2017, 9, 30);
}


static PyObject *
spam_extract_fields_fromdate(PyObject *self, PyObject *date)
{
    int year, month, day;
    if (!PyDate_Check(date)) {
        PyErr_SetString(PyExc_TypeError, "date subtype required");
        return NULL;
    }

    year = PyDateTime_GET_YEAR(date);
    month = PyDateTime_GET_MONTH(date);
    day = PyDateTime_GET_DAY(date);

    return PyLong_FromLong(year * 10000 + month * 100 + day);
}


static PyObject *
spam_extract_fields_fromtime(PyObject *self, PyObject *time)
{
    int hour, minute, second, microsecond;
    if (PyDateTime_Check(time)) {
        hour = PyDateTime_DATE_GET_HOUR(time);
        minute = PyDateTime_DATE_GET_MINUTE(time);
        second = PyDateTime_DATE_GET_SECOND(time);
        microsecond = PyDateTime_DATE_GET_MICROSECOND(time);
    } else if (PyTime_Check(time)) {
        hour = PyDateTime_TIME_GET_HOUR(time);
        minute = PyDateTime_TIME_GET_MINUTE(time);
        second = PyDateTime_TIME_GET_SECOND(time);
        microsecond = PyDateTime_TIME_GET_MICROSECOND(time);
    } else {
        PyErr_SetString(PyExc_TypeError, "datetime subtype or time subtype required");
        return NULL;
    }

    return Py_BuildValue("ii", hour * 10000 + minute * 100 + second, microsecond);
}


static struct PyMethodDef spammethods[] = {
    {"date20170930", spam_date20170930, METH_NOARGS, NULL},
    {"extract_fields_fromdate", spam_extract_fields_fromdate, METH_O, NULL},
    {"extract_fields_fromtime", spam_extract_fields_fromtime, METH_O, NULL},
    {NULL, NULL, 0, NULL}
};


static PyModuleDef_Slot spamslots[] = {
    {0, NULL}
};


static struct PyModuleDef spammodule = {
    PyModuleDef_HEAD_INIT,
    "spam",      /* m_name */
    NULL,        /* m_doc */
    0,           /* m_size */
    spammethods, /* m_methods */
    spamslots,   /* m_slots */
    NULL,        /* m_traverse */
    NULL,        /* m_clear */
    NULL         /* m_free */
};


PyMODINIT_FUNC
PyInit_spam(void)
{
    PyDateTime_IMPORT;
    return PyModuleDef_Init(&spammodule);
}
