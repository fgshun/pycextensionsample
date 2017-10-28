#include <Python.h>
#include <structmember.h>

typedef struct {
    PyObject_HEAD
    unsigned char b;
    Py_ssize_t export_count;
} SpamObject;

int Spam_getbuffer(SpamObject *exporter, Py_buffer *view, int flags) {
    int ret;

    if (view == NULL) {
        PyErr_SetString(PyExc_BufferError, "spamspam");
        return -1;
    }

    ret = PyBuffer_FillInfo(view, (PyObject *)exporter, &exporter->b, 1, 0 , flags);
    if (!ret) {
        exporter->export_count++;
    }
    return ret;
}

void Spam_releasebuffer(SpamObject *exporter, Py_buffer *view) {
    exporter->export_count--;
}

static PyBufferProcs spam_buffer_procs= {
    (getbufferproc)Spam_getbuffer,
    (releasebufferproc)Spam_releasebuffer
};

static PyObject *
Spam_new(PyTypeObject *cls, PyObject *args, PyObject *kwargs) {
    SpamObject *self;
    self = PyObject_New(SpamObject, cls);
    if (!self) { return NULL; }
    self->b = 0;
    self->export_count = 0;
    return (PyObject *)self;
}

static int
Spam_init(SpamObject *self, PyObject *args) {
    if (!PyArg_ParseTuple(args, "c", &self->b)) { return -1; }
    return 0;;
}

static void
Spam_dealloc(SpamObject *self)
{
    if (self->export_count > 0) {
        PyErr_SetString(PyExc_SystemError, "spam");
        PyErr_Print();
    }
    PyObject_Del(self);
}


static PyMemberDef Spam_members[] = {
    {"b", T_UBYTE, offsetof(SpamObject, b), 0, NULL},
    {"export_count", T_PYSSIZET, offsetof(SpamObject, export_count), 0, NULL},
    {NULL}
};

static PyType_Slot Spam_Type_slots[] = {
    {Py_tp_new, Spam_new},
    {Py_tp_init, Spam_init},
    {Py_tp_dealloc, Spam_dealloc},
    {Py_tp_members, Spam_members},
    {0, 0},
};

static PyType_Spec Spam_Type_spec = {
    "spam.Spam",
    sizeof(SpamObject),
    0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    Spam_Type_slots
};

int spam_exec(PyObject *module)
{
    PyObject *spam_type;
    spam_type = PyType_FromSpec(&Spam_Type_spec);
    if (!spam_type) { goto fail; }
    if (PyModule_AddObject(module, "Spam", spam_type)) { goto fail; }
    ((PyTypeObject *)spam_type)->tp_as_buffer = &spam_buffer_procs;

    return 0;
fail:
    Py_XDECREF(spam_type);
    Py_XDECREF(module);

    return -1;
}


static PyObject *
spam_use_readonly_buffer(SpamObject *self, PyObject *args) {
    Py_buffer buf;
    const unsigned char *a;
    unsigned long v=0;

    if (!PyArg_ParseTuple(args, "y*", &buf)) { return NULL; }
    a = buf.buf;
    for (Py_ssize_t i=0; i<buf.len; i++) {
        v += *a;
        a++;
    }

    PyBuffer_Release(&buf);

    return PyLong_FromUnsignedLong(v);
}


static PyObject *
spam_use_writable_buffer(SpamObject *self, PyObject *args) {
    Py_buffer buf;
    unsigned char *a;

    if (!PyArg_ParseTuple(args, "w*", &buf)) { return NULL; }
    a = buf.buf;
    for (Py_ssize_t i=0; i<buf.len; i++) {
        (*a)++;
        a++;
    }

    PyBuffer_Release(&buf);

    Py_RETURN_NONE;
}


static PyMethodDef spam_methods[] = {
    {"use_readonly_buffer", (PyCFunction)spam_use_readonly_buffer, METH_VARARGS, NULL},
    {"use_writable_buffer", (PyCFunction)spam_use_writable_buffer, METH_VARARGS, NULL},
    {NULL, NULL, 0, NULL}
};
static PyModuleDef_Slot spamslots[] = {
    {Py_mod_exec, spam_exec},
    {0, NULL}
};

static struct PyModuleDef spammodule = {
    PyModuleDef_HEAD_INIT,
    "spam",    /* m_name */
    NULL,      /* m_doc */
    0,         /* m_size */
    spam_methods, /* m_methods */
    spamslots, /* m_slots */
    NULL,      /* m_traverse */
    NULL,      /* m_clear */
    NULL       /* m_free */
};


PyMODINIT_FUNC
PyInit_spam(void)
{
    return PyModuleDef_Init(&spammodule);
}
