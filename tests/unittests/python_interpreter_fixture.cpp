// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)

#include <QCoreApplication>
#include <QFile>
#include <QProcess>

// Subprocess protocol fixture: the production interpreter class still performs
// every query through QProcess, including error handling and path decoding.
int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    const auto args = app.arguments();
    QByteArray output;
    if (args.size() == 2) return 0; // Shared-library discovery script.
    if (args.size() != 3 || args[1] != "-c") return 2;
    const auto code = args[2];
    if (code.contains("sys.prefix"))
    {
        if (qgetenv("GTLAB_TEST_PREFIX_FAIL") == "1") return 1;
        output = qgetenv("GTLAB_TEST_PYTHON_PREFIX");
        const auto realPython = qEnvironmentVariable("GTLAB_TEST_REAL_PYTHON");
        if (!realPython.isEmpty())
        {
            // Exercise the exact production query with real Python stdout
            // encoding, substituting only its prefix for this protocol test.
            QProcess python;
            python.start(realPython, {"-c", "import sys; sys.prefix=sys.argv[1]; " + code,
                                     QString::fromUtf8(output)});
            if (!python.waitForFinished() || python.exitCode() != 0) return 1;
            output = python.readAllStandardOutput();
        }
    }
    else if (code.contains("sys.version_info")) output = "3.12.15";
    else if (code != "import sys" && !code.contains("sys.path")) return 2;
    QFile stream;
    if (!stream.open(stdout, QIODevice::WriteOnly)) return 3;
    return stream.write(output) == output.size() ? 0 : 4;
}
