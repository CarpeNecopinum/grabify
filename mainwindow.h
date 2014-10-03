#pragma once

#include <QMainWindow>
#include <QTimer>
#include <QFile>

#include "Song.h"

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
    void startRecording(const QString& songTitle, const QString &songArtist, const QString &songAlbum);

    Ui::MainWindow *ui;
    QTimer* timer;
    bool recording = false;
    bool automatic = false;
    QFile* output;
    QString tempFile;

    Song* currentSong;

public slots:
    void checkSong();
private slots:
    void on_checkBox_toggled(bool checked);
    void on_checkBox_2_toggled(bool checked);
};
