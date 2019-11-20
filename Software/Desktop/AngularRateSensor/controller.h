#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QSerialPort>
#include <QJsonArray>
#include <commandprocessor.h>

class Controller : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int leftRPM READ leftRPM WRITE setLeftRPM NOTIFY leftRPMChanged)
    Q_PROPERTY(int rightRPM READ rightRPM WRITE setRightRPM NOTIFY rightRPMChanged)
    Q_PROPERTY(int diferentRPM READ diferentRPM WRITE setDiferentRPM NOTIFY diferentRPMChanged)
    Q_PROPERTY(bool isRun READ isRun WRITE setIsRun NOTIFY isRunChanged)

    //! \todo Определится с типоим и рамерностью данных
    // Q_PROPERTY(dooble leftDyno READ leftDyno WRITE setLeftDyno NOTIFY leftDynoChanged)
   // Q_PROPERTY(dooble rightDyno READ rightDyno WRITE setRightDyno NOTIFY rightDynoChanged

    int m_leftRPM;
    int m_rightRPM;
    int m_diferentRPM;
    bool m_isRun;


public:
    explicit Controller(QObject *parent = nullptr);
    ~Controller();
    int leftRPM() const;
    int rightRPM() const;
    int diferentRPM() const;
    bool isRun() const;

    Q_INVOKABLE QJsonArray getListPorts();


    Q_INVOKABLE bool connectToPort(const QString &portName, const qint32 & baudRate);
    bool disconnectPort();


private:
    QSerialPort *m_port;
    CommandProcessor m_commandProcessor;
public slots:
    void setLeftRPM(int leftRPM);
    void setRightRPM(int rightRPM);
    void setDiferentRPM(int diferentRPM);
    void setIsRun(bool isRun);

private slots:
    void receivingData();

signals:
    void leftRPMChanged(int leftRPM);
    void rightRPMChanged(int rightRPM);
    void diferentRPMChanged(int diferentRPM);
    void isRunChanged(bool isRun);
};

#endif // CONTROLLER_H
