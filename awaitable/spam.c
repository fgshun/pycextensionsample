#define PY_SSISE_T_CLEAN
#include <Python.h>
#include "structmember.h"

typedef struct {
    PyObject_HEAD
    PyObject *iter;
    PyObject *value;
} HamObject;

static PyObject *
Ham_new(PyTypeObject *cls, PyObject *args, PyObject *kwargs) {
    PyObject *iter, *value;
    if (!PyArg_ParseTuple(args, "OO", &iter, &value)) { return NULL; }

    HamObject *self;
    self = PyObject_GC_New(HamObject, cls);
    if (!self) { return NULL; }

    Py_INCREF(iter);
    self->iter = iter;
    Py_INCREF(value);
    self->value = value;

    PyObject_GC_Track(self);
    return (PyObject *)self;
}

static int
Ham_traverse(HamObject *self, visitproc visit, void *arg)
{
    Py_VISIT(self->iter);
    Py_VISIT(self->value);
    return 0;
}

static int
Ham_clear(HamObject *self)
{
    Py_CLEAR(self->iter);
    Py_CLEAR(self->value);
    return 0;
}

static void
Ham_dealloc(HamObject *self)
{
    PyObject_GC_UnTrack(self);
    Ham_clear(self);
    PyObject_GC_Del(self);
}

static PyObject *
Ham_iter(HamObject *self) {
    return (PyObject *)self;
}

static PyObject *
Ham_iternext(HamObject *self) {
    PyObject *value;
    value = PyIter_Next(self->iter);
    if (value) {
        return value;
    }

    if (PyErr_Occurred()) {
        return NULL;
    }

    PyErr_SetObject(PyExc_StopIteration, self->value);
    return NULL;
}

static PyObject *
Ham_send(HamObject *self, PyObject *arg) {
    PyObject *value, *ret;
    value = Ham_iternext(self);
    if (value == NULL) {
        return NULL;
    }

    ret = PyNumber_Multiply(value, arg);
    Py_DECREF(value);
    return ret;
}

static PyObject *
Ham_throw(HamObject *self, PyObject *args) {
    PyObject *type, *value = NULL, *traceback = NULL;
    if (!PyArg_ParseTuple(args, "O|OO", &type, &value, traceback)) { return -1; }

    if (!PyErr_GivenExceptionMatches(type, PyExc_ValueError)) {
        PyErr_SetObject(type, value);
        return NULL;
    }

    return Ham_iternext(self);
}

static PyMethodDef Ham_methods[] = {
    {"send", (PyCFunction)Ham_send, METH_O, NULL},
    {"throw", (PyCFunction)Ham_throw, METH_VARARGS, NULL},
    {NULL, NULL, 0, NULL}
};

static PyMemberDef Ham_members[] = {
    {"iter", T_OBJECT_EX, offsetof(HamObject, iter), READONLY, NULL},
    {"value", T_OBJECT_EX, offsetof(HamObject, value), READONLY, NULL},
    {NULL}
};

static PyTypeObject HamType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "Ham",
    .tp_basicsize = sizeof(HamObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC,
    .tp_new = (newfunc)Ham_new,
    .tp_traverse = (traverseproc)Ham_traverse,
    .tp_clear = (inquiry)Ham_clear,
    .tp_dealloc = (destructor)Ham_dealloc,
    .tp_methods = Ham_methods,
    .tp_members = Ham_members,
    .tp_iter = (getiterfunc)Ham_iter,
    .tp_iternext = (iternextfunc)Ham_iternext,
};

typedef struct {
    PyObject_HEAD
    PyObject *seq;
    PyObject *value;
} SpamObject;

static PyObject *
Spam_new(PyTypeObject *cls, PyObject *args, PyObject *kwargs) {
    SpamObject *self;
    self = PyObject_GC_New(SpamObject, cls);
    if (!self) { return NULL; }
    self->seq = NULL;
    self->value = NULL;
    PyObject_GC_Track(self);
    return (PyObject *)self;
}

static int
Spam_init(SpamObject *self, PyObject *args) {
    PyObject *seq, *value, *tmp;
    if (!PyArg_ParseTuple(args, "OO", &seq, &value)) { return -1; }

    tmp = self->seq;
    Py_INCREF(seq);
    self->seq = seq;
    Py_XDECREF(tmp);

    tmp = self->value;
    Py_INCREF(value);
    self->value = value;
    Py_XDECREF(tmp);
    return 0;;
}

static int
Spam_traverse(SpamObject *self, visitproc visit, void *arg)
{
    Py_VISIT(self->seq);
    Py_VISIT(self->value);
    return 0;
}

static int
Spam_clear(SpamObject *self)
{
    Py_CLEAR(self->seq);
    Py_CLEAR(self->value);
    return 0;
}

static void
Spam_dealloc(SpamObject *self)
{
    PyObject_GC_UnTrack(self);
    Spam_clear(self);
    PyObject_GC_Del(self);
}

static PyObject *
Spam_await(SpamObject *self) {
    PyObject *temp, *iter;
    if (self->seq && self->value) {
        temp = PyObject_GetIter(self->seq);
        if (temp == NULL) {
            return NULL;
        }

        iter = PyObject_CallFunctionObjArgs((PyObject *) &HamType, temp, self->value, NULL);
        Py_DECREF(temp);
        return iter;
    }

    PyErr_SetString(PyExc_TypeError, ".seq not iterable");
    return NULL;
}

static PyAsyncMethods Spam_async_methods = {
    .am_await = (unaryfunc)Spam_await,
};

static PyMemberDef Spam_members[] = {
    {"seq", T_OBJECT_EX, offsetof(SpamObject, seq), 0, NULL},
    {"value", T_OBJECT_EX, offsetof(SpamObject, value), 0, NULL},
    {NULL}
};

static PyTypeObject SpamType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "Spam",
    .tp_basicsize = sizeof(SpamObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC,
    .tp_new = (newfunc)Spam_new,
    .tp_init = (initproc)Spam_init,
    .tp_traverse = (traverseproc)Spam_traverse,
    .tp_clear = (inquiry)Spam_clear,
    .tp_dealloc = (destructor)Spam_dealloc,
    .tp_members = Spam_members,
    .tp_as_async = &Spam_async_methods,
};

static struct PyModuleDef spammodule = {
    PyModuleDef_HEAD_INIT,
    .m_name = "spam",
    .m_size = -1,
};

PyMODINIT_FUNC
PyInit_spam(void)
{
    PyObject *module;

    if (PyType_Ready(&SpamType) < 0) {
        return NULL;
    }

    if (PyType_Ready(&HamType) < 0) {
        return NULL;
    }

    module = PyModule_Create(&spammodule);
    if (module == NULL) {
        return NULL;
    }

    Py_INCREF(&SpamType);
    PyModule_AddObject(module, "Spam", (PyObject *) &SpamType);

    Py_INCREF(&HamType);
    PyModule_AddObject(module, "Ham", (PyObject *) &HamType);

    return module;
}
