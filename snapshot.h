#pragma once
#include <QString>

struct SystemSnapshot
{
    double cpuLoad = 0.0;
    double gpuLoad = 0.0;
    double ramLoad = 0.0;

    double ramUsedGb = 0.0;
    double ramTotalGb = 0.0;

    double cpuTempC = -1.0;
    double gpuTempC = -1.0;
    double gpuMemUsedMb = 0.0;
    double gpuMemTotalMb = 0.0;

    QString cpuName;
    QString gpuName;
    QString ramName;
    QString mbName;
    QString osName;
};
