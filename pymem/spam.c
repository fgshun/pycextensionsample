#include <Python.h>
#include <structmember.h>

typedef struct _Node Node;

typedef struct {
    PyObject_HEAD
    Node *root;
    Py_ssize_t export_count;
} BinaryTreeObject;

struct _Node {
    long value;
    Node *left;
    Node *right;
};

static PyObject *
BinaryTree_new(PyTypeObject *cls, PyObject *args, PyObject *kwargs)
{
    BinaryTreeObject *self;

    self = PyObject_New(BinaryTreeObject, cls);
    if (!self) { return NULL; }
    self->export_count = 0;

    self->root = NULL;

    return (PyObject *)self;
}

static void
BinaryTree__dealloc(Node *node)
{
    if (node == NULL) { return; }

    BinaryTree__dealloc(node->left);
    BinaryTree__dealloc(node->right);
    PyMem_Free(node);

    return;
}

static void
BinaryTree_dealloc(BinaryTreeObject *self)
{
    if (self->export_count > 0) {
        PyErr_SetString(PyExc_SystemError, "binary_tree dealloc");
        PyErr_Print();
    }
    BinaryTree__dealloc(self->root);
    PyObject_Del(self);
}

static PyMemberDef BinaryTree_members[] = {
    {"export_count", T_PYSSIZET, offsetof(BinaryTreeObject, export_count), READONLY, NULL},
    {NULL}
};

static PyObject *
BinaryTree__search(Node *node, long v)
{
    if (node == NULL) {
        Py_RETURN_FALSE;
    }
    if (node->value == v) {
        Py_RETURN_TRUE;
    }
    if (node->value > v) {
        return BinaryTree__search(node->left, v);
    }
    return BinaryTree__search(node->right, v);
}

static PyObject *
BinaryTree_search(BinaryTreeObject *self, PyObject *args, PyObject *kwargs)
{
    long v;
    if (!PyArg_ParseTuple(args, "l", &v)) { return NULL; }
    return BinaryTree__search(self->root, v);
}

Node *
BinaryTree__insert(Node *node, long v)
{
    if (node == NULL) {
        node = PyMem_Malloc(sizeof(Node));
        node->value = v;
        node->left = NULL;
        node->right = NULL;
        return node;
    }
    if (node->value > v) {
        node->left = BinaryTree__insert(node->left, v);
        return node;
    }
    node->right = BinaryTree__insert(node->right, v);
    return node;
}

static PyObject *
BinaryTree_insert(BinaryTreeObject *self, PyObject *args, PyObject *kwargs)
{
    long v;
    if (!PyArg_ParseTuple(args, "l", &v)) { return NULL; }

    self->root = BinaryTree__insert(self->root, v);
    Py_RETURN_NONE;
}

static PyObject *
BinaryTree__delete(Node **node, long v)
{
    Node *temp, *temp2;

    while (*node != NULL) {
        if ((*node)->value > v) {
            node = &(*node)->left;
        } else if ((*node)->value < v) {
            node = &(*node)->right;
        } else {
            if ((*node)->left && (*node)->right) {
                temp2 = (*node)->left;
                while (temp2->right) {
                    temp2 = temp2->right;
                }
                temp = *node;
                *node = temp2;
                /* XXX 5 の */ //temp2 = temp2->left;
                (*node)->left = temp->left;
                (*node)->right = temp->right;
            } else if ((*node)->left) {
                temp = *node;
                *node = (*node)->left;
            } else if ((*node)->right) {
                temp = *node;
                *node = (*node)->right;
            } else {
                temp = *node;
                *node = NULL;
            }
            PyMem_Free(temp);
            Py_RETURN_TRUE;
        }

    }

    Py_RETURN_FALSE;
}

static PyObject *
BinaryTree_delete(BinaryTreeObject *self, PyObject *args, PyObject *kwargs)
{
    long v;
    if (!PyArg_ParseTuple(args, "l", &v)) { return NULL; }

    return BinaryTree__delete(&self->root, v);
}

static PyMethodDef BinaryTree_methods[] = {
    {"search", (PyCFunction)BinaryTree_search, METH_VARARGS, NULL},
    {"insert", (PyCFunction)BinaryTree_insert, METH_VARARGS, NULL},
    {"delete", (PyCFunction)BinaryTree_delete, METH_VARARGS, NULL},
    {NULL, NULL, 0, NULL}
};

static PyObject *
BinaryTree_iter(BinaryTreeObject *self, PyObject *args, PyObject *kwargs)
{
    return PyObject_CallMethod((PyObject *)self, "_iter_type", "O", self);
}

