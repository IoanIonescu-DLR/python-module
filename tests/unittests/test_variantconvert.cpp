/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: Apache-2.0
 * SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)
 */

#include <Python.h>

#include "test_helper.h"

#include <gtpypp.h>
#include <gtpy_decorator.h>
#include <gtest/gtest.h>

TEST(VariantConvert, intAsVariant)
{
    TestPythonContext context;
    auto ctxObj = context.obj();

    GTPY_GIL_SCOPE

    PyPPModule_AddObject(ctxObj, "integer", PyPPObject::fromLong(123));
    auto var = GtpyContextManager::instance()->getVariable(context.id(), "integer");
    EXPECT_EQ(123, var.toInt());
}

TEST(VariantConvert, getGtObjectAsVariant)
{
    auto ctxMgr = GtpyContextManager::instance();

    TestPythonContext context;
    auto ctxObj = context.obj();

    GTPY_GIL_SCOPE

    // Add gtobject to python as name "object"
    MyObject o;
    PyPPModule_AddObject(ctxObj, "object", GtpyDecorator::wrapGtObject(&o));

    // get back the object from python
    auto objPtr = ctxMgr->getVariable(context.id(), "object").value<GtObject*>();
    ASSERT_TRUE(objPtr != nullptr);

    EXPECT_EQ("MyObjectName", objPtr->objectName());
}

TEST(VariantConvert, addGtObject)
{
    auto ctxMgr = GtpyContextManager::instance();

    TestPythonContext context;
    auto ctxObj = context.obj();

    GTPY_GIL_SCOPE

    // Add gtobject to python as name "obj2"
    MyObject o;
    o.setObjectName("Object2");
    ctxMgr->addGtObject(context.id(), "obj2", &o, false);

    // get back the object from python
    auto objPtr = ctxMgr->getVariable(context.id(), "obj2").value<GtObject*>();
    ASSERT_TRUE(objPtr != nullptr);

    EXPECT_EQ("Object2", objPtr->objectName());
}

TEST(VariantConvert, addVariableObject)
{
    auto ctxMgr = GtpyContextManager::instance();

    TestPythonContext context;
    auto ctxObj = context.obj();

    GTPY_GIL_SCOPE

    // Add gtobject to python as name "obj3"
    MyObject o;
    o.setObjectName("Object3");

    // Register before QVariant -> Python conversion. Otherwise a fresh
    // process treats MyObject* as an opaque C++ pointer; registration by
    // an earlier test must not be required for the QObject round trip.
    PythonQt::self()->registerClass(o.metaObject());
    ASSERT_TRUE(ctxMgr->addVariable(context.id(), "obj3", QVariant::fromValue(&o)));

    // get back the object from python
    auto objPtr = ctxMgr->getVariable(context.id(), "obj3").value<GtObject*>();
    ASSERT_EQ(static_cast<GtObject*>(&o), objPtr);

    EXPECT_EQ("Object3", objPtr->objectName());
}
