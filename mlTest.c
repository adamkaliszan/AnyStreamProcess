#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <stdio.h>

int main()
{
	Py_Initialize();
    PyRun_SimpleString(
        "import os, sys \n"
        "sys.path.append(os.getcwd()) \n"
        "print(os.getcwd()) \n"
    );

	//PyRun_SimpleString("print('Hello World from Embedded Python!!!')");
	
    PyObject *pName = PyUnicode_DecodeFSDefault("core.ml.trDistributionML");
    PyObject *pModule = PyImport_Import(pName);
    //PyObject *pModule = PyImport_ImportModule("core.ml.trDistributionML");
    if (pModule != NULL)
    {
        printf("pModule = %p\r\n", pModule);
    }
    else
    {
        PyErr_Print();
        return 1;
    }
    Py_DECREF(pName);
    PyObject *pFunc;
    PyObject *pArgs = NULL;


    pFunc = PyObject_GetAttrString(pModule, "test");
    if (pFunc == NULL)
    {
        printf("pFunc = NULL");
    }
    else
    {
       printf("pFunx = %p\r\n", pFunc);
    }
    PyObject *pResult = PyObject_CallObject(pFunc, pArgs);

	Py_Finalize();

	return 0;
}
