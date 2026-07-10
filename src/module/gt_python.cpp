/* GTlab - Gas Turbine laboratory
 * Source File:
 * 
 * SPDX-License-Identifier: Apache-2.0
 * SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)
 *
 * Created on: 11.08.2015
 * Author: Stanislaus Reitenbach (DLR AT-TWK)
 */

#include <QApplication>
#include <QTabWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDir>

#include "gt_taskdata.h"
#include "gt_calculatordata.h"
#include "gt_application.h"
#include "gt_logging.h"

#if GT_VERSION >= 0x020000
#include "gt_functionalinterface.h"
#include "gt_commandlinefunction.h"
#endif

#include "gtpy_globals.h"
#include "gtpy_icons_compat.h"

// data model classes

// calculator classes
#include "gtpy_scriptcalculator.h"

// task classes
#include "gtpy_task.h"

// ui classes
#include "gtpy_console.h"

// mdi items
#include "gt_extendedcalculatordata.h"
#include "gt_extendedtaskdata.h"
#include "gt_customprocesswizard.h"

// wizards
#include "gtpy_scriptcalculatorwizardpage.h"
#include "python_task/gtpy_taskwizardpage.h"

// post
#include "gtpy_pythonplotitem.h"
#include "gtpy_pythonplotdata.h"

// network
#include "gt_accessdataconnection.h"

#include "gtpy_scriptcollectionsettings.h"
#include "gtpy_moduleupgrader.h"

#include "gt_python.h"


#if GT_VERSION >= 0x010700
GtVersionNumber
GtPythonModule::version()
{
    return GtVersionNumber(1, 8, 2);
}
#else
int
GtPythonModule::version()
{
    return 1;
}
#endif

QString
GtPythonModule::description() const
{
    return QStringLiteral("GTlab Python Module");
}

#if GT_VERSION >= 0x020000
GtPythonModule::MetaInformation
GtPythonModule::metaInformation() const
{
    MetaInformation m;
    m.author = "AT-TWK";
    return m;
}
#endif

void
GtPythonModule::init()
{
    GtpyContextManager::instance()->initContexts();

#if GT_VERSION >= 0x020000
    if (gtApp->batchMode())
    {
        return;
    }
#endif

    QStringList widPath;
    widPath << "GtMainWin" << "Output" << "centralWidget" << "tabWidget";

    QWidget* outDock = findWidget(widPath);

    QTabWidget* tab = qobject_cast<QTabWidget*>(outDock);

    if (!tab)
    {
        gtWarning() << "Output dock tab widget not found!";
        return;
    }

    // python console
    QWidget* pythonConsoleTab = new QWidget(tab);
    tab->addTab(pythonConsoleTab, "Python Console");
    QVBoxLayout* pythonConsoleLayout = new QVBoxLayout;
    pythonConsoleLayout->setContentsMargins(0, 0, 0, 0);
    pythonConsoleLayout->setSpacing(0);
    pythonConsoleTab->setLayout(pythonConsoleLayout);

    GtpyConsole* pythonConsole =
        new GtpyConsole(GtpyContextManager::GlobalContext,
                        pythonConsoleTab);

    pythonConsole->showAdditionalContextOutput(
        GtpyContextManager::CalculatorRunContext);
    pythonConsole->showAdditionalContextOutput(
        GtpyContextManager::TaskRunContext);

    connect(GtpyContextManager::instance(), SIGNAL(newContextCreated(int)),
            pythonConsole, SLOT(showAdditionalContextOutput(int)));
    connect(GtpyContextManager::instance(), SIGNAL(contextDeleted(int)),
            pythonConsole, SLOT(removeAdditionalContextOutput(int)));

    pythonConsoleLayout->addWidget(pythonConsole);

    // python console tools layout
    QHBoxLayout* pyToolLayout = new QHBoxLayout;
    pyToolLayout->setContentsMargins(0, 0, 0, 0);
    pyToolLayout->setSpacing(0);
    pyToolLayout->addStretch(1);

    // clear python console button
    QPushButton* pyClearButton = new QPushButton;
    pyClearButton->setIcon(GTPY_ICON(clear));
    pyClearButton->setMaximumSize(QSize(20, 20));
    pyClearButton->setFlat(true);
    pyClearButton->setToolTip(tr("Clear Output"));
    pyToolLayout->addWidget(pyClearButton);

    pythonConsoleLayout->addLayout(pyToolLayout);

    connect(pyClearButton, SIGNAL(clicked(bool)), pythonConsole,
            SLOT(clearConsole()));


    if (!GtpyContextManager::instance()->initMatplotlib())
    {
        gtError() << "Unable to register matplotlib backend. Is matplotlib installed?";
    }
}

