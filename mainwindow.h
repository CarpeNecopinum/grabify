#pragma once

#include <QMainWindow>
#include <QTimer>
#include <QFile>

#include "configwindow.h"
#include "Song.h"

class QWebFrame;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void stopRecording();
    void startRecording(const QString& songTitle, const QString &songArtist);
    QWebFrame* findPlayerFrame(QWebFrame* root);

    Ui::MainWindow *ui;
    QTimer *checkInterval, *albumDelay;
    bool recording = false;
    bool automatic = false;
    QFile* output;
    QString tempFile;

    Song* currentSong;

    ConfigWindow config;

public slots:
    void checkSong();
    void readAlbum();

private slots:
    void on_actionRecord_automatically_triggered(bool checked);
    void on_actionConfiguration_triggered();
};
