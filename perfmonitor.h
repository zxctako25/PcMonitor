#pragma once
#include <QObject>
#include <QString>
#include "snapshot.h"

class PerfMonitor : public QObject
{
    Q_OBJECT

public:
    explicit PerfMonitor(QObject* parent = nullptr);
    ~PerfMonitor() override;

    void setStaticInfo(const SystemSnapshot& info)
    {
        m_static = info;
    }
    SystemSnapshot sample();

private:
    double sampleCpuLoad();
    double sampleGpuLoadNvml();
    double sampleGpuTempNvml();
    void sampleGpuVramNvml(SystemSnapshot& s);
    bool initNvml();

    SystemSnapshot m_static;

    quint64 m_prevIdle = 0;
    quint64 m_prevKernel = 0;
    quint64 m_prevUser = 0;
    bool m_firstCpuSample = true;

    void* m_nvmlLib = nullptr;
    void* m_nvmlDevice = nullptr;
    bool  m_nvmlReady = false;

    void* m_fnUtil = nullptr;
    void* m_fnMemInfo = nullptr;
    void* m_fnTemp = nullptr;
};
