# SPDX-License-Identifier: Apache-2.0
# SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)
#
# Cmake script to build python qt from git using cmake
# Modernized version of https://github.com/commontk/PythonQt

cmake_minimum_required(VERSION 3.15)

project(PythonQt)

set(PythonQT_VERSION Qt6.5)

#----------------------------------------------------------------------------
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

include(RequireQt)
require_qt(COMPONENTS Core Widgets Network OpenGL Sql Svg UiTools Xml)

set(PythonQt_QT_VERSION ${QT_VERSION_MAJOR})

set(CMAKE_AUTOMOC ON)

# Requirements
set(minimum_required_qt5_version "5.3.0")
set(minimum_required_qt_version ${minimum_required_qt${PythonQt_QT_VERSION}_version})

# Download pythonqt source code
include(FetchContent)
FetchContent_Declare(
    pythonqt
    GIT_REPOSITORY https://github.com/dlr-gtlab/pythonqt.git
    GIT_TAG ${PythonQT_VERSION}
)

if(NOT pythonqt_POPULATED)
    message(STATUS "Downloading pythonqt library")
    FetchContent_Populate(pythonqt)
endif()

set(PYTHON_QT_SRC_DIR ${pythonqt_SOURCE_DIR})

#----------------------------------------------------------------------------
# Qt components
set(qtlibs
  Core
  Widgets
  Network
  OpenGL
  Sql
  Svg
  Multimedia
  UiTools
  Xml
  XmlPatterns
  )
# Webkit has been removed in Qt >= 5.6
if("${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}" VERSION_LESS "5.7")
  list(APPEND qtlibs
    WebKitWidgets
    )
endif()
if("${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}" VERSION_GREATER "5.5")
  list(APPEND qtlibs
    Qml
    Quick
    )
endif()

#-----------------------------------------------------------------------------
# Python libraries

find_package(Python3 REQUIRED COMPONENTS Development)

#-----------------------------------------------------------------------------
# Build options

if(NOT DEFINED PythonQt_INSTALL_RUNTIME_DIR)
  set(PythonQt_INSTALL_RUNTIME_DIR bin)
endif()

if(NOT DEFINED PythonQt_INSTALL_LIBRARY_DIR)
  set(PythonQt_INSTALL_LIBRARY_DIR lib${LIB_SUFFIX})
endif()

if(NOT DEFINED PythonQt_INSTALL_ARCHIVE_DIR)
  set(PythonQt_INSTALL_ARCHIVE_DIR lib${LIB_SUFFIX})
endif()

if(NOT DEFINED PythonQt_INSTALL_INCLUDE_DIR)
  set(PythonQt_INSTALL_INCLUDE_DIR include/PythonQt)
endif()

if(NOT DEFINED PythonQt_INSTALL_CONFIG_DIR)
  set(PythonQt_INSTALL_CONFIG_DIR ${PythonQt_INSTALL_LIBRARY_DIR}/cmake/PythonQt)
endif()



if(NOT DEFINED PythonQt_DEBUG_POSTFIX)
  set(CMAKE_DEBUG_POSTFIX "_d")
else()
  set(CMAKE_DEBUG_POSTFIX ${PythonQt_DEBUG_POSTFIX})
endif()

#-----------------------------------------------------------------------------
# Set qtlib_to_wraplib_* variables

set(qtlib_to_wraplib_Widgets gui)
set(qtlib_to_wraplib_WebKitWidgets webkit)

set(qt5_wrapped_lib_depends_gui Multimedia PrintSupport)
set(qt5_wrapped_lib_depends_multimedia MultimediaWidgets)
set(qt5_wrapped_lib_depends_quick QuickWidgets)

foreach(qtlib ${qtlibs})
  string(TOLOWER ${qtlib} qtlib_lowercase)
  if(DEFINED qtlib_to_wraplib_${qtlib})
    set(qtlib_lowercase ${qtlib_to_wraplib_${qtlib}})
  endif()
  set(qtlib_to_wraplib_${qtlib} ${qtlib_lowercase})
endforeach()

