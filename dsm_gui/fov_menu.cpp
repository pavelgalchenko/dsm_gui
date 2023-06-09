#include "fov_menu.h"
#include "ui_fov_menu.h"

#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QRegularExpression>
#include <QColorDialog>

FOV_Menu::FOV_Menu(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FOV_Menu)
{
    ui->setupUi(this);
    set_validators();
}

FOV_Menu::~FOV_Menu()
{
    delete ui;
}

void FOV_Menu::set_validators()
{
    ui->num_sides->setValidator(new QIntValidator);
    ui->length_sides->setValidator(new QDoubleValidator);
    ui->horizontal_width->setValidator(new QDoubleValidator);
    ui->vertical_height->setValidator(new QDoubleValidator);
    ui->fov_type->addItems(fovtype_inputs);
    ui->sc_num->setValidator(new QIntValidator);
    ui->bdy_num->setValidator(new QIntValidator);
    ui->pos_x->setValidator(new QDoubleValidator);
    ui->pos_y->setValidator(new QDoubleValidator);
    ui->pos_z->setValidator(new QDoubleValidator);
    ui->rot1->setValidator(new QDoubleValidator);
    ui->rot2->setValidator(new QDoubleValidator);
    ui->rot3->setValidator(new QDoubleValidator);
    ui->euler_seq->addItems(euler_inputs);
    ui->boresightaxis->addItems(axis_inputs);
}

void FOV_Menu::receive_fovpath(QString path)
{
    inout_path = path;
    file_path = path+"Inp_FOV.txt";
    receive_data();
    apply_data();
    populate_list();
}

void FOV_Menu::receive_data()
{
    fov_data.clear();
    fov_string.clear();
    fov_update.clear();
    static QRegularExpression rx1("(.*?)*!");
    static QRegularExpression rx2("\"(.*?)\"");

    QFile file(file_path);
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(0, "error", file.errorString());
    }

    QTextStream in(&file);
    while(!in.atEnd()) {
        QString line = in.readLine();
        QRegularExpressionMatch match1 = rx1.match(line);
        if (match1.hasMatch()) {
            QString capture = match1.captured(0);
            fov_data.append(capture);
        }
        else {
            fov_data.append("blankline");
        }
        QRegularExpressionMatch match2 = rx2.match(line);
        if (match2.hasMatch()) {
            QString capture = match2.captured(0);
            fov_string.append(capture);
        }
        else {
            fov_string.append("blankline");
        }
        line.append("\n");
        fov_update.append(line);
    }
    file.close();
}

void FOV_Menu::write_data()
{
    QFile::remove(file_path);
    QFile file(file_path);
    if(!file.open(QFile::WriteOnly)) {
        QMessageBox::information(0, "error", file.errorString());
    }
    else {
        QTextStream in(&file);
        for (int i=0; i < fov_update.size(); i++){
            in << fov_update.at(i);
        }
    }
    file.close();
}


