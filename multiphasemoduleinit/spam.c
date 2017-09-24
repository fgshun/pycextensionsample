#define Py_LIMITED_API 0x03050000
#include <Python.h>

int spam_exec(PyObject *module)
{
    PyObject *e;

    e = PyErr_NewException("spam.error", NULL, NULL);
    if (!e) { goto fail; }
    Py_INCREF(e);
    if (PyModule_AddObject(module, "error", e)) { goto fail; }

    return 0;
 fail:
    Py_XDECREF(module);
    return -1;
}


static PyModuleDef_Slot spamslots[] = {
    {Py_mod_exec, spam_exec},
    {0, NULL}
};

static struct PyModuleDef spammodule = {
    PyModuleDef_HEAD_INIT,
    "spam",    /* m_name */
    NULL,      /* m_doc */
    0,         /* m_size */
    NULL,      /* m_methods */
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
