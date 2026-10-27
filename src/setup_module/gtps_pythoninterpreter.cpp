/* GTlab - Gas Turbine laboratory
 * Source File: gtps_pythoninterpreter.cpp
 * 
 * SPDX-License-Identifier: Apache-2.0
 * SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)
 *
 * Created on: 20.10.2022
 * Author: Marvin Noethen (DLR AT-TWK)
 */
#include "gtps_pythoninterpreter.h"

#include "gt_logging.h"

#include "find_libpython.h"
#include "gtps_systemsettings.h"
#include "gtps_globals.h"

#include <QProcess>
#include <QDir>
#include <QFileInfo>
#include <QTemporaryFile>

GtpsPythonInterpreter::GtpsPythonInterpreter(const QString& pythonExe) :
        m_pythonExe{pythonExe}
{
    if (isValid())
    {
        setPythonVersion();
        setSharedLibPath();
        setSysPaths();
    }
}

QString
GtpsPythonInterpreter::runCommand(const QString& pythonCommand, bool* ok) const
{
    return runPythonInterpreter(QStringList() << "-c" << pythonCommand, ok);
}

QString
GtpsPythonInterpreter::runScript(const QFile& scriptFile, bool* ok) const
{
    // Workaround, since QTemporaryFile().exists() is always false
    if (!QFile(scriptFile.fileName()).exists())
    {
        if (ok != nullptr)
        {
            *ok = false;
        }

        return {};
    }

    return runPythonInterpreter({scriptFile.fileName()}, ok);
}

QString
GtpsPythonInterpreter::runPythonInterpreter(const QStringList& args,
                                            bool* ok) const
{
    // temporarily clear the PYTHONPATH and PYTHONHOME variables to
    // ensure that the Python executable can be executed without
    // configuration problems
    QByteArray pySysPaths = gtps::system::pythonPath();
    QByteArray pyHome = gtps::system::pythonHome();
    gtps::system::clearPythonVars();

    // run Python in a separate process
    QProcess process;
    process.start(m_pythonExe, args);

    QByteArray retval;
    bool success{process.waitForFinished()};

    if (success && process.exitCode() == 0)
    {
        retval = process.readAll();
    }
    else
    {
        retval = process.readAllStandardError();
        success = false;
    }

    if (ok)
    {
        *ok = success;
    }

    // set PYTHONPATH and PYTHONHOME variables again
    gtps::system::setPythonPath(pySysPaths);
    gtps::system::setPythonHome(pyHome);

    return {retval};
}

GtpsPythonInterpreter::Status
GtpsPythonInterpreter::status() const
{
    return m_status;
}

bool
GtpsPythonInterpreter::isValid()
{
    bool ok{false};
    // Explicit UTF-8 also works with Windows' locale-encoded redirected stdout.
    const auto prefix = runCommand(
        "import sys; sys.stdout.buffer.write(sys.prefix.encode('utf-8'))", &ok);
    const QFileInfo home(prefix);
    ok = ok && !prefix.isEmpty() && home.isAbsolute() && home.isDir();
    // Python's environment root is not necessarily the executable directory
    // (for example, Unix environments place the executable in <prefix>/bin).
    m_pythonHome = ok ? QDir::toNativeSeparators(prefix) : QString{};
    m_status = ok ? Status::Valid : Status::Invalid;
    return ok;
}

const GtVersionNumber&
GtpsPythonInterpreter::version() const
{
    return m_pyVersion;
}

const QString&
GtpsPythonInterpreter::sharedLib() const
{
    return m_sharedLib;
}

const QStringList&
GtpsPythonInterpreter::sysPaths() const
{
    return m_sysPaths;
}

QString
GtpsPythonInterpreter::pythonHomePath() const
{
    return m_pythonHome;
}

void
GtpsPythonInterpreter::setPythonVersion()
{
    QString pyCode{"import sys;"
                   "sys.stdout.write('%s.%s.%s' % (sys.version_info.major, "
                   "sys.version_info.minor, sys.version_info.micro))"};

    bool ok{false};
    auto version = runCommand(pyCode, &ok);

    if (ok)
    {
        m_pyVersion = GtVersionNumber{version};

        if (!gtps::python::version::isSupported(m_pyVersion))
        {
            m_status = Status::NotSupported;
        }
    }
}

std::unique_ptr<QTemporaryFile>
getFindLibPythonScriptFile()
{
    auto file = std::make_unique<QTemporaryFile>();
    if (file->open())
    {
        file->write(findPythonLibCode);
        file->flush();
    }
    return file;
}

void
GtpsPythonInterpreter::setSharedLibPath()
{
    bool ok{false};
    auto sharedLib = runScript(*getFindLibPythonScriptFile(), &ok);

    if (ok)
    {
        m_sharedLib = sharedLib.replace("\\\\", "\\");
    }
}

void
GtpsPythonInterpreter::setSysPaths()
{
    QString pyCode{"import sys;print(', '.join([x for x in sys.path if x]), "
                   "end='')"};
    bool ok{false};
    auto pyPaths = runCommand(pyCode, &ok);

    if (ok)
    {
        m_sysPaths = pyPaths.split(", ");
        auto toNativeSep = [](QString& pyPath){
                            pyPath = QDir::toNativeSeparators(pyPath);};
        std::for_each(m_sysPaths.begin(), m_sysPaths.end(), toNativeSep);
    }
}
