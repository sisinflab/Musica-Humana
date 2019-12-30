#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//7 indica che la nota di riferimento è il sol. in midi Do=0 e cresce di 1 ogni semitono
#define SOL  7

//Ottave di partenza per i vari intervalli
#define OTTAVA_FRONTE           0
#define OTTAVA_NASO             1
#define OTTAVA_BOCCA_NASO       2
#define OTTAVA_OCCHI            3
#define OTTAVA_BOCCA            4

//limite inferiore delle note dell'ottava centrale del pianoforte (per il superiore basta fare +11)
#define MIN_OTTAVA_CENTRALE     60

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>

#include "flandLibs/flandmark_detector.h"

#include "selezionaformavolto.h"
#include "CVImageWidget.h"
#include "dialogsettings.h"
#include "dialogusadefaultpath.h"

#include <iostream>
#include <fstream>
#include <cmath>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButtonCarica_clicked();

    void on_pushButtonScatta_clicked();

    //aggiorna l'immagine visualizzata in CVImageWidget con cv::Mat passata come primo parametro
    void visualizzaImmagine(cv::Mat, CVImageWidget*);

    void on_pushButtonCalcolaDistanze_clicked();

    // restituisce il colore HSV di riferimento
    //cv::Vec3b segmentazioneColore(cv::Mat, cv::Point);

    // restituisce la lunghezza dell'orecchio
//    float lunghezzaOrecchio(cv::Mat, cv::Vec3b);

    //restituisce l'angolo fra la retta individuata dai primi due punti e la retta individuata dagli altri due punti
    float calcolaAngolo(cv::Point, cv::Point, cv::Point, cv::Point);

    //input: vettore delle note, numero di elementi nel vettore
    void salvaMIDI(int*, int);

    //input: intero che identifica la forma del volto, modifica vettoreIntervalli inserendo i valori caratteristici del modo selezionato
    void impostaIntervalli(int);

    void impostaIntervalliDaTonica();

    //input intervallo, ottava di partenza
    int calcolaNumeroNotaMidi(int,int);

    //input: una nota qualsiasi. outpu: stessa nota spostata sull'ottava centrale del pianoforte
    int notaCorrispondenteOttavaCentrale(int);

    bool trovaDuplicatiESpostaOttava(int*, int);

    //se la nota appartiene all'ottava centrale restituisce true
    bool notaOttavaCentrale(int);

    //input angoloNaso
    void calcolaFondamentale(float);

    //restituiscie la media del colore HSV calcolata nell'immagine, intorno al punto, considerando un rettangolo intorno al punto
    //cv::Vec3b mediaColore (cv::Mat, cv::Point, int, int, int);

    //fa la media del colore su tutto il volto. prende la maschera da eroded.jpg
    //cv::Vec3b mediaColoreCompleta(cv::Mat, int);

    void on_actionPreferenze_triggered();

    void on_actionSalva_immagine_triggered();

private:
    Ui::MainWindow *ui;

    CVImageWidget *imgWidget;

    //immagine originale
    cv::Mat image;

    //frame con linee e disegni vari
    cv::Mat frame;

    //maschera del volto
    cv::Mat imgEroded;

    cv::CascadeClassifier face_cascade;
    cv::CascadeClassifier eyes_cascade;
    cv::CascadeClassifier nose_cascade;
    cv::CascadeClassifier mouth_cascade;

    //intervalli della scala modale a partire da sol
    int vettoreIntervalli[7];

    //intervalli della scala modale di sol, a partire dalla tonica
    int vettoreIntervalliDaTonica[7];

    //vettoreNote[0] = TONICA (il numero rappresenta il corrispondente numero in midi)
    int vettoreNote[6];

    //0=sol, 1=la, 2=si ecc...
    int tonica;

    std::string face_cascade_name;
    std::string eyes_cascade_name;
    std::string nose_cascade_name;
    std::string mouth_cascade_name;

};

#endif // MAINWINDOW_H
