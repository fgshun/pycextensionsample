#include <Python.h>

static PyObject *
fetch_print_func(void)
{
    PyObject *builtins;
    if (!(builtins = PyEval_GetBuiltins())) { return NULL; }

    PyObject *print_func;
    print_func = PyMapping_GetItemString(builtins, "print");
    Py_DECREF(builtins);

    return print_func;
}

static PyObject *
spam_use_iterable(PyObject *self, PyObject *iterable)
{
    PyObject *iterator;
    if (!(iterator = PyObject_GetIter(iterable))) { return NULL; }

    PyObject *print_func;
    if (!(print_func = fetch_print_func())) {
        Py_DECREF(iterator);
        return NULL;
    }

    PyObject *item;
    while ((item = PyIter_Next(iterator))) {
        // do something
        PyObject *ret;
        if (!(ret = PyObject_CallFunctionObjArgs(print_func, item, NULL))) {
            Py_DECREF(item);
            Py_DECREF(print_func);
            Py_DECREF(iterator);
            return NULL;
        }
        Py_DECREF(ret);

        Py_DECREF(item);
    }
    Py_DECREF(print_func);
    Py_DECREF(iterator);

    if (PyErr_Occurred()) {
        return NULL;
    }

    Py_RETURN_NONE;
}


static PyObject *
spam_use_iterator(PyObject *self, PyObject *iterator)
{
    if (!PyIter_Check(iterator)) {
        PyErr_SetString(PyExc_TypeError, "arg is not iterator");
        return NULL;
    }

    PyObject *print_func;
    if (!(print_func = fetch_print_func())) { return NULL; }

    PyObject *item;
    while ((item = PyIter_Next(iterator))) {
        // do something
        PyObject *ret;
        if (!(ret = PyObject_CallFunctionObjArgs(print_func, item, NULL))) {
            Py_DECREF(item);
            Py_DECREF(print_func);
            return NULL;
        }
        Py_DECREF(ret);

        Py_DECREF(item);
    }
    Py_DECREF(print_func);

    if (PyErr_Occurred()) {
        return NULL;
    }

    Py_RETURN_NONE;
}


static struct PyMethodDef spammethods[] = {
    {"use_iterable", spam_use_iterable, METH_O, NULL},
    {"use_iterator", spam_use_iterator, METH_O, NULL},
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
