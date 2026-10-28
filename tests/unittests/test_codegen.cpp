/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: Apache-2.0
 * SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)
 */

#include <QVariant>

#include <gtest/gtest.h>

#include <gt_calculatorfactory.h>
#include <gt_coredatamodel.h>
#include <gt_coreapplication.h>

#include "gtpy_codegen.h"

#include "test_helper.h"

class TestCodegen : public ::testing::Test
{
protected:
    virtual void SetUp() override
    {
        // initialize calculator data for MyCalculator
        auto testCalcData = GT_CALC_DATA(MyCalculator);
        testCalcData->id = "My GtCalculator";
        testCalcData->version = GtVersionNumber(0, 1);
        testCalcData->status = GtCalculatorDataImpl::PROTOTYPE;
        testCalcData->category = "Test";
        testCalcData->author = "Author";

        // register calculator data in the calculator factory
        gtCalculatorFactory->registerCalculatorData(testCalcData);

        // setup calculator
        calc.setObjectName("My GtCalculator");
        calc.setFactory(gtCalculatorFactory);
    }

    virtual void TearDown() override
    {
        gtCalculatorFactory->unregisterClass(MyCalculator::staticMetaObject);
    }

    MyCalculator calc{};
    MyCoreDataModel dataModel{};
};

TEST_F(TestCodegen, TestPyCalcCode)
{
    auto code = gtpy::codegen::pyCalcCode(&calc);
    EXPECT_EQ(code.toStdString(),
              R"(my_gt_calculator = MyCalculator("My GtCalculator")
)");

    calc.strProp = "new value";
    calc.intProp = 42;
    calc.doubleProp = 4.2;
    code = gtpy::codegen::pyCalcCode(&calc);

    EXPECT_EQ(code.toStdString(),
              R"(my_gt_calculator = MyCalculator("My GtCalculator")
my_gt_calculator.setStrprop("new value")
my_gt_calculator.setIntprop(42)
my_gt_calculator.setDoubleprop(4.2)
)");

    calc.boolPropDefaultTrue = false;
    calc.boolPropDefaultFalse = true;
    calc.objLinkProp.setVal("{6c43fb0e-f7c0-4c78-bcec-3e66613647ac}");
    code = gtpy::codegen::pyCalcCode(&calc);

    EXPECT_EQ(code.toStdString(),
              R"(my_gt_calculator = MyCalculator("My GtCalculator")
my_gt_calculator.setStrprop("new value")
my_gt_calculator.setIntprop(42)
my_gt_calculator.setDoubleprop(4.2)
my_gt_calculator.setBoolproptrue(False)
my_gt_calculator.setBoolpropfalse(True)
my_gt_calculator.setObjlinkprop("{6c43fb0e-f7c0-4c78-bcec-3e66613647ac}")
)");
}

TEST_F(TestCodegen, TestPyCalcCodeWithHelpers)
{
    REGISTER_HELPER(MyCalculator, FirstCalculatorHelper);
    REGISTER_HELPER(FirstCalculatorHelper, SubHelper);

    REGISTER_HELPER(MyCalculator, SecondCalculatorHelper);

    FirstCalculatorHelper firstHelper1{};
    firstHelper1.setObjectName("First Helper");
    firstHelper1.strProp = "str prop val";
    firstHelper1.strVal = "str val";

    SubHelper subHelper{};
    subHelper.intVal = 42;

    firstHelper1.appendChild(&subHelper);

    FirstCalculatorHelper firstHelper2{};
    firstHelper2.setObjectName("First Helper");

    SecondCalculatorHelper secHelper{};
    secHelper.doubleProp = 4.2;
    secHelper.doubleVal = 42.42;

    calc.appendChild(&firstHelper1);
    calc.appendChild(&firstHelper2);
    calc.appendChild(&secHelper);

    auto code = gtpy::codegen::pyCalcCode(&calc);

    EXPECT_EQ(code.toStdString(),
              R"(my_gt_calculator = MyCalculator("My GtCalculator")

first_helper = my_gt_calculator.createFirstCalculatorHelper("First Helper")
first_helper.strVal = "str val"
first_helper.setPropertyValue("str prop", "str prop val")

sub_helper = first_helper.createSubHelper("")
sub_helper.intVal = 42
sub_helper.setPropertyValue("int prop", 0)

first_helper2 = my_gt_calculator.createFirstCalculatorHelper("First Helper")
first_helper2.strVal = "strVal"
first_helper2.setPropertyValue("str prop", "strPropVal")

second_calculator_helper = my_gt_calculator.createSecondCalculatorHelper("")
second_calculator_helper.doubleVal = 42.42
second_calculator_helper.setPropertyValue("double prop", 4.2)
)");

}

