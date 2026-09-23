#pragma once
#include <QObject>
#include "snapshot.h"

class SystemInfo : public QObject
{
    Q_OBJECT

public:
    explicit SystemInfo(QObject* parent = nullptr);

    void load();

    const SystemSnapshot& staticInfo() const
    {
        return m_info;
    }
private:
    SystemSnapshot m_info;
};
