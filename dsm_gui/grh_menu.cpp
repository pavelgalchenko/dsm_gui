#include "grh_menu.h"
#include "ui_grh_menu.h"

#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QRegularExpression>

GRH_Menu::GRH_Menu(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GRH_Menu)
{
    ui->setupUi(this);
    set_validators();
}

GRH_Menu::~GRH_Menu()
{
    delete ui;
}

void GRH_Menu::set_validators()
{
    ui->gl_output->setValidator(new QDoubleValidator);
    ui->povMode->addItems(povMode_inputs);
    ui->hostType->addItems(hosttarget_inputs);
    ui->hostSC->setValidator(new QIntValidator);
    ui->hostBDY->setValidator(new QIntValidator);
    ui->hostFrame->addItems(frame_inputs);
    ui->targetType->addItems(hosttarget_inputs);
    ui->targetSC->setValidator(new QIntValidator);
    ui->targetBDY->setValidator(new QIntValidator);
    ui->targetFrame->addItems(frame_inputs);
    ui->boresightaxis->addItems(axis_inputs);
    ui->upaxis->addItems(axis_inputs);
    ui->povrange->setValidator(new QDoubleValidator);
    ui->povangle->setValidator(new QDoubleValidator);
    ui->povinhost_x->setValidator(new QDoubleValidator);
    ui->povinhost_y->setValidator(new QDoubleValidator);
    ui->povinhost_z->setValidator(new QDoubleValidator);
    ui->povview->addItems(view_inputs);
    ui->camwidth->setValidator(new QIntValidator);
    ui->camheight->setValidator(new QIntValidator);
    ui->mousescale->setValidator(new QDoubleValidator);
    ui->mapwidth->setValidator(new QIntValidator);
    ui->mapheight->setValidator(new QIntValidator);
}

void GRH_Menu::receive_grhpath(QString path)
{
    inout_path = path;
    graphics_path = path+"Inp_Graphics.txt";
    receive_data();
    apply_data();
}

void GRH_Menu::receive_data()
{
    grh_data.clear();
    grh_string.clear();
    static QRegularExpression rx1("(.*?)*!");
    static QRegularExpression rx2("\"(.*?)\"");

    QFile file(graphics_path);
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(0, "error", file.errorString());
    }

    QTextStream in(&file);
    while(!in.atEnd()) {
        QString line = in.readLine();
        QRegularExpressionMatch match1 = rx1.match(line);
        if (match1.hasMatch()) {
            QString capture = match1.captured(0);
            grh_data.append(capture);
        }
        else {
            grh_data.append("blankline");
        }
        QRegularExpressionMatch match2 = rx2.match(line);
        if (match2.hasMatch()) {
            QString capture = match2.captured(0);
            grh_string.append(capture);
        }
        else {
            grh_string.append("blankline");
        }
    }
    file.close();
}

void GRH_Menu::write_data()
{
    QFile::remove(graphics_path);
    QFile file(graphics_path);
    if(!file.open(QFile::WriteOnly)) {
        QMessageBox::information(0, "error", file.errorString());
    }
    else {
        QTextStream in(&file);
        for (int i=0; i < grh_update.size(); i++){
            in << grh_update.at(i);
        }
    }
    file.close();
}

