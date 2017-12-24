#define Py_LIMITED_API 0x03050000
#include <Python.h>
#include <structmember.h>

typedef struct {
    PyObject_HEAD
    long v;
} SpamObject;

static PyObject *
Spam_new(PyTypeObject *cls, PyObject *args, PyObject *kwargs) {
    SpamObject *self;
    self = PyObject_New(SpamObject, cls);
    if (!self) { return NULL; }
    self->v = 0;
    return (PyObject *)self;
}

static int
Spam_init(SpamObject *self, PyObject *args) {
    long v = 0;
    if (!PyArg_ParseTuple(args, "|l", &v)) { return -1; }

    self->v = v;
    return 0;;
}

static void
Spam_dealloc(SpamObject *self)
{
    PyObject_Del(self);
}

static PyMethodDef Spam_methods[] = {
    {NULL, NULL, 0, NULL}
};

static PyMemberDef Spam_members[] = {
    {"v", T_LONG, offsetof(SpamObject, v), 0, NULL},
    {NULL}
};

static PyObject *
Spam_add(SpamObject *self, PyObject *other)
{
    PyObject *cls;
    if (!(cls = PyObject_Type((PyObject *)self))) { return NULL; }
    if (!PyObject_IsInstance(other, cls)) {
        Py_DECREF(cls);
        PyErr_Format(PyExc_TypeError, "%R is not Spam object", other);
        return NULL;
    }

    PyObject *v;
    if (!(v = PyLong_FromLong(self->v + ((SpamObject *)other)->v))) {
        Py_DECREF(cls);
        return NULL;
    }

    PyObject *ret;
    if (!(ret = PyObject_CallFunctionObjArgs(cls, v, NULL))) {
        Py_DECREF(v);
        Py_DECREF(cls);
        return NULL;
    }

    Py_DECREF(v);
    Py_DECREF(cls);
    return ret;
};

static PyType_Slot Spam_Type_slots[] = {
    {Py_tp_new, Spam_new},
    {Py_tp_init, Spam_init},
    {Py_tp_dealloc, Spam_dealloc},
    {Py_tp_methods, Spam_methods},
    {Py_tp_members, Spam_members},
    {Py_nb_add, Spam_add},
    {0, 0},
};

static PyType_Spec Spam_Type_spec = {
    "spam.Spam",
    sizeof(SpamObject),
    0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    Spam_Type_slots
};

static int spam_exec(PyObject *module)
{
    PyObject *spam_type;
    spam_type = PyType_FromSpec(&Spam_Type_spec);
    if (!spam_type) { goto fail; }
    if (PyModule_AddObject(module, "Spam", spam_type)) { goto fail; }

    return 0;
fail:
    Py_XDECREF(spam_type);
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
