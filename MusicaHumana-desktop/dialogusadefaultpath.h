#ifndef DIALOGUSADEFAULTPATH_H
#define DIALOGUSADEFAULTPATH_H

#include <QDialog>

namespace Ui {
class DialogUsaDefaultPath;
}

class DialogUsaDefaultPath : public QDialog
{
    Q_OBJECT

public:
    explicit DialogUsaDefaultPath(QWidget *parent = 0);
    ~DialogUsaDefaultPath();

    bool isCheckedNonChiederePiu();

private:
    Ui::DialogUsaDefaultPath *ui;
};

#endif // DIALOGUSADEFAULTPATH_H
