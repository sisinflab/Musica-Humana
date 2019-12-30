#ifndef DIALOGSETTINGS_H
#define DIALOGSETTINGS_H

#include <QDialog>
#include <QSettings>
#include <QFileDialog>

namespace Ui {
class DialogSettings;
}

class DialogSettings : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSettings(QWidget *parent = 0);
    ~DialogSettings();

    QString getMidiPath() const;
    void setMidiPath(const QString &value);

    bool getCheckBoxUsaDefault();

private slots:
    void on_pushButtonSelezionaPercorsoMidi_clicked();

private:
    Ui::DialogSettings *ui;

    QString midiPath;
};

#endif // DIALOGSETTINGS_H