void GRH_Menu::apply_data()
{
    QStringList line_items;
    QString line_item;
    int line_num = 0;

    //line_num = 1; // blank line

    line_num = 2;
    line_items = grh_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    ui->gl_output->setText(line_items[0]);

    line_num = 3;
    line_items = grh_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    ui->star_catalog->setText(line_items[0]);

    line_num = 4;
    line_items = grh_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    if (line_items[0] == "TRUE") ui->mapwindow_on->setChecked(true);
    else ui->mapwindow_off->setChecked(true);

    line_num = 5;
    line_items = grh_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    if (line_items[0] == "TRUE") ui->orreywindown_on->setChecked(true);
    else ui->orreywindow_off->setChecked(true);

    line_num = 6;
    line_items = grh_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    if (line_items[0] == "TRUE") ui->spherewindow_on->setChecked(true);
    else ui->spherewindow_off->setChecked(true);

    //line_num = 7; //blank line

    line_num = 8;
    line_items = grh_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    if (line_items[0] == "TRUE") ui->pause_on->setChecked(true);
    else ui->pause_off->setChecked(true);

    line_num = 9;
    line_items = grh_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    for( int i=0; i<povMode_inputs.count(); ++i )
    {
        if (line_items[0] == povMode_inputs[i]) ui->povMode->setCurrentIndex(i);
    }

    line_num = 10;
    line_items = grh_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    for( int i=0; i<hosttarget_inputs.count(); ++i )
    {
        if (line_items[0] == hosttarget_inputs[i]) ui->hostType->setCurrentIndex(i);
    }

    line_num = 11;
    line_items = grh_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    ui->hostSC->setText(line_items[0]);
    ui->hostBDY->setText(line_items[1]);
    for( int i=0; i<frame_inputs.count(); ++i )
    {
        if (line_items[2] == frame_inputs[i]) ui->hostFrame->setCurrentIndex(i);
    }

    line_num = 12;
    line_items = grh_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    for( int i=0; i<hosttarget_inputs.count(); ++i )
    {
        if (line_items[0] == hosttarget_inputs[i]) ui->targetType->setCurrentIndex(i);
    }

    line_num = 13;
    line_items = grh_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    ui->targetSC->setText(line_items[0]);
    ui->targetBDY->setText(line_items[1]);
    for( int i=0; i<frame_inputs.count(); ++i )
    {
        if (line_items[2] == frame_inputs[i]) ui->targetFrame->setCurrentIndex(i);
    }

    line_num = 14;
    line_items = grh_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    for( int i=0; i<axis_inputs.count(); ++i )
    {
        if (line_items[0] == axis_inputs[i]) ui->boresightaxis->setCurrentIndex(i);
    }
    on_boresightaxis_activated(ui->boresightaxis->currentIndex());

    line_num = 15;
    line_items = grh_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    for( int i=0; i<upaxis_inputs.count(); ++i )
    {
        if (line_items[0] == upaxis_inputs[i]) ui->upaxis->setCurrentIndex(i);
    }

    line_num = 16;
    line_items = grh_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    ui->povrange->setText(line_items[0]);

    line_num = 17;
    line_items = grh_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    ui->povangle->setText(line_items[0]);

    line_num = 18;
    line_items = grh_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    ui->povinhost_x->setText(line_items[0]);
    ui->povinhost_y->setText(line_items[1]);
    ui->povinhost_z->setText(line_items[2]);

    line_num = 19;
    line_items = grh_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    for( int i=0; i<view_inputs.count(); ++i )
    {
        if (line_items[0] == view_inputs[i]) ui->povview->setCurrentIndex(i);
    }

    //line_num = 20; //blank line

    line_num = 21;
    line_item = grh_string[line_num-1];
    ui->camtitle->setText(line_item);

    line_num = 22;
    line_items = grh_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    ui->camwidth->setText(line_items[0]);
    ui->camheight->setText(line_items[1]);

    line_num = 23;
    line_items = grh_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    ui->mousescale->setText(line_items[0]);

    line_num = 24;
    line_items = grh_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    line_item = line_items[0];
    ui->displaygamma->setValue(line_item.toDouble());

    //line_num = 25; //blank line

    line_num = 26;
    line_items = grh_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    line_item = grh_string[line_num-1];
    if (line_items[0] == "TRUE") ui->show_naxis_on->setChecked(true);
    else ui->show_naxis_off->setChecked(true);
    ui->show_naxis_name->setText(line_item);

    line_num = 27;
    line_items = grh_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    line_item = grh_string[line_num-1];
    if (line_items[0] == "TRUE") ui->show_laxis_on->setChecked(true);
    else ui->show_laxis_off->setChecked(true);
    ui->show_laxis_name->setText(line_item);

    line_num = 28;
    line_items = grh_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    line_item = grh_string[line_num-1];
    if (line_items[0] == "TRUE") ui->show_faxis_on->setChecked(true);
    else ui->show_faxis_off->setChecked(true);
    ui->show_faxis_name->setText(line_item);

    line_num = 29;
    line_items = grh_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    line_item = grh_string[line_num-1];
    if (line_items[0] == "TRUE") ui->show_baxis_on->setChecked(true);
    else ui->show_baxis_off->setChecked(true);
    ui->show_baxis_name->setText(line_item);

    line_num = 30;
    line_items = grh_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    line_item = grh_string[line_num-1];
    if (line_items[0] == "TRUE") ui->show_ngrid_on->setChecked(true);
    else ui->show_ngrid_off->setChecked(true);
    ui->show_ngrid_name->setText(line_item);

    line_num = 31;
    line_items = grh_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    line_item = grh_string[line_num-1];
    if (line_items[0] == "TRUE") ui->show_lgrid_on->setChecked(true);
    else ui->show_lgrid_off->setChecked(true);
    ui->show_lgrid_name->setText(line_item);

    line_num = 32;
    line_items = grh_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    line_item = grh_string[line_num-1];
    if (line_items[0] == "TRUE") ui->show_fgrid_on->setChecked(true);
    else ui->show_fgrid_off->setChecked(true);
    ui->show_fgrid_name->setText(line_item);

    line_num = 33;
    line_items = grh_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    line_item = grh_string[line_num-1];
    if (line_items[0] == "TRUE") ui->show_bgrid_on->setChecked(true);
    else ui->show_bgrid_off->setChecked(true);
    ui->show_bgrid_name->setText(line_item);

    line_num = 34;
    line_items = grh_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    line_item = grh_string[line_num-1];
    if (line_items[0] == "TRUE") ui->show_galgrid_on->setChecked(true);
    else ui->show_galgrid_off->setChecked(true);
    ui->show_galgrid_name->setText(line_item);

    line_num = 35;
    line_items = grh_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    line_item = grh_string[line_num-1];
    if (line_items[0] == "TRUE") ui->show_fov_on->setChecked(true);
    else ui->show_fov_off->setChecked(true);
    ui->show_fov_name->setText(line_item);

    line_num = 36;
    line_items = grh_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    line_item = grh_string[line_num-1];
    if (line_items[0] == "TRUE") ui->show_proxops_on->setChecked(true);
    else ui->show_proxops_off->setChecked(true);
    ui->show_proxops_name->setText(line_item);

    line_num = 37;
    line_items = grh_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    line_item = grh_string[line_num-1];
    if (line_items[0] == "TRUE") ui->show_tdrs_on->setChecked(true);
    else ui->show_tdrs_off->setChecked(true);
    ui->show_tdrs_name->setText(line_item);

    line_num = 38;
    line_items = grh_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    line_item = grh_string[line_num-1];
    if (line_items[0] == "TRUE") ui->show_shdws_on->setChecked(true);
    else ui->show_shdws_off->setChecked(true);
    ui->show_shdws_name->setText(line_item);

    line_num = 39;
    line_items = grh_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    line_item = grh_string[line_num-1];
    if (line_items[0] == "TRUE") ui->show_astro_on->setChecked(true);
    else ui->show_astro_off->setChecked(true);
    ui->show_astro_name->setText(line_item);

    line_num = 40;
    line_items = grh_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    line_item = grh_string[line_num-1];
    if (line_items[0] == "TRUE") ui->show_truth_on->setChecked(true);
    else ui->show_truth_off->setChecked(true);
    ui->show_truth_name->setText(line_item);

    line_num = 41;
    line_items = grh_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    line_item = grh_string[line_num-1];
    if (line_items[0] == "TRUE") ui->show_fsw_on->setChecked(true);
    else ui->show_fsw_off->setChecked(true);
    ui->show_fsw_name->setText(line_item);

    line_num = 42;
    line_items = grh_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    line_item = grh_string[line_num-1];
    if (line_items[0] == "TRUE") ui->show_milky_on->setChecked(true);
    else ui->show_milky_off->setChecked(true);
    ui->show_milky_name->setText(line_item);

    line_num = 43;
    line_items = grh_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    line_item = grh_string[line_num-1];
    if (line_items[0] == "TRUE") ui->show_fermi_on->setChecked(true);
    else ui->show_fermi_off->setChecked(true);
    ui->show_fermi_name->setText(line_item);

    //line_num = 44; //blank line

    line_num = 45;
    line_item = grh_string[line_num-1];
    ui->maptitle->setText(line_item);

    line_num = 46;
    line_items = grh_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    ui->mapwidth->setText(line_items[0]);
    ui->mapheight->setText(line_items[1]);

    //line_num = 47; //blank line

    line_num = 48;
    line_items = grh_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    line_item = grh_string[line_num-1];
    if (line_items[0] == "TRUE") ui->showclock_on->setChecked(true);
    else ui->showclock_off->setChecked(true);
    ui->showclock_name->setText(line_item);

    line_num = 49;
    line_items = grh_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    line_item = grh_string[line_num-1];
    if (line_items[0] == "TRUE") ui->showtlmclock_on->setChecked(true);
    else ui->showtlmclock_off->setChecked(true);
    ui->showtlmclock_name->setText(line_item);

    line_num = 50;
    line_items = grh_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    line_item = grh_string[line_num-1];
    if (line_items[0] == "TRUE") ui->showcredits_on->setChecked(true);
    else ui->showcredits_off->setChecked(true);
    ui->showcredits_name->setText(line_item);

    line_num = 51;
    line_items = grh_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    line_item = grh_string[line_num-1];
    if (line_items[0] == "TRUE") ui->shownight_on->setChecked(true);
    else ui->shownight_off->setChecked(true);
    ui->shownight_name->setText(line_item);

    //line_num = 52; //blank line

    line_num = 53;
    line_items = grh_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    if (line_items[0] == "TRUE") ui->showmajorconst_on->setChecked(true);
    else ui->showmajorconst_off->setChecked(true);

    line_num = 54;
    line_items = grh_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    if (line_items[0] == "TRUE") ui->showzodiac_on->setChecked(true);
    else ui->showzodiac_off->setChecked(true);

    line_num = 55;
    line_items = grh_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    if (line_items[0] == "TRUE") ui->showminorconst_on->setChecked(true);
    else ui->showminorconst_off->setChecked(true);
}

