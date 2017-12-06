#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <structmember.h>
#include <zstd.h>

typedef struct {
    ZSTD_CCtx *cctx;
    ZSTD_DCtx *dctx;
} myzstd_state;

static void myzstd_free(void *self) {
    myzstd_state *s;
    if (!(s= PyModule_GetState(self))) { return; }

    ZSTD_freeCCtx(s->cctx);
    ZSTD_freeDCtx(s->dctx);
}

static PyObject *
myzstd_compress_stream(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *src, *dst;
    const int max_compression_level = ZSTD_maxCLevel();
    int compression_level = max_compression_level;
    char *keywords[] = {"src", "dst", "compression_level", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OO|i", keywords, &src, &dst, &compression_level)) { return NULL; }

    PyObject *ret = NULL;
    PyObject *read = NULL, *write = NULL, *b = NULL, *w = NULL;
    char *buff_out = NULL;
    ZSTD_CStream *cstream = NULL;

    if (compression_level < 0 || compression_level > max_compression_level) {
        PyErr_Format(PyExc_ValueError, "0 <= compression_level <= %i, but %i", max_compression_level, compression_level);
        goto finally;
    }

    cstream = ZSTD_createCStream();
    if (!cstream) {
        PyErr_SetString(PyExc_ValueError, "failed createCStream");
        goto finally;
    }

    size_t err = ZSTD_initCStream(cstream, compression_level);
    if (ZSTD_isError(err)) {
        PyErr_Format(PyExc_ValueError, "failed initCStream %s", ZSTD_getErrorName(err));
        ZSTD_freeCStream(cstream);
        return NULL;
    }

    size_t const buff_in_size = ZSTD_CStreamInSize();
    size_t const buff_out_size = ZSTD_CStreamOutSize();
    if (!(buff_out = PyMem_RawMalloc(buff_out_size))) { goto finally; }

    if(!(read = PyObject_GetAttrString(src, "read"))) { goto finally; }
    if(!(write = PyObject_GetAttrString(dst, "write"))) { goto finally; }

    size_t to_read = buff_in_size;
    while (1) {
        b = PyObject_CallFunction(read, "k", to_read);
        if (b == NULL) { goto finally; }
        int bb = PyObject_Not(b);
        if (bb == 1) {
            break;
        } else if (bb == -1) {
            goto finally;
        }
        Py_buffer py_buf;
        if (PyObject_GetBuffer(b, &py_buf, PyBUF_SIMPLE)) { goto finally; }
        ZSTD_inBuffer input = {py_buf.buf, py_buf.len, 0};
        while (input.pos < input.size) {
            ZSTD_outBuffer output = {buff_out, buff_out_size, 0};
            to_read = ZSTD_compressStream(cstream, &output, &input);
            if (ZSTD_isError(to_read)) {
                PyErr_Format(PyExc_ValueError, "failed compressStream %s", ZSTD_getErrorName(to_read));
                PyBuffer_Release(&py_buf);
                goto finally;
            }
            if (to_read > buff_in_size) {
                to_read = buff_in_size;
            }
            if (!(w = PyObject_CallFunction(write, "y#", buff_out, output.pos))) {
                PyBuffer_Release(&py_buf);
                goto finally;
            }
            Py_DECREF(w);
            w = NULL;
        }

        PyBuffer_Release(&py_buf);
        Py_DECREF(b);
        b = NULL;
    }

    ZSTD_outBuffer output = {buff_out, buff_out_size, 0};
    size_t remaining_to_flush = ZSTD_endStream(cstream, &output);
    if (remaining_to_flush) {
        PyErr_SetString(PyExc_ValueError, "failed endStream");
        goto finally;
    }
    if (!(w = PyObject_CallFunction(write, "y#", buff_out, output.pos))) {
        goto finally;
    }
    Py_DECREF(w);
    w = NULL;

    Py_INCREF(Py_None);
    ret = Py_None;
finally:
    Py_XDECREF(b);
    Py_XDECREF(w);
    Py_XDECREF(read);
    Py_XDECREF(write);

    if (buff_out) { PyMem_RawFree(buff_out); }
    if (cstream) { ZSTD_freeCStream(cstream); }

    return ret;
}

static PyObject *
myzstd_decompress_stream(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *src, *dst;
    char *keywords[] = {"src", "dst", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OO", keywords, &src, &dst)) { return NULL; }

    PyObject *ret = NULL;
    PyObject *read = NULL, *write = NULL, *b = NULL, *w = NULL;
    char *buff_out = NULL;
    ZSTD_DStream *dstream = NULL;

    dstream = ZSTD_createDStream();
    if (!dstream) {
        PyErr_SetString(PyExc_ValueError, "failed createDStream");
        goto finally;
    }

    size_t err = ZSTD_initDStream(dstream);
    if (ZSTD_isError(err)) {
        PyErr_Format(PyExc_ValueError, "failed initDStream %s", ZSTD_getErrorName(err));
        goto finally;
    }

    size_t const buff_in_size = ZSTD_DStreamInSize();
    size_t const buff_out_size = ZSTD_DStreamOutSize();
    if (!(buff_out = PyMem_RawMalloc(buff_out_size))) { goto finally; }

    if(!(read = PyObject_GetAttrString(src, "read"))) { goto finally; }
    if(!(write = PyObject_GetAttrString(dst, "write"))) { goto finally; }

    size_t to_read = buff_in_size;
    while (1) {
        b = PyObject_CallFunction(read, "k", to_read);
        if (b == NULL) { goto finally; }
        int bb = PyObject_Not(b);
        if (bb == 1) {
            break;
        } else if (bb == -1) {
            goto finally;
        }
        Py_buffer py_buf;
        if (PyObject_GetBuffer(b, &py_buf, PyBUF_SIMPLE)) { goto finally; }
        ZSTD_inBuffer input = {py_buf.buf, py_buf.len, 0};
        while (input.pos < input.size) {
            ZSTD_outBuffer output = {buff_out, buff_out_size, 0};
            to_read = ZSTD_decompressStream(dstream, &output, &input);
            if (ZSTD_isError(to_read)) {
                PyErr_Format(PyExc_ValueError, "failed decompressStream %s", ZSTD_getErrorName(to_read));
                PyBuffer_Release(&py_buf);
                goto finally;
            }
            if (to_read > buff_in_size) {
                to_read = buff_in_size;
            }
            if (!(w = PyObject_CallFunction(write, "y#", buff_out, output.pos))) {
                PyBuffer_Release(&py_buf);
                goto finally;
            }
            Py_DECREF(w);
            w = NULL;
        }

        PyBuffer_Release(&py_buf);
        Py_DECREF(b);
        b = NULL;
    }

    Py_INCREF(Py_None);
    ret = Py_None;
finally:
    Py_XDECREF(b);
    Py_XDECREF(w);
    Py_XDECREF(read);
    Py_XDECREF(write);

    if (buff_out) { PyMem_RawFree(buff_out); }
    if (dstream) { ZSTD_freeDStream(dstream); }

    return ret;
}

static PyObject *
myzstd_compress(PyObject *self, PyObject *args)
{
    Py_buffer src;
    const int max_compression_level = ZSTD_maxCLevel();
    int compression_level = max_compression_level;
    if (!PyArg_ParseTuple(args, "y*|i", &src, &compression_level)) { return NULL; }

    if (compression_level < 0 || compression_level > max_compression_level) {
        PyErr_Format(PyExc_ValueError, "0 <= compression_level <= %i, but %i", max_compression_level, compression_level);
        PyBuffer_Release(&src);
        return NULL;
    }

    size_t dst_bound = ZSTD_compressBound(src.len);
    PyObject *ret;
    ret = PyBytes_FromStringAndSize(NULL, dst_bound);
    if (!ret) { PyBuffer_Release(&src); return NULL; }
    char *dst = PyBytes_AsString(ret);
    if (!dst) { Py_DECREF(ret); PyBuffer_Release(&src); return NULL; }

    myzstd_state *s;
    if (!(s= PyModule_GetState(self))) {
        Py_DECREF(ret);
        PyBuffer_Release(&src);
        return NULL;
    }

    size_t dst_len = ZSTD_compressCCtx(s->cctx, dst, dst_bound, src.buf, src.len, compression_level);
    PyBuffer_Release(&src);
    unsigned int e = ZSTD_isError(dst_len);
    if (e) {
        PyErr_Format(PyExc_Exception, "ZSTD_compress ERROR: %u, %s", e, ZSTD_getErrorName(dst_len));
        Py_DECREF(ret);
        return NULL;
    }

    _PyBytes_Resize(&ret, dst_len);
    return ret;
}

static PyObject *
myzstd_decompress(PyObject *self, PyObject *args)
{
    Py_buffer src;
    if (!PyArg_ParseTuple(args, "y*", &src)) { return NULL; }

    unsigned long long dst_capacity = ZSTD_getFrameContentSize(src.buf, src.len);
    if (dst_capacity == ZSTD_CONTENTSIZE_UNKNOWN) {
        PyErr_SetString(PyExc_Exception, "getFrameContentSize: CONTENTSIZE_UNKNOWNA");
        PyBuffer_Release(&src);
        return NULL;
    } else if (dst_capacity == ZSTD_CONTENTSIZE_ERROR) {
        PyErr_SetString(PyExc_Exception, "getFrameContentSize: CONTENTSIZE_ERROR");
        PyBuffer_Release(&src);
        return NULL;
    }
    PyObject *ret;
    ret = PyBytes_FromStringAndSize(NULL, dst_capacity);
    if (!ret) { PyBuffer_Release(&src); return NULL; }
    char *dst = PyBytes_AsString(ret);

    myzstd_state *s;
    if (!(s= PyModule_GetState(self))) {
        Py_DECREF(ret);
        PyBuffer_Release(&src);
        return NULL;
    }

    size_t dst_len = ZSTD_decompressDCtx(s->dctx, dst, dst_capacity, src.buf, src.len);
    PyBuffer_Release(&src);
    unsigned int e = ZSTD_isError(dst_len);
    if (e) {
        PyErr_Format(PyExc_Exception, "ZSTD_decompress ERROR: %u, %s", e, ZSTD_getErrorName(dst_len));
        Py_DECREF(ret);
        return NULL;
    }

    _PyBytes_Resize(&ret, dst_len);
    return ret;
}

static PyMethodDef myzstdmethods[] = {
    {"compress", myzstd_compress, METH_VARARGS, NULL},
    {"decompress", myzstd_decompress, METH_VARARGS, NULL},
    {"compress_stream", (PyCFunction)myzstd_compress_stream, METH_VARARGS | METH_KEYWORDS, NULL},
    {"decompress_stream", (PyCFunction)myzstd_decompress_stream, METH_VARARGS | METH_KEYWORDS, NULL},
    {NULL, NULL, 0, NULL}
};

static int myzstd_exec(PyObject *module) {
    myzstd_state *s;

    if (!(s= PyModule_GetState(module))) { return -1; }

    s->cctx = ZSTD_createCCtx();
    s->dctx = ZSTD_createDCtx();

    return 0;
}

static PyModuleDef_Slot myzstdslots[] = {
    {Py_mod_exec, myzstd_exec},
    {0, NULL}
};

static struct PyModuleDef myzstdmodule = {
    PyModuleDef_HEAD_INIT,
    "myzstd", /* m_name */
    NULL, /* m_doc */
    sizeof(myzstd_state), /* m_size */
    myzstdmethods, /* m_methods */
    myzstdslots, /* m_slots */
    NULL, /* m_traverse */
    NULL, /* m_clear */
    myzstd_free /* m_free */
};


PyMODINIT_FUNC
PyInit_myzstd(void)
{
    return PyModuleDef_Init(&myzstdmodule);
}
