#define Py_LIMITED_API 0x03050000
#include <Python.h>

static PyObject *SpamError = NULL;

static int spam_exec(PyObject *module)
{
    PyObject *zero;

    if (!SpamError) {
        SpamError = PyErr_NewException("spam.error", NULL, NULL);
        if (!SpamError) { goto fail; }
    }
    Py_INCREF(SpamError);
    if (PyModule_AddObject(module, "error", SpamError)) { goto fail; }

    zero = PyLong_FromLong(0);
    if (!zero) { goto fail; }
    if (PyModule_AddObject(module, "zero", zero)) { goto fail; }

    if (PyModule_AddIntConstant(module, "one", 1)) { goto fail; }

    if (PyModule_AddStringConstant(module, "spam", "spamspamspam")) { goto fail; }
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
