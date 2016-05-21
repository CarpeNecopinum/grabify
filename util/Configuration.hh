#pragma once

#include "Singleton.h"

#include <QString>
#include <QDir>
#include <QStandardPaths>

#include <map>

class Configuration : public Singleton<Configuration>
{
public:
    enum Options
    {
        OUTPUT_PATTERN,
        TEMP_PATTERN,
        RECORD_COMMAND,
        AUDIO_SYSTEM,
        AUDIO_SOURCE,

        OPTIONS_COUNT
    };

    Configuration()
    {
        const QString s = QDir::separator();

        set(OUTPUT_PATTERN, QStandardPaths::writableLocation(QStandardPaths::MusicLocation)
                        + s + "Grabify"
                        + s + "%artist%"
                        + s + "%album%"
                        + s + "%song%.mp3");

        set(TEMP_PATTERN, QStandardPaths::writableLocation(QStandardPaths::TempLocation)
                       + s + "grabifytemp_%num%.mp3");
    }

    const QString& get(Options option) { return mOptions[option]; }
    Configuration& set(Options option, const QString& value) {
        mOptions[option] = value;
        return *this; // allows chained sets
    }

    QString makeTempName() { return QString(get(TEMP_PATTERN)).replace("%num%", QString::number(qrand())); }

protected:
    QString mOptions[OPTIONS_COUNT];
};
