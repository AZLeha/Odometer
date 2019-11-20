#ifndef COMMANDPROCESSOR_H
#define COMMANDPROCESSOR_H

#include <QObject>
#include <QByteArray>
#include <QQueue>

struct CommandObject
{
  uint8_t comdan;
  QByteArray data;
};

class CommandProcessor : public QObject
{
    Q_OBJECT
public:
    explicit CommandProcessor(QObject *parent = nullptr);

    bool FindeComand(const QByteArray &data);
    CommandObject GetLastComand();

private:
    QQueue<uint8_t> m_aCommandReaderBuffer;
    CommandObject m_lastCommand;
    uint32_t m_Splitter;

    enum {
        isSplitterFinde,
        isCommandFinde,
        isDataFinde
    } m_stusFindeComand = isSplitterFinde;
};

#endif // COMMANDPROCESSOR_H