TEST(TestCodegenUtilities, TestPyIdentifier)
{
    auto ident = gtpy::codegen::pyIdentifier("gtlab");
    EXPECT_EQ(ident.toStdString(), "gtlab");

    ident = gtpy::codegen::pyIdentifier("_gtlab");
    EXPECT_EQ(ident.toStdString(), "_gtlab");

    ident = gtpy::codegen::pyIdentifier("gtlab_");
    EXPECT_EQ(ident.toStdString(), "gtlab_");

    ident = gtpy::codegen::pyIdentifier("gt_lab");
    EXPECT_EQ(ident.toStdString(), "gt_lab");

    ident = gtpy::codegen::pyIdentifier("gt lab");
    EXPECT_EQ(ident.toStdString(), "gt_lab");

    ident = gtpy::codegen::pyIdentifier("GT Lab");
    EXPECT_EQ(ident.toStdString(), "gt_lab");

    ident = gtpy::codegen::pyIdentifier("GtLaB");
    EXPECT_EQ(ident.toStdString(), "gt_la_b");

    ident = gtpy::codegen::pyIdentifier("gT Lab");
    EXPECT_EQ(ident.toStdString(), "g_t_lab");

    ident = gtpy::codegen::pyIdentifier("gT LAb");
    EXPECT_EQ(ident.toStdString(), "g_t_la_b");

    ident = gtpy::codegen::pyIdentifier("GT 123 Lab");
    EXPECT_EQ(ident.toStdString(), "gt_123_lab");

    ident = gtpy::codegen::pyIdentifier("GT/*-+,!§$%&/()=?)\" !123! #Lab");
    EXPECT_EQ(ident.toStdString(), "gt_123_lab");

    ident = gtpy::codegen::pyIdentifier("   gt LAB   ");
    EXPECT_EQ(ident.toStdString(), "gt_lab");

    ident = gtpy::codegen::pyIdentifier("123");
    EXPECT_EQ(ident.toStdString(), "_123");

    // And now forget everything you have read before.
    // Here is the one and only correct way to write it — no exceptions!
    ident = gtpy::codegen::pyIdentifier("GTlab");
    EXPECT_EQ(ident.toStdString(), "gt_lab");
}

TEST(TestCodegenUtilities, TestPyObjectIdentifier)
{
    MyObject o;
    o.setObjectName("MyObjectName");

    auto ident = gtpy::codegen::pyObjectIdentifier(&o);
    EXPECT_EQ(ident.toStdString(), "my_object_name");

    // test if the class name is used to generate the identifier when the
    // object name is empty
    o.setObjectName("");
    ident = gtpy::codegen::pyObjectIdentifier(&o);
    EXPECT_EQ(ident.toStdString(), "my_object");
}

TEST(TestCodegenUtilities, TestPyObjectPath)
{
    MyPackage p;
    p.setObjectName("MyPackageName");

    MyObject first;
    first.setObjectName("FirstObj");
    p.appendChild(&first);

    MyObject sec;
    sec.setObjectName("Second Object");
    first.appendChild(&sec);

    MyObject searchedObj;
    searchedObj.setObjectName("SearchedObject");
    sec.appendChild(&searchedObj);

    auto path = gtpy::codegen::pyObjectPath(&searchedObj);
    EXPECT_EQ(path.toStdString(),
              "MyPackageName.FirstObj.findGtChild(\"Second Object\")"
              ".SearchedObject");
}

TEST(TestCodegenUtilities, TestPySetterName)
{
    auto ident = gtpy::codegen::pySetterName("Prop");
    EXPECT_EQ(ident.toStdString(), "setProp");

    ident = gtpy::codegen::pySetterName("prop");
    EXPECT_EQ(ident.toStdString(), "setProp");

    ident = gtpy::codegen::pySetterName("My Prop");
    EXPECT_EQ(ident.toStdString(), "setMyProp");

    ident = gtpy::codegen::pySetterName("my PROP");
    EXPECT_EQ(ident.toStdString(), "setMyPROP");

    ident = gtpy::codegen::pySetterName("my 123");
    EXPECT_EQ(ident.toStdString(), "setMy123");

    ident = gtpy::codegen::pySetterName("my /*-+$%&/()=?\"");
    EXPECT_EQ(ident.toStdString(), "setMy");

    ident = gtpy::codegen::pySetterName("");
    EXPECT_EQ(ident.toStdString(), "");

    ident = gtpy::codegen::pySetterName(" /*-+$%&/()=?\"");
    EXPECT_EQ(ident.toStdString(), "");
}

TEST(TestCodegenUtilities, TestPyPropSetterName)
{
    MyProperty p;
    p.setIdent("propIdent");

    auto setterName = gtpy::codegen::pyPropSetterName(&p);
    EXPECT_EQ(setterName.toStdString(), "setPropIdent");

    p.setIdent("");

    setterName = gtpy::codegen::pyPropSetterName(&p);
    EXPECT_EQ(setterName.toStdString(), "");

    setterName = gtpy::codegen::pyPropSetterName(nullptr);
    EXPECT_EQ(setterName.toStdString(), "");
}

TEST(TestCodegenUtilities, TestPyPropValue)
{
    GtStringProperty strProp{"str prop", "name", "brief", "str value"};
    auto code = gtpy::codegen::pyPropValue(&strProp);
    EXPECT_EQ(code.toStdString(), R"("str value")");

    GtDoubleProperty doubleProp{"double prop", "name", "brief"};
    doubleProp = 4.2;
    code = gtpy::codegen::pyPropValue(&doubleProp);
    EXPECT_EQ(code.toStdString(), R"(4.2)");

    GtIntProperty intProp{"int prop", "name", "brief"};
    intProp = 42;
    code = gtpy::codegen::pyPropValue(&intProp);
    EXPECT_EQ(code.toStdString(), R"(42)");

    GtBoolProperty boolProp{"bool prop", "name", "brief", true};
    code = gtpy::codegen::pyPropValue(&boolProp);
    EXPECT_EQ(code.toStdString(), R"(True)");

    boolProp = false;
    code = gtpy::codegen::pyPropValue(&boolProp);
    EXPECT_EQ(code.toStdString(), R"(False)");

    // Keep the singleton's model alive while resolving the object link below.
    MyCoreDataModel dataModel{};

    GtObjectLinkProperty objLinkProp{"obj link prop", "name", "brief", nullptr,
                                     QStringList{} << GT_CLASSNAME(GtObject),
                                     true};
    objLinkProp.setVal("{6c43fb0e-f7c0-4c78-bcec-3e66613647ac}");
    code = gtpy::codegen::pyPropValue(&objLinkProp);
    EXPECT_EQ(code.toStdString(),
              R"("{6c43fb0e-f7c0-4c78-bcec-3e66613647ac}")");
}
