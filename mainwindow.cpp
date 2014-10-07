#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "iostream"

#include <QDir>
#include <QTimer>
#include <QWebElement>
#include <QWebFrame>
#include <QVariant>
#include <QStandardPaths>
#include <QProcess>
#include <QNetworkProxy>

#include "util/Configuration.hh"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    currentSong(nullptr)
{
    //Make sure spotify will run properly
    QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::LocalStorageEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::LocalStorageDatabaseEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::AcceleratedCompositingEnabled, false);

    //All Traffic send over privoxy to get rid of ads
    QNetworkProxy proxy;
    proxy.setType(QNetworkProxy::HttpProxy);
    proxy.setHostName("localhost");
    proxy.setPort(8118);
    QNetworkProxy::setApplicationProxy(proxy);

    //Go to Spotify
    ui->setupUi(this);
    ui->webView->settings()->enablePersistentStorage(QDir::homePath());
    ui->webView->setUrl(QUrl("http://play.spotify.com"));

    //Search Music folder
    ui->lineEdit->setText(Configuration::the().getOutputPattern());

    //Setup Timer looking for songs to grab
    checkInterval = new QTimer();
    checkInterval->setInterval(100);
    connect(checkInterval, SIGNAL(timeout()), this, SLOT(checkSong()));

    //Setup Timer to fetch Album and Cover (both aren't updated immediately, so we need some delay)
    albumDelay = new QTimer();
    albumDelay->setInterval(2500);
    albumDelay->setSingleShot(true);
    connect(albumDelay, SIGNAL(timeout()), this, SLOT(readAlbum()));

    setWindowTitle("Grabify");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::stopRecording()
{
    recording = false;

    ui->statusBar->showMessage(QString("Stopping"));

    if (currentSong)
    {
        currentSong->stop();
        delete currentSong;
        currentSong = nullptr;
    }

    ui->statusBar->showMessage(QString("Done"));
    setWindowTitle("Grabify");
}

void MainWindow::startRecording(const QString &songTitle, const QString& songArtist)
{
    recording = true;

    currentSong = new Song();
    currentSong->setTitle(songTitle);
    currentSong->setArtist(songArtist);
    currentSong->record(ui->lineEdit->text());

    ui->statusBar->showMessage(QString("Recording..."));
    setWindowTitle(QString("Grabify - Recording ") + songTitle + " by " + songArtist);
}

QWebFrame *MainWindow::findPlayerFrame(QWebFrame *root)
{
    for (QWebFrame *temp : root->childFrames())
        if (temp->frameName() == "app-player")
            return temp;
    return nullptr;
}

void MainWindow::readAlbum()
{
    if (!currentSong) return;

    QWebFrame *frame = ui->webView->page()->mainFrame();
    QWebFrame *playerFrame = findPlayerFrame(frame);
    QString albumName = frame->documentElement().evaluateJavaScript("window.frames[document.querySelector('#section-queue iframe').id].document.querySelector('.current .list-row-albums').innerText").toString();

    if (albumName != "")
    {
        currentSong->setAlbum(albumName);
        if (currentSong->needsCover())
        {
            QWebElement el = playerFrame->documentElement().findFirst(".sp-image-img");
            QImage image(el.geometry().width(), el.geometry().height(), QImage::Format_RGB32);
            QPainter painter(&image);
            el.render(&painter);
            painter.end();

            currentSong->saveCover(image);
        }
    }
}


void MainWindow::checkSong()
{
    checkInterval->stop();

    QWebFrame *playerFrame = findPlayerFrame(ui->webView->page()->mainFrame());

    // Only look for songs if the player is active
    if (playerFrame)
    {
        QVariant songTitle = playerFrame->documentElement().evaluateJavaScript("document.getElementById('track-name').innerText");

        //Panic! We missed the ending of a song
        if (currentSong && (songTitle.toString() != currentSong->getTitle()))
        {
            stopRecording();
            if (!automatic) return;
        }


        //Check, if a song is currently playing
        if (songTitle.toString() != "") {

            QVariant length = playerFrame->documentElement().evaluateJavaScript("document.getElementById('track-length').innerText");
            QVariant progress = playerFrame->documentElement().evaluateJavaScript("document.getElementById('track-current').innerText");

            //Song just started => begin recording
            if (progress.toString() == "0:00" && !recording)
            {
                QVariant songArtist = playerFrame->documentElement().evaluateJavaScript("document.getElementById('track-artist').innerText");
                QVariant songAlbum = playerFrame->documentElement().evaluateJavaScript("document.querySelector('#cover-art a').getAttribute('data-tooltip').replace(/ by .*?$/, '')");
                startRecording(songTitle.toString(), songArtist.toString());
                albumDelay->start();

            //Song is ending => finish recording
            } else if (progress.toString() == length.toString() && recording) {
                stopRecording();
            }
        }
    }

    //Keep timer running, if still recording or looking for songs
    if (recording || automatic) checkInterval->start();
}

void MainWindow::on_checkBox_toggled(bool checked) //"automatic record"
{

    automatic = checked;
    if (checked) checkInterval->start(); else if (!recording) checkInterval->stop();
}

void MainWindow::on_checkBox_2_toggled(bool checked) //"force record"
{
    static Song* testSong;

    if (checked)
    {
        testSong = new Song();
        testSong->recordTest();
    } else {
        testSong->stop();
        delete testSong;
    }
}
