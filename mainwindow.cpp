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


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    currentSong(nullptr)
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

    //Search Music folder
    ui->lineEdit->setText(QStandardPaths::writableLocation(QStandardPaths::MusicLocation) + "/Grabify/%artist%-%song%.mp3");

    //Setup Timer looking for songs to grab
    timer = new QTimer();
    timer->setInterval(100);
    connect(timer, SIGNAL(timeout()), this, SLOT(checkSong()));

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
}

void MainWindow::startRecording(const QString &songTitle, const QString& songArtist)
{
    recording = true;

    currentSong = new Song();
    currentSong->record(songTitle, songArtist, ui->lineEdit->text());

    ui->statusBar->showMessage(QString("Recording..."));
}

void MainWindow::checkSong()
{
    timer->stop();

    QWebFrame *frame = ui->webView->page()->mainFrame();
    QVariant songTitle = frame->documentElement().evaluateJavaScript("window.frames['app-player'].document.getElementById('track-name').innerText");

    //Panic! We missed the ending of a song
    if (currentSong && (songTitle.toString() != currentSong->getTitle()))
        stopRecording();


    //Check, if a song is currently playing
    if (songTitle.toString() != "") {

        QVariant length = frame->documentElement().evaluateJavaScript("window.frames['app-player'].document.getElementById('track-length').innerText");
        QVariant progress = frame->documentElement().evaluateJavaScript("window.frames['app-player'].document.getElementById('track-current').innerText");

        //Song just started => begin recording
        if (progress.toString() == "0:00" && !recording)
        {
            QVariant songArtist = frame->documentElement().evaluateJavaScript("window.frames['app-player'].document.getElementById('track-artist').innerText");
            startRecording(songTitle.toString(), songArtist.toString());
        //Song is ending => finish recording
        } else if (progress.toString() == length.toString() && recording) {
            stopRecording();
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
