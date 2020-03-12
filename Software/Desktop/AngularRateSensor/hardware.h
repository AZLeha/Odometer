#ifndef Hardware_H
#define Hardware_H

#include <QObject>
#include <QSerialPort>
#include <QList>
#include "commandprocessor.h"
#include "model.h"

enum class Hardware_COMAND: quint8
{
    STOP_CMD = 0x80,
    RUN_CMD,
    DATA
};




class Hardware : public QObject
{
    Q_OBJECT
public:
    explicit Hardware(double wheelRadius = 0, double shaftRadius = 0,  QObject *parent = nullptr);
    ~Hardware();

    bool HardwareConnect(const QString &portName, const qint32 & baudRate);
    bool HardwareDisconnect(void);
    static QList<QSerialPortInfo> GetListPorts(void);

    void writeComand(Hardware_COMAND comand);
protected:
    void commnadRyader(CommandObject data);
    qint8 calculateGlide(double mainRadius, int mainRPM, double secondaryRadius, int secondaryRPM, qint8 ofset);

private:
    QSerialPort *m_port;
    CommandProcessor m_commandProcessor;
    double m_wheelRadius;
    double m_shaftRadius;
    qint8 m_ofset = 0;


signals:
    void si_ComandIsReady(Hardware_COMAND comand);
    void si_DataIsReady(MainModel data);


public slots:
    void sl_setOfset(qint8 ofset);

private slots:
    void sl_receivingData();


};

#endif // Hardware_H
