#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QAudioRecorder>
#include <QAudioInput>
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
    Ui::MainWindow *ui;
    QTimer* timer;
    QAudioRecorder* recorder;
    QAudioInput* input;
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

#endif // MAINWINDOW_H
