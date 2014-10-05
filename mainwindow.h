#pragma once

#include <QMainWindow>
#include <QTimer>
#include <QFile>

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

public slots:
    void checkSong();
    void readAlbum();

private slots:
    void on_checkBox_toggled(bool checked);
    void on_checkBox_2_toggled(bool checked);
};
