#include "spc_submenu.h"
#include "ui_spc_submenu.h"
#include "dsm_gui_lib.h"

#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QComboBox>
#include <QRegularExpression>
#include <QDir>
#include <QDebug>

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
    file_path = spc_cur_file;

    receive_data();
    apply_data();
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
        qDebug() << line_num;
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
            break;

        }

        // Begin non-fixed length section
        // overall logic: count number of item. Reset a new counting index to start at 0 at each new item. Use a switch/case to set individual elements.
        long i = 0;
        long reset_ind_body = 29;

        long reset_ind_joint = 50000;
        long reset_ind_wheel = 50000;
        long reset_ind_mtb = 50000;
        long reset_ind_thr = 50000;
        long reset_ind_gyro = 50000;
        long reset_ind_mag = 50000;
        long reset_ind_css = 50000;
        long reset_ind_fss = 50000;
        long reset_ind_strack = 50000;
        long reset_ind_gps = 50000;
        long reset_ind_acc = 50000;

        if (line_num > reset_ind_body){ // start at line 30
            long body_entries = 10;
            for (i=0; i<bodies; i++)
            {
                ui->spc_cur_body_list->addItem("Body " + QString::number(i));
                ui->spc_cur_body_list->setCurrentRow(i);
                long body_line_num = line_num - (reset_ind_body + 1) - body_entries*i;
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
            reset_ind_joint = reset_ind_body + (1 + body_entries*bodies);
        } // end body section
        // begin joint section
        else if (i > reset_ind_joint) // now at beginning of joints including multi-line header
        {
            joints = bodies - 1;
            long joint_entries = 16;
            for (i=0; i<joints; i++)
            {
                long joint_line_num = line_num - (reset_ind_joint + 4) - joint_entries*i;
                ui->spc_cur_joint_list->addItem("Joint " + QString::number(i));
                ui->spc_cur_joint_list->setCurrentRow(i);
                switch (joint_line_num) {
                case 0: // Joint X Header
                    break;
                case 1:
                    setQComboBox(ui->spc_cur_joint_type, line_items[0]);
                    break;
                case 2:
                    ui->spc_cur_joint_in->setValue(line_items[0].toInt());
                    ui->spc_cur_joint_out->setValue(line_items[1].toInt());
                    break;
                case 3:
                    ui->spc_cur_joint_rotdof->setValue(line_items[0].toInt());
                    setQComboBox(ui->spc_cur_joint_rotdof_seq, line_items[1]);
                    setQComboBox(ui->spc_cur_joint_rottype, line_items[2]);
                    break;
                case 4:
                    ui->spc_cur_joint_trndof->setValue(line_items[0].toInt());
                    setQComboBox(ui->scp_cur_joint_trndof_seq, line_items[1]);
                    break;
                case 5:
                    if (QString::compare(line_items[0], "FALSE")) ui->spc_cur_joint_rlock1->setCheckState(Qt::Checked);
                    else ui->spc_cur_joint_rlock1->setCheckState(Qt::Unchecked);

                    if (QString::compare(line_items[1], "FALSE")) ui->spc_cur_joint_rlock2->setCheckState(Qt::Checked);
                    else ui->spc_cur_joint_rlock2->setCheckState(Qt::Unchecked);

                    if (QString::compare(line_items[2], "FALSE")) ui->spc_cur_joint_rlock3->setCheckState(Qt::Checked);
                    else ui->spc_cur_joint_rlock3->setCheckState(Qt::Unchecked);
                    break;

                case 6:
                    if (QString::compare(line_items[0], "FALSE")) ui->spc_cur_joint_tlock1->setCheckState(Qt::Checked);
                    else ui->spc_cur_joint_tlock1->setCheckState(Qt::Unchecked);

                    if (QString::compare(line_items[1], "FALSE")) ui->spc_cur_joint_tlock2->setCheckState(Qt::Checked);
                    else ui->spc_cur_joint_tlock2->setCheckState(Qt::Unchecked);

                    if (QString::compare(line_items[2], "FALSE")) ui->spc_cur_joint_tlock3->setCheckState(Qt::Checked);
                    else ui->spc_cur_joint_tlock3->setCheckState(Qt::Unchecked);
                    break;
                case 7:
                    ui->spc_cur_joint_ang0_1->setText(line_items[0]);
                    ui->spc_cur_joint_ang0_2->setText(line_items[1]);
                    ui->spc_cur_joint_ang0_3->setText(line_items[2]);
                    break;
                case 8:
                    ui->spc_cur_joint_angrate0_1->setText(line_items[0]);
                    ui->spc_cur_joint_angrate0_2->setText(line_items[1]);
                    ui->spc_cur_joint_angrate0_3->setText(line_items[2]);
                    break;
                case 9:
                    ui->spc_cur_joint_disp0_1->setText(line_items[0]);
                    ui->spc_cur_joint_disp0_2->setText(line_items[1]);
                    ui->spc_cur_joint_disp0_3->setText(line_items[2]);
                    break;
                case 10:
                    ui->spc_cur_joint_dispr0_1->setText(line_items[0]);
                    ui->spc_cur_joint_dispr0_2->setText(line_items[1]);
                    ui->spc_cur_joint_dispr0_3->setText(line_items[2]);
                    break;
                case 11:
                    ui->spc_cur_joint_bigi_1->setText(line_items[0]);
                    ui->spc_cur_joint_bigi_2->setText(line_items[1]);
                    ui->spc_cur_joint_bigi_3->setText(line_items[2]);

                    setQComboBox(ui->spc_cur_joint_bigi_seq, line_items[3]);
                    break;
                case 12:
                    ui->spc_cur_joint_bogo_1->setText(line_items[0]);
                    ui->spc_cur_joint_bogo_2->setText(line_items[1]);
                    ui->spc_cur_joint_bogo_3->setText(line_items[2]);

                    setQComboBox(ui->spc_cur_joint_bogo_seq, line_items[3]);
                    break;
                case 13:
                    ui->spc_cur_joint_poswrt_in_1->setText(line_items[0]);
                    ui->spc_cur_joint_poswrt_in_2->setText(line_items[1]);
                    ui->spc_cur_joint_poswrt_in_3->setText(line_items[2]);
                    break;
                case 14:
                    ui->spc_cur_joint_poswrt_out_1->setText(line_items[0]);
                    ui->spc_cur_joint_poswrt_out_2->setText(line_items[1]);
                    ui->spc_cur_joint_poswrt_out_3->setText(line_items[2]);
                    break;
                case 15:
                    ui->spc_cur_joint_param_file->setText(line_items[0]);
                    break;
                }
            }
            reset_ind_wheel = reset_ind_joint + (4 + joint_entries*joints);
        }
        else if (i > reset_ind_wheel) // now at wheel parameters header - skip wheel drag and jitter active as this can be set based on sub settings of indiv. wheels
        {
            long wheel_entries = 7;
            if (line_num == reset_ind_wheel + 3)
            {
                wheels = line_items[0].toInt();
            }
            else
            {
                for (i=0; i<wheels; i++)
                {
                    long wheel_line_num = line_num - (reset_ind_wheel + 3) - wheel_entries*i;
                    ui->spc_cur_wheel_list->addItem("Wheel " + QString::number(i));
                    ui->spc_cur_wheel_list->setCurrentRow(i);
                    switch (wheel_line_num){
                    case 0:
                        break; // header
                    case 1:
                        ui->spc_cur_wheel_initmom->setText(line_items[0]);
                        break;
                    case 2:
                        ui->spc_cur_wheel_axis_1->setText(line_items[0]);
                        ui->spc_cur_wheel_axis_2->setText(line_items[1]);
                        ui->spc_cur_wheel_axis_3->setText(line_items[2]);
                        break;
                    case 3:
                        ui->spc_cur_wheel_maxtrq->setText(line_items[0]);
                        ui->spc_cur_wheel_maxmom->setText(line_items[1]);
                        break;
                    case 4:
                        ui->spc_cur_wheel_body->setValue(line_items[0].toInt());
                        break;
                    case 5:
                        ui->spc_cur_wheel_node->setValue(line_items[0].toInt());
                        break;
                    case 6:
                        if (QString::compare(line_items[0], "NONE"))
                        {
                            ui->spc_cur_wheel_drjit_on->setCheckState(Qt::Checked);
                        }
                        else
                        {
                            ui->spc_cur_wheel_drjit_on->setCheckState(Qt::Unchecked);
                            ui->spc_cur_wheel_drjit_file->setText(line_items[0]);
                        }
                        break;
                    }
                }
            }
            reset_ind_mtb = reset_ind_wheel + 3 + wheel_entries*wheels;
        }
        else if (i > reset_ind_mtb)
        {
            long mtb_entries = 4;
            if (line_num == reset_ind_mtb + 1)
            {
                mtbs = line_items[0].toInt();
            }
            else
            {
                for (i=0; i<mtbs; i++)
                {
                    long mtb_line_num = line_num - (reset_ind_mtb + 1) - mtb_entries*i;
                    ui->spc_cur_mtb_list->addItem("MTB " + QString::number(i));
                    ui->spc_cur_mtb_list->setCurrentRow(i);
                    switch (mtb_line_num){
                    case 0:
                        break; // header
                    case 1:
                        ui->spc_cur_mtb_sat->setText(line_items[0]);
                        break;
                    case 2:
                        ui->spc_cur_mtb_axis_1->setText(line_items[0]);
                        ui->spc_cur_mtb_axis_2->setText(line_items[1]);
                        ui->spc_cur_mtb_axis_3->setText(line_items[2]);
                        break;
                    case 3:
                        ui->spc_cur_mtb_node->setValue(line_items[0].toInt());
                        break;
                    }
                }
            }
            reset_ind_thr = reset_ind_mtb + (1 + mtb_entries*mtbs);
        }
        else if (i > reset_ind_thr)
        {
            long thr_entries = 6;
            if (line_num == reset_ind_thr + 1)
            {
                thrusters = line_items[0].toInt();
            }
            else
            {
                for (i=0; i<thrusters; i++)
                {
                    long thr_line_num = line_num - (reset_ind_thr + 1) - thr_entries*i;
                    ui->spc_cur_thruster_list->addItem("Thr " + QString::number(i));
                    ui->spc_cur_thruster_list->setCurrentRow(i);
                    switch (thr_line_num){
                    case 0:
                        break; // header
                    case 1:
                        setQComboBox(ui->spc_cur_thruster_mode, line_items[0]);
                        break;
                    case 2:
                        ui->spc_cur_thruster_force->setText(line_items[0]);
                        break;
                    case 3:
                        ui->spc_cur_thruster_axis_1->setText(line_items[0]);
                        ui->spc_cur_thruster_axis_2->setText(line_items[1]);
                        ui->spc_cur_thruster_axis_3->setText(line_items[2]);
                        break;
                    case 4:
                        ui->spc_cur_thruster_body->setValue(line_items[0].toInt());
                        break;
                    case 5:
                        ui->spc_cur_thruster_node->setValue(line_items[0].toInt());
                        break;
                    }
                }
            }
            reset_ind_gyro = reset_ind_thr + (1 + thr_entries*thrusters);
        }
        else if (i > reset_ind_gyro)
        {
            long gyro_entries = 11;
            if (line_num == reset_ind_gyro + 1)
            {
                gyros = line_items[0].toInt();
            }
            else
            {
                for (i=0; i<gyros; i++)
                {
                    long gyro_line_num = line_num - (reset_ind_gyro + 1) - gyro_entries*i;
                    ui->spc_cur_gyro_list->addItem("Axis " + QString::number(i));
                    ui->spc_cur_gyro_list->setCurrentRow(i);
                    switch (gyro_line_num){
                    case 0:
                        break; // header
                    case 1:
                        ui->spc_cur_gyro_samptime->setText(line_items[0]);
                        break;
                    case 2:
                        ui->spc_cur_gyro_axis_1->setText(line_items[0]);
                        ui->spc_cur_gyro_axis_2->setText(line_items[1]);
                        ui->spc_cur_gyro_axis_3->setText(line_items[2]);
                        break;
                    case 3:
                        ui->spc_cur_gyro_maxrate->setText(line_items[0]);
                        break;
                    case 4:
                        ui->spc_cur_gyro_scaleferror->setText(line_items[0]);
                        break;
                    case 5:
                        ui->spc_cur_gyro_quant->setText(line_items[0]);
                        break;
                    case 6:
                        ui->spc_cur_gyro_angrwalk->setText(line_items[0]);
                        break;
                    case 7:
                        ui->spc_cur_gyro_bias_stab->setText(line_items[0]);
                        ui->spc_cur_gyro_bias_tspan->setText(line_items[1]);
                        break;
                    case 8:
                        ui->spc_cur_gyro_angnoise->setText(line_items[0]);
                        break;
                    case 9:
                        ui->spc_cur_gyro_initbias->setText(line_items[0]);
                        break;
                    case 10:
                        ui->spc_cur_gyro_node->setValue(line_items[0].toInt());
                        break;
                    }
                }
            }
            reset_ind_mag = reset_ind_gyro + (1 + gyro_entries*gyros);
        }
        else if (i > reset_ind_mag)
        {
            long mag_entries = 8;
            if (line_num == reset_ind_mag + 1)
            {
                mags = line_items[0].toInt();
            }
            else
            {
                for (i=0; i<mags; i++)
                {
                    long mag_line_num = line_num - (reset_ind_mag + 1) - mag_entries*i;
                    ui->spc_cur_mag_list->addItem("Axis " + QString::number(i));
                    ui->spc_cur_mag_list->setCurrentRow(i);
                    switch (mag_line_num){
                    case 0:
                        break; // header
                    case 1:
                        ui->spc_cur_mag_samptime->setText(line_items[0]);
                        break;
                    case 2:
                        ui->spc_cur_mag_axis_1->setText(line_items[0]);
                        ui->spc_cur_mag_axis_2->setText(line_items[1]);
                        ui->spc_cur_mag_axis_3->setText(line_items[2]);
                        break;
                    case 3:
                        ui->spc_cur_mag_sat->setText(line_items[0]);
                        break;
                    case 4:
                        ui->spc_cur_mag_scaleferror->setText(line_items[0]);
                        break;
                    case 5:
                        ui->spc_cur_mag_quant->setText(line_items[0]);
                        break;
                    case 6:
                        ui->spc_cur_mag_noise->setText(line_items[0]);
                        break;
                    case 7:
                        ui->spc_cur_mag_node->setValue(line_items[0].toInt());
                        break;
                    }
                }
            }
            reset_ind_css = reset_ind_mag + (1 + mag_entries*mags);
        }
        else if (i > reset_ind_css)
        {
            long css_entries = 11;
            if (line_num == reset_ind_css + 1)
            {
                css_s = line_items[0].toInt();
            }
            else
            {
                for (i=0; i<css_s; i++)
                {
                    long css_line_num = line_num - (reset_ind_css + 1) - css_entries*i;
                    ui->spc_cur_css_list->addItem("CSS " + QString::number(i));
                    ui->spc_cur_css_list->setCurrentRow(i);
                    switch (css_line_num){
                    case 0:
                        break; // header
                    case 1:
                        ui->spc_cur_css_samptime->setText(line_items[0]);
                        break;
                    case 2:
                        ui->spc_cur_css_axis_1->setText(line_items[0]);
                        ui->spc_cur_css_axis_2->setText(line_items[1]);
                        ui->spc_cur_css_axis_3->setText(line_items[2]);
                        break;
                    case 3:
                        ui->spc_cur_css_halfcone->setText(line_items[0]);
                        break;
                    case 4:
                        ui->spc_cur_css_scale->setText(line_items[0]);
                        break;
                    case 5:
                        ui->spc_cur_css_quant->setText(line_items[0]);
                        break;
                    case 6:
                        ui->spc_cur_css_body->setValue(line_items[0].toInt());
                        break;
                    case 10:
                        ui->spc_cur_css_node->setValue(line_items[0].toInt());
                        break;
                    }
                }
            }
            reset_ind_fss = reset_ind_css + (1 + css_entries*css_s);
        }
        else if (i > reset_ind_fss)
        {
            long fss_entries = 11;
            if (line_num == reset_ind_fss + 1)
            {
                fss_s = line_items[0].toInt();
            }
            else
            {
                for (i=0; i<fss_s; i++)
                {
                    long fss_line_num = line_num - (reset_ind_fss + 1) - fss_entries*i;
                    ui->spc_cur_mag_list->addItem("FSS " + QString::number(i));
                    ui->spc_cur_mag_list->setCurrentRow(i);
                    switch (fss_line_num){
                    case 0:
                        break; // header
                    case 1:
                        ui->spc_cur_fss_samptime->setText(line_items[0]);
                        break;
                    case 2:
                        ui->spc_cur_fss_mount_1->setText(line_items[0]);
                        ui->spc_cur_fss_mount_2->setText(line_items[1]);
                        ui->spc_cur_fss_mount_3->setText(line_items[2]);
                        break;
                    case 3:
                        setQComboBox(ui->spc_cur_fss_mountseq, line_items[3]);
                        break;
                    case 4:
                        ui->spc_cur_fss_hfov->setText(line_items[0]);
                        ui->spc_cur_fss_vfov->setText(line_items[1]);
                        break;
                    case 5:
                        ui->spc_cur_fss_noiseang->setText(line_items[0]);
                        break;
                    case 6:
                        ui->spc_cur_fss_quant->setText(line_items[0]);
                        break;
                    case 10:
                        ui->spc_cur_fss_node->setValue(line_items[0].toInt());
                        break;
                    }
                }
            }
            reset_ind_strack += reset_ind_fss + (1 + fss_entries*fss_s);
        }
        else if (i > reset_ind_strack)
        {
            long strack_entries = 11;
            if (line_num == reset_ind_strack + 1)
            {
                stracks = line_items[0].toInt();
            }
            else
            {
                for (i=0; i<stracks; i++)
                {
                    long star_line_num = line_num - (reset_ind_strack + 1) - strack_entries*i;
                    ui->spc_cur_mag_list->addItem("ST " + QString::number(i));
                    ui->spc_cur_mag_list->setCurrentRow(i);
                    switch (star_line_num){
                    case 0:
                        break; // header
                    case 1:
                        ui->spc_cur_strack_samptime->setText(line_items[0]);
                        break;
                    case 2:
                        ui->spc_cur_strack_mount_1->setText(line_items[0]);
                        ui->spc_cur_strack_mount_2->setText(line_items[1]);
                        ui->spc_cur_strack_mount_3->setText(line_items[2]);
                        break;
                    case 3:
                        setQComboBox(ui->spc_cur_strack_mountseq, line_items[3]);
                        break;
                    case 4:
                        ui->spc_cur_strack_hfov->setText(line_items[0]);
                        ui->spc_cur_strack_vfov->setText(line_items[1]);
                        break;
                    case 5:
                        ui->spc_cur_strack_sun->setText(line_items[0]);
                        ui->spc_cur_strack_earth->setText(line_items[1]);
                        ui->spc_cur_strack_moon->setText(line_items[2]);
                        break;
                    case 6:
                        ui->spc_cur_strack_noiseang_1->setText(line_items[0]);
                        ui->spc_cur_strack_noiseang_2->setText(line_items[1]);
                        ui->spc_cur_strack_noiseang_3->setText(line_items[2]);
                        break;
                    case 10:
                        ui->spc_cur_strack_node->setValue(line_items[0].toInt());
                        break;
                    }
                }
            }
            reset_ind_gps = reset_ind_strack + (1 + strack_entries*stracks);
        }
        else if (i > reset_ind_gps)
        {
            long gps_entries = 6;
            if (line_num == reset_ind_gps + 1)
            {
                gps_s = line_items[0].toInt();
            }
            else
            {
                for (i=0; i<gps_s; i++)
                {
                    long gps_line_num = line_num - (reset_ind_gps + 1) - gps_entries*i;
                    ui->spc_cur_mag_list->addItem("Mag " + QString::number(i));
                    ui->spc_cur_mag_list->setCurrentRow(i);
                    switch (gps_line_num){
                    case 0:
                        break; // header
                    case 1:
                        ui->spc_cur_gps_samptime->setText(line_items[0]);
                        break;
                    case 2:
                        ui->spc_cur_gps_posnoise->setText(line_items[0]);
                        break;
                    case 3:
                        ui->spc_cur_gps_velnoise->setText(line_items[0]);
                        break;
                    case 4:
                        ui->spc_cur_gps_timenoise->setText(line_items[0]);
                        break;
                    case 5:
                        ui->spc_cur_gps_node->setValue(line_items[0].toInt());
                        break;
                    }
                }
            }
            reset_ind_acc = reset_ind_gps + (1 + gps_entries*gps_s);
        }
        else if (i > reset_ind_acc)
        {
            long acc_entries = 11;
            if (line_num == reset_ind_acc + 1)
            {
                accels = line_items[0].toInt();
            }
            else
            {
                for (i=0; i<accels; i++)
                {
                    long accel_line_num = line_num - (1 + reset_ind_acc) - acc_entries*i;
                    ui->spc_cur_mag_list->addItem("Mag " + QString::number(i));
                    ui->spc_cur_mag_list->setCurrentRow(i);
                    switch (accel_line_num){
                    case 0:
                        break; // header
                    case 1:
                        ui->spc_cur_acc_samptime->setText(line_items[0]);
                        break;
                    case 2:
                        ui->spc_cur_acc_axis_1->setText(line_items[0]);
                        ui->spc_cur_acc_axis_2->setText(line_items[1]);
                        ui->spc_cur_acc_axis_3->setText(line_items[2]);
                        break;
                    case 3:
                        ui->spc_cur_acc_maxacc->setText(line_items[0]);
                        break;
                    case 4:
                        ui->spc_cur_acc_scaleerror->setText(line_items[0]);
                        break;
                    case 5:
                        ui->spc_cur_acc_quant->setText(line_items[0]);
                        break;
                    case 6:
                        ui->spc_cur_acc_dvrandwalk->setText(line_items[0]);
                        break;
                    case 7:
                        ui->spc_cur_acc_bias_stab->setText(line_items[0]);
                        ui->spc_cur_acc_bias_tspan->setText(line_items[1]);
                        break;
                    case 8:
                        ui->spc_cur_acc_dvnoise->setText(line_items[0]);
                        break;
                    case 9:
                        ui->spc_cur_acc_initbias->setText(line_items[0]);
                        break;
                    case 10:
                        ui->spc_cur_acc_node->setValue(line_items[0].toInt());
                        break;
                    }
                }
            }
        }
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



