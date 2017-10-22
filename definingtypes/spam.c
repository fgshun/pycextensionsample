#define Py_LIMITED_API 0x03050000
#include <Python.h>

typedef struct {
    PyObject_HEAD
    PyObject *multiplier;
} SpamObject;

static PyObject *
Spam_new(PyTypeObject *cls, PyObject *args, PyObject *kwargs) {
    SpamObject *self;
    self = PyObject_New(SpamObject, cls);
    if (!self) { return NULL; }
    return (PyObject *)self;
}

static int
Spam_init(SpamObject *self, PyObject *args) {
    PyObject *mul = NULL, *tmp;
    if (!PyArg_ParseTuple(args, "O", &mul)) { return -1; }

    tmp = self->multiplier;
    Py_INCREF(mul);
    self->multiplier = mul;
    Py_XDECREF(tmp);
    return 0;;
}

static int
Spam_traverse(SpamObject *self, visitproc visit, void *arg)
{
    Py_VISIT(self->multiplier);
    return 0;
}

static int
Spam_clear(SpamObject *self)
{
    Py_CLEAR(self->multiplier);
    return 0;
}

static void
Spam_dealloc(SpamObject *self)
{
    PyObject_GC_UnTrack(self);
    Spam_clear(self);
    PyObject_Del(self);
}

static PyObject *
Spam_spam(SpamObject *self, PyObject *args) {
    PyObject *multiplicand = NULL;
    if (!PyArg_ParseTuple(args, "O", &multiplicand)) { return NULL; }
    return PyNumber_Multiply(multiplicand, self->multiplier);
}

static PyMethodDef Spam_methods[] = {
    {"spam", (PyCFunction)Spam_spam, METH_VARARGS, NULL},
    {NULL, NULL, 0, NULL}
};
static PyType_Slot Spam_Type_slots[] = {
    {Py_tp_new, Spam_new},
    {Py_tp_init, Spam_init},
    {Py_tp_traverse, Spam_traverse},
    {Py_tp_clear, Spam_clear},
    {Py_tp_dealloc, Spam_dealloc},
    {Py_tp_methods, Spam_methods},
    {0, 0},
};

static PyType_Spec Spam_Type_spec = {
    "spam.Spam",
    sizeof(SpamObject),
    0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC,
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
