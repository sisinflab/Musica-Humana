#include "dialogsettings.h"
#include "ui_dialogsettings.h"

DialogSettings::DialogSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSettings)
{
    ui->setupUi(this);
    //apro il file delle impostazioni
    QSettings settings("sisinflab", "MusicaHumana");

    //leggo dal file delle impostazioni il valore salvato in precedenza e aggiorno la GUI
    setMidiPath(settings.value("midiPath").toString());
    ui->lineEditPercorsoMidi->setText(getMidiPath().toLocal8Bit().data());
    if(settings.value("nonChiedereMidiPath")==true){
        ui->checkBoxUsaDefault->setChecked(true);
    }

}

DialogSettings::~DialogSettings()
{
    delete ui;
}
QString DialogSettings::getMidiPath() const
{
    return midiPath;
}

void DialogSettings::setMidiPath(const QString &value)
{
    midiPath = value;
}

bool DialogSettings::getCheckBoxUsaDefault()
{
    if (ui->checkBoxUsaDefault->isChecked()){
        return true;
    } else {
        return false;
    }
}

void DialogSettings::on_pushButtonSelezionaPercorsoMidi_clicked()
{
    //l'utente sceglie percorso e nome file
    QString percorsoENome = QFileDialog::getSaveFileName(this);

    //se l'utente ha selezionato OK vado avanti
    if (percorsoENome!=NULL){
        //se il file non finisce con .mid o .midi aggiungo l'estensione
        if( ! percorsoENome.endsWith(".mid") && ! percorsoENome.endsWith(".midi")){
            percorsoENome += ".mid";
        }

        //salvo il path e aggiorno la GUI
        setMidiPath(percorsoENome);
        ui->lineEditPercorsoMidi->setText(percorsoENome);
    }
}
