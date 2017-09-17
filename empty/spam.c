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
    PyObject *m;

    m = PyModule_Create(&spammodule);
    if (m == NULL)
        return NULL;

    return m;
}
