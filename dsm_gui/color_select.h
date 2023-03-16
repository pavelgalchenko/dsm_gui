#ifndef COLOR_SELECT_H
#define COLOR_SELECT_H

#include <QDialog>
#include <QDebug>

namespace Ui {
class COLOR_Select;
}

class COLOR_Select : public QDialog
{
    Q_OBJECT

public:
    explicit COLOR_Select(QWidget *parent = nullptr);
    ~COLOR_Select();

signals:
    void send_newrgbavalues(QStringList);

private slots:
    void receive_rgbavalues(QStringList);
    void set_color();
    void on_red_actionTriggered(int action);
    void on_blue_actionTriggered(int action);
    void on_green_actionTriggered(int action);
    void on_alpha_actionTriggered(int action);
    void on_closeButton_clicked();
    //void on_applyButton_clicked();

    void on_applyButton_clicked();

private:
    Ui::COLOR_Select *ui;
};

#endif // COLOR_SELECT_H
