#pragma once

#include <QString>
#include <QProcess>
#include <id3/tag.h>
#include <QDir>
#include <QFileInfo>

class Song
{

public:

    void setArtist(const QString& artist) { mArtist = artist; }
    void setAlbum(const QString& album) { mAlbum = album; }
    void setTitle(const QString& title) { mTitle = title; }

    void record(QString pattern)
    {
        mTesting = false;
        mFilename = pattern.replace(QString("%artist%"), mArtist)
                           .replace(QString("%song%"), mTitle)
                           .replace(QString("%album%"), mAlbum);

        QFileInfo(mFilename).absoluteDir().mkpath(".");
        mRecording.start("avconv", QStringList{"-f", "pulse", "-i", "default", "-ab", "192k", mFilename});
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

        // Edit tags for recorded file
        ID3_Tag tags(mFilename.toStdString().c_str());

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

    const QString& getTitle() { return mTitle; }

protected:
    QString mTitle, mArtist, mFilename, mAlbum;
    QProcess mRecording;
    bool mTesting;
};
