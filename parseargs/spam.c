#include <Python.h>

static PyObject *
spam_parse_int(PyObject *self, PyObject *args)
{
    int a;
    if (!PyArg_ParseTuple(args, "i", &a)) { return NULL; }
    return Py_BuildValue("i", a);
}


static PyObject *
spam_parse_str(PyObject *self, PyObject *args)
{
    PyObject *u;
    if (!PyArg_ParseTuple(args, "U", &u)) { return NULL; }
    if (PyUnicode_READY(u) == -1) { return NULL; }
    return Py_BuildValue("n", PyUnicode_GET_LENGTH(u));
}


static PyObject *
spam_parse_byte(PyObject *self, PyObject *args)
{
    Py_buffer b;
    Py_ssize_t len;

    if (!PyArg_ParseTuple(args, "y*", &b)) { return NULL; }
    len = b.len;
    PyBuffer_Release(&b);

    return Py_BuildValue("n", len);
}


static PyObject *
spam_optional_args(PyObject *self, PyObject *args)
{
    int a;
    int b = 0;
    if (!PyArg_ParseTuple(args, "i|i",&a, &b)) { return NULL; }
    return Py_BuildValue("ii", a, b);
}


static PyObject *
spam_parse_keywords(PyObject *self, PyObject *args, PyObject *kwargs) {
    static char *format[] = {"a", "b", NULL};
    PyObject *a, *b;
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OO", format, &a, &b)) { return NULL; }
    return Py_BuildValue("OO", a, b);
}


typedef struct {
    char *buf;
    Py_ssize_t len;
} mybuf;


static int
spam_bytes_mybuf_converter(PyObject *bytes_obj, mybuf *buf_p)
{
    char *buf;
    Py_ssize_t len;

    if (PyBytes_Check(bytes_obj)) {
        if (PyBytes_AsStringAndSize(bytes_obj, &buf, &len)) { return 0; }
        buf_p->buf = buf;
        buf_p->len = len;
        return 1;
    } else if (PyByteArray_Check(bytes_obj)) {
        if (!(buf = PyByteArray_AsString(bytes_obj))) { return 0; }
        if ((len = PyByteArray_Size(bytes_obj)) < 0) { return 0; }
        buf_p->buf = buf;
        buf_p->len = len;
        return 1;
    }

    PyErr_SetString(PyExc_TypeError, "require bytes or bytearray");
    return 0;
}


static PyObject *
spam_use_converter(PyObject *self, PyObject *args) {
    mybuf mybuf_obj;
    if (!PyArg_ParseTuple(args, "O&", &spam_bytes_mybuf_converter, &mybuf_obj)) { return NULL; }
    return PyBytes_FromStringAndSize(mybuf_obj.buf, mybuf_obj.len);
}


static struct PyMethodDef spammethods[] = {
    /* ml_name, ml_meth, ml_flags, ml_doc */
    {"parse_int", spam_parse_int, METH_VARARGS, NULL},
    {"parse_str", spam_parse_str, METH_VARARGS, NULL},
    {"parse_byte", spam_parse_byte, METH_VARARGS, NULL},
    {"optional_args", spam_optional_args, METH_VARARGS, NULL},
    {"parse_keywords", (PyCFunction)spam_parse_keywords, METH_VARARGS|METH_KEYWORDS, NULL},
    {"use_converter", spam_use_converter, METH_VARARGS, NULL},
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
