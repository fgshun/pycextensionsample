#define Py_LIMITED_API
#include <Python.h>

static struct PyModuleDef spammodule = {
    PyModuleDef_HEAD_INIT,
    "spam",   /* m_name */
    NULL,     /* m_doc */
    0,        /* m_size */
    NULL,     /* m_methods */
    NULL,     /* m_slots */
    NULL,     /* m_traverse */
    NULL,     /* m_clear */
    NULL      /* m_free */
};

static int spam_exec(PyObject *module)
{
    if (PyModule_AddStringConstant(module, "spam", "spamspamspam")) {
        Py_DECREF(module);
        return -1;
    }

    return 0;
}


PyMODINIT_FUNC
PyInit_spam(void)
{
    PyObject *module;

    module = PyModule_Create(&spammodule);
    if (!module) { return NULL; }
    if (spam_exec(module) == -1) { return NULL; }

    return module;
}
