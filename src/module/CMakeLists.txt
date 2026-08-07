#             ______________      __
#            / ____/_  __/ /___  / /_
#           / / __  / / / / __ `/ __
#          / /_/ / / / / / /_/ / /_/ /
#         ____/ /_/ /_/\__,_/_.___/
#
######################################################################

# SPDX-License-Identifier: Apache-2.0
# SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)

set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(HEADERS
    collection/browser/gtpy_collectionbrowser.h
    collection/browser/gtpy_collectionbrowsermodel.h
    collection/browser/gtpy_collectionbrowsersortmodel.h
    collection/browser/items/gtpy_abstractbrowseritem.h
    collection/browser/items/gtpy_browseritem.h
    collection/browser/items/gtpy_collapsiblebrowseritem.h
    collection/browser/items/gtpy_rootbrowseritem.h
    collection/gtpy_collectionwidget.h
    collection/gtpy_scriptcollectionsettings.h
    collection/itemwidget/gtpy_collectionitemwidget.h
    collection/localwidget/gtpy_collectionlocalmodel.h
    collection/localwidget/gtpy_collectionlocalwidget.h
    collection/localwidget/items/gtpy_abstractlocalitem.h
    collection/localwidget/items/gtpy_collapsiblelocalitem.h
    collection/localwidget/items/gtpy_localitem.h
    extensions/gtpy_pythonfunctions.h
    gt_compat.h
    gt_python.h
    gt_pythonmodule_exports.h
    gtpy_icons_compat.h
    gtpy_stylesheet_compat.h
    models/gtpy_completermodel.h
    models/gtpy_objectmodel.h
    models/gtpy_taskstylemodel.h
    post/gtpy_pythonplotconfigdialog.h
    post/gtpy_pythonplotdata.h
    post/gtpy_pythonplotitem.h
    post/gtpy_pythonplotwidget.h
    post/gtpy_pythonsvgwidget.h
    processcomponents/gtpy_abstractscriptcomponent.h
    processcomponents/gtpy_scriptcalculator.h
    processcomponents/gtpy_task.h
    utilities/gtpy_calculatorfactory.h
    utilities/gtpy_code.h
    utilities/gtpy_codegen.h
    utilities/gtpy_context.h
    utilities/gtpy_contextmanager.h
    utilities/gtpy_convert.h
    utilities/gtpy_decorator.h
    utilities/gtpy_gilscope.h
    utilities/gtpy_threadscope.h
    utilities/gtpy_globals.h
    utilities/gtpy_interruptrunnable.h
    utilities/gtpy_matplotlib.h
    utilities/gtpy_module.h
    utilities/gtpy_packageiteration.h
    utilities/gtpy_processdatadistributor.h
    utilities/gtpy_regexp.h
    utilities/gtpy_scriptrunnable.h
    utilities/gtpy_utils.h
    utilities/gtpy_taskapi.h
    utilities/gtpy_tempdir.h
    utilities/gtpy_transfer.h
    utilities/pythonextensions/gtpy_createhelperfunction.h
    utilities/pythonextensions/gtpy_extendedwrapper.h
    utilities/pythonextensions/gtpy_importfunction.h
    utilities/pythonextensions/gtpy_loggingmodule.h
    utilities/pythonextensions/gtpy_propertysetter.h
    utilities/pythonextensions/gtpy_stdout.h
    utilities/gtpypp.h
    widgets/gtpy_completer.h
    widgets/gtpy_console.h
    widgets/gtpy_lineedit.h
    widgets/gtpy_replacewidget.h
    widgets/gtpy_scripteditor.h
    widgets/gtpy_scripteditorwidget.h
    widgets/gtpy_scriptview.h
    widgets/gtpy_taskdelegate.h
    widgets/gtpy_tasktreeview.h
    wizards/editor_settings/gtpy_editorsettings.h
    wizards/editor_settings/gtpy_editorsettingsdialog.h
    wizards/editor_settings/pages/gtpy_abstractsettingspage.h
    wizards/editor_settings/pages/gtpy_generalpage.h
    wizards/gtpy_abstractscriptingwizardpage.h
    wizards/gtpy_wizardgeometries.h
    wizards/gtpy_wizardgeometryitem.h
    wizards/python_task/gtpy_taskwizardpage.h
    wizards/script_calculator/gtpy_scriptcalculatorwizardpage.h
    utilities/gtpy_moduleupgrader.h
)

