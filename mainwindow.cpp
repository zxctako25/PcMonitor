#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "systeminfo.h"
#include "perfmonitor.h"
#include "snapshot.h"
#include "circulargauge.h"
#include "infocard.h"

#include <QTimer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_sysInfo = new SystemInfo(this);
    m_perfMonitor = new PerfMonitor(this);

    m_sysInfo->load();
    m_perfMonitor->setStaticInfo(m_sysInfo->staticInfo());

    buildUi();
    applyTheme();

    const auto& info = m_sysInfo->staticInfo();
    m_cpuCard->setValue(info.cpuName);
    m_gpuCard->setValue(info.gpuName);
    m_ramCard->setValue(info.ramName);
    m_mbCard ->setValue(info.mbName);
    m_osCard ->setValue(info.osName);

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &MainWindow::onTick);
    m_timer->start(1000);

    onTick();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::buildUi()
{
    setWindowTitle("Performance");
    resize(960, 620);

    auto* central = new QWidget(this);
    setCentralWidget(central);

    auto* root = new QVBoxLayout(central);
    root->setContentsMargins(20, 18, 20, 18);
    root->setSpacing(14);

    auto* title = new QLabel("Performance", central);
    title->setStyleSheet("color:#e6ecff; font-size:19px; font-weight:600;");
    root->addWidget(title);

    auto* gaugesRow = new QHBoxLayout;
    gaugesRow->setSpacing(10);

    m_cpuGauge = new CircularGauge(central);
    m_cpuGauge->setTitle("CPU");
    m_cpuGauge->setAccentColor(QColor(0, 200, 255));

    m_gpuGauge = new CircularGauge(central);
    m_gpuGauge->setTitle("GPU");
    m_gpuGauge->setAccentColor(QColor(180, 90, 255));

    m_ramGauge = new CircularGauge(central);
    m_ramGauge->setTitle("RAM");
    m_ramGauge->setAccentColor(QColor(90, 220, 140));

    gaugesRow->addWidget(m_cpuGauge);
    gaugesRow->addWidget(m_gpuGauge);
    gaugesRow->addWidget(m_ramGauge);
    root->addLayout(gaugesRow);

    auto* info = new QFrame(central);
    info->setObjectName("InfoBlock");
    auto* infoLay = new QVBoxLayout(info);
    infoLay->setContentsMargins(18, 10, 18, 10);
    infoLay->setSpacing(0);

    m_cpuCard = new InfoCard("CPU", info);
    m_gpuCard = new InfoCard("GPU", info);
    m_gpuVramCard = new InfoCard("VRAM", info);
    m_ramCard = new InfoCard("RAM", info);
    m_mbCard = new InfoCard("Motherboard", info);
    m_osCard = new InfoCard("OS", info);

    infoLay->addWidget(m_cpuCard);
    infoLay->addWidget(m_gpuCard);
    infoLay->addWidget(m_gpuVramCard);
    infoLay->addWidget(m_ramCard);
    infoLay->addWidget(m_mbCard);
    infoLay->addWidget(m_osCard);

    root->addWidget(info);
    root->addStretch();
}

void MainWindow::applyTheme()
{
    setStyleSheet(R"(
        QMainWindow, QWidget {
            background: #0a0a0a;
        }
        QFrame#InfoBlock {
            background: #141414;
            border-radius: 12px;
        }
    )");
}

void MainWindow::onTick()
{
    const SystemSnapshot s = m_perfMonitor->sample();

    m_cpuGauge->setValue(s.cpuLoad);
    if (s.cpuTempC > 0)
        m_cpuGauge->setTempText(QString("%1 °C").arg(s.cpuTempC, 0, 'f', 0));
    else
        m_cpuGauge->setTempText("");

    m_gpuGauge->setValue(s.gpuLoad);
    if (s.gpuTempC > 0)
        m_gpuGauge->setTempText(QString("%1 °C").arg(s.gpuTempC, 0, 'f', 0));
    else
        m_gpuGauge->setTempText("");

    m_ramGauge->setValue(s.ramLoad);
    m_ramGauge->setSubtitle(QString("%1 / %2 GB")
                                .arg(s.ramUsedGb, 0, 'f', 1)
                                .arg(s.ramTotalGb, 0, 'f', 1));

    if (s.gpuMemTotalMb > 0)
        m_gpuVramCard->setValue(QString("%1 / %2 MB")
                                    .arg(s.gpuMemUsedMb, 0, 'f', 0)
                                    .arg(s.gpuMemTotalMb, 0, 'f', 0));
    else
        m_gpuVramCard->setValue("N/A");
}