#-----------------------------------------------------------------------------
# Define PythonQt_Wrap_Qt* options
option(PythonQt_Wrap_QtAll "Make all Qt components available in python" OFF)
foreach(qtlib ${qtlibs})
  OPTION(PythonQt_Wrap_Qt${qtlib_to_wraplib_${qtlib}} "Make all of Qt${qtlib} available in python" OFF)
endforeach()

#-----------------------------------------------------------------------------
# Force option if it applies
if(PythonQt_Wrap_QtAll)
  foreach(qtlib ${qtlibs})
    # XXX xmlpatterns wrapper does *NOT* build at all :(
    if(${qtlib} STREQUAL "XmlPatterns")
      continue()
    endif()
    set(qt_wrapped_lib ${qtlib_to_wraplib_${qtlib}})
    if(NOT ${PythonQt_Wrap_Qt${qt_wrapped_lib}})
      set(PythonQt_Wrap_Qt${qt_wrapped_lib} ON CACHE BOOL "Make all of Qt${qt_wrapped_lib} available in python" FORCE)
      message(STATUS "Enabling [PythonQt_Wrap_Qt${qt_wrapped_lib}] because of [PythonQt_Wrap_QtAll] evaluates to True")
    endif()
  endforeach()
endif()

option(PythonQt_DEBUG "Enable/Disable PythonQt debug output" OFF)


#-----------------------------------------------------------------------------
# Setup Qt

# Required components
set(qt_required_components Core Widgets)
foreach(qtlib ${qtlibs})
  set(qt_wrapped_lib ${qtlib_to_wraplib_${qtlib}})
  if(${PythonQt_Wrap_Qt${qt_wrapped_lib}})
    list(APPEND qt_required_components ${qtlib} ${qt${PythonQt_QT_VERSION}_wrapped_lib_depends_${qt_wrapped_lib}})
  endif()
endforeach()
if(BUILD_TESTING)
  list(APPEND qt_required_components Test)
endif()
list(REMOVE_DUPLICATES qt_required_components)

message(STATUS "${PROJECT_NAME}: Required Qt components [${qt_required_components}]")
require_qt(COMPONENTS ${qt_required_components})

if(UNIX)
  find_package(OpenGL)
  if(OPENGL_FOUND)
    list(APPEND QT_LIBRARIES ${OPENGL_LIBRARIES})
  endif()
endif()

#-----------------------------------------------------------------------------
# The variable "generated_cpp_suffix" allows to conditionnally compile the generated wrappers
# associated with the Qt version being used.

set(generated_cpp_suffix_515 _515)
set(generated_cpp_suffix_65 _65)

set(generated_cpp_suffix "_${QT_VERSION_MAJOR}${QT_VERSION_MINOR}")
if(DEFINED generated_cpp_suffix_${QT_VERSION_MAJOR}${QT_VERSION_MINOR})
  set(generated_cpp_suffix "${generated_cpp_suffix_${QT_VERSION_MAJOR}${QT_VERSION_MINOR}}")
elseif("${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}" VERSION_GREATER "6.4")
  set(generated_cpp_suffix "_65")
elseif("${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}" VERSION_GREATER "5.14")
  set(generated_cpp_suffix "_515")
endif()

#-----------------------------------------------------------------------------
# Sources

set(sources
    src/PythonQtBoolResult.cpp
    src/PythonQtClassInfo.cpp
    src/PythonQtClassWrapper.cpp
    src/PythonQtConversion.cpp
    src/PythonQt.cpp
    src/PythonQtImporter.cpp
    src/PythonQtInstanceWrapper.cpp
    src/PythonQtMethodInfo.cpp
    src/PythonQtMisc.cpp
    src/PythonQtObjectPtr.cpp
    src/PythonQtProperty.cpp
    src/PythonQtQFileImporter.cpp
    src/PythonQtSignalReceiver.cpp
    src/PythonQtSlot.cpp
    src/PythonQtSlotDecorator.cpp
    src/PythonQtSignal.cpp
    src/PythonQtStdDecorators.cpp
    src/PythonQtStdIn.cpp
    src/PythonQtStdOut.cpp
    src/PythonQtThreadSupport.cpp
    src/gui/PythonQtScriptingConsole.cpp

    #generated_cpp${generated_cpp_suffix}/PythonQt_QtBindings.cpp

    generated_cpp${generated_cpp_suffix}/com_trolltech_qt_core_builtin/com_trolltech_qt_core_builtin0.cpp
    generated_cpp${generated_cpp_suffix}/com_trolltech_qt_core_builtin/com_trolltech_qt_core_builtin_init.cpp
    generated_cpp${generated_cpp_suffix}/com_trolltech_qt_gui_builtin/com_trolltech_qt_gui_builtin0.cpp
    generated_cpp${generated_cpp_suffix}/com_trolltech_qt_gui_builtin/com_trolltech_qt_gui_builtin_init.cpp
)

