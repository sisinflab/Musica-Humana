#ifndef SELEZIONAFORMAVOLTO_H
#define SELEZIONAFORMAVOLTO_H

#define VOLTOOVALE          0
#define VOLTOROTONDO        1
#define VOLTOQUADRATO       2
#define VOLTODIAMANTE       3
#define VOLTOCUORE          4
#define VOLTORETTANGOLARE   5
#define VOLTOTRIANGOLARE    6

#include <QDialog>

namespace Ui {
class SelezionaFormaVolto;
}

class SelezionaFormaVolto : public QDialog
{
    Q_OBJECT

public:
    explicit SelezionaFormaVolto(QWidget *parent = 0);
    ~SelezionaFormaVolto();

    int getFormaVolto();

private slots:
    void on_pushButtonOvale_clicked();

    void setFormaVolto(int);

    void on_pushButtonRotondo_clicked();

    void on_pushButtonQuadrato_clicked();

    void on_pushButtonDiamante_clicked();

    void on_pushButtonCuore_clicked();

    void on_pushButtonRettangolare_clicked();

    void on_pushButtonTriangolare_clicked();

private:
    Ui::SelezionaFormaVolto *ui;

    int formaVolto;
};

#endif // SELEZIONAFORMAVOLTO_H
