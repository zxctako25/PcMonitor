#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class CircularGauge;
class InfoCard;
class QTimer;

class SystemInfo;
class PerfMonitor;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onTick();

private:
    void buildUi();
    void applyTheme();

    Ui::MainWindow *ui;

    SystemInfo* m_sysInfo = nullptr;
    PerfMonitor* m_perfMonitor = nullptr;

    CircularGauge* m_cpuGauge = nullptr;
    CircularGauge* m_gpuGauge = nullptr;
    CircularGauge* m_ramGauge = nullptr;

    InfoCard* m_cpuCard = nullptr;
    InfoCard* m_gpuCard = nullptr;
    InfoCard* m_gpuVramCard = nullptr;
    InfoCard* m_ramCard = nullptr;
    InfoCard* m_mbCard = nullptr;
    InfoCard* m_osCard = nullptr;

    QTimer* m_timer = nullptr;
};

#endif
