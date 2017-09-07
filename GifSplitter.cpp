#include "GifSplitter.h"

GifSplitter::GifSplitter(QWidget *parent)
    : QMainWindow(parent)
{
    setMainWidget();
}

GifSplitter::~GifSplitter()
{

}

void GifSplitter::setMainWidget()
{
    setFixedSize(550, 420);
    setWindowTitle(tr("gif图片分离工具"));
    mainWidget = new QWidget(this);
    setCentralWidget(mainWidget);
    gifLabel = new QLabel(tr("gif文件："), this);
    gifEdit = new QLineEdit(this);
    gifEdit->setReadOnly(true);
    selectGifBtn = new QPushButton(tr("选择"), this);
    QHBoxLayout *gifHLayout = new QHBoxLayout;
    gifHLayout->addWidget(gifLabel);
    gifHLayout->addWidget(gifEdit);
    gifHLayout->addWidget(selectGifBtn);
    showLabel = new QLabel(this);
    showLabel->setAlignment(Qt::AlignCenter);
    outFileLabel = new QLabel(tr("选择输出文件夹："), this);
    outFileEdit = new QLineEdit(this);
    outFileEdit->setPlaceholderText(tr("若未选择输出文件夹，则根据gif文件名自动创建文件夹"));
    selectOutBtn = new QPushButton(tr("选择"));
    outImgLabel = new QLabel(tr("输出图片格式："), this);
    imgCbB = new QComboBox(this);
    imgCbB->addItems(QStringList() << "png" << "jpg");
    splitBtn = new QPushButton(tr("分离"), this);
    splitBtn->setEnabled(false);
    QGridLayout *splitGLayout = new QGridLayout;
    splitGLayout->addWidget(outFileLabel, 0, 0);
    splitGLayout->addWidget(outFileEdit, 0, 1);
    splitGLayout->addWidget(selectOutBtn, 0, 2);
    splitGLayout->addWidget(outImgLabel, 1, 0);
    splitGLayout->addWidget(imgCbB, 1, 1, Qt::AlignLeft);
    splitGLayout->addWidget(splitBtn, 1, 2);
    QVBoxLayout *mainVLayout = new QVBoxLayout;
    mainVLayout->addLayout(gifHLayout);
    mainVLayout->addWidget(showLabel);
    mainVLayout->addLayout(splitGLayout);
    mainWidget->setLayout(mainVLayout);
    statusBar = new QStatusBar(this);
    statusBar->setStyleSheet(QString("color: blue"));
    setStatusBar(statusBar);
    setAcceptDrops(true);

    connect(selectGifBtn, &QPushButton::clicked, this, &GifSplitter::onSelectGif);
    connect(selectOutBtn, &QPushButton::clicked, this, &GifSplitter::onSelectOutFile);
    connect(splitBtn, &QPushButton::clicked, [this]{setSplitWorker();});
    connect(gifEdit, &QLineEdit::textChanged, [this](QString text){splitBtn->setEnabled(!text.isEmpty());});
}

void GifSplitter::onSelectGif()
{
    QString gifFile = QFileDialog::getOpenFileName(this,
                                                   tr("选择gif文件"),
                                                   lastPath,
                                                   tr("gif文件(*.gif)"));
    if(gifFile.isEmpty())
        return;
    startMovie(gifFile);
}

void GifSplitter::startMovie(const QString &gifFile)
{
    gifEdit->setText(gifFile);
    QMovie *movie = new QMovie(gifFile);
    movie->setCacheMode(QMovie::CacheAll);
    showLabel->setMovie(movie);
    QImage img(gifFile);
    QString size = QString("%1 × %2").arg(QString::number(img.width())).arg(QString::number(img.height()));
    QString frame = QString("%1帧").arg(QString::number(movie->frameCount()));

    //设置显示gif图片的最大宽高
    int tempWidth = img.width();
    int tempHeight = img.height();
    if(img.height() > 240) {
        tempHeight = 240;
        tempWidth = img.width() * 240 / img.height();
        if(tempWidth > 520) {
            tempHeight = 520 * 240 / tempWidth;
            tempWidth = 520;
        }
    }
    if(img.width() > 520) {
        tempWidth = 520;
        tempHeight = img.height() * 520 / img.width();
    }
    movie->setScaledSize(QSize(tempWidth ,tempHeight));

    QString msg = QString("像素：%1，帧数：%2").arg(size).arg(frame);
    statusBar->showMessage(msg);
    movie->start();
}

void GifSplitter::onSelectOutFile()
{
    QString outDir = QFileDialog::getExistingDirectory(this,
                                                       tr("选择输出文件夹"),
                                                       lastPath);
    if(outDir.isEmpty())
        return;
    outFileEdit->setText(outDir);
}

//将分离操作放入线程
void GifSplitter::setSplitWorker()
{
    QThread *thread = new QThread();
    splitWorker *worker = new splitWorker(gifEdit->text(), outFileEdit->text(), imgCbB->currentText());
    worker->moveToThread(thread);

    connect(thread, SIGNAL(started()), worker, SLOT(gifSplit()));

    connect(worker, SIGNAL(sendMsg(QString,QString)), this, SLOT(receiveMsg(QString,QString)));

    connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
    connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    // 启动线程
    thread->start();
}

void GifSplitter::receiveMsg(QString gifDir, QString msg)
{
    if(msg.isEmpty()) {
        int ret = QMessageBox::information(this,
                                           windowTitle(),
                                           tr("gif文件分离完毕"),
                                           tr("确定"),
                                           tr("打开文件夹"));
        if(ret == 1) {
            QProcess proc(this);
            QString tempDir = gifDir;
            tempDir.replace("/", "\\");
            proc.start("explorer", QStringList(tempDir));
            proc.waitForFinished();
        }
    }
    statusBar->showMessage(msg);
}

//快速拖放gif文件
void GifSplitter::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void GifSplitter::dropEvent(QDropEvent *event)
{
    const QMimeData *mime = event->mimeData();

    QList<QUrl> duplicates;

    foreach (QUrl url, mime->urls()) {
        if (duplicates.contains(url))
            continue;
        else
            duplicates << url;

        QString fileName(url.toLocalFile());
        QFileInfo fi(fileName);

        if (fi.exists() && fi.suffix().toLower() == "gif") {
            startMovie(fileName);
            return;
        }
    }
}