QList<GtCalculatorData>
GtPythonModule::calculators()
{
    QList<GtCalculatorData> metaData;

    // Python Script Runner
    GtExtendedCalculatorData python = GT_EXTENDED_CALC_DATA(
                                          GtpyScriptCalculator)
    python->id = QStringLiteral("Python Script Calculator");

#if GT_VERSION >= 0x010700
    python->version = GtVersionNumber(0, 1);
#else
    python->version = 0.1;
#endif

    python->author = QStringLiteral("Marvin Nöthen");
    python->icon = GTPY_ICON(python);

    python->status = GtAbstractProcessData::RELEASE;

    GtCustomProcessWizard* pythonWizard = new GtCustomProcessWizard;
    pythonWizard->setPage(1, GtpyScriptCalculatorWizardPage::staticMetaObject);
    pythonWizard->setSize(1000, 750);
    python->wizard = pythonWizard;

    metaData << python;

    return metaData;
}

QList<GtTaskData>
GtPythonModule::tasks()
{
    QList<GtTaskData> metaData;

    GtExtendedTaskData pythonTask = GT_EXTENDED_TASK_DATA(GtpyTask)
    pythonTask->id = QStringLiteral("Python Task");

#if GT_VERSION >= 0x020000
    pythonTask->version = GtVersionNumber(0, 1);
#else
    pythonTask->version = 0.1;
#endif

    pythonTask->author = QStringLiteral("Marvin Nöthen");
    pythonTask->icon = GTPY_ICON(python);
    pythonTask->status = GtAbstractProcessData::RELEASE;

    GtCustomProcessWizard* pythonWizard = new GtCustomProcessWizard;
    pythonWizard->setPage(1, GtpyTaskWizardPage::staticMetaObject);
    pythonWizard->setSize(1000, 750);
    pythonTask->wizard = pythonWizard;

    metaData << pythonTask;

    return metaData;
}

QList<QMetaObject>
GtPythonModule::mdiItems()
{
    QList<QMetaObject> metaData;

    return metaData;
}

QList<QMetaObject>
GtPythonModule::dockWidgets()
{
    QList<QMetaObject> metaData;

    return metaData;
}

QMap<const char*, QMetaObject>
GtPythonModule::uiItems()
{
    return {};
}

QList<QMetaObject>
GtPythonModule::postItems()
{
    QList<QMetaObject> retval;

    retval.append(GT_METADATA(GtpyPythonPlotData));

    return retval;
}

QList<QMetaObject>
GtPythonModule::postPlots()
{
    QList<QMetaObject> retval;

    retval.append(GT_METADATA(GtpyPythonPlotItem));

    return retval;
}

QIcon
GtPythonModule::collectionIcon() const
{
    return GTPY_ICON(python);
}

QString
GtPythonModule::collectionId() const
{
    return gtpy::constants::COLLECTION_ID;
}

QMetaObject
GtPythonModule::collectionSettings() const
{
    return GtpyScriptCollectionSettings::staticMetaObject;
}

