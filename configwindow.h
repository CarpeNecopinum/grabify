#pragma once

#include <QDialog>
#include "util/Configuration.hh"
#include <QMap>

namespace Ui {
class ConfigWindow;
}

class ConfigWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigWindow(QWidget *parent = 0);

    virtual ~ConfigWindow();

    void prepare();

private slots:
    void on_cbSoundSystem_currentIndexChanged(const QString &text);

    void on_btApply_clicked();

    void on_btCancel_clicked();

private:
    Ui::ConfigWindow *ui;
    QMap<QString, QStringList> soundSystem;
};