void FOV_Menu::apply_data()
{
    QStringList line_items;
    QString line_item;
    int num_fov = 0;

    fov_names.clear();

    line_items = fov_data[1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    line_item = line_items[0];
    num_fov = line_item.toInt();

    for (int i=0; i < num_fov; i++) {
        line_item = fov_string[11*i+3];
        fov_names.append(line_item);
    }
}

void FOV_Menu::populate_list()
{
    ui->fovlist->clear();
    ui->fovlist->addItems(fov_names);
}

void FOV_Menu::on_pickcolor_clicked()
{
    QColor prev_color;
    prev_color.setRedF(ui->redvalue->text().toDouble());
    prev_color.setBlueF(ui->bluevalue->text().toDouble());
    prev_color.setGreenF(ui->greenvalue->text().toDouble());
    prev_color.setAlphaF(ui->alphavalue->text().toDouble());

    QString title = "Color Picker";

    QColor color = QColorDialog::getColor(prev_color, this, title, QColorDialog::ShowAlphaChannel);

    QStringList rgbavalues;

    QString red;
    QString green;
    QString blue;
    QString alpha;

    QString color_text;

    if( color.isValid() )
    {
        red.setNum(color.redF());
        green.setNum(color.greenF());
        blue.setNum(color.blueF());
        alpha.setNum(color.alphaF());

        rgbavalues.append(red);
        rgbavalues.append(green);
        rgbavalues.append(blue);
        rgbavalues.append(alpha);

        color_text = rgbavalues[0];
        ui->redvalue->setValue(color_text.toDouble());
        color_text = rgbavalues[1];
        ui->greenvalue->setValue(color_text.toDouble());
        color_text = rgbavalues[2];
        ui->bluevalue->setValue(color_text.toDouble());
        color_text = rgbavalues[3];
        ui->alphavalue->setValue(color_text.toDouble());
    }

    if (!color.isValid()) return;
}

void FOV_Menu::receive_newrgbavalues(QStringList newrgbavalues)
{
    QString color;;

    color = newrgbavalues[0];
    ui->redvalue->setValue(color.toDouble());
    color = newrgbavalues[1];
    ui->greenvalue->setValue(color.toDouble());
    color = newrgbavalues[2];
    ui->bluevalue->setValue(color.toDouble());
    color = newrgbavalues[3];
    ui->alphavalue->setValue(color.toDouble());
}

void FOV_Menu::on_fov_remove_clicked()
{
    int removeitem = ui->fovlist->currentRow();
    if (removeitem == -1) {
        return;
    }
    else {
        int fov_index = 11*(removeitem)+2;
        fov_names.removeAt(removeitem);
        for (int i = 0; i < 11; i++) {
            fov_data.removeAt(fov_index);
            fov_string.removeAt(fov_index);
            fov_update.removeAt(fov_index);
        }

        delete ui->fovlist->takeItem(removeitem);

        QString data_inp;
        int fov_num = ui->fovlist->count();

        data_inp = QString::number(fov_num);
        fov_update[1] = whitespace(data_inp) + " ! Number of FOVs\n";

        write_data();

        ui->fovlist->setCurrentRow(-1);
        global_fov_index = -1;
    }
}

void FOV_Menu::on_fov_add_clicked()
{
    QString newfov = "\"NEWFOV\"";

    fov_names.append(newfov);

    fov_data.append("blankline");
    fov_data.append("\"NEWFOV\"                          !");
    fov_data.append("4   4.0                           !");
    fov_data.append("8.0  4.0                          !");
    fov_data.append("0.0 1.0 0.0 0.5                   !");
    fov_data.append("SOLID                             !");
    fov_data.append("TRUE  TRUE                        !");
    fov_data.append("0  0                              !");
    fov_data.append("0.0  0.0  1.0                     !");
    fov_data.append("0.0  0.0  0.0  321                !");
    fov_data.append("Z_AXIS                            !");

    fov_string.append("blankline");
    fov_string.append(newfov);
    for (int i = 0; i < 9; i++) fov_string.append("blankline");

    fov_update.append("--------------------------------------------------------------------\n");
    fov_update.append("\"NEWFOV\"                          ! Label\n");
    fov_update.append("4   4.0                           ! Number of Sides, Length [m]\n");
    fov_update.append("8.0  4.0                          ! H Width, V Height [deg]\n");
    fov_update.append("0.0 1.0 0.0 0.5                   ! Color RGB+Alpha\n");
    fov_update.append("SOLID                             ! WIREFRAME, SOLID, VECTOR, or PLANE\n");
    fov_update.append("TRUE  TRUE                        ! Draw Near Field, Draw Far Field\n");
    fov_update.append("0  0                              ! SC, Body\n");
    fov_update.append("0.0  0.0  1.0                     ! Position in Body [m]\n");
    fov_update.append("0.0  0.0  0.0  321                ! Euler Angles [deg], Sequence\n");
    fov_update.append("Z_AXIS                            ! Boresight Axis X_AXIS, Y_AXIS, or Z_AXIS\n");

    ui->fovlist->addItem(newfov);

    write_data();

    ui->fovlist->setCurrentRow(ui->fovlist->count()-1);
    on_fovlist_itemClicked(ui->fovlist->currentItem());
}

void FOV_Menu::on_fovlist_itemClicked(QListWidgetItem *item)
{
    int index = ui->fovlist->row(item);

    if (index == global_fov_index) {
        return;
    }
    else {
        if ( (global_fov_index != -1) && (global_fov_ignore == 0) ) {
            int response = warning_message("Note that changes to the previous selected FOV are lost unless you first select \"Apply\"! This is your only warning.");
            if (response == QMessageBox::Cancel) {
                ui->fovlist->setCurrentRow(global_fov_index);
                global_fov_ignore = 1;
                return;
            }
            else if (response == QMessageBox::Ok) {
                global_fov_ignore = 1;
            }
        }

        receive_data();
        apply_data();

        global_fov_index = index;

        int string_index = 11*index+3;
        int data_index = 11*index+4;
        int line_num;

        QStringList line_items;
        QString line_item;

        line_item = fov_string[string_index];
        ui->fov_name->setText(line_item);

        line_num = data_index+0;
        line_items = fov_data[line_num].split(QRegExp("\\s"), QString::SkipEmptyParts);
        ui->num_sides->setText(line_items[0]);
        ui->length_sides->setText(line_items[1]);

        line_num = data_index+1;
        line_items = fov_data[line_num].split(QRegExp("\\s"), QString::SkipEmptyParts);
        ui->horizontal_width->setText(line_items[0]);
        ui->vertical_height->setText(line_items[1]);

        line_num = data_index+2;
        line_items = fov_data[line_num].split(QRegExp("\\s"), QString::SkipEmptyParts);
        line_item = line_items[0];
        ui->redvalue->setValue(line_item.toDouble());
        line_item = line_items[1];
        ui->greenvalue->setValue(line_item.toDouble());
        line_item = line_items[2];
        ui->bluevalue->setValue(line_item.toDouble());
        line_item = line_items[3];
        ui->alphavalue->setValue(line_item.toDouble());

        line_num = data_index+3;
        line_items = fov_data[line_num].split(QRegExp("\\s"), QString::SkipEmptyParts);
        for( int i=0; i<fovtype_inputs.count(); ++i )
        {
            if (line_items[0] == fovtype_inputs[i]) ui->fov_type->setCurrentIndex(i);
        }

        line_num = data_index+4;
        line_items = fov_data[line_num].split(QRegExp("\\s"), QString::SkipEmptyParts);
        if (line_items[0] == "TRUE") ui->nearfield_on->setChecked(true);
        else ui->nearfield_off->setChecked(true);
        if (line_items[1] == "TRUE") ui->farfield_on->setChecked(true);
        else ui->farfield_off->setChecked(true);

        line_num = data_index+5;
        line_items = fov_data[line_num].split(QRegExp("\\s"), QString::SkipEmptyParts);
        ui->sc_num->setText(line_items[0]);
        ui->bdy_num->setText(line_items[1]);

        line_num = data_index+6;
        line_items = fov_data[line_num].split(QRegExp("\\s"), QString::SkipEmptyParts);
        ui->pos_x->setText(line_items[0]);
        ui->pos_y->setText(line_items[1]);
        ui->pos_z->setText(line_items[2]);

        line_num = data_index+7;
        line_items = fov_data[line_num].split(QRegExp("\\s"), QString::SkipEmptyParts);
        ui->rot1->setText(line_items[0]);
        ui->rot2->setText(line_items[1]);
        ui->rot3->setText(line_items[2]);
        for( int i=0; i<euler_inputs.count(); ++i )
        {
            if (line_items[3] == euler_inputs[i]) ui->euler_seq->setCurrentIndex(i);
        }

        line_num = data_index+8;
        line_items = fov_data[line_num].split(QRegExp("\\s"), QString::SkipEmptyParts);
        for( int i=0; i<axis_inputs.count(); ++i )
        {
            if (line_items[0] == axis_inputs[i]) ui->boresightaxis->setCurrentIndex(i);
        }
    }
}

int FOV_Menu::warning_message(QString warningText)
{
    QMessageBox warningMsg;
    warningMsg.setIcon(QMessageBox::Warning);
    warningMsg.setText(warningText);
    warningMsg.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    int ret = warningMsg.exec();
    return ret;
}

void FOV_Menu::on_loaddefaultButton_clicked()
{
    int response = warning_message("Overwrite FOV file?");
    if (response == QMessageBox::Ok) {
        QFile::remove(inout_path+"Inp_FOV.txt");
        QFile::copy(inout_path+"__default__/Inp_FOV.txt", inout_path+"Inp_FOV.txt");
        receive_data();
        apply_data();
    }
    else {
        return;
    }
}

void FOV_Menu::on_savedefaultButton_clicked()
{
    int response = warning_message("Overwrite Default FOV file?");
    if (response == QMessageBox::Ok) {
        QFile::remove(inout_path+"__default__/Inp_FOV.txt");
        QFile::copy(inout_path+"Inp_FOV.txt",inout_path+"__default__/Inp_FOV.txt");
        receive_data();
        apply_data();
    }
    else {
        return;
    }
}

void FOV_Menu::on_closeButton_clicked()
{
    FOV_Menu::close();
}

void FOV_Menu::on_applyButton_clicked()
{
    QString data_inp;

    int index = ui->fovlist->currentRow();

    if (index == -1) {
        return;
    }

    int fov_num = ui->fovlist->count();

    int data_index = 11*index+2;

    ui->fovlist->item(index)->setText(ui->fov_name->text());

    fov_update[0] = "************************* Fields of View ***************************\n";

    data_inp = QString::number(fov_num);
    fov_update[1] = whitespace(data_inp) + " ! Number of FOVs\n";

    fov_update[data_index+0] = "--------------------------------------------------------------------\n";

    data_inp = ui->fov_name->text();
    fov_update[data_index+1] = whitespace(data_inp) + " ! Label\n";

    data_inp = ui->num_sides->text() + "  " + ui->length_sides->text();
    fov_update[data_index+2] = whitespace(data_inp) + " ! Number of Sides, Length [m]\n";

    data_inp = ui->horizontal_width->text() + "  " + ui->vertical_height->text();
    fov_update[data_index+3] = whitespace(data_inp) + " ! H Width, V Height [deg]\n";

    data_inp = ui->redvalue->text() + "  " + ui->greenvalue->text()+ "  " + ui->bluevalue->text()+ "  " + ui->alphavalue->text();
    fov_update[data_index+4] = whitespace(data_inp) + " ! Color RGB+Alpha\n";

    data_inp = ui->fov_type->currentText();
    fov_update[data_index+5] = whitespace(data_inp) + " ! WIREFRAME, SOLID, VECTOR, or PLANE\n";

    if (ui->nearfield_on->isChecked()) data_inp = "TRUE";
    else data_inp = "FALSE";
    data_inp.append("  ");
    if (ui->farfield_on->isChecked()) data_inp.append("TRUE");
    else data_inp.append("FALSE");
    fov_update[data_index+6] = whitespace(data_inp) + " ! Draw Near Field, Draw Far Field\n";

    data_inp = ui->sc_num->text() + "  " + ui->bdy_num->text();
    fov_update[data_index+7] = whitespace(data_inp) + " ! SC, Body\n";

    data_inp = ui->pos_x->text() + "  " + ui->pos_y->text()+ "  " + ui->pos_z->text();
    fov_update[data_index+8] = whitespace(data_inp) + " ! Position in Body [m]\n";

    data_inp = ui->rot1->text() + "  " + ui->rot2->text()+ "  " + ui->rot3->text()+ "  " + ui->euler_seq->currentText();
    fov_update[data_index+9] = whitespace(data_inp) + " ! Euler Angles [deg], Sequence\n";

    data_inp = ui->boresightaxis->currentText();
    fov_update[data_index+10] = whitespace(data_inp) + " ! Boresight Axis X_AXIS, Y_AXIS, or Z_AXIS\n";

    write_data();

}

QString FOV_Menu::whitespace(QString data)
{
    QString empty_space = "                              ";
    int data_len = empty_space.count()-data.count();
    if (data_len < 1) data_len = 1;
    for (int i = 0; i < data_len; i++){
        data.append(" ");
    }
    return data;
}

