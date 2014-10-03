#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "iostream"

#include <QDir>
#include <QTimer>
#include <QWebElement>
#include <QWebFrame>
#include <QVariant>
#include <QAudioRecorder>
#include <QStandardPaths>
#include <QProcess>
#include <QNetworkProxy>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //Make sure spotify will run properly
    //QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
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

    //Setup Timer looking for songs to grab
    timer = new QTimer();
    timer->setInterval(100);
    connect(timer, SIGNAL(timeout()), this, SLOT(checkSong()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::checkSong()
{
    timer->stop();

    QWebFrame *frame = ui->webView->page()->mainFrame();
    QVariant songTitle = frame->documentElement().evaluateJavaScript("window.frames['app-player'].document.getElementById('track-name').innerText");

    //Check, if a song is currently playing
    if (songTitle.toString() != "") {

        QVariant songArtist = frame->documentElement().evaluateJavaScript("window.frames['app-player'].document.getElementById('track-artist').innerText");

        QVariant progress = frame->documentElement().evaluateJavaScript("window.frames['app-player'].document.getElementById('track-current').innerText");
        QVariant length = frame->documentElement().evaluateJavaScript("window.frames['app-player'].document.getElementById('track-length').innerText");

        //Song just started => begin recording
        if (progress.toString() == "0:00" && !recording)
        {
            recording = true;
            std::cout << "Start recording" << std::endl;

            /*
            QAudioEncoderSettings audioSettings;
            audioSettings.setCodec("audio/FLAC");
            audioSettings.setQuality(QMultimedia::HighQuality);


            recorder = new QAudioRecorder();
            recorder->setAudioSettings(audioSettings);
            recorder->setOutputLocation(QUrl(tempFile));
            recorder->record();
            */

            currentSong = new Song();
            currentSong->record(songTitle.toString(), songArtist.toString(), ui->lineEdit->text());

            ui->statusBar->showMessage(QString("Recording..."));

        //Song is ending => finish recording
        } else if (progress.toString() == length.toString() && recording) {
            recording = false;

            ui->statusBar->showMessage(QString("Stopping"));
            std::cout << "Stop recording and save "
                      << songTitle.toString().toStdString() << " by "
                      << songArtist.toString().toStdString() << std::endl;
            //recorder->stop();

            currentSong->stop();
            delete currentSong;

            ui->statusBar->showMessage(QString("Done"));
        }
    }

    //Keep timer running, if still recording or looking for songs
    if (recording || automatic) timer->start();
}

void MainWindow::on_checkBox_toggled(bool checked) //"automatic record"
{

    automatic = checked;
    if (checked) timer->start(); else if (!recording) timer->stop();
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
