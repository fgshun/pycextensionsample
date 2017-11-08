#include <Python.h>
#include <structmember.h>

typedef struct _Node Node;

typedef struct {
    PyObject_HEAD
    Node *root;
} BinaryTreeObject;

struct _Node {
    long value;
    Node *left;
    Node *right;
};

static PyObject *
BinaryTree_new(PyTypeObject *cls, PyObject *args)
{
    BinaryTreeObject *self;

    self = PyObject_New(BinaryTreeObject, cls);
    if (!self) { return NULL; }

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
    BinaryTree__dealloc(self->root);
    PyObject_Del(self);
}

static PyObject *
BinaryTree__search(Node *node, long v)
{
    PyObject *ret;

    if (node == NULL) {
        Py_RETURN_FALSE;
    }
    if (node->value == v) {
        Py_RETURN_TRUE;
    }
    if (node->value > v) {
        if (Py_EnterRecursiveCall(" BinaryTree__search")) { return NULL; }
        ret = BinaryTree__search(node->left, v);
        Py_LeaveRecursiveCall();
        return ret;
    }
    if (Py_EnterRecursiveCall(" BinaryTree__search")) { return NULL; }
    ret = BinaryTree__search(node->right, v);
    Py_LeaveRecursiveCall();
    return ret;
}

static PyObject *
BinaryTree_search(BinaryTreeObject *self, PyObject *args)
{
    long v;
    if (!PyArg_ParseTuple(args, "l", &v)) { return NULL; }
    return BinaryTree__search(self->root, v);
}

Node *
BinaryTree__insert(Node *node, long v)
{
    Node *temp;

    if (node == NULL) {
        node = PyMem_Malloc(sizeof(Node));
        node->value = v;
        node->left = NULL;
        node->right = NULL;
        return node;
    }
    if (node->value > v) {
        if (Py_EnterRecursiveCall(" BinaryTree__insert")) { return NULL; }
        temp = BinaryTree__insert(node->left, v);
        Py_LeaveRecursiveCall();
        if (!temp) { return NULL; }
        node->left = temp;
        return node;
    }
    if (Py_EnterRecursiveCall(" BinaryTree__insert")) { return NULL; }
    temp = BinaryTree__insert(node->right, v);
    Py_LeaveRecursiveCall();
    if (!temp) { return NULL; }
    node->right = temp;
    return node;
}

static PyObject *
BinaryTree_insert(BinaryTreeObject *self, PyObject *args)
{
    long v;
    Node *node;
    if (!PyArg_ParseTuple(args, "l", &v)) { return NULL; }

    node = BinaryTree__insert(self->root, v);
    if (!node) { return NULL; }
    self->root = node;
    Py_RETURN_NONE;
}

static PyObject *
BinaryTree__delete(Node **node, long v)
{
    Node *del_node, **move_node;

    while (*node != NULL) {
        if ((*node)->value > v) {
            node = &(*node)->left;
        } else if ((*node)->value < v) {
            node = &(*node)->right;
        } else {
            if ((*node)->left && (*node)->right) {
                del_node = *node;

                move_node = &(*node)->left;
                while ((*move_node)->right) {
                    move_node = &(*move_node)->right;
                }
                *node = *move_node;
                *move_node = (*move_node)->left;
                (*node)->left = del_node->left;
                (*node)->right = del_node->right;
            } else if ((*node)->left) {
                del_node = *node;
                *node = (*node)->left;
            } else if ((*node)->right) {
                del_node = *node;
                *node = (*node)->right;
            } else {
                del_node = *node;
                *node = NULL;
            }
            PyMem_Free(del_node);
            Py_RETURN_TRUE;
        }

    }

    Py_RETURN_FALSE;
}

static PyObject *
BinaryTree_delete(BinaryTreeObject *self, PyObject *args)
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
BinaryTree_iter(PyObject *self)
{
    return PyObject_CallMethod(self, "_iter_type", "O", self);
}

static PyType_Slot BinaryTree_Type_slots[] = {
    {Py_tp_new, BinaryTree_new},
    {Py_tp_dealloc, BinaryTree_dealloc},
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
    PyObject *treeobj;
    Node *node;
    NodeStack *stack;
    char state;
} BinaryTreeIterObject;

static PyObject *
BinaryTreeIter_new(PyTypeObject *cls, PyObject *args)
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
    self->treeobj = binary_tree;

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

    Py_DECREF(self->treeobj);
    PyObject_Del(self);
}

static PyObject *
BinaryTreeIter_iter(PyObject *self)
{
    Py_INCREF(self);
    return (PyObject *)self;
}

static PyObject *
BinaryTreeIter_iternext(BinaryTreeIterObject *self)
{
    NodeStack *temp;

    switch (self->state) {
        case 1:
            /* suspended */
            goto restert;
        case 2:
            /* finished */
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

    self->state = 2; /* finished */
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
