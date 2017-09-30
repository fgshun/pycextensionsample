#define Py_LIMITED_API 0x03050000
#include <Python.h>

static PyObject *
spam_getattr(PyObject *self, PyObject *args)
{
    PyObject *o, *attr_name;
    if (!PyArg_ParseTuple(args, "OU", &o, &attr_name)) { return NULL; }
    return PyObject_GetAttr(o, attr_name);
}


static PyObject *
spam_callmethod(PyObject *self, PyObject *args)
{
    PyObject *os, *ret;
    if (!(os = PyImport_ImportModule("os"))) { return NULL; }
    ret = PyObject_CallMethod(os, "getcwd", NULL);
    Py_DECREF(os);
    return ret;
}


static PyObject *
spam_getitem(PyObject *self, PyObject *args)
{
    PyObject *o, *k;
    if (!PyArg_ParseTuple(args, "OO", &o, &k)) { return NULL; }
    return PyObject_GetItem(o, k);
}


static PyObject *
spam_getitem_from_seq(PyObject *self, PyObject *args)
{
    PyObject *o;
    Py_ssize_t i;
    if (!PyArg_ParseTuple(args, "On", &o, &i)) { return NULL; }
    return PySequence_GetItem(o, i);
}


static PyObject *
spam_useiter(PyObject *self, PyObject *args)
{
    PyObject *o, *iter, *item;
    Py_ssize_t hash = 0, temp = 0;
    if (!PyArg_ParseTuple(args, "O", &o)) { return NULL; }
    if (!(iter = PyObject_GetIter(o))) { return NULL; }

    while ((item = PyIter_Next(iter)) != NULL) {
        temp = PyObject_Hash(item);
        if (PyErr_Occurred()) { goto error; }
        hash ^= temp;

        Py_DECREF(item);
    }
    Py_DECREF(iter);

    if (PyErr_Occurred()) { return NULL; }
    return Py_BuildValue("n", temp);

error:
    Py_XDECREF(iter);
    Py_XDECREF(item);
    return NULL;
}


static struct PyMethodDef spammethods[] = {
    /* ml_name, ml_meth, ml_flags, ml_doc */
    {"getattr", spam_getattr, METH_VARARGS, NULL},
    {"callmethod", spam_callmethod, METH_VARARGS, NULL},
    {"getitem", spam_getitem, METH_VARARGS, NULL},
    {"getitem_from_seq", spam_getitem_from_seq, METH_VARARGS, NULL},
    {"useiter", spam_useiter, METH_VARARGS, NULL},
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