static PyType_Slot BinaryTree_Type_slots[] = {
    {Py_tp_new, BinaryTree_new},
    {Py_tp_dealloc, BinaryTree_dealloc},
    {Py_tp_members, BinaryTree_members},
    {Py_tp_methods, BinaryTree_methods},
    {Py_tp_iter, BinaryTree_iter},
    {0, 0},
};

static PyType_Spec BinaryTree_Type_spec = {
    "spam.BinaryTree",
    sizeof(BinaryTreeObject),
    0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    BinaryTree_Type_slots
};

typedef struct _NodeStack NodeStack;

struct _NodeStack{
    Node *node;
    NodeStack *parent;
};

typedef struct {
    PyObject_HEAD
    PyObject *p;
    Node *node;
    NodeStack *stack;
    char state;
} BinaryTreeIterObject;

static PyObject *
BinaryTreeIter_new(PyTypeObject *cls, PyObject *args, PyObject *kwargs)
{
    BinaryTreeIterObject *self;
    PyObject *binary_tree = NULL;

    if (!PyArg_ParseTuple(args, "O", &binary_tree)) { return NULL; }

    self = PyObject_New(BinaryTreeIterObject, cls);
    if (!self) { return NULL; }

    self->node = ((BinaryTreeObject *)binary_tree)->root;
    self->stack = NULL;
    self->state = 0;
    Py_INCREF(binary_tree);
    self->p = binary_tree;

    return (PyObject *)self;
}

void
BinaryTreeIter_dealloc(BinaryTreeIterObject *self)
{
    NodeStack *temp;
    while (self->stack != NULL) {
        temp = self->stack;
        self->stack = temp->parent;
        PyMem_Free(temp);
    }

    Py_DECREF(self->p);
    PyObject_Del(self);
}

static PyObject *
BinaryTreeIter_iter(BinaryTreeObject *self, PyObject *args, PyObject *kwargs)
{
    return (PyObject *)self;
}

static PyObject *
BinaryTreeIter_iternext(BinaryTreeIterObject *self, PyObject *args, PyObject *kwargs)
{
    NodeStack *temp;

    switch (self->state) {
        case 1:
            /* suspended */
            goto restert;
        case 2:
            /* stop iteration */
            return NULL;
        case -1:
            PyErr_SetString(PyExc_MemoryError, "");
            return NULL;
    }

    while (self->stack != NULL || self->node != NULL) {
        if (self->node != NULL) {
            temp = PyMem_Malloc(sizeof(NodeStack));
            if (!temp) {
                self->state = -1;
                PyErr_SetString(PyExc_MemoryError, "malloc BinaryTreeIterObject.stack");
                return NULL;
            }
            temp->parent = self->stack;
            temp->node = self->node;
            self->stack = temp;

            self->node = self->node->left;
        } else {
            temp = self->stack;
            self->node = temp->node;
            self->stack = temp->parent;
            PyMem_Free(temp);

            self->state = 1;  /* suspended */
            return PyLong_FromLong(self->node->value);
restert:
            self->state = 0;

            self->node = self->node->right;
        }
    }

    self->state = 2; /* stop iteration. */
    return NULL;
}

static PyType_Slot BinaryTreeIter_Type_slots[] = {
    {Py_tp_new, BinaryTreeIter_new},
    {Py_tp_dealloc, BinaryTreeIter_dealloc},
    {Py_tp_iter, BinaryTreeIter_iter},
    {Py_tp_iternext, BinaryTreeIter_iternext},
    {0, 0},
};

static PyType_Spec BinaryTreeIter_Type_spec = {
    "spam.BinaryTreeIter",
    sizeof(BinaryTreeIterObject),
    0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    BinaryTreeIter_Type_slots
};

int spam_exec(PyObject *module)
{
    PyObject *binary_tree_type = NULL, *binary_tree_iter_type = NULL;

    binary_tree_iter_type = PyType_FromSpec(&BinaryTreeIter_Type_spec);
    if (!binary_tree_iter_type) { goto fail; }

    binary_tree_type = PyType_FromSpec(&BinaryTree_Type_spec);
    if (!binary_tree_type) { goto fail; }
    PyObject_SetAttrString(binary_tree_type, "_iter_type", binary_tree_iter_type);
    Py_DECREF(binary_tree_iter_type);
    binary_tree_iter_type = NULL;

    if (PyModule_AddObject(module, "BinaryTree", binary_tree_type)) { goto fail; }
    binary_tree_type = NULL;

    return 0;
fail:
    Py_XDECREF(binary_tree_iter_type);
    Py_XDECREF(binary_tree_type);
    Py_XDECREF(module);

    return -1;
}

static PyMethodDef spam_methods[] = {
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
