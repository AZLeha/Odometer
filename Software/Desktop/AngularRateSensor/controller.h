
#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QSerialPort>
#include <QJsonArray>
#include <commandprocessor.h>
#include <QFile>
#include <QTextStream>


//#define DEBUG

#ifdef DEBUG
#include <QTimer>
#endif


class Controller : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int leftRPM READ leftRPM WRITE setLeftRPM NOTIFY leftRPMChanged)
    Q_PROPERTY(int rightRPM READ rightRPM WRITE setRightRPM NOTIFY rightRPMChanged)

    Q_PROPERTY(int leftDyno READ leftDyno WRITE setLeftDyno NOTIFY leftDynoChanged)
    Q_PROPERTY(int rightDyno READ rightDyno WRITE setRightDyno NOTIFY rightDynoChanged)

    Q_PROPERTY(double diferentRPM READ diferentRPM WRITE setDiferentRPM NOTIFY diferentRPMChanged)
    Q_PROPERTY(int offSetDiferentRPM READ offSetDiferentRPM WRITE setOffSetDiferentRPM NOTIFY offSetDiferentRPMChanged)

    Q_PROPERTY(bool isRun READ isRun WRITE setIsRun NOTIFY isRunChanged)

    Q_PROPERTY(float wheelRadius READ wheelRadius WRITE setWheelRadius NOTIFY wheelRadiusChanged)
    Q_PROPERTY(float shaftRadius READ shaftRadius WRITE setShaftRadius NOTIFY shaftRadiusChanged)
public:
    explicit Controller(QObject *parent = nullptr);
    ~Controller();

    Q_INVOKABLE QJsonArray getListPorts();
    Q_INVOKABLE bool connectToPort(const QString &portName, const qint32 & baudRate = 115200, const QString filePath = "./");
    Q_INVOKABLE bool disconnectPort();
    Q_INVOKABLE double windowFilter(double data);


    int leftRPM() const;
    int rightRPM() const;

    int leftDyno() const;
    int rightDyno() const;

    double diferentRPM() const;
    int offSetDiferentRPM() const;

    bool isRun() const;

    float wheelRadius() const;
    float shaftRadius() const;

private:
    struct OdometerData{
        int16_t leftRPM;
        int16_t rightRPM;

        int32_t leftDyno;
        int32_t rightDyno;
        int diferentRPM;
    };


    QSerialPort *m_port;
    CommandProcessor m_commandProcessor;

    QFile m_file;
    QTextStream m_stream;
    QString m_folderPath;


    int m_leftDyno {0};
    int m_rightDyno {0};

    int m_leftRPM {0};
    int m_rightRPM {0};

    double m_diferentRPM {0};
    int m_offSetDiferentRPM {0};

    bool m_isRun;

    float m_wheelRadius;
    float m_shaftRadius;

protected:
    void commnadRyader(CommandObject data);
    void dataHandler(const OdometerData &data);
    int calculateGlide(double mainRadius, int mainRPM, double secondaryRadius, int secondaryRPM, int ofset);
public slots:
    void setLeftRPM(int leftRPM);
    void setRightRPM(int rightRPM);
    void setDiferentRPM(double diferentRPM);
    void setIsRun(bool isRun);
    void setLeftDyno(int leftDyno);
    void setRightDyno(int rightDyno);
    void setOffSetDiferentRPM(int offSetDiferentRPM);
    void setWheelRadius(double wheelRadius);
    void setShaftRadius(double shaftRadius);

private slots:
    void receivingData();


signals:
    void leftRPMChanged(int leftRPM);
    void rightRPMChanged(int rightRPM);
    void diferentRPMChanged(int diferentRPM);
    void isRunChanged(bool isRun);
    void leftDynoChanged(int leftDyno);
    void rightDynoChanged(int rightDyno);
    void offSetDiferentRPMChanged(int offSetDiferentRPM);
    void wheelRadiusChanged(double wheelRadius);
    void shaftRadiusChanged(double shaftRadius);

#ifdef DEBUG
private:
    QTimer *timer;
public slots:
    void debugSequence();
#endif


};

#endif // CONTROLLER_H