set(SOURCES
    collection/browser/gtpy_collectionbrowser.cpp
    collection/browser/gtpy_collectionbrowsermodel.cpp
    collection/browser/gtpy_collectionbrowsersortmodel.cpp
    collection/browser/items/gtpy_abstractbrowseritem.cpp
    collection/browser/items/gtpy_browseritem.cpp
    collection/browser/items/gtpy_collapsiblebrowseritem.cpp
    collection/browser/items/gtpy_rootbrowseritem.cpp
    collection/gtpy_collectionwidget.cpp
    collection/gtpy_scriptcollectionsettings.cpp
    collection/itemwidget/gtpy_collectionitemwidget.cpp
    collection/localwidget/gtpy_collectionlocalmodel.cpp
    collection/localwidget/gtpy_collectionlocalwidget.cpp
    collection/localwidget/items/gtpy_abstractlocalitem.cpp
    collection/localwidget/items/gtpy_collapsiblelocalitem.cpp
    collection/localwidget/items/gtpy_localitem.cpp
    extensions/gtpy_pythonfunctions.cpp
    gt_python.cpp
    models/gtpy_completermodel.cpp
    models/gtpy_objectmodel.cpp
    models/gtpy_taskstylemodel.cpp
    post/gtpy_pythonplotconfigdialog.cpp
    post/gtpy_pythonplotdata.cpp
    post/gtpy_pythonplotitem.cpp
    post/gtpy_pythonplotwidget.cpp
    post/gtpy_pythonsvgwidget.cpp
    processcomponents/gtpy_abstractscriptcomponent.cpp
    processcomponents/gtpy_scriptcalculator.cpp
    processcomponents/gtpy_task.cpp
    utilities/gtpy_calculatorfactory.cpp
    utilities/gtpy_code.cpp
    utilities/gtpy_codegen.cpp
    utilities/gtpy_context.cpp
    utilities/gtpy_contextmanager.cpp
    utilities/gtpy_convert.cpp
    utilities/gtpy_decorator.cpp
    utilities/gtpy_gilscope.cpp
    utilities/gtpy_interruptrunnable.cpp
    utilities/gtpy_module.cpp
    utilities/gtpy_processdatadistributor.cpp
    utilities/gtpy_regexp.cpp
    utilities/gtpy_scriptrunnable.cpp
    utilities/gtpy_utils.cpp
    utilities/gtpy_taskapi.cpp
    utilities/gtpy_tempdir.cpp
    utilities/gtpy_transfer.cpp
    utilities/pythonextensions/gtpy_calculatorsmodule.cpp
    utilities/pythonextensions/gtpy_createhelperfunction.cpp
    utilities/pythonextensions/gtpy_extendedwrapper.cpp
    utilities/pythonextensions/gtpy_importfunction.cpp
    utilities/pythonextensions/gtpy_loggingmodule.cpp
    utilities/pythonextensions/gtpy_propertysetter.cpp
    utilities/pythonextensions/gtpy_stdout.cpp
    widgets/gtpy_completer.cpp
    widgets/gtpy_console.cpp
    widgets/gtpy_lineedit.cpp
    widgets/gtpy_replacewidget.cpp
    widgets/gtpy_scripteditor.cpp
    widgets/gtpy_scripteditorwidget.cpp
    widgets/gtpy_scriptview.cpp
    widgets/gtpy_taskdelegate.cpp
    widgets/gtpy_tasktreeview.cpp
    wizards/editor_settings/gtpy_editorsettings.cpp
    wizards/editor_settings/gtpy_editorsettingsdialog.cpp
    wizards/editor_settings/pages/gtpy_abstractsettingspage.cpp
    wizards/editor_settings/pages/gtpy_generalpage.cpp
    wizards/gtpy_abstractscriptingwizardpage.cpp
    wizards/gtpy_wizardgeometries.cpp
    wizards/gtpy_wizardgeometryitem.cpp
    wizards/python_task/gtpy_taskwizardpage.cpp
    wizards/script_calculator/gtpy_scriptcalculatorwizardpage.cpp
    utilities/gtpy_moduleupgrader.cpp
)

