# SPDX-License-Identifier: Apache-2.0
# SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)

project(GTlabPython-UnitTests)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

if (NOT TARGET Qt5::Core AND NOT TARGET Qt6::Core)
    include(RequireQt)
    require_qt(COMPONENTS Core)
endif()

if (NOT TARGET gtest)
    include(AddGoogleTest)
endif()


add_executable(GTlabPythonUnitTest
    main.cpp
    test_helper.h
    test_variantconvert.cpp
    test_codegen.cpp
    test_contextconfig.cpp
)

target_compile_definitions(GTlabPythonUnitTest
    PRIVATE GT_MODULE_ID="Python Unit Tests"
)

target_link_libraries(GTlabPythonUnitTest PRIVATE GTlab::Core GTlab::Python gtest Qt${QT_VERSION_MAJOR}::Core)

include(GoogleTest)
gtest_discover_tests(GTlabPythonUnitTest TEST_PREFIX "PythonModule." DISCOVERY_MODE PRE_TEST)

if (TARGET GTlabPythonSetup)
    find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
    add_executable(PythonInterpreterFixture python_interpreter_fixture.cpp)
    target_link_libraries(PythonInterpreterFixture PRIVATE Qt${QT_VERSION_MAJOR}::Core)

    add_executable(GTlabPythonSetupUnitTest
        main.cpp
        test_pythoninterpreter.cpp
        ../../src/setup_module/gtps_pythoninterpreter.cpp
        ../../src/setup_module/gtps_systemsettings.cpp
        ../../src/setup_module/gtps_globals.cpp
    )
    target_include_directories(GTlabPythonSetupUnitTest PRIVATE ../../src/setup_module)
    target_compile_definitions(GTlabPythonSetupUnitTest PRIVATE
        GT_MODULE_ID="Python Setup Tests"
        TEST_INTERPRETER_FIXTURE="$<TARGET_FILE:PythonInterpreterFixture>"
        TEST_SELECTED_PYTHON="${Python3_EXECUTABLE}"
    )
    target_link_libraries(GTlabPythonSetupUnitTest PRIVATE GTlab::Gui gtest)
    add_dependencies(GTlabPythonSetupUnitTest PythonInterpreterFixture)
    gtest_discover_tests(GTlabPythonSetupUnitTest
        TEST_PREFIX "PythonSetup." DISCOVERY_MODE PRE_TEST)
endif()
