#include "spc_submenu.h"
#include "ui_spc_submenu.h"
#include "dsm_gui_lib.h"

#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QComboBox>
#include <QRegularExpression>
#include <QDir>

SPC_submenu::SPC_submenu(QWidget *parent):
    QDialog(parent),
    ui(new Ui::SPC_submenu)
{
    ui->setupUi(this);
}

SPC_submenu::~SPC_submenu()
{
    delete ui;
}

void SPC_submenu::receive_spc_sm_path(QString name, QString path)
{
    spc_cur_name = name; // store name
    spc_cur_file = path + "SC_" + spc_cur_name + ".txt";
}


void SPC_submenu::receive_data()
{
    // Daniel's regex from ORB_Menu
    // Return everything up to and including ! (exclamation point)
    static QRegularExpression rx1("(.*?)!");

    // Return everything between a set of " " (quotation marks)
    static QRegularExpression rx2("\"(.*?)\"");

    // If the line does NOT start with an alphanumeric character or " (single quotation), then return the line as first group.
    // Otherwise return everything after ! (exclamation point) as second group
    static QRegularExpression rx3("(?:(?=^[^[:alnum:]|\"])([^[:alnum:]|\"].*)|(!.*))");

    QFile file(file_path);
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(0, "error", file.errorString());
    }

    QTextStream in(&file);
    while(!in.atEnd()) {
        QString line = in.readLine();
        QRegularExpressionMatch match1 = rx1.match(line);
        spc_data.append(match1.captured(1)); // index 0 includes ! character, index 1 does not

        QRegularExpressionMatch match2 = rx2.match(line);
        spc_string.append(match2.captured(1)); // index 0 includes "" characters, index 1 does not

        //        line.append("\n");
        QRegularExpressionMatch match3 = rx3.match(line);
        if (match3.hasMatch()) {
            QString capture = match3.captured(1);
            if (!capture.isEmpty()) capture += "\n";
            spc_file_headers.append(capture);
            capture = match3.captured(2);
            if (!capture.isEmpty()) capture += "\n";
            spc_file_descrip.append(capture);
        }

    }
    file.close();

}