set(MODULE_ID "Python Module (Python ${Python3_VERSION_MAJOR}.${Python3_VERSION_MINOR})")

add_gtlab_module(GTlabPython SOURCES ${HEADERS} ${SOURCES}
    MODULE_ID ${MODULE_ID}
    README_FILE "${PROJECT_SOURCE_DIR}/README.md"
    CHANGELOG_FILE "${PROJECT_SOURCE_DIR}/CHANGELOG.md"
)

set_target_properties(GTlabPython PROPERTIES OUTPUT_NAME
    GTlabPython${Python3_VERSION_SUFFIX})

add_library(GTlab::Python ALIAS GTlabPython)

target_compile_definitions(GTlabPython PRIVATE
    GT_PYTHON_DLL
    GT_LOG_USE_QT_BINDINGS
    PYTHON_LIBRARY="$<TARGET_FILE_NAME:Python3::Python>"
)

set(INSTALL_SUBDIR python${Python3_VERSION_SUFFIX})

target_include_directories(GTlabPython PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/.>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/collection>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/collection/browser>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/collection/browser/items>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/collection/itemwidget>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/collection/localwidget>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/collection/localwidget/items>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/extensions>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/models>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/post>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/processcomponents>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/utilities>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/utilities/pythonextensions>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/widgets>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/wizards>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/wizards/editor_settings>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/wizards/editor_settings/pages>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/wizards/script_calculator>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/wizards/python_task>

    $<INSTALL_INTERFACE:include/${INSTALL_SUBDIR}>
)

target_link_libraries(GTlabPython
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Xml
    Qt${QT_VERSION_MAJOR}::Svg
    GTlab::Logging
    GTlab::Core
    PUBLIC
    PythonQt::PythonQt
)

# include SvgWidgets, if Qt major version is 6 or higher
if (QT_VERSION_MAJOR GREATER_EQUAL 6)
    target_link_libraries(GTlabPython
        PRIVATE Qt6::SvgWidgets
    )
endif()

if (GTlab_VERSION_MAJOR GREATER_EQUAL 2)
    target_link_libraries(GTlabPython PRIVATE GTlab::Gui)
else()
    target_link_libraries(GTlabPython PRIVATE GTlab::Mdi GTlab::Calculators GTlab::Numerics)
endif()

install (TARGETS GTlabPython
  EXPORT GTlabPython-targets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/${INSTALL_SUBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/${INSTALL_SUBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_MODULEDIR}
)

install (FILES ${HEADERS} DESTINATION include/${INSTALL_SUBDIR})

# cmake configuration export
set(CMAKE_INSTALL_INCLUDE_DIR "include")
set(CONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/GTlabPython")

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/GTlabPythonConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
  "${PROJECT_SOURCE_DIR}/cmake/GTlabPythonConfig.in"
  "${CMAKE_CURRENT_BINARY_DIR}/GTlabPythonConfig.cmake"
  INSTALL_DESTINATION "${CONFIG_INSTALL_DIR}"
  NO_SET_AND_CHECK_MACRO
  NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

install(EXPORT  GTlabPython-targets
    DESTINATION ${CONFIG_INSTALL_DIR}
)

install (FILES
  "${CMAKE_CURRENT_BINARY_DIR}/GTlabPythonConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/GTlabPythonConfigVersion.cmake"
  DESTINATION ${CONFIG_INSTALL_DIR})

######################################################################