#-----------------------------------------------------------------------------
# List headers.  This is list is used for the install command.

set(headers
    src/PythonQtBoolResult.h
    src/PythonQtClassInfo.h
    src/PythonQtClassWrapper.h
    src/PythonQtConversion.h
    src/PythonQtCppWrapperFactory.h
    src/PythonQtDoc.h
    src/PythonQt.h
    src/PythonQtImporter.h
    src/PythonQtImportFileInterface.h
    src/PythonQtInstanceWrapper.h
    src/PythonQtMethodInfo.h
    src/PythonQtMisc.h
    src/PythonQtObjectPtr.h
    src/PythonQtProperty.h
    src/PythonQtQFileImporter.h
    src/PythonQtSignalReceiver.h
    src/PythonQtSlot.h
    src/PythonQtSlotDecorator.h
    src/PythonQtSignal.h
    src/PythonQtStdDecorators.h
    src/PythonQtStdIn.h
    src/PythonQtStdOut.h
    src/PythonQtSystem.h
    src/PythonQtThreadSupport.h
    src/PythonQtUtils.h
    src/PythonQtVariants.h
    src/PythonQtPythonInclude.h
    # generated_cpp${generated_cpp_suffix}/PythonQt_QtBindings.h
)

#-----------------------------------------------------------------------------
# Headers that should run through moc

set(moc_sources
    src/PythonQt.h
    src/PythonQtSignalReceiver.h
    src/PythonQtStdDecorators.h
    src/gui/PythonQtScriptingConsole.h

    generated_cpp${generated_cpp_suffix}/com_trolltech_qt_core_builtin/com_trolltech_qt_core_builtin0.h
    generated_cpp${generated_cpp_suffix}/com_trolltech_qt_gui_builtin/com_trolltech_qt_gui_builtin0.h
)

list(TRANSFORM sources PREPEND "${PYTHON_QT_SRC_DIR}/")
list(TRANSFORM headers PREPEND "${PYTHON_QT_SRC_DIR}/")
list(TRANSFORM moc_sources PREPEND "${PYTHON_QT_SRC_DIR}/")

#-----------------------------------------------------------------------------
# Add extra sources
set(PYTHONQT_DEFINES "")
foreach(qtlib ${qtlibs})

  set(qt_wrapped_lib ${qtlib_to_wraplib_${qtlib}})

  if (${PythonQt_Wrap_Qt${qt_wrapped_lib}})
    LIST(APPEND PYTHONQT_DEFINES -DPYTHONQT_WRAP_Qt${qt_wrapped_lib})

    set(file_prefix generated_cpp${generated_cpp_suffix}/com_trolltech_qt_${qt_wrapped_lib}/com_trolltech_qt_${qt_wrapped_lib})

    foreach(index RANGE 0 12)

      # Source files
      if(EXISTS ${PYTHON_QT_SRC_DIR}/${file_prefix}${index}.cpp)
        list(APPEND sources ${PYTHON_QT_SRC_DIR}/${file_prefix}${index}.cpp)
      endif()

      # Headers that should run through moc
      if(EXISTS ${PYTHON_QT_SRC_DIR}/${file_prefix}${index}.h)
        list(APPEND moc_sources ${PYTHON_QT_SRC_DIR}/${file_prefix}${index}.h)
      endif()

    endforeach()

    list(APPEND sources ${PYTHON_QT_SRC_DIR}/${file_prefix}_init.cpp)

  endif()