void SPC_submenu::apply_data()
{
    QStringList line_items;
    QString line_string;

    ui->spc_cur_name_sub->setText(spc_cur_name);

    for(int line_num=7; line_num<spc_data.length(); line_num++)
    { // stop after general information
        line_string = spc_string[line_num-1];
        line_items = spc_data[line_num-1].split(QRegExp("\\s"), Qt::SkipEmptyParts);

        switch (line_num) {
            /******************************************* HEADER ***************************************************/
            // Orbit Parameters
        case 7: // Orbit Prop
            setQComboBox(ui->spc_cur_orb_type, line_items[0]);
            break;
        case 8: // Pos of CM or ORIGIN wrt F
            setQComboBox(ui->spc_cur_pos_ref, line_items[0]);
            break;
        case 9: // Pos wrt Formation (m) expressed in F
            ui->spc_cur_xpos_form->setText(line_items[0]);
            ui->spc_cur_ypos_form->setText(line_items[1]);
            ui->spc_cur_zpos_form->setText(line_items[2]);
            break;
        case 10: // Vel wrt Formation (m) expressed in F
            ui->spc_cur_xvel_form->setText(line_items[0]);
            ui->spc_cur_yvel_form->setText(line_items[1]);
            ui->spc_cur_zvel_form->setText(line_items[2]);
            break;
        case 11: // Initial Attitude Header
            break;
        case 12: // Ang Vel wrt [NL], Att [QA] wrt [NLF]
            line_items = spc_data[line_num-1].split(QRegExp(""), Qt::SkipEmptyParts);
            setQComboBox(ui->spc_cur_angvel_frame1, line_items[0]);
            setQComboBox(ui->spc_cur_angvel_frame2, line_items[2]);

            setQComboBox(ui->spc_cur_att_param, line_items[1]);
            break;
        case 13:
            ui->spc_cur_angvel_x->setText(line_items[0]);
            ui->spc_cur_angvel_y->setText(line_items[1]);
            ui->spc_cur_angvel_z->setText(line_items[2]);
            break;
        case 14:
            ui->spc_cur_q1->setText(line_items[1]);
            ui->spc_cur_q2->setText(line_items[2]);
            ui->spc_cur_q3->setText(line_items[3]);
            ui->spc_cur_q4->setText(line_items[4]);

            if (QString::compare(ui->spc_cur_att_param->currentText(), "Q"))
            {
                ui->spc_cur_initeul_1->setEnabled(false);
                ui->spc_cur_initeul_2->setEnabled(false);
                ui->spc_cur_initeul_3->setEnabled(false);

                ui->spc_cur_initeul_seq->setEnabled(false);

                ui->spc_cur_q1->setEnabled(true);
                ui->spc_cur_q2->setEnabled(true);
                ui->spc_cur_q3->setEnabled(true);
                ui->spc_cur_q4->setEnabled(true);
            }
            else
            {
                ui->spc_cur_initeul_1->setEnabled(true);
                ui->spc_cur_initeul_2->setEnabled(true);
                ui->spc_cur_initeul_3->setEnabled(true);

                ui->spc_cur_initeul_seq->setEnabled(true);

                ui->spc_cur_q1->setEnabled(false);
                ui->spc_cur_q2->setEnabled(false);
                ui->spc_cur_q3->setEnabled(false);
                ui->spc_cur_q4->setEnabled(false);
            }
            break;
        case 15:
            ui->spc_cur_initeul_1->setText(line_items[0]);
            ui->spc_cur_initeul_2->setText(line_items[1]);
            ui->spc_cur_initeul_3->setText(line_items[2]);

            setQComboBox(ui->spc_cur_initeul_seq, line_items[3]);

            if (QString::compare(ui->spc_cur_att_param->currentText(), "Q"))
            {
                ui->spc_cur_initeul_1->setEnabled(false);
                ui->spc_cur_initeul_2->setEnabled(false);
                ui->spc_cur_initeul_3->setEnabled(false);

                ui->spc_cur_initeul_seq->setEnabled(false);

                ui->spc_cur_q1->setEnabled(true);
                ui->spc_cur_q2->setEnabled(true);
                ui->spc_cur_q3->setEnabled(true);
                ui->spc_cur_q4->setEnabled(true);
            }
            else
            {
                ui->spc_cur_initeul_1->setEnabled(true);
                ui->spc_cur_initeul_2->setEnabled(true);
                ui->spc_cur_initeul_3->setEnabled(true);

                ui->spc_cur_initeul_seq->setEnabled(true);

                ui->spc_cur_q1->setEnabled(false);
                ui->spc_cur_q2->setEnabled(false);
                ui->spc_cur_q3->setEnabled(false);
                ui->spc_cur_q4->setEnabled(false);
            }
            break;
        case 16: // Dynamics Flags Header
            break;
        case 17:
            setQComboBox(ui->spc_cur_solver, line_items[0]);
            break;
        case 18: // Compute constraint forces and torques
            if (QString::compare(line_items[0], "TRUE")) // if one is selected, the other is automatically unselected
            {
                ui->spc_cur_con_on->setChecked(true);
            }
            break;
        case 19: // mass props referenced to ...
            setQComboBox(ui->spc_cur_masspropref, line_items[0]);
        case 20: // Flex active
            if (QString::compare(line_items[0], "TRUE")) // if one is selected, the other is automatically unselected
            {
                ui->spc_cur_flex_on->setChecked(true);
            }
            break;
        case 21: // include 2nd order flex terms
            if (QString::compare(line_items[0], "TRUE")) // if one is selected, the other is automatically unselected
            {
                ui->spc_cur_2flex_on->setChecked(true);
            }
        case 22: // Shaker file name
            if (QString::compare(line_items[0], "NONE"))
            {
                ui->spc_cur_shaker_enab->setCheckState(Qt::Checked);
            }
            else
            {
                ui->spc_cur_shaker_enab->setCheckState(Qt::Unchecked);
                ui->spc_cur_shaker_file->setText(line_items[0]);
            }
            break;
        case 23: // Drag coefficient
            ui->spc_cur_drag->setText(line_items[0]);
            break;
        case 24:
            break;
        case 25:
            break;
        case 26:
            break;
        case 27: // number of bodies
            bodies = line_items[0].toInt();

        }

        // Begin non-fixed length section
        // overall logic: count number of bodies. Reset a new counting index to start at 0 at each new body.
        long i = 0;
        if (line_num > 27){
            for (i=0; i<bodies; i++)
            {
                ui->spc_cur_body_list->setCurrentRow(i);
                long body_line_num = line_num - 28 - 9*i;
                switch (body_line_num) {
                case 0: // Body X Header
                    break;
                case 1:
                    ui->spc_cur_body_mass->setText(line_items[0]);
                    break;
                case 2: // Moments of Inertia (kg-m^2)
                    ui->spc_cur_body_pmoi_x->setText(line_items[0]);
                    ui->spc_cur_body_pmoi_y->setText(line_items[1]);
                    ui->spc_cur_body_pmoi_z->setText(line_items[2]);
                    break;
                case 3: // Products of Inertia
                    ui->spc_cur_body_poi_x->setText(line_items[0]);
                    ui->spc_cur_body_poi_y->setText(line_items[1]);
                    ui->spc_cur_body_poi_z->setText(line_items[2]);
                    break;
                case 4: // Location of mass center
                    ui->spc_cur_body_com_x->setText(line_items[0]);
                    ui->spc_cur_body_com_y->setText(line_items[1]);
                    ui->spc_cur_body_com_z->setText(line_items[2]);
                    break;
                case 5: // constant embedded momentum
                    ui->spc_cur_body_cem_x->setText(line_items[0]);
                    ui->spc_cur_body_cem_y->setText(line_items[1]);
                    ui->spc_cur_body_cem_z->setText(line_items[2]);
                    break;
                case 6: // constant embedded momentum
                    ui->spc_cur_body_cemd_x->setText(line_items[0]);
                    ui->spc_cur_body_cemd_y->setText(line_items[1]);
                    ui->spc_cur_body_cemd_z->setText(line_items[2]);
                    break;
                case 7:
                    ui->spc_cur_body_geom->setText(line_items[0]);
                    break;
                case 8:
                    if (QString::compare(line_items[0], "NONE"))
                    {
                        ui->spc_cur_node_enab->setCheckState(Qt::Checked);
                    }
                    else
                    {
                        ui->spc_cur_node_enab->setCheckState(Qt::Unchecked);
                        ui->spc_cur_node_file->setText(line_items[0]);
                    }
                    break;
                case 9:
                    if (QString::compare(line_items[0], "NONE"))
                    {
                        ui->spc_cur_flex_enab->setCheckState(Qt::Checked);
                    }
                    else
                    {
                        ui->spc_cur_flex_enab->setCheckState(Qt::Unchecked);
                        ui->spc_cur_flex_file->setText(line_items[0]);
                    }
                    break;
                }
            }
        } // end body section
        // else if (i > 27 + bodies*9) ...
    }


}

void SPC_submenu::write_data()
{
    QFile::remove(file_path);
    QFile file(file_path);
    if(!file.open(QFile::WriteOnly)) {
        QMessageBox::information(0, "error", file.errorString());
    }
    else {
        QTextStream in(&file);
        for (int i=0; i < spc_update.size(); i++){
            in << spc_update.at(i);
        }
    }
    spc_update.clear();
    file.close();
}


void SPC_submenu::on_spc_cur_load_clicked()
{

    file_path = spc_cur_file;
    int response = dsm_gui_lib::warning_message("Overwrite SC file?");
    if (response == QMessageBox::Ok) {
        QFile::remove(file_path);
        QFile::copy(inout_path+"__default__/SC_Simple.txt", file_path);

        receive_data();
        apply_data();
    }

}


void SPC_submenu::on_spc_cur_save_clicked()
{

}


void SPC_submenu::on_spc_cur_close_clicked()
{

}


void SPC_submenu::on_spc_cur_apply_clicked()
{

}

void SPC_submenu::setQComboBox(QComboBox *comboBox, QString string) {
    comboBox->setCurrentIndex(comboBox->findText(string));
}



