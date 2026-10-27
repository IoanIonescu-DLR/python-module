// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)

#include "gtps_pythoninterpreter.h"
#include <gtest/gtest.h>
#include <QDir>
#include <QProcess>
#include <QTemporaryDir>

namespace
{
class SavedEnvironment
{
public:
    explicit SavedEnvironment(const char* name) :
        m_name(name), m_set(qEnvironmentVariableIsSet(name)), m_value(qgetenv(name)) {}
    ~SavedEnvironment()
    {
        if (m_set) qputenv(m_name, m_value);
        else qunsetenv(m_name);
    }
private:
    const char* m_name;
    bool m_set;
    QByteArray m_value;
};

class PythonHome : public testing::Test
{
protected:
    SavedEnvironment prefixEnv{"GTLAB_TEST_PYTHON_PREFIX"};
    SavedEnvironment failEnv{"GTLAB_TEST_PREFIX_FAIL"};
    SavedEnvironment homeEnv{"PYTHONHOME"};
    SavedEnvironment pathEnv{"PYTHONPATH"};
    SavedEnvironment realPythonEnv{"GTLAB_TEST_REAL_PYTHON"};
    SavedEnvironment encodingEnv{"PYTHONIOENCODING"};
    QTemporaryDir temporary;
    QString prefix;

    void SetUp() override
    {
        ASSERT_TRUE(temporary.isValid());
        prefix = temporary.path() + "/environment with spaces";
        ASSERT_TRUE(QDir().mkpath(prefix + "/bin"));
        qputenv("GTLAB_TEST_PYTHON_PREFIX", prefix.toUtf8());
        qunsetenv("GTLAB_TEST_PREFIX_FAIL");
        qunsetenv("GTLAB_TEST_REAL_PYTHON");
    }

    QString executable(bool inBin = true)
    {
        const auto path = prefix + (inBin ? "/bin/python.exe" : "/python.exe");
        if (!QFile::copy(QString::fromUtf8(TEST_INTERPRETER_FIXTURE), path)) return {};
        return path;
    }
};

TEST_F(PythonHome, BinExecutableUsesReportedPrefix)
{
    GtpsPythonInterpreter interpreter(executable());
    EXPECT_TRUE(interpreter.isValid());
    EXPECT_EQ(interpreter.pythonHomePath(), QDir::toNativeSeparators(prefix));
}

TEST_F(PythonHome, RootExecutableUsesReportedPrefix)
{
    GtpsPythonInterpreter interpreter(executable(false));
    EXPECT_TRUE(interpreter.isValid());
    EXPECT_EQ(interpreter.pythonHomePath(), QDir::toNativeSeparators(prefix));
}

TEST_F(PythonHome, PrefixNeedNotContainExecutable)
{
    const auto reported = temporary.path() + "/another environment";
    ASSERT_TRUE(QDir().mkpath(reported));
    qputenv("GTLAB_TEST_PYTHON_PREFIX", reported.toUtf8());
    GtpsPythonInterpreter interpreter(executable(false));
    EXPECT_EQ(interpreter.pythonHomePath(), QDir::toNativeSeparators(reported));
}

TEST_F(PythonHome, RejectsEmptyRelativeAndMissingPrefixes)
{
    const auto exe = executable();
    for (const auto& invalid : {QString{}, QString{"relative"}, prefix + "/missing"})
    {
        qputenv("GTLAB_TEST_PYTHON_PREFIX", invalid.toUtf8());
        GtpsPythonInterpreter interpreter(exe);
        EXPECT_EQ(interpreter.status(), GtpsPythonInterpreter::Invalid);
        EXPECT_TRUE(interpreter.pythonHomePath().isEmpty());
    }
}

TEST_F(PythonHome, FailedQueryClearsPreviouslyValidHome)
{
    GtpsPythonInterpreter interpreter(executable());
    ASSERT_TRUE(interpreter.isValid());
    qputenv("GTLAB_TEST_PREFIX_FAIL", "1");
    EXPECT_FALSE(interpreter.isValid());
    EXPECT_EQ(interpreter.status(), GtpsPythonInterpreter::Invalid);
    EXPECT_TRUE(interpreter.pythonHomePath().isEmpty());
}

TEST_F(PythonHome, MissingExecutableHasNoHome)
{
    GtpsPythonInterpreter interpreter(prefix + "/does-not-exist");
    EXPECT_EQ(interpreter.status(), GtpsPythonInterpreter::Invalid);
    EXPECT_TRUE(interpreter.pythonHomePath().isEmpty());
}

TEST_F(PythonHome, QueryRestoresCallerPythonEnvironment)
{
    qputenv("PYTHONHOME", "caller home");
    qputenv("PYTHONPATH", "caller path");
    GtpsPythonInterpreter interpreter(executable());
    EXPECT_EQ(qgetenv("PYTHONHOME"), QByteArray{"caller home"});
    EXPECT_EQ(qgetenv("PYTHONPATH"), QByteArray{"caller path"});
}

TEST_F(PythonHome, SelectedRealPythonUsesReportedPrefix)
{
    qunsetenv("PYTHONHOME");
    qunsetenv("PYTHONPATH");
    QProcess python;
    python.start(QString::fromUtf8(TEST_SELECTED_PYTHON),
                 {"-c", "import sys; sys.stdout.buffer.write(sys.prefix.encode('utf-8'))"});
    ASSERT_TRUE(python.waitForFinished());
    ASSERT_EQ(python.exitCode(), 0);
    const auto expected = QString::fromUtf8(python.readAllStandardOutput());
    ASSERT_FALSE(expected.isEmpty());
    GtpsPythonInterpreter interpreter(QString::fromUtf8(TEST_SELECTED_PYTHON));
    EXPECT_TRUE(interpreter.isValid());
    EXPECT_EQ(interpreter.pythonHomePath(), QDir::toNativeSeparators(expected));
}

TEST_F(PythonHome, UnicodePrefixDoesNotDependOnStdoutEncoding)
{
    const auto reported = temporary.path() + QString::fromUtf8(u8"/M\u00fcller environment");
    ASSERT_TRUE(QDir().mkpath(reported));
    qputenv("GTLAB_TEST_PYTHON_PREFIX", reported.toUtf8());
    qputenv("GTLAB_TEST_REAL_PYTHON", QByteArray{TEST_SELECTED_PYTHON});
    qputenv("PYTHONIOENCODING", "cp1252");
    GtpsPythonInterpreter interpreter(executable());
    EXPECT_TRUE(interpreter.isValid());
    EXPECT_EQ(interpreter.pythonHomePath(), QDir::toNativeSeparators(reported));
}
} // namespace
