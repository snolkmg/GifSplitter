#ifndef GIFSPLITTER_H
#define GIFSPLITTER_H

#include <QMainWindow>
#include <QImage>
#include <QFileDialog>
#include <QLabel>
#include <QComboBox>
#include <QMovie>
#include <QLineEdit>
#include <QLayout>
#include <QLayoutItem>
#include <QPushButton>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStatusBar>
#include <QMessageBox>
#include <QProcess>
#include <QThread>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QDebug>
#include "splitWorker.h"

class GifSplitter : public QMainWindow
{
    Q_OBJECT

public:
    GifSplitter(QWidget *parent = 0);
    ~GifSplitter();

private slots:
    void onSelectGif();
    void onSelectOutFile();
    void receiveMsg(QString gifDir, QString msg);

private:
    void setMainWidget();
    QWidget *mainWidget;
    QLabel *gifLabel;
    QLabel *showLabel;
    QLabel *outFileLabel;
    QLabel *outImgLabel;
    QLineEdit *gifEdit;
    QLineEdit *outFileEdit;
    QComboBox *imgCbB;
    QPushButton *selectGifBtn;
    QPushButton *selectOutBtn;
    QPushButton *splitBtn;
    QStatusBar *statusBar;
    QString lastPath;
    void startMovie(const QString &gifFile);

    void setSplitWorker();

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
};

#endif // GIFSPLITTER_H
