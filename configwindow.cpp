#include "configwindow.h"
#include "ui_configwindow.h"

#include <QProcess>
#include <QDebug>

ConfigWindow::ConfigWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigWindow)
{
    ui->setupUi(this);
}

ConfigWindow::~ConfigWindow()
{
    delete ui;
}

void ConfigWindow::prepare()
{
    ui->leOutputPattern->setText(Configuration::the().get(Configuration::OUTPUT_PATTERN));

    soundSystem.clear();

    QProcess process;

    // Use pactl to get all PulseAudio-Sources, if installed
    process.start("pactl list sources short");
    process.waitForFinished();
    if (process.exitCode() == 0)
    {
        soundSystem.insert(QString("pulse"), QStringList());

        // lines output by pactl are like: "<index>\t<source>\t....", only source is interesting
        for (const QString& line : QString(process.readAll()).split('\n'))
        {
            QStringList items = line.split('\t');
            if (items.size() > 1)
                soundSystem["pulse"].append(items[1]);
        }
    }

    ui->cbSoundSystem->clear();
    for (const QString& name : soundSystem.keys())
    {
        ui->cbSoundSystem->addItem(name);
    }
}

void ConfigWindow::on_cbSoundSystem_currentIndexChanged(const QString &text)
{
    ui->cbSoundSource->clear();
    ui->cbSoundSource->addItems(soundSystem[text]);
}

void ConfigWindow::on_btApply_clicked()
{
    Configuration::the().set(Configuration::OUTPUT_PATTERN, ui->leOutputPattern->text())
            .set(Configuration::AUDIO_SYSTEM, ui->cbSoundSystem->currentText())
            .set(Configuration::AUDIO_SOURCE, ui->cbSoundSource->currentText());

    this->close();
}

void ConfigWindow::on_btCancel_clicked()
{
    this->close();
}
