#include "selezionaformavolto.h"
#include "ui_selezionaformavolto.h"

#include <iostream>

using namespace std;

SelezionaFormaVolto::SelezionaFormaVolto(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelezionaFormaVolto)
{
    ui->setupUi(this);
    this->setWindowTitle("Seleziona la forma del volto");

    //imposto le immagini come pulsanti

    QPixmap *mapOvale = new QPixmap (":/img/ovale.png");
    QIcon *ovale = new QIcon (*mapOvale);
/*    ovale->pixmap(QSize(mapOvale->width(), mapOvale->height()),
                  isEnabled() ? QIcon::Normal
                              : QIcon::Selected);   */
    ui->pushButtonOvale->setIcon(*ovale);
    ui->pushButtonOvale->setIconSize(QSize(mapOvale->width(), mapOvale->height()));

    QPixmap *mapRotondo = new QPixmap (":/img/rotondo.png");
    QIcon *rotondo = new QIcon (*mapRotondo);
    ui->pushButtonRotondo->setIcon(*rotondo);
    ui->pushButtonRotondo->setIconSize(QSize(mapRotondo->width(), mapRotondo->height()));

    QPixmap *mapQuadrato = new QPixmap (":/img/quadrato.png");
    QIcon *quadrato = new QIcon (*mapQuadrato);
    ui->pushButtonQuadrato->setIcon(*quadrato);
    ui->pushButtonQuadrato->setIconSize(QSize(mapQuadrato->width(), mapQuadrato->height()));

    QPixmap *mapDiamante = new QPixmap (":/img/diamante.png");
    QIcon *diamante = new QIcon (*mapDiamante);
    ui->pushButtonDiamante->setIcon(*diamante);
    ui->pushButtonDiamante->setIconSize(QSize(mapDiamante->width(), mapDiamante->height()));

    QPixmap *mapCuore = new QPixmap (":/img/cuore.png");
    QIcon *cuore= new QIcon (*mapCuore);
    ui->pushButtonCuore->setIcon(*cuore);
    ui->pushButtonCuore->setIconSize(QSize(mapCuore->width(), mapCuore->height()));

    QPixmap *mapRettangolare = new QPixmap (":/img/rettangolare.png");
    QIcon *rettangolare = new QIcon (*mapRettangolare);
    ui->pushButtonRettangolare->setIcon(*rettangolare);
    ui->pushButtonRettangolare->setIconSize(QSize(mapRettangolare->width(), mapRettangolare->height()));

    QPixmap *mapTriangolare = new QPixmap (":/img/triangolare.png");
    QIcon *triangolare = new QIcon (*mapTriangolare);
    ui->pushButtonTriangolare->setIcon(*triangolare);
    ui->pushButtonTriangolare->setIconSize(QSize(mapTriangolare->width(), mapTriangolare->height()));

}

SelezionaFormaVolto::~SelezionaFormaVolto()
{
    delete ui;
}

void SelezionaFormaVolto::on_pushButtonOvale_clicked()
{

    cerr << endl << endl << "ovale";
    //disabilito il pulsante per mostrare l'effetto grafico (il pulsante diventa grigio)
    ui->pushButtonOvale->setEnabled(false);

    setFormaVolto(VOLTOOVALE);

    done(QDialog::Accepted);

}

void SelezionaFormaVolto::setFormaVolto(int volto)
{
    formaVolto=volto;
}

int SelezionaFormaVolto::getFormaVolto()
{
    return formaVolto;
}

void SelezionaFormaVolto::on_pushButtonRotondo_clicked()
{
    setFormaVolto(VOLTOROTONDO);

    done(QDialog::Accepted);
}

void SelezionaFormaVolto::on_pushButtonQuadrato_clicked()
{
    setFormaVolto(VOLTOQUADRATO);

    done(QDialog::Accepted);
}

void SelezionaFormaVolto::on_pushButtonDiamante_clicked()
{
    setFormaVolto(VOLTODIAMANTE);

    done(QDialog::Accepted);
}

void SelezionaFormaVolto::on_pushButtonCuore_clicked()
{
    setFormaVolto(VOLTOCUORE);

    done(QDialog::Accepted);
}

void SelezionaFormaVolto::on_pushButtonRettangolare_clicked()
{
    setFormaVolto(VOLTORETTANGOLARE);

    done(QDialog::Accepted);
}

void SelezionaFormaVolto::on_pushButtonTriangolare_clicked()
{
    setFormaVolto(VOLTOTRIANGOLARE);

    done(QDialog::Accepted);
}
