#pragma once

#include <QString>
#include <QProcess>
#include <id3/tag.h>
#include <QDir>
#include <QFileInfo>
#include <QPixmap>
#include <QStandardPaths>

class Song
{

public:

    void setArtist(const QString& artist) { mArtist = artist; }
    void setAlbum(const QString& album) { mAlbum = album; }
    void setTitle(const QString& title) { mTitle = title; }

    void record(QString pattern)
    {
        mPattern = pattern;

        mTesting = false;

        mTempFile = QStandardPaths::writableLocation(QStandardPaths::TempLocation).append(QDir::separator()).append("grabifytemp_").append(QString::number(qrand())).append(".mp3");
        mRecording.start("avconv", QStringList{"-f", "pulse", "-i", "default", "-ab", "192k", mTempFile});
    }

    /// Checks, if the cover for the current Song still needs to be downloaded
    bool needsCover()
    {
        updateOutputfile();
        return (!(QFile(mFolder.absoluteFilePath("cover.jpg")).exists()));
    }

    void saveCover(const QImage& cover)
    {
        mFolder.mkpath(".");
        cover.save(mFolder.absoluteFilePath("cover.jpg"), "JPG");
    }

    void recordTest()
    {
        mTesting = true;
        mRecording.start("avconv", QStringList{"-f", "pulse", "-i", "default", "-f", "flac", "-"});
    }

    void stop()
    {
        mRecording.terminate();
        mRecording.waitForFinished(1000);

        // Don't try to tag stdout...
        if (mTesting) return;

        updateOutputfile();
        writeTags();

        // Create necessary folders
        mFolder.mkpath(".");

        //Move file to destination
        QFile(mTempFile).rename(mFilename);
    }

    const QString& getTitle() { return mTitle; }

protected:
    void writeTags()
    {
        // Edit tags for recorded file
        ID3_Tag tags(mTempFile.toStdString().c_str());

        // Add Artist
        ID3_Frame artistFrame(ID3FID_LEADARTIST);
        artistFrame.Field(ID3FN_TEXT).Set(mArtist.toStdString().c_str());
        tags.AddFrame(artistFrame);

        // Add Songtitle
        ID3_Frame titleFrame(ID3FID_TITLE);
        titleFrame.Field(ID3FN_TEXT).Set(mTitle.toStdString().c_str());
        tags.AddFrame(titleFrame);

        // Add Album, if known
        if (mAlbum != QString(""))
        {
            ID3_Frame albumFrame(ID3FID_ALBUM);
            albumFrame.Field(ID3FN_TEXT).Set(mAlbum.toStdString().c_str());
            tags.AddFrame(albumFrame);
        }

        // Write the new tags to the file
        tags.Update();
    }

    void updateOutputfile()
    {

        // Evaluate the Pattern for the filename
        mFilename = mPattern.replace(QString("%artist%"), mArtist)
                           .replace(QString("%song%"), mTitle)
                           .replace(QString("%album%"), mAlbum);

        mFolder = QFileInfo(mFilename).absoluteDir();
    }

    QString mTitle, mArtist, mFilename, mAlbum, mTempFile, mPattern;
    QProcess mRecording;
    QDir mFolder;
    bool mTesting;
};
