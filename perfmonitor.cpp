#include "perfmonitor.h"

#include <QDebug>
#include <QStringList>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#ifdef Q_OS_WIN
typedef int nvmlReturn_t;
typedef void* nvmlDevice_t;

struct nvmlUtilization_t
{
    unsigned int gpu;
    unsigned int memory;
};

struct nvmlMemory_t
{
    unsigned long long total;
    unsigned long long free;
    unsigned long long used;
};

typedef nvmlReturn_t (*nvmlInit_v2_t)();
typedef nvmlReturn_t (*nvmlShutdown_t)();
typedef nvmlReturn_t (*nvmlDeviceGetHandleByIndex_v2_t)(unsigned int, nvmlDevice_t*);
typedef nvmlReturn_t (*nvmlDeviceGetUtilizationRates_t)(nvmlDevice_t, nvmlUtilization_t*);
typedef nvmlReturn_t (*nvmlDeviceGetMemoryInfo_t)(nvmlDevice_t, nvmlMemory_t*);
typedef nvmlReturn_t (*nvmlDeviceGetTemperature_t)(nvmlDevice_t, unsigned int, unsigned int*);

#define NVML_SUCCESS 0
#define NVML_TEMPERATURE_GPU 0u
#endif

namespace {
#ifdef Q_OS_WIN
quint64 fileTimeToU64(const FILETIME& ft)
{
    ULARGE_INTEGER u;
    u.LowPart  = ft.dwLowDateTime;
    u.HighPart = ft.dwHighDateTime;
    return u.QuadPart;
}
#endif
}

PerfMonitor::PerfMonitor(QObject* parent) : QObject(parent)
{
    initNvml();
}

PerfMonitor::~PerfMonitor()
{
#ifdef Q_OS_WIN
    if (m_nvmlReady && m_nvmlLib)
    {
        auto fn = reinterpret_cast<nvmlShutdown_t>(GetProcAddress((HMODULE)m_nvmlLib, "nvmlShutdown"));
        if (fn)
            fn();
    }
    if (m_nvmlLib)
        FreeLibrary((HMODULE)m_nvmlLib);
#endif
}

bool PerfMonitor::initNvml()
{
#ifdef Q_OS_WIN
    HMODULE lib = nullptr;

    lib = LoadLibraryA("C:\\Program Files\\NVIDIA Corporation\\NVSMI\\nvml.dll");
    if (!lib)
        lib = LoadLibraryA("C:\\Windows\\System32\\nvml.dll");
    if (!lib)
        lib = LoadLibraryA("nvml.dll");

    if (!lib)
    {
        qDebug() << "NVML: library not found";
        return false;
    }
    m_nvmlLib = lib;

    char path[MAX_PATH] = {};
    GetModuleFileNameA(lib, path, MAX_PATH);
    qDebug() << "NVML loaded from:" << path;

    auto fnInit = reinterpret_cast<nvmlInit_v2_t>(GetProcAddress(lib, "nvmlInit_v2"));
    auto fnHandle = reinterpret_cast<nvmlDeviceGetHandleByIndex_v2_t>(
        GetProcAddress(lib, "nvmlDeviceGetHandleByIndex_v2"));

    m_fnUtil = (void*)GetProcAddress(lib, "nvmlDeviceGetUtilizationRates");
    m_fnMemInfo = (void*)GetProcAddress(lib, "nvmlDeviceGetMemoryInfo");
    m_fnTemp = (void*)GetProcAddress(lib, "nvmlDeviceGetTemperature");

    if (!fnInit || !fnHandle)
    {
        qDebug() << "NVML: missing core symbols";
        return false;
    }

    if (fnInit() != NVML_SUCCESS)
    {
        qDebug() << "NVML: init failed";
        return false;
    }

    nvmlDevice_t dev = nullptr;
    if (fnHandle(0, &dev) != NVML_SUCCESS)
    {
        qDebug() << "NVML: no device 0";
        return false;
    }

    m_nvmlDevice = dev;
    m_nvmlReady  = true;
    qDebug() << "NVML: ready";
    return true;
#else
    return false;
#endif
}

