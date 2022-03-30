
#ifndef MODULE_TEST_H
#define MODULE_TEST_H


#include "pch.h"
#include "Constants.h"
#include "vec_op.h"
#include "DataProc.h"
#include <gsl/gsl_sf_bessel.h>
#include "MathProc.h"
#include "passPyCpp.h"
#include "matplotlibCpp.h"
#include <boost/python.hpp>
#include <boost/python/numpy.hpp>

#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <Python.h>
#endif


void opTest();
void DPTest();
void ConstantsTest();
void timerTest();
void vecTest();
void saveTest();
void EigenTest();
void boostTest();
void MPTest();
void pyTest(const bool py_initialize, const bool py_finalize);
void pyNumpyTest(const bool py_initialize, const bool py_finalize);
void matplotlibTest(const bool py_initialize, const bool py_finalize);
void sharedTest();

#endif //MODULE_TEST