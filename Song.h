#pragma once

#include <QString>
#include <QProcess>

class Song
{

public:
    void record(QString title, QString artist, QString pattern)
    {
        mTitle = title;
        mArtist = artist;
        mFilename = pattern.replace(QString("%artist%"), artist).replace(QString("%song%"), title);
        recording.start("avconv", QStringList{"-f", "pulse", "-i", "default", "-ab", "192k", mFilename});
    }

    void recordTest()
    {
        recording.start("avconv", QStringList{"-f", "pulse", "-i", "default", "-f", "flac", "-"});
    }

    void stop()
    {
        recording.terminate();
        recording.waitForFinished(1000);
    }

protected:
    QString mTitle, mArtist, mFilename;
    QProcess recording;

};
