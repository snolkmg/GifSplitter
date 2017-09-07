#include "splitWorker.h"

splitWorker::splitWorker(QString gif_file, QString out_file, QString format_str, QObject *parent) :
    QObject(parent),
    m_gif_file(gif_file),
    m_out_file(out_file),
    m_format_str(format_str)
{
}

void splitWorker::gifSplit()
{
    QString outFile = m_out_file;
    QDir outDir(outFile);
    if(outFile.isEmpty() || !outDir.exists()) {
        QString tempFile = m_gif_file;
        tempFile = tempFile.mid(0, tempFile.lastIndexOf("."));
        QDir dir(tempFile);
        if(!dir.exists()) {
            QString former = tempFile;
            former = former.mid(0, former.lastIndexOf("/"));
            QDir formerDir(former);
            formerDir.mkpath(tempFile);
        }
        getImage(tempFile);
    } else
        getImage(outFile);
}

void splitWorker::getImage(const QString &gifDir)
{
    QString gifFile = m_gif_file;
    QFileInfo info(gifFile);
    QString name = info.completeBaseName();

    QString formatStr = m_format_str;
    QByteArray ba = formatStr.toLatin1();
    const char *format = ba.data();

    QMovie *movie = new QMovie(gifFile);;
    int count = movie->frameCount();
    QString cStr = QString::number(count);
    int size = cStr.size();
    QString msg;
    for(int i = 0; i < count; i++) {
        movie->jumpToFrame(i);
        QImage img = movie->currentImage();
        QString sort = QString("%1").arg(QString::number(i + 1), size, QChar('0'));
        QString imgFile = QString("%1/%2-%3.%4").arg(gifDir).arg(name).arg(sort).arg(formatStr);
        if(QFile::exists(imgFile))
            QFile::remove(imgFile);
        img.save(imgFile, format);
        msg = QString("%1：第 %2/%3 帧").arg(info.fileName()).arg(QString::number(i + 1)).arg(cStr);
        emit sendMsg(gifDir ,msg);
    }
    emit sendMsg(gifDir ,"");
    emit finished();
}