QMap<QString, QMetaType::Type>
GtPythonModule::collectionStructure() const
{
    QMap<QString, QMetaType::Type> retval;

    retval.insert(gtpy::constants::COLLECTION_CAT, QMetaType::QString);
    retval.insert(gtpy::constants::COLLECTION_SUBCAT, QMetaType::QString);

    return retval;
}

QString
GtPythonModule::accessId()
{
    return collectionId();
}

QMetaObject
GtPythonModule::accessConnection()
{
    return GT_METADATA(GtAccessDataConnection);
}

#if GT_VERSION >= GT_VERSION_CHECK(2, 0, 0)
QList<gt::SharedFunction> GtPythonModule::sharedFunctions() const
{
    auto result = QList<gt::SharedFunction>();

    // add some shared methods for testing
    if(gtApp->devMode())
    {
        result.append(gt::interface::makeSharedFunction("get_object_name", [](GtObject* o) ->QString {
            if (!o) return "";
            return o->objectName();
        }));

        result.append(gt::interface::makeSharedFunction("add", [](double a, double b) {
            return a+b;
        }));
    }

    return result;
}
#endif

QList<gt::VersionUpgradeRoutine>
GtPythonModule::upgradeRoutines() const
{
    return {
#if GT_VERSION >= GT_VERSION_CHECK(2, 1, 0)
                gt::VersionUpgradeRoutine {GtVersionNumber(1, 8, 2),
                                          &gtpy::module_upgrader::to_2_0_0::run}
#endif
    };
}


namespace PythonExecution
{

QString
parseScriptFile(QFile& file)
{
    if (!file.exists())
    {
        gtError() << "ERROR: script file not found!";
        return QString();
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        gtError() << "ERROR: could not open script file!";
        return QString();
    }

    QTextStream out(&file);

    return out.readAll();
}


int
runPythonInterpreter(const QStringList& args)
{
    if (args.isEmpty())
    {
        return -1;
    }

    QFile f(args.first());
    QString scriptContent = parseScriptFile(f);

    if (scriptContent.isEmpty())
    {
        gtError() << "ERROR: empty script file!";
        return -1;
    }

    gtInfo() << "Start Python Script Execution for file" << args.first();
    GtpyContextManager* python = GtpyContextManager::instance();
    assert(python);

    python->initContexts();

    // print errors to console
    QObject::connect(python,
                     &GtpyContextManager::errorMessage,
                     GtpyContextManager::instance(),
                     [](const QString& message, int contextId){
        std::cerr << message.toStdString();
    });

    bool success = python->evalScript(GtpyContextManager::BatchContext,
                                      scriptContent, true);
    return success ? 0 : -1;
}

} // namespace PythonExecution

#if GT_VERSION >= 0x020000
QList<GtCommandLineFunction>
GtPythonModule::commandLineFunctions() const
{
    QList<GtCommandLineArgument> args;
    args.append(GtCommandLineArgument{"<file>", "python file to execute"});
    auto fun = gt::makeCommandLineFunction(
                "python", PythonExecution::runPythonInterpreter,
                "Executes python").setArgs(args);

    return {fun};
}
#endif

QWidget*
GtPythonModule::findWidget(QStringList path, QWidget* parent)
{
    if (path.isEmpty())
    {
        return parent;
    }

    if (!parent)
    {
        // no parent given... get all top level widgets from application
        QWidgetList wids = QApplication::topLevelWidgets();
        QString firstId = path.takeFirst();

        foreach (QWidget* wid, wids)
        {
            if (wid->objectName() == firstId)
            {
                return findWidget(path, wid);
            }
        }

        gtWarning() << "Could not find target widget!";
        return nullptr;
    }

    QString widId = path.takeFirst();
    QWidget* child = parent->findChild<QWidget*>(widId,
                     Qt::FindDirectChildrenOnly);

    if (!child)
    {
        gtWarning() << "Could not find target widget!";
        return nullptr;
    }

    return findWidget(path, child);
}

