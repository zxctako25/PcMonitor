#include "systeminfo.h"

#include <QProcess>
#include <QStringList>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

namespace
{

QString psQuery(const QString& command)
{
    QProcess p;
    p.setProgram("powershell");
    p.setArguments({
        "-NoProfile",
        "-NonInteractive",
        "-Command",
        command
    });
    p.start();
    if (!p.waitForStarted(2000))
        return {};

    if (!p.waitForFinished(5000))
    {
        p.kill();
        return {};
    }
    return QString::fromLocal8Bit(p.readAllStandardOutput()).trimmed();
}
}

SystemInfo::SystemInfo(QObject* parent) : QObject(parent) {}

void SystemInfo::load()
{
#ifdef Q_OS_WIN
    m_info.cpuName = psQuery(
        "(Get-CimInstance Win32_Processor).Name");

    m_info.gpuName = psQuery(
        "(Get-CimInstance Win32_VideoController | "
        "Select-Object -First 1).Name");

    m_info.osName = psQuery(
        "(Get-CimInstance Win32_OperatingSystem).Caption");

    m_info.mbName = psQuery(
        "(Get-CimInstance Win32_BaseBoard).Product");

    MEMORYSTATUSEX ms;
    ms.dwLength = sizeof(ms);
    if (GlobalMemoryStatusEx(&ms))
    {
        m_info.ramTotalGb = ms.ullTotalPhys / 1024.0 / 1024.0 / 1024.0;
        m_info.ramName = QString("%1 GB").arg(m_info.ramTotalGb, 0, 'f', 1);
    }
#endif
    if (m_info.cpuName.isEmpty())
        m_info.cpuName = "Unnamed CPU";
    if (m_info.gpuName.isEmpty())
        m_info.gpuName = "Unnamed GPU";
    if (m_info.osName.isEmpty())
        m_info.osName  = "Windows";
    if (m_info.mbName.isEmpty())
        m_info.mbName  = "Unnamed";
    if (m_info.ramName.isEmpty())
        m_info.ramName = "N/A";
}
