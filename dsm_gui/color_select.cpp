#include "color_select.h"
#include "ui_color_select.h"

#include <QDebug>
#include <QColor>

COLOR_Select::COLOR_Select(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::COLOR_Select)
{
    ui->setupUi(this);
    ui->rbga_color->setAutoFillBackground(true);
}

COLOR_Select::~COLOR_Select()
{
    delete ui;
}

void COLOR_Select::set_color()
{
    QString red = QString::number(ui->red->value());
    QString green = QString::number(ui->green->value());
    QString blue = QString::number(ui->blue->value());
    QString alpha = QString::number(ui->alpha->value());

    ui->rbga_color->setStyleSheet("background-color: rgba("+red+", "+green+", "+blue+", "+alpha+");");
}

void COLOR_Select::on_red_actionTriggered(int action)
{
    set_color();
}

void COLOR_Select::on_blue_actionTriggered(int action)
{
    set_color();
}

void COLOR_Select::on_green_actionTriggered(int action)
{
    set_color();
}

void COLOR_Select::on_alpha_actionTriggered(int action)
{
    set_color();
}

void COLOR_Select::receive_rgbavalues(QStringList rgbavalues)
{
    QString sred = rgbavalues[0];
    QString sgreen = rgbavalues[1];
    QString sblue = rgbavalues[2];
    QString salpha = rgbavalues[3];

    double red = sred.toDouble()*255;
    double green = sgreen.toDouble()*255;
    double blue = sblue.toDouble()*255;
    double alpha = salpha.toDouble()*255;

    ui->red->setValue(red);
    ui->green->setValue(green);
    ui->blue->setValue(blue);
    ui->alpha->setValue(alpha);

    set_color();
}

void COLOR_Select::on_closeButton_clicked()
{
    COLOR_Select::close();
}

void COLOR_Select::on_applyButton_clicked()
{

    QStringList rgbavalues;
    double red = ui->red->value()/255.0;
    double green = ui->green->value()/255.0;
    double blue = ui->blue->value()/255.0;
    double alpha = ui->alpha->value()/255.0;

    rgbavalues.append(QString::number(red));
    rgbavalues.append(QString::number(green));
    rgbavalues.append(QString::number(blue));
    rgbavalues.append(QString::number(alpha));

    emit send_newrgbavalues(rgbavalues);
    disconnect(this, SIGNAL(send_newrgbavalues(QStringList)), 0, 0);
    COLOR_Select::close();
}

