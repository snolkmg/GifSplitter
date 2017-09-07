#ifndef SPLITWORKER_H
#define SPLITWORKER_H

#include <QObject>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMovie>
#include <QTimer>

class splitWorker : public QObject
{
    Q_OBJECT
public:
    explicit splitWorker(QString gif_file, QString out_file, QString format_str, QObject *parent = 0);

signals:
    void finished();
    void sendMsg(QString gifDir, QString msg);

public slots:
    void gifSplit();

private:
    QString m_gif_file;
    QString m_out_file;
    QString m_format_str;
    void getImage(const QString &gifDir);
};

#endif // SPLITWORKER_H
