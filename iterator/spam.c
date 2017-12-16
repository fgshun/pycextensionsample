#include <Python.h>

typedef struct {
    PyObject_HEAD
    char *buf;
    Py_ssize_t len;
} SpamObject;

static PyObject *
Spam_new(PyTypeObject *cls, PyObject *args, PyObject *kwargs) {
    Py_buffer bytes_buf;
    char *keywords[] = {"bytes_obj", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "y*", keywords, &bytes_buf)) { return NULL; }

    SpamObject *self;
    self = PyObject_New(SpamObject, cls);
    if (!self) {
        PyBuffer_Release(&bytes_buf);
        return NULL;
    }

    self->len = bytes_buf.len;

    if (!(self->buf = PyMem_Malloc(bytes_buf.len))) {
        PyObject_Del(self);
        PyBuffer_Release(&bytes_buf);
        return NULL;
    }

    memcpy(self->buf, bytes_buf.buf, bytes_buf.len);

    PyBuffer_Release(&bytes_buf);

    return (PyObject *)self;
}

static PyObject *
Spam_iter(SpamObject *self)
{
    return PyObject_CallMethod((PyObject *)self, "_iter_type", "O", (PyObject *)self);
}

static void
Spam_dealloc(SpamObject *self)
{
    PyMem_Free(self->buf);
    PyObject_Del(self);
}

static PyType_Slot Spam_Type_slots[] = {
    {Py_tp_new, Spam_new},
    {Py_tp_iter, Spam_iter},
    {Py_tp_dealloc, Spam_dealloc},
    {0, 0},
};

static PyType_Spec Spam_Type_spec = {
    "spam.Spam",
    sizeof(SpamObject),
    0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    Spam_Type_slots
};

typedef struct {
    PyObject_HEAD
    SpamObject *spam_obj;
    Py_ssize_t pos;
} SpamIteratorObject;

static PyObject *
SpamIterator_new(PyTypeObject *cls, PyObject *args, PyObject *kwargs) {
    PyObject *spam_obj;
    char *keywords[] = {"spam_obj", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &spam_obj)) { return NULL; }

    PyObject *spam_type;
    if (!(spam_type = PyObject_GetAttrString((PyObject *)cls, "_spam_type"))) { return NULL; }
    if (!PyObject_IsInstance(spam_obj, spam_type)) {
        Py_DECREF(spam_type);
        PyErr_SetString(PyExc_TypeError, "not Spam instance");
        return NULL;
    }
    Py_DECREF(spam_type);

    SpamIteratorObject *self;
    self = PyObject_GC_New(SpamIteratorObject, cls);
    if (!self) {
        return NULL;
    }

    Py_INCREF(spam_obj);
    self->spam_obj = (SpamObject *)spam_obj;
    self->pos = 0;

    PyObject_GC_Track(self);

    return (PyObject *)self;
}

static PyObject *
SpamIterator_iter(SpamIteratorObject *self)
{
    Py_INCREF(self);
    return (PyObject *)self;
}

static PyObject *
SpamIterator_iternext(SpamIteratorObject *self)
{
    PyObject *ret = NULL;
    if (self->pos < self->spam_obj->len) {
        if (!(ret = PyLong_FromLong(*(self->spam_obj->buf + self->pos)))) {
            return NULL;
        }
        self->pos += 1;
    }
    return ret;
}

static int
SpamIterator_traverse(SpamIteratorObject *self, visitproc visit, void *arg)
{
    Py_VISIT(self->spam_obj);
    return 0;
}
static int
SpamIterator_clear(SpamIteratorObject *self)
{
    Py_CLEAR(self->spam_obj);
    return 0;
}

static void
SpamIterator_dealloc(SpamIteratorObject *self)
{
    PyObject_GC_UnTrack(self);
    SpamIterator_clear(self);
    PyObject_GC_Del(self);
}

static PyType_Slot SpamIterator_Type_slots[] = {
    {Py_tp_new, SpamIterator_new},
    {Py_tp_iter, SpamIterator_iter},
    {Py_tp_iternext, SpamIterator_iternext},
    {Py_tp_traverse, SpamIterator_traverse},
    {Py_tp_clear, SpamIterator_clear},
    {Py_tp_dealloc, SpamIterator_dealloc},
    {0, 0},
};

static PyType_Spec SpamIterator_Type_spec = {
    "spam.SpamIterator",
    sizeof(SpamIteratorObject),
    0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC,
    SpamIterator_Type_slots
};

static int spam_exec(PyObject *module)
{
    PyObject *spam_type = NULL, *spamiterator_type = NULL;

    spam_type = PyType_FromSpec(&Spam_Type_spec);
    if (!spam_type) { goto fail; }
    if (PyModule_AddObject(module, "Spam", spam_type)) { goto fail; }

    spamiterator_type = PyType_FromSpec(&SpamIterator_Type_spec);
    if (!spamiterator_type) { goto fail; }
    if (PyModule_AddObject(module, "SpamIterator", spamiterator_type)) { goto fail; }

    if (PyObject_SetAttrString(spam_type, "_iter_type", spamiterator_type)) { goto fail; }
    if (PyObject_SetAttrString(spamiterator_type, "_spam_type", spam_type)) { goto fail; }

    return 0;
fail:
    Py_XDECREF(spam_type);
    Py_XDECREF(spamiterator_type);
    Py_XDECREF(module);

    return -1;
}
static PyModuleDef_Slot spamslots[] = {
    {Py_mod_exec, spam_exec},
    {0, NULL}
};


static struct PyModuleDef spammodule = {
    PyModuleDef_HEAD_INIT,
    "spam",      /* m_name */
    NULL,        /* m_doc */
    0,           /* m_size */
    NULL,        /* m_methods */
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
