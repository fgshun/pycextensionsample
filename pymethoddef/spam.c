#define Py_LIMITED_API 0x03050000
#include <Python.h>

static PyObject *
spam_noargs(PyObject *self, PyObject *args)
{
    Py_RETURN_NONE;
}


static PyObject *
spam_varargs(PyObject *self, PyObject *args)
{
    Py_INCREF(args);
    return args;
}


static PyObject *
spam_keywords(PyObject *self, PyObject *args, PyObject *kwargs)
{
    return Py_BuildValue("(OO)", args, kwargs);
}


static PyObject *
spam_o(PyObject *self, PyObject *args)
{
    Py_INCREF(args);
    return args;
}


static struct PyMethodDef spammethods[] = {
    /* ml_name, ml_meth, ml_flags, ml_doc */
    {"noargs", spam_noargs, METH_NOARGS, NULL},
    {"varargs", spam_varargs, METH_VARARGS, NULL},
    {"keywords", (PyCFunction)spam_keywords, METH_VARARGS|METH_KEYWORDS, NULL},
    {"o", spam_o, METH_O, NULL},
    {NULL, NULL, 0, NULL} /* Sentinel */
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
    return PyModuleDef_Init(&spammodule);
}
