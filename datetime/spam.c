#include <Python.h>
#include <datetime.h>

static PyModuleDef_Slot spamslots[] = {
    {0, NULL}
};


static PyObject *
spam_date20170930(PyObject *self, PyObject *args)
{
    return PyDate_FromDate(2017, 9, 30);
}


static struct PyMethodDef spammethods[] = {
    /* ml_name, ml_meth, ml_flags, ml_doc */
    {"date20170930", spam_date20170930, METH_NOARGS, NULL},
    {NULL, NULL, 0, NULL} /* Sentinel */
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
