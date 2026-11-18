/* D-Bus exception base classes.
 *
 * Copyright (C) 2006 Collabora Ltd. <http://www.collabora.co.uk/>
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "dbus_bindings-internal.h"

static PyObject *imported_dbus_exception = NULL;
#ifdef Py_GIL_DISABLED
static PyMutex imported_dbus_exception_mutex;
#endif

/* Return the cached, process-lifetime reference to DBusException. */
static PyObject *
get_dbus_exception(void)
{
    PyObject *exception;
    PyObject *module;
    PyObject *cached;

#ifdef Py_GIL_DISABLED
    PyMutex_Lock(&imported_dbus_exception_mutex);
#endif
    exception = imported_dbus_exception;
#ifdef Py_GIL_DISABLED
    PyMutex_Unlock(&imported_dbus_exception_mutex);
#endif
    if (exception != NULL)
        return exception;

    /* Importing and looking up the attribute can execute Python code. */
    module = PyImport_ImportModule("dbus.exceptions");
    if (module == NULL)
        return NULL;
    exception = PyObject_GetAttrString(module, "DBusException");
    Py_DECREF(module);
    if (exception == NULL)
        return NULL;

#ifdef Py_GIL_DISABLED
    PyMutex_Lock(&imported_dbus_exception_mutex);
#endif
    if (imported_dbus_exception == NULL) {
        Py_INCREF(exception);
        imported_dbus_exception = exception;
    }
    cached = imported_dbus_exception;
#ifdef Py_GIL_DISABLED
    PyMutex_Unlock(&imported_dbus_exception_mutex);
#endif
    Py_DECREF(exception);
    return cached;
}

PyObject *
DBusPyException_SetString(const char *msg)
{
    PyObject *exception = get_dbus_exception();

    if (exception != NULL)
        PyErr_SetString(exception, msg);
    return NULL;
}

PyObject *
DBusPyException_ConsumeError(DBusError *error)
{
    PyObject *exception = get_dbus_exception();
    PyObject *exc_value = NULL;

    if (exception == NULL) {
        goto finally;
    }

    exc_value = PyObject_CallFunction(exception,
                                      "s",
                                      error->message ? error->message
                                                     : "");

    if (!exc_value) {
        goto finally;
    }

    if (error->name) {
        PyObject *name = PyUnicode_FromString(error->name);
        int ret;

        if (!name)
            goto finally;
        ret = PyObject_SetAttrString(exc_value, "_dbus_error_name", name);
        Py_CLEAR(name);
        if (ret < 0) {
            goto finally;
        }
    }

    PyErr_SetObject(exception, exc_value);

finally:
    Py_CLEAR(exc_value);
    dbus_error_free(error);
    return NULL;
}

/* vim:set ft=c cino< sw=4 sts=4 et: */
