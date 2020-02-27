#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QSerialPort>
#include <QJsonArray>
#include <commandprocessor.h>
#include <QFile>
#include <QTextStream>

class Controller : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int leftRPM READ leftRPM WRITE setLeftRPM NOTIFY leftRPMChanged)
    Q_PROPERTY(int rightRPM READ rightRPM WRITE setRightRPM NOTIFY rightRPMChanged)
    Q_PROPERTY(int leftDyno READ leftDyno WRITE setLeftDyno NOTIFY leftDynoChanged)
    Q_PROPERTY(int rightDyno READ rightDyno WRITE setRightDyno NOTIFY rightDynoChanged)
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


    Q_INVOKABLE bool connectToPort(const QString &portName, const qint32 & baudRate = 115200, const QString filePath = "./");
    bool disconnectPort();


    int leftDyno() const
    {
        return m_leftDyno;
    }

    int rightDyno() const
    {
        return m_rightDyno;
    }

private:
    QSerialPort *m_port;
    CommandProcessor m_commandProcessor;

    QFile m_file;
    QTextStream m_stream;
    QString m_folderPath;

    struct OdometerData{
        int16_t leftRPM;
        int16_t rightRPM;

        int32_t leftDyno;
        int32_t rightDyno;
    };
    int m_leftDyno;

    int m_rightDyno;

protected:
    void commnadRyader(CommandObject data);
    void dataHandler(const OdometerData &data);

public slots:
    void setLeftRPM(int leftRPM);
    void setRightRPM(int rightRPM);
    void setDiferentRPM(int diferentRPM);
    void setIsRun(bool isRun);

    void setLeftDyno(int leftDyno)
    {
        if (m_leftDyno == leftDyno)
            return;

        m_leftDyno = leftDyno;
        emit leftDynoChanged(m_leftDyno);
    }

    void setRightDyno(int rightDyno)
    {
        if (m_rightDyno == rightDyno)
            return;

        m_rightDyno = rightDyno;
        emit rightDynoChanged(m_rightDyno);
    }

private slots:
    void receivingData();


signals:
    void leftRPMChanged(int leftRPM);
    void rightRPMChanged(int rightRPM);
    void diferentRPMChanged(int diferentRPM);
    void isRunChanged(bool isRun);
    void leftDynoChanged(int leftDyno);
    void rightDynoChanged(int rightDyno);
};

#endif // CONTROLLER_H
