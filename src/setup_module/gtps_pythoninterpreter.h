/* GTlab - Gas Turbine laboratory
 * Source File: gtps_pythoninterpreter.h
 * 
 * SPDX-License-Identifier: Apache-2.0
 * SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)
 *
 * Created on: 20.10.2022
 * Author: Marvin Noethen (DLR AT-TWK)
 */

#ifndef GTPSPYTHONINTERPRETER_H
#define GTPSPYTHONINTERPRETER_H

#include <QString>
#include <QStringList>
#include <QFile>

#include "gt_versionnumber.h"

/**
 * @brief The GtpsPythonInterpreter class
 */
class GtpsPythonInterpreter
{
public:
    enum Status
    {
        Valid = 0,
        Invalid,
        NotSupported
    };

    /**
     * @brief GtpsPythonInterpreter
     * @param pythonExe Path to the Python executable.
     */
    explicit GtpsPythonInterpreter(const QString& pythonExe);

    /**
     * @brief Queries the interpreter and caches its environment prefix.
     * @return True if Python runs and reports an existing absolute prefix.
     */
    bool isValid();

    /**
     * @brief Returns the Python version the evaluator uses.
     * @return The Python version the evaluator uses.
     */
    const GtVersionNumber& version() const;

    /**
     * @brief Returns the shared Python library filename
     *        (e.g. /path/to/libpython.3.9.so)
     * @return Filename of to the shared Python library.
     */
    const QString& sharedLib() const;

    /**
     * @brief Returns the list of paths that are registered in sys.path.
     * @return The list of paths that are registered in sys.path.
     */
    const QStringList& sysPaths() const;

    /**
     * @brief Returns the environment prefix reported by the interpreter.
     * @return The PYTHONHOME path, or an empty string if validation failed.
     */
    QString pythonHomePath() const;

    /**
     * @brief Evaluates the given pythonCommand using the Python executable
     * passed to the constructor.
     * @param pythonCommand Python command to evaluate.
     * @param ok Optional control flag. Is true if the evaluation
     * was successful.
     * @return The output string resulting from the execution of the
     * Python code.
     */
    QString runCommand(const QString& pythonCommand, bool* ok = nullptr) const;

    /**
     * @brief Runs a python script
     * @param scriptFile The file to the script. Note, this file needs to
     *        be already written to disk and must have a filename.
     *
     * @param ok Optional control flag. Is true if the evaluation
     * was successful.
     * @return The output string resulting from the execution of the
     * Python code.
     */
    QString runScript(const QFile& scriptFile, bool* ok = nullptr) const;

    /**
     * @brief Function to call the python interpreter with generic arguments
     * @param args The argument list to pass to python
     * @param ok Optional control flag. Is true if the evaluation
     * was successful.
     * @return The output string resulting from the execution of the
     * Python code.
     */
    QString runPythonInterpreter(const QStringList& args, bool* ok = nullptr) const;

    Status status() const;

private:
    // Python sys paths.
    QStringList m_sysPaths{};

    // Python executalbe.
    QString m_pythonExe{};

    // Validated environment prefix reported by the selected interpreter.
    QString m_pythonHome{};

    // Shared python library
    QString m_sharedLib{};

    // Python version.
    GtVersionNumber m_pyVersion{};

    // Interpreter status.
    Status m_status{Invalid};

    /**
     * @brief Asks the Python interpreter for the Python version and stores
     * the result in the version variable.
     */
    void setPythonVersion();

    /**
     * @brief Asks the interpreter for the path to the shared library and
     * stores it in the shared lib variable.
     */
    void setSharedLibPath();

    /**
     * @brief Asks the interpreter for the paths registered in sys.path and
     * stores them in the sys paths variable.
     */
    void setSysPaths();
};

#endif // GTPSPYTHONINTERPRETER_H