void GRH_Menu::on_boresightaxis_activated(int index)
{
    upaxis_inputs.clear();
    upaxis_inputs.append(axis_inputs);

    QStringList removelist;

    if (index == 0 || index == 3) {
        removelist.clear();
        removelist.append({axis_inputs[0],axis_inputs[3]});
    }
    else if (index == 1 || index == 4) {
        removelist.clear();
        removelist.append({axis_inputs[1],axis_inputs[4]});
    }
    else if (index == 2 || index == 5) {
        removelist.clear();
        removelist.append({axis_inputs[2],axis_inputs[5]});
    }

    upaxis_inputs.removeAll(removelist[0]);
    upaxis_inputs.removeAll(removelist[1]);

    ui->upaxis->clear();
    ui->upaxis->addItems(upaxis_inputs);

}

int GRH_Menu::warning_message(QString warningText)
{
    QMessageBox warningMsg;
    warningMsg.setIcon(QMessageBox::Warning);
    warningMsg.setText(warningText);
    warningMsg.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    int ret = warningMsg.exec();
    return ret;
}

void GRH_Menu::on_loaddefaultButton_clicked()
{
    int response = warning_message("Overwrite Graphics file?");
    if (response == QMessageBox::Ok) {
        QFile::remove(inout_path+"Inp_Graphics.txt");
        QFile::copy(inout_path+"__default__/Inp_Graphics.txt", inout_path+"Inp_Graphics.txt");
        receive_data();
        apply_data();
    }
    else {
        return;
    }
}