double PerfMonitor::sampleGpuLoadNvml()
{
#ifdef Q_OS_WIN
    if (!m_nvmlReady || !m_fnUtil)
        return 0.0;

    nvmlUtilization_t util{};
    auto fn = reinterpret_cast<nvmlDeviceGetUtilizationRates_t>(m_fnUtil);
    if (fn(m_nvmlDevice, &util) != NVML_SUCCESS)
    {
        return 0.0;
    }
    return qBound(0.0, (double)util.gpu, 100.0);
#else
    return 0.0;
#endif
}

double PerfMonitor::sampleGpuTempNvml()
{
#ifdef Q_OS_WIN
    if (!m_nvmlReady || !m_fnTemp)
        return -1.0;

    unsigned int t = 0;
    auto fn = reinterpret_cast<nvmlDeviceGetTemperature_t>(m_fnTemp);
    if (fn(m_nvmlDevice, NVML_TEMPERATURE_GPU, &t) != NVML_SUCCESS)
    {
        return -1.0;
    }
    return (double)t;
#else
    return -1.0;
#endif
}

void PerfMonitor::sampleGpuVramNvml(SystemSnapshot& s)
{
#ifdef Q_OS_WIN
    if (!m_nvmlReady || !m_fnMemInfo)
        return;

    nvmlMemory_t mem{};
    auto fn = reinterpret_cast<nvmlDeviceGetMemoryInfo_t>(m_fnMemInfo);
    if (fn(m_nvmlDevice, &mem) != NVML_SUCCESS)
        return;

    s.gpuMemUsedMb = mem.used / 1024.0 / 1024.0;
    s.gpuMemTotalMb = mem.total / 1024.0 / 1024.0;
#endif
}

double PerfMonitor::sampleCpuLoad()
{
#ifdef Q_OS_WIN
    FILETIME idleT, kernelT, userT;
    if (!GetSystemTimes(&idleT, &kernelT, &userT))
        return 0.0;

    const quint64 idle = fileTimeToU64(idleT);
    const quint64 kernel = fileTimeToU64(kernelT);
    const quint64 user = fileTimeToU64(userT);

    if (m_firstCpuSample)
    {
        m_prevIdle = idle;
        m_prevKernel = kernel;
        m_prevUser = user;
        m_firstCpuSample = false;
        return 0.0;
    }

    const quint64 idleDiff = idle - m_prevIdle;
    const quint64 kernelDiff = kernel - m_prevKernel;
    const quint64 userDiff = user - m_prevUser;
    const quint64 totalDiff = kernelDiff + userDiff;

    m_prevIdle = idle;
    m_prevKernel = kernel;
    m_prevUser = user;

    if (totalDiff == 0)
        return 0.0;
    const double load = 100.0 * (1.0 - double(idleDiff) / double(totalDiff));
    return qBound(0.0, load, 100.0);
#else
    return 0.0;
#endif
}

SystemSnapshot PerfMonitor::sample()
{
    SystemSnapshot s = m_static;

    s.cpuLoad  = sampleCpuLoad();
    s.gpuLoad  = sampleGpuLoadNvml();
    s.gpuTempC = sampleGpuTempNvml();
    sampleGpuVramNvml(s);

    s.cpuTempC = -1.0;

#ifdef Q_OS_WIN
    MEMORYSTATUSEX ms; ms.dwLength = sizeof(ms);
    if (GlobalMemoryStatusEx(&ms))
    {
        s.ramUsedGb = (ms.ullTotalPhys - ms.ullAvailPhys) / 1024.0 / 1024.0 / 1024.0;
        s.ramTotalGb = ms.ullTotalPhys / 1024.0 / 1024.0 / 1024.0;
        s.ramLoad = s.ramTotalGb > 0.0 ? 100.0 * s.ramUsedGb / s.ramTotalGb : 0.0;
    }
#endif

    return s;
}
