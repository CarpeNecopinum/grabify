#pragma once

#include <QString>
#include <QProcess>
#include <id3/tag.h>

class Song
{

public:
    void record(QString title, QString artist, QString pattern)
    {
        mTesting = false;

        mTitle = title;
        mArtist = artist;
        mFilename = pattern.replace(QString("%artist%"), artist).replace(QString("%song%"), title);
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

        // Write the new tags to the file
        tags.Update();
    }

protected:
    QString mTitle, mArtist, mFilename;
    QProcess mRecording;
    bool mTesting;
};