void GRH_Menu::on_savedefaultButton_clicked()
{
    int response = warning_message("Overwrite Default Graphics file?");
    if (response == QMessageBox::Ok) {
        QFile::remove(inout_path+"__default__/Inp_Graphics.txt");
        QFile::copy(inout_path+"Inp_Graphics.txt",inout_path+"__default__/Inp_Graphics.txt");
        receive_data();
        apply_data();
    }
    else {
        return;
    }
}

void GRH_Menu::on_closeButton_clicked()
{
    GRH_Menu::close();
}

void GRH_Menu::on_applyButton_clicked()
{
    QString data_inp;

    grh_update.clear();

    grh_update.append("<<<<<<<<<<<<<<<<  42 Graphics Configuration File  >>>>>>>>>>>>>>>>>>>\n");

    data_inp = ui->gl_output->text();
    grh_update.append(whitespace(data_inp) + " ! GL Output Interval [sec]\n");

    data_inp = ui->star_catalog->text();
    grh_update.append(whitespace(data_inp) + " ! Star Catalog File Name\n");

    if (ui->mapwindow_on->isChecked()) data_inp = "TRUE";
    else data_inp = "FALSE";
    grh_update.append(whitespace(data_inp) + " ! Map Window Exists\n");

    if (ui->orreywindown_on->isChecked()) data_inp = "TRUE";
    else data_inp = "FALSE";
    grh_update.append(whitespace(data_inp) + " ! Orrery Window Exists\n");

    if (ui->spherewindow_on->isChecked()) data_inp = "TRUE";
    else data_inp = "FALSE";
    grh_update.append(whitespace(data_inp) + " ! Unit Sphere Window Exists\n");

    grh_update.append("******************************* POV *********************************\n");

    if (ui->pause_on->isChecked()) data_inp = "TRUE";
    else data_inp = "FALSE";
    grh_update.append(whitespace(data_inp) + " ! Pause at Startup\n");

    data_inp = ui->povMode->currentText();
    grh_update.append(whitespace(data_inp) + " ! POV Mode (TRACK_HOST, TRACK_TARGET, FIXED_IN_HOST)\n");

    data_inp = ui->hostType->currentText();
    grh_update.append(whitespace(data_inp) + " ! Host Type (WORLD, REFORB, FRM, SC, BODY)\n");

    data_inp = ui->hostSC->text()+"  "+ui->hostBDY->text()+"  "+ui->hostFrame->currentText();
    grh_update.append(whitespace(data_inp) + " ! Initial Host SC, Body, POV Frame\n");

    data_inp = ui->targetType->currentText();
    grh_update.append(whitespace(data_inp) + " ! Target Type (WORLD, REFORB, FRM, SC, BODY)\n");

    data_inp = ui->targetSC->text()+"  "+ui->targetBDY->text()+"  "+ui->targetFrame->currentText();
    grh_update.append(whitespace(data_inp) + " ! Initial Host SC, Body, POV Frame\n");

    data_inp = ui->boresightaxis->currentText();
    grh_update.append(whitespace(data_inp) + " ! Boresight Axis\n");

    data_inp = ui->upaxis->currentText();
    grh_update.append(whitespace(data_inp) + " ! Up Axis\n");

    data_inp = ui->povrange->text();
    grh_update.append(whitespace(data_inp) + " ! Initial POV Range from Target [m]\n");

    data_inp = ui->povangle->text();
    grh_update.append(whitespace(data_inp) + " ! POV Angle (Vertical) [deg]\n");

    data_inp = ui->povinhost_x->text()+"  "+ui->povinhost_y->text()+"  "+ui->povinhost_z->text();
    grh_update.append(whitespace(data_inp) + " ! POV Position in Host [m]\n");

    data_inp = ui->povview->currentText();
    grh_update.append(whitespace(data_inp) + " ! Initial POV View (FRONT, FRONT_RIGHT, etc)\n");

    grh_update.append("******************************* CAM *********************************\n");

    data_inp = ui->camtitle->text();
    grh_update.append(whitespace(data_inp) + " ! Cam Title [delimited by \"]\n");

    data_inp = ui->camwidth->text()+"  "+ui->camheight->text();
    grh_update.append(whitespace(data_inp) + " ! Width, Height [pixels]\n");

    data_inp = ui->mousescale->text();
    grh_update.append(whitespace(data_inp) + " ! Mouse Scale Factor\n");

    data_inp = ui->displaygamma->text();
    grh_update.append(whitespace(data_inp) + " ! Display's Gamma Exponent (1.8-4.0)\n");

    grh_update.append("************************** CAM Show Menu ****************************\n");

    if (ui->show_naxis_on->isChecked()) data_inp = "TRUE";
    else data_inp = "FALSE";
    data_inp.append(" "+ui->show_naxis_name->text());
    grh_update.append(whitespace(data_inp) + " ! Show N Axes\n");

    if (ui->show_laxis_on->isChecked()) data_inp = "TRUE";
    else data_inp = "FALSE";
    data_inp.append(" "+ui->show_laxis_name->text());
    grh_update.append(whitespace(data_inp) + " ! Show L Axes\n");

    if (ui->show_faxis_on->isChecked()) data_inp = "TRUE";
    else data_inp = "FALSE";
    data_inp.append(" "+ui->show_faxis_name->text());
    grh_update.append(whitespace(data_inp) + " ! Show F Axes\n");

    if (ui->show_baxis_on->isChecked()) data_inp = "TRUE";
    else data_inp = "FALSE";
    data_inp.append(" "+ui->show_baxis_name->text());
    grh_update.append(whitespace(data_inp) + " ! Show B Axes\n");

    if (ui->show_ngrid_on->isChecked()) data_inp = "TRUE";
    else data_inp = "FALSE";
    data_inp.append(" "+ui->show_ngrid_name->text());
    grh_update.append(whitespace(data_inp) + " ! Show N Grid\n");

    if (ui->show_lgrid_on->isChecked()) data_inp = "TRUE";
    else data_inp = "FALSE";
    data_inp.append(" "+ui->show_lgrid_name->text());
    grh_update.append(whitespace(data_inp) + " ! Show L Grid\n");

    if (ui->show_fgrid_on->isChecked()) data_inp = "TRUE";
    else data_inp = "FALSE";
    data_inp.append(" "+ui->show_fgrid_name->text());
    grh_update.append(whitespace(data_inp) + " ! Show F Grid\n");

    if (ui->show_bgrid_on->isChecked()) data_inp = "TRUE";
    else data_inp = "FALSE";
    data_inp.append(" "+ui->show_bgrid_name->text());
    grh_update.append(whitespace(data_inp) + " ! Show B Grid\n");

    if (ui->show_galgrid_on->isChecked()) data_inp = "TRUE";
    else data_inp = "FALSE";
    data_inp.append(" "+ui->show_galgrid_name->text());
    grh_update.append(whitespace(data_inp) + " ! Show G Grid\n");

    if (ui->show_fov_on->isChecked()) data_inp = "TRUE";
    else data_inp = "FALSE";
    data_inp.append(" "+ui->show_fov_name->text());
    grh_update.append(whitespace(data_inp) + " ! Show Fields of View\n");

    if (ui->show_proxops_on->isChecked()) data_inp = "TRUE";
    else data_inp = "FALSE";
    data_inp.append(" "+ui->show_proxops_name->text());
    grh_update.append(whitespace(data_inp) + " ! Show Prox Ops\n");

    if (ui->show_tdrs_on->isChecked()) data_inp = "TRUE";
    else data_inp = "FALSE";
    data_inp.append(" "+ui->show_tdrs_on->text());
    grh_update.append(whitespace(data_inp) + " ! Show TDRS Satellites\n");

    if (ui->show_shdws_on->isChecked()) data_inp = "TRUE";
    else data_inp = "FALSE";
    data_inp.append(" "+ui->show_shdws_name->text());
    grh_update.append(whitespace(data_inp) + " ! Show Shadows\n");

    if (ui->show_astro_on->isChecked()) data_inp = "TRUE";
    else data_inp = "FALSE";
    data_inp.append(" "+ui->show_astro_name->text());
    grh_update.append(whitespace(data_inp) + " ! Show Astro Labels\n");

    if (ui->show_truth_on->isChecked()) data_inp = "TRUE";
    else data_inp = "FALSE";
    data_inp.append(" "+ui->show_truth_name->text());
    grh_update.append(whitespace(data_inp) + " ! Show Truth Vectors\n");

    if (ui->show_fsw_on->isChecked()) data_inp = "TRUE";
    else data_inp = "FALSE";
    data_inp.append(" "+ui->show_fsw_name->text());
    grh_update.append(whitespace(data_inp) + " ! Show FSW Vectors\n");

    if (ui->show_milky_on->isChecked()) data_inp = "TRUE";
    else data_inp = "FALSE";
    data_inp.append(" "+ui->show_milky_name->text());
    grh_update.append(whitespace(data_inp) + " ! Show Milky Way\n");

    if (ui->show_fermi_on->isChecked()) data_inp = "TRUE";
    else data_inp = "FALSE";
    data_inp.append(" "+ui->show_fermi_name->text());
    grh_update.append(whitespace(data_inp) + " ! Show Fermi Sky\n");

    grh_update.append("******************************* MAP *********************************\n");

    data_inp = ui->maptitle->text();
    grh_update.append(whitespace(data_inp) + " ! Map Title [delimited by \"]\n");

    data_inp = ui->mapwidth->text()+"  "+ui->mapheight->text();
    grh_update.append(whitespace(data_inp) + " ! Width, Height [pixels]\n");

    grh_update.append("************************** MAP Show Menu ****************************\n");

    if (ui->showclock_on->isChecked()) data_inp = "TRUE";
    else data_inp = "FALSE";
    data_inp.append(" "+ui->showclock_name->text());
    grh_update.append(whitespace(data_inp) + " ! Show Clock\n");

    if (ui->showtlmclock_on->isChecked()) data_inp = "TRUE";
    else data_inp = "FALSE";
    data_inp.append(" "+ui->showtlmclock_name->text());
    grh_update.append(whitespace(data_inp) + " ! Show Tlm Clock\n");

    if (ui->showcredits_on->isChecked()) data_inp = "TRUE";
    else data_inp = "FALSE";
    data_inp.append(" "+ui->showcredits_name->text());
    grh_update.append(whitespace(data_inp) + " ! Show Credits\n");

    if (ui->shownight_on->isChecked()) data_inp = "TRUE";
    else data_inp = "FALSE";
    data_inp.append(" "+ui->shownight_name->text());
    grh_update.append(whitespace(data_inp) + " ! Show Night\n");

    grh_update.append("********************* Unit Sphere Show Menu *************************\n");

    if (ui->showmajorconst_on->isChecked()) data_inp = "TRUE";
    else data_inp = "FALSE";
    grh_update.append(whitespace(data_inp) + " ! Show Major Constellations\n");

    if (ui->showzodiac_on->isChecked()) data_inp = "TRUE";
    else data_inp = "FALSE";
    grh_update.append(whitespace(data_inp) + " ! Show Zodiac Constellations\n");

    if (ui->showminorconst_on->isChecked()) data_inp = "TRUE";
    else data_inp = "FALSE";
    grh_update.append(whitespace(data_inp) + " ! Show Minor Constellations\n");

    write_data();

}

QString GRH_Menu::whitespace(QString data)
{
    QString empty_space = "                              ";
    int data_len = empty_space.count()-data.count();
    if (data_len < 1) data_len = 1;
    for (int i = 0; i < data_len; i++){
        data.append(" ");
    }
    return data;
}