endforeach()

#-----------------------------------------------------------------------------
# Build the library


add_library(PythonQt SHARED
            ${sources}
            ${moc_sources}
)

add_library(PythonQt::PythonQt ALIAS PythonQt)

target_include_directories(PythonQt PUBLIC
  $<BUILD_INTERFACE:${PYTHON_QT_SRC_DIR}/src>
  $<INSTALL_INTERFACE:${PythonQt_INSTALL_INCLUDE_DIR}>
)

target_compile_definitions(PythonQt PRIVATE
  # -DPYTHONQT_USE_RELEASE_PYTHON_FALLBACK
  # -DPYTHONQT_SUPPORT_NAME_PROPERTY
  ${PYTHONQT_DEFINES}
)

if(PythonQt_DEBUG)
  target_compile_definitions(PythonQt PRIVATE -DPYTHONQT_DEBUG)
endif()

set_target_properties(PythonQt PROPERTIES DEFINE_SYMBOL PYTHONQT_EXPORTS)

# make library name compatible to the qmake one
set_target_properties(PythonQt PROPERTIES 
  OUTPUT_NAME "PythonQt-Qt${PythonQt_QT_VERSION}-Python${Python3_VERSION_MAJOR}.${Python3_VERSION_MINOR}")

target_compile_options(PythonQt PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/bigobj>
)

target_link_libraries(PythonQt PUBLIC Qt${PythonQt_QT_VERSION}::Core)
foreach(qtlib ${qt_required_components})
  target_link_libraries(PythonQt PRIVATE Qt${PythonQt_QT_VERSION}::${qtlib})
endforeach()

find_package(Qt${QT_VERSION_MAJOR}CorePrivate CONFIG REQUIRED)

target_link_libraries(PythonQt PUBLIC
            Python3::Python
            PRIVATE
            # Required for use of "QtCore/private/qmetaobjectbuilder_p.h" in "PythonQt.cpp"
            Qt${QT_VERSION_MAJOR}::CorePrivate
)

#-----------------------------------------------------------------------------
# Install library (on windows, put the dll in 'bin' and the archive in 'lib')

install(TARGETS PythonQt
        EXPORT PythonQt-targets
        RUNTIME DESTINATION ${PythonQt_INSTALL_RUNTIME_DIR}
        LIBRARY DESTINATION ${PythonQt_INSTALL_LIBRARY_DIR}
        ARCHIVE DESTINATION ${PythonQt_INSTALL_ARCHIVE_DIR})
install(FILES ${headers} DESTINATION ${PythonQt_INSTALL_INCLUDE_DIR})


file(GENERATE
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/PythonQtConfig.cmake"
  INPUT  "${PROJECT_SOURCE_DIR}/PythonQtConfig.in"
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/PythonQtConfigVersion.cmake"
  VERSION ${PythonQT_VERSION}
  COMPATIBILITY AnyNewerVersion
)

install (FILES
  "${CMAKE_CURRENT_BINARY_DIR}/PythonQtConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/PythonQtConfigVersion.cmake"
  DESTINATION ${PythonQt_INSTALL_CONFIG_DIR})

install(EXPORT  PythonQt-targets
    DESTINATION ${PythonQt_INSTALL_CONFIG_DIR}
)

#-----------------------------------------------------------------------------
# Testing

option(PythonQt_BUILD_TESTING "Build the testing tree." OFF)
include(CTest)

if(PythonQt_BUILD_TESTING)
  set(test_sources
    ${PYTHON_QT_SRC_DIR}/tests/PythonQtTestMain.cpp
    ${PYTHON_QT_SRC_DIR}/tests/PythonQtTests.cpp
    ${PYTHON_QT_SRC_DIR}/tests/PythonQtTests.h
  )

  add_executable(PythonQtTest ${test_sources})
  target_link_libraries(PythonQtTest PRIVATE PythonQt)

  add_test(
    NAME tests_PythonQtTestMain
    COMMAND ${Slicer_LAUNCH_COMMAND} $<TARGET_FILE:PythonQtTest>
    )
endif()
