#define Py_LIMITED_API 0x03050000
#include <Python.h>

static PyObject *SpamError = NULL;

static PyObject *
spam_raise_spamerror(PyObject *self, PyObject *args)
{
    /* raise spam.error("spamspamspam") */
    PyErr_SetString(SpamError, "spamspamspam");
    return NULL;
}

static PyObject *
spam_div(PyObject *self, PyObject *args)
{
    /*
       try:
           c = a / b
       except TypeError:
           return "spam"
       return c
     */
    PyObject *a, *b, *c;
    if (!PyArg_ParseTuple(args, "OO", &a, &b)) { return NULL; }

    c = PyNumber_TrueDivide(a, b);

    if (c == NULL) {
        if (PyErr_ExceptionMatches(PyExc_TypeError)) {
            PyErr_Clear();
            return Py_BuildValue("s", "spam");
        }
        return NULL;
    }

    return c;
}

static PyObject *
spam_getitem(PyObject *self, PyObject *args)
{
    /*
       try:
           c = a[b]
       except IndexError:
           return "spam"
       except TypeError as e:
           raise spam.error("spamspam") from e
       except KeyError as e:
           raise spam.error("spamspamspam")
       return c
     */
    PyObject *a, *b, *c, *error, *value=NULL, *traceback=NULL;
    PyObject *error2, *value2, *traceback2;
    if (!PyArg_ParseTuple(args, "OO", &a, &b)) { return NULL; }

    c = PyObject_GetItem(a, b);

    error = PyErr_Occurred();
    if (error) {
        if (PyErr_GivenExceptionMatches(error, PyExc_IndexError)) {
            PyErr_Clear();
            return Py_BuildValue("s", "spam");
        } else if (PyErr_GivenExceptionMatches(error, PyExc_TypeError)) {
            PyErr_Fetch(&error, &value, &traceback);
            PyErr_NormalizeException(&error, &value, &traceback);

            PyErr_SetString(SpamError, "spamspam");
            PyErr_Fetch(&error2, &value2, &traceback2);
            PyErr_NormalizeException(&error2, &value2, &traceback2);

            Py_INCREF(value);
            PyException_SetContext(value2, value);
            PyException_SetCause(value2, value);
            PyErr_Restore(error2, value2, traceback2);

            Py_DECREF(error);
            Py_XDECREF(traceback);
            return NULL;
        } else if (PyErr_GivenExceptionMatches(error, PyExc_KeyError)) {
            PyErr_Fetch(&error, &value, &traceback);
            PyErr_NormalizeException(&error, &value, &traceback);

            PyErr_SetString(SpamError, "spamspamspam");
            PyErr_Fetch(&error2, &value2, &traceback2);
            PyErr_NormalizeException(&error2, &value2, &traceback2);

            PyException_SetContext(value2, value);
            PyErr_Restore(error2, value2, traceback2);

            Py_DECREF(error);
            Py_XDECREF(traceback);
            return NULL;
        }
        return NULL;
    }

    return c;
}


static int spam_exec(PyObject *module)
{
    if (!SpamError) {
        SpamError = PyErr_NewException("spam.error", NULL, NULL);
        if (!SpamError) { goto fail; }
    }
    Py_INCREF(SpamError);
    if (PyModule_AddObject(module, "error", SpamError)) { goto fail; }

    return 0;
fail:
    Py_XDECREF(module);
    return -1;
}


static struct PyMethodDef spammethods[] = {
    {"raise_spamerror", spam_raise_spamerror, METH_NOARGS, NULL},
    {"div", spam_div, METH_VARARGS, NULL},
    {"getitem", spam_getitem, METH_VARARGS, NULL},
    {NULL, NULL, 0, NULL}
};


static PyModuleDef_Slot spamslots[] = {
    {Py_mod_exec, spam_exec},
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
