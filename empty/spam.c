#define Py_LIMITED_API
#include <Python.h>


static struct PyModuleDef spammodule = {
    PyModuleDef_HEAD_INIT,
    "spam",   /* m_name */
    NULL,     /* m_doc */
    -1,       /* m_size */
    NULL,     /* m_methods */
    NULL,     /* m_slots */
    NULL,     /* m_traverse */
    NULL,     /* m_clear */
    NULL      /* m_free */
};


PyMODINIT_FUNC
PyInit_spam(void)
{
    PyObject *module, *error;

    module = PyModule_Create(&spammodule);
    if (!module) { goto fail; }

    /* initialize module */
    error = PyErr_NewException("spam.error", NULL, NULL);
    if (!error) { goto fail; }
    Py_INCREF(error);
    if (PyModule_AddObject(module, "error", error)) { goto fail; }

    return module;
 fail:
    Py_XDECREF(module);
    return NULL;
}
