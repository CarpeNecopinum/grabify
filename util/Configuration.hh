#pragma once

#include "Singleton.h"

#include <QString>
#include <QDir>
#include <QStandardPaths>

class Configuration : public Singleton<Configuration>
{
public:
    Configuration()
    {
        const QString s = QDir::separator();

        mOutputPattern = QStandardPaths::writableLocation(QStandardPaths::MusicLocation)
                        + s + "Grabify"
                        + s + "%artist%"
                        + s + "%album%"
                        + s + "%song%.mp3";

        mTempPattern = QStandardPaths::writableLocation(QStandardPaths::TempLocation)
                       + s + "grabifytemp_%num%.mp3";
    }

    const QString& getOutputPattern() { return mOutputPattern; }
    const QString& getTempPattern()   { return mTempPattern;   }

    const QString& makeTempName() { return mTempPattern.replace("%num%", QString::number(qrand())); }

protected:
    QString mOutputPattern;
    QString mTempPattern;
}
