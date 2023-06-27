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
#include <QList>

SPC_submenu::SPC_submenu(QWidget *parent):
    QDialog(parent),
    ui(new Ui::SPC_submenu)
{
    ui->setupUi(this);
    set_validators();
    ui->sections->setCurrentIndex(0);
    ui->actuator_sections->setCurrentIndex(0);
    ui->sensor_sections->setCurrentIndex(0);
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

void SPC_submenu::set_validators()
{

    // Fixed
    ui->spc_cur_orb_type->addItems(dsm_gui_lib::sortStringList(orbit_type));
    ui->spc_cur_pos_ref->addItems(dsm_gui_lib::sortStringList(pos_wrt_F));

    ui->spc_cur_angvel_frame1->addItems(dsm_gui_lib::sortStringList(angvel_wrt));
    ui->spc_cur_angvel_frame2->addItems(dsm_gui_lib::sortStringList(angvel_wrt));
    ui->spc_cur_att_param->addItems(dsm_gui_lib::sortStringList(att_params));

    ui->spc_cur_initeul_seq->addItems(dsm_gui_lib::sortStringList(euler_seq));

    ui->spc_cur_solver->addItems(dsm_gui_lib::sortStringList(solve_dynamics));

    ui->spc_cur_masspropref->addItems(dsm_gui_lib::sortStringList(mass_prop_ref));

    // Body - N/A
    // Joints
    ui->spc_cur_joint_type->addItems(dsm_gui_lib::sortStringList(joint_type));

    ui->spc_cur_joint_rotdof_seq->addItems(dsm_gui_lib::sortStringList(euler_seq));
    ui->spc_cur_joint_rottype->addItems(dsm_gui_lib::sortStringList(rot_joint_seqtype));

    ui->spc_cur_joint_bigi_seq->addItems(dsm_gui_lib::sortStringList(euler_seq));
    ui->spc_cur_joint_bogo_seq->addItems(dsm_gui_lib::sortStringList(euler_seq));

    ui->spc_cur_joint_trndof_seq->addItems(dsm_gui_lib::sortStringList(euler_seq));

    // Wheels - N/A
    // MTBs - N/A
    // Thrusters
    ui->spc_cur_thruster_mode->addItems(dsm_gui_lib::sortStringList(thruster_mode));

    // Gyro - N/A
    // Magnetometer - N/A
    // CSS - N/A
    // FSS
    ui-> spc_cur_fss_boreaxis->addItems(dsm_gui_lib::sortStringList(axis));

    // Star Tracker
    ui->spc_cur_strack_mountseq->addItems(dsm_gui_lib::sortStringList(euler_seq));
    ui->spc_cur_strack_boreaxis->addItems(dsm_gui_lib::sortStringList(axis));

    // No more in following sections

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

    QStringList tmp_list = {};

    long cur_item;
    long cur_entry;

    QStringList tmp_line_item;
    QStringList tmp_data = {};


    ui->spc_cur_name_sub->setText(spc_cur_name);
    for(int line_num=8; line_num<reset_ind_body; line_num++)
    {
        line_string = spc_string[line_num-1];
        line_items = spc_data[line_num-1].split(QRegExp("\\s"), Qt::SkipEmptyParts);

        switch (line_num) {
            /******************************************* HEADER ***************************************************/
            // Orbit Parameters
        case 8: // Orbit Prop
            setQComboBox(ui->spc_cur_orb_type, line_items[0]);
            break;
        case 9: // Pos of CM or ORIGIN wrt F
            setQComboBox(ui->spc_cur_pos_ref, line_items[0]);
            break;
        case 10: // Pos wrt Formation (m) expressed in F
            ui->spc_cur_xpos_form->setText(line_items[0]);
            ui->spc_cur_ypos_form->setText(line_items[1]);
            ui->spc_cur_zpos_form->setText(line_items[2]);
            break;
        case 11: // Vel wrt Formation (m) expressed in F
            ui->spc_cur_xvel_form->setText(line_items[0]);
            ui->spc_cur_yvel_form->setText(line_items[1]);
            ui->spc_cur_zvel_form->setText(line_items[2]);
            break;
        case 12: // Initial Attitude Header
            break;
        case 13: // Ang Vel wrt [NL], Att [QA] wrt [NLF]
            line_items = spc_data[line_num-1].split(QRegExp(""), Qt::SkipEmptyParts);
            setQComboBox(ui->spc_cur_angvel_frame1, line_items[0]);
            setQComboBox(ui->spc_cur_angvel_frame2, line_items[2]);

            setQComboBox(ui->spc_cur_att_param, line_items[1]);
            break;
        case 14:
            ui->spc_cur_angvel_x->setText(line_items[0]);
            ui->spc_cur_angvel_y->setText(line_items[1]);
            ui->spc_cur_angvel_z->setText(line_items[2]);
            break;
        case 15:
            ui->spc_cur_q1->setText(line_items[0]);
            ui->spc_cur_q2->setText(line_items[1]);
            ui->spc_cur_q3->setText(line_items[2]);
            ui->spc_cur_q4->setText(line_items[3]);

            if (!QString::compare(ui->spc_cur_att_param->currentText(), "Q"))
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
        case 16:
            ui->spc_cur_initeul_1->setText(line_items[0]);
            ui->spc_cur_initeul_2->setText(line_items[1]);
            ui->spc_cur_initeul_3->setText(line_items[2]);

            setQComboBox(ui->spc_cur_initeul_seq, line_items[3]);

            if (!QString::compare(ui->spc_cur_att_param->currentText(), "Q"))
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
        case 17: // Dynamics Flags Header
            break;
        case 18:
            setQComboBox(ui->spc_cur_solver, line_items[0]);
            break;
        case 19: // Compute constraint forces and torques
            if (!QString::compare(line_items[0], "TRUE")) ui->spc_cur_con_on->setChecked(true);
            else ui->spc_cur_con_off->setChecked(true);
            break;
        case 20: // mass props referenced to ...
            setQComboBox(ui->spc_cur_masspropref, line_items[0]);
        case 21: // Flex active
            if (!QString::compare(line_items[0], "TRUE")) ui->spc_cur_flex_on->setChecked(true);
            else ui->spc_cur_flex_off->setChecked(true);
            break;
        case 22: // include 2nd order flex terms
            if (!QString::compare(line_items[0], "TRUE")) ui->spc_cur_2flex_on->setChecked(true);
            else ui->spc_cur_2flex_off->setChecked(true);

        case 23: // Shaker file name
            if (!QString::compare(line_items[0], "NONE"))
            {
                ui->spc_cur_shaker_enab->setCheckState(Qt::Checked);
                ui->spc_cur_shaker_file->setText("");
                ui->spc_cur_shaker_file->setEnabled(false);
            }
            else
            {
                ui->spc_cur_shaker_enab->setCheckState(Qt::Unchecked);
                ui->spc_cur_shaker_file->setText(line_items[0]);
                ui->spc_cur_shaker_file->setEnabled(true);
            }
            break;
        case 24: // Drag coefficient
            ui->spc_cur_drag->setText(line_items[0]);
            break;
        case 25: // header
            break;
        case 26: // header
            break;
        case 27: // header
            break;
        case 28: // number of bodies
            bodies = line_items[0].toInt();
            break;
        }
    }

    reset_ind_joint = reset_ind_body + body_entries*bodies;

    ui->spc_cur_body_list->clear();
    tmp_data.clear();

    for (int line_num = reset_ind_body; line_num<reset_ind_joint; line_num++)
    {
        line_string = spc_string[line_num-1];
        line_items = spc_data[line_num-1].split(QRegExp("\\s"), Qt::SkipEmptyParts);

        long body_line_num = line_num - reset_ind_body;
        cur_item = floor(body_line_num/body_entries);
        cur_entry = body_line_num % body_entries;

        if (cur_entry == 0){
            ui->spc_cur_body_list->addItem("Body " + QString::number(cur_item));
        }

        switch (cur_entry) {
        case 0: // Body X Header
            tmp_data.append("blankline");
            break;
        case 1: // Mass
            tmp_data.append(line_items[0]);

            break;
        case 2: // Moments of Inertia (kg-m^2)
            tmp_data.append(line_items[0]);
            tmp_data.append(line_items[1]);
            tmp_data.append(line_items[2]);

            break;
        case 3: // Products of Inertia
            tmp_data.append(line_items[0]);
            tmp_data.append(line_items[1]);
            tmp_data.append(line_items[2]);

            break;
        case 4: // Location of mass center
            tmp_data.append(line_items[0]);
            tmp_data.append(line_items[1]);
            tmp_data.append(line_items[2]);

            break;
        case 5: // constant embedded momentum
            tmp_data.append(line_items[0]);
            tmp_data.append(line_items[1]);
            tmp_data.append(line_items[2]);

            break;
        case 6: // constant embedded momentum
            tmp_data.append(line_items[0]);
            tmp_data.append(line_items[1]);
            tmp_data.append(line_items[2]);

            break;
        case 7:// Geometry File
            tmp_data.append(line_items[0]);

            break;
        case 8: // Node
            tmp_data.append(line_items[0]);
            if (!QString::compare(line_items[0], "NONE"))
            {
                ui->spc_cur_node_enab->setCheckState(Qt::Checked);
            }
            else
            {
                ui->spc_cur_node_enab->setCheckState(Qt::Unchecked);
                ui->spc_cur_node_file->setText(line_items[0]);
            }
            break;
        case 9: // Flex
            tmp_data.append(line_items[0]);
            if (!QString::compare(line_items[0], "NONE"))
            {
                ui->spc_cur_flex_enab->setCheckState(Qt::Checked);
                ui->spc_cur_flex_file->setEnabled(false);
            }
            else
            {
                ui->spc_cur_flex_enab->setCheckState(Qt::Unchecked);
                ui->spc_cur_flex_file->setEnabled(true);
                ui->spc_cur_flex_file->setText(line_items[0]);
            }
            break;
        }
        if (cur_entry==body_entries-1){
            ui->spc_cur_body_list->setCurrentRow(cur_item);
            ui->spc_cur_body_list->currentItem()->setData(0, "Body " + QString::number(cur_item));
            ui->spc_cur_body_list->currentItem()->setData(1, tmp_data);
            tmp_data.clear();
        }
    }

    /***************** JOINTS **************************/

    joints = bodies - 1;

    ui->spc_cur_joint_list->clear();
    tmp_data.clear();

    if (joints == 0) reset_ind_wheel = reset_ind_joint + joint_headers + joint_entries; // SC_Simple has an example joint
    else reset_ind_wheel = reset_ind_joint + joint_headers + joint_entries*joints;

    if (joints > 0){
        for (int line_num = reset_ind_joint + joint_headers; line_num<reset_ind_wheel; line_num++)
        {
            line_string = spc_string[line_num-1];
            line_items = spc_data[line_num-1].split(QRegExp("\\s"), Qt::SkipEmptyParts);

            long joint_line_num = line_num - reset_ind_joint - joint_headers;
            cur_item = floor(joint_line_num/joint_entries);
            cur_entry = joint_line_num % joint_entries;

            if (cur_entry == 0){
                ui->spc_cur_joint_list->addItem("Joint " + QString::number(cur_item));
            }

            switch (cur_entry) {
            case 0: // Joint X Header
                tmp_data.append("blankline");
                break;
            case 1: // Joint Type
                tmp_data.append(line_items[0]);
                break;
            case 2: // Joint Connections (inner and outer body)
                tmp_data.append(line_items[0]);
                tmp_data.append(line_items[1]);
                break;
            case 3: // RotDOF, RotDOF Seq, Rot Type
                tmp_data.append(line_items[0]);
                tmp_data.append(line_items[1]);
                tmp_data.append(line_items[2]);
                break;
            case 4: // Trn DOF, Trn Seq
                tmp_data.append(line_items[0]);
                tmp_data.append(line_items[1]);
                break;
            case 5: // Rotational Axes Locked?
                tmp_data.append(line_items[0]);
                tmp_data.append(line_items[1]);
                tmp_data.append(line_items[2]);

                break;

            case 6: // Translational Axes Locked?
                tmp_data.append(line_items[0]);
                tmp_data.append(line_items[1]);
                tmp_data.append(line_items[2]);

                break;
            case 7: // Joint initial angle
                tmp_data.append(line_items[0]);
                tmp_data.append(line_items[1]);
                tmp_data.append(line_items[2]);

                break;
            case 8: // Joint initial angle rate
                tmp_data.append(line_items[0]);
                tmp_data.append(line_items[1]);
                tmp_data.append(line_items[2]);

                break;
            case 9: // Joint initial displacement
                tmp_data.append(line_items[0]);
                tmp_data.append(line_items[1]);
                tmp_data.append(line_items[2]);

                break;
            case 10: // Joint initial velocity
                tmp_data.append(line_items[0]);
                tmp_data.append(line_items[1]);
                tmp_data.append(line_items[2]);

                break;
            case 11: // Joint Bi Gi angles, sequence
                tmp_data.append(line_items[0]);
                tmp_data.append(line_items[1]);
                tmp_data.append(line_items[2]);
                tmp_data.append(line_items[3]);

                break;
            case 12: // Joint Go Bo angles, sequence
                tmp_data.append(line_items[0]);
                tmp_data.append(line_items[1]);
                tmp_data.append(line_items[2]);
                tmp_data.append(line_items[3]);

                break;
            case 13: // Joint Position wrt inner body
                tmp_data.append(line_items[0]);
                tmp_data.append(line_items[1]);
                tmp_data.append(line_items[2]);

                break;
            case 14: // Joint Position wrt outer body
                tmp_data.append(line_items[0]);
                tmp_data.append(line_items[1]);
                tmp_data.append(line_items[2]);

                break;
            case 15: // Joint parameter file
                tmp_data.append(line_items[0]);
                if (!QString::compare(line_items[0], "NONE"))
                {
                    ui->spc_cur_joint_param_none->setCheckState(Qt::Checked);
                }
                else
                {
                    ui->spc_cur_joint_param_none->setCheckState(Qt::Unchecked);
                    ui->spc_cur_joint_param_file->setText(line_items[0]);
                }
                break;
            }
            if (cur_entry==joint_entries-1){
                ui->spc_cur_joint_list->setCurrentRow(cur_item);
                ui->spc_cur_joint_list->currentItem()->setData(0, "Joint " + QString::number(cur_item));
                ui->spc_cur_joint_list->currentItem()->setData(1, tmp_data);
                tmp_data.clear();
            }
        }
    }

    /********************** WHEELS *************************/

    wheels = spc_data[reset_ind_wheel].toInt();

    if (wheels == 0) reset_ind_mtb = reset_ind_wheel + wheel_headers + wheel_entries; // SC_Simple has an example wheel
    else reset_ind_mtb = reset_ind_wheel + wheel_headers + wheel_entries*wheels;

    ui->spc_cur_wheel_list->clear();
    tmp_data.clear();
    if (wheels > 0){
        for (int line_num = reset_ind_wheel + wheel_headers; line_num<reset_ind_mtb; line_num++)
        {
            line_string = spc_string[line_num-1];
            line_items = spc_data[line_num-1].split(QRegExp("\\s"), Qt::SkipEmptyParts);

            long wheel_line_num = line_num - reset_ind_wheel - wheel_headers;
            cur_item = floor(wheel_line_num/wheel_entries);
            cur_entry = wheel_line_num % wheel_entries;

            if (cur_entry == 0){
                ui->spc_cur_wheel_list->addItem("Wheel " + QString::number(cur_item));
            }

            switch (cur_entry){
            case 0:
                tmp_data.append("blankline");
                break; // header
            case 1:
                tmp_data.append(line_items[0]);
                break; // wheel axis
            case 2:
                tmp_data.append(line_items[0]);
                tmp_data.append(line_items[1]);
                tmp_data.append(line_items[2]);
                break;
            case 3: // Max torque, max momentum
                tmp_data.append(line_items[0]);
                tmp_data.append(line_items[1]);
                break;
            case 4: // wheel inertia
                tmp_data.append(line_items[0]);
                break;
            case 5: // wheel body
                tmp_data.append(line_items[0]);
                break;
            case 6: // wheel node
                tmp_data.append(line_items[0]);
                break;
            case 7: // wheel drag/jitter file
                tmp_data.append(line_items[0]);
                if (!QString::compare(line_items[0], "NONE"))
                {
                    ui->spc_cur_wheel_drjit_on->setCheckState(Qt::Checked);
                }
                else
                {
                    ui->spc_cur_wheel_drjit_on->setCheckState(Qt::Unchecked);
                    ui->spc_cur_joint_param_file->setText(line_items[0]);
                }
                break;
            }
            if (cur_entry==wheel_entries-1){
                ui->spc_cur_wheel_list->setCurrentRow(cur_item);
                ui->spc_cur_wheel_list->currentItem()->setData(0, "Wheel " + QString::number(cur_item));
                ui->spc_cur_wheel_list->currentItem()->setData(1, tmp_data);
                tmp_data.clear();
            }
        }
    }

    /**************** MTBS ************************/

    mtbs = spc_data[reset_ind_mtb].toInt();

    if (mtbs == 0) reset_ind_thr = reset_ind_mtb + mtb_headers + mtb_entries; // SC_Simple has an example wheel
    else reset_ind_thr = reset_ind_mtb + mtb_headers + mtb_entries*mtbs;


    ui->spc_cur_mtb_list->clear();
    tmp_data.clear();
    if (mtbs > 0){
        for (int line_num = reset_ind_mtb + mtb_headers; line_num<reset_ind_thr; line_num++)
        {
            line_string = spc_string[line_num-1];
            line_items = spc_data[line_num-1].split(QRegExp("\\s"), Qt::SkipEmptyParts);

            long mtb_line_num = line_num - reset_ind_mtb - mtb_headers;
            cur_item = floor(mtb_line_num/mtb_entries);
            cur_entry = mtb_line_num % mtb_entries;

            if (cur_entry == 0){
                ui->spc_cur_mtb_list->addItem("MTB " + QString::number(cur_item));
            }

            switch (cur_entry){
            case 0:
                tmp_data.append("blankline");
                break; // header
            case 1:
                tmp_data.append(line_items[0]);
                break;
            case 2:
                tmp_data.append(line_items[0]);
                tmp_data.append(line_items[1]);
                tmp_data.append(line_items[2]);
                break;
            case 3:
                tmp_data.append(line_items[0]);
                break;
            }
            if (cur_entry==mtb_entries-1){
                ui->spc_cur_mtb_list->setCurrentRow(cur_item);
                ui->spc_cur_mtb_list->currentItem()->setData(0, "MTB " + QString::number(cur_item));
                ui->spc_cur_mtb_list->currentItem()->setData(1, tmp_data);
                tmp_data.clear();
            }
        }
    }

    /**************** THRUSTERS ************************/

    thrusters = spc_data[reset_ind_thr].toInt();

    if (thrusters == 0) reset_ind_gyro = reset_ind_thr + thr_headers + thr_entries; // SC_Simple has an example wheel
    else reset_ind_thr = reset_ind_thr + thr_headers + thr_entries*thrusters;


    ui->spc_cur_thruster_list->clear();
    tmp_data.clear();
    if (thrusters > 0){
        for (int line_num = reset_ind_mtb + mtb_headers; line_num<reset_ind_thr; line_num++)
        {
            line_string = spc_string[line_num-1];
            line_items = spc_data[line_num-1].split(QRegExp("\\s"), Qt::SkipEmptyParts);

            long thr_line_num = line_num - reset_ind_thr - thr_headers;
            cur_item = floor(thr_line_num/thr_entries);
            cur_entry = thr_line_num % thr_entries;

            if (cur_entry == 0){
                ui->spc_cur_thruster_list->addItem("Thr " + QString::number(cur_item));
            }

            switch (cur_entry){
            case 0:
                tmp_data.append("blankline");
                break; // header
            case 1:
                tmp_data.append(line_items[0]);
                break;
            case 2:
                tmp_data.append(line_items[0]);
            case 3:
                tmp_data.append(line_items[0]);
                tmp_data.append(line_items[1]);
                tmp_data.append(line_items[2]);
                break;
            case 4:
                tmp_data.append(line_items[0]);
                break;
            case 5:
                tmp_data.append(line_items[0]);
                break;
            }
            if (cur_entry==mtb_entries-1){
                ui->spc_cur_thruster_list->setCurrentRow(cur_item);
                ui->spc_cur_thruster_list->currentItem()->setData(0, "thr " + QString::number(cur_item));
                ui->spc_cur_thruster_list->currentItem()->setData(1, tmp_data);
                tmp_data.clear();
            }
        }
    }

//    tmp_line_item = spc_data[reset_ind_thr + 1 - 1].split(QRegExp("\\s"), Qt::SkipEmptyParts);
//    thrusters = tmp_line_item[0].toInt();

//    ui->spc_cur_thruster_list->clear();
//    tmp_list.clear();

//    for (int i=0; i<thrusters; i++) tmp_list.append("Thr " + QString::number(i));
//    ui->spc_cur_thruster_list->addItems(tmp_list);

//    if (thrusters == 0) thrusters = 1;

//    reset_ind_gyro = reset_ind_thr + thr_headers + thr_entries*thrusters;

//    for (int line_num = reset_ind_thr + thr_headers; line_num<reset_ind_gyro; line_num++)
//    {
//        line_string = spc_string[line_num-1];
//        line_items = spc_data[line_num-1].split(QRegExp("\\s"), Qt::SkipEmptyParts);

//        long thr_line_num = line_num - reset_ind_thr - thr_headers;
//        cur_item = floor(thr_line_num/thr_entries);
//        cur_entry = thr_line_num % thr_entries;

//        ui->spc_cur_thruster_list->setCurrentRow(cur_item);

//        switch (cur_entry){
//        case 0:
//            break; // header
//        case 1:
//            setQComboBox(ui->spc_cur_thruster_mode, line_items[0]);
//            break;
//        case 2:
//            ui->spc_cur_thruster_force->setText(line_items[0]);
//            break;
//        case 3:
//            ui->spc_cur_thruster_axis_1->setText(line_items[0]);
//            ui->spc_cur_thruster_axis_2->setText(line_items[1]);
//            ui->spc_cur_thruster_axis_3->setText(line_items[2]);
//            break;
//        case 4:
//            ui->spc_cur_thruster_body->setValue(line_items[0].toInt());
//            break;
//        case 5:
//            ui->spc_cur_thruster_node->setValue(line_items[0].toInt());
//            break;
//        }
//    }

//    tmp_line_item = spc_data[reset_ind_gyro + 1 - 1].split(QRegExp("\\s"), Qt::SkipEmptyParts);
//    gyros = tmp_line_item[0].toInt();

//    ui->spc_cur_gyro_list->clear();
//    tmp_list.clear();

//    for (int i=0; i<gyros; i++) tmp_list.append("Axis " + QString::number(i));
//    ui->spc_cur_gyro_list->addItems(tmp_list);

//    if (gyros == 0) gyros = 1;
//    reset_ind_mag = reset_ind_gyro + gyro_headers + gyro_entries*gyros;

//    for (int line_num = reset_ind_gyro + gyro_headers; line_num<reset_ind_mag; line_num++)
//    {
//        line_string = spc_string[line_num-1];
//        line_items = spc_data[line_num-1].split(QRegExp("\\s"), Qt::SkipEmptyParts);

//        long gyro_line_num = line_num - reset_ind_gyro - gyro_headers;
//        cur_item = floor(gyro_line_num/gyro_entries);
//        cur_entry = gyro_line_num % gyro_entries;

//        ui->spc_cur_gyro_list->setCurrentRow(cur_item);

//        switch (cur_entry){
//        case 0:
//            break; // header
//        case 1:
//            ui->spc_cur_gyro_samptime->setText(line_items[0]);
//            break;
//        case 2:
//            ui->spc_cur_gyro_axis_1->setText(line_items[0]);
//            ui->spc_cur_gyro_axis_2->setText(line_items[1]);
//            ui->spc_cur_gyro_axis_3->setText(line_items[2]);
//            break;
//        case 3:
//            ui->spc_cur_gyro_maxrate->setText(line_items[0]);
//            break;
//        case 4:
//            ui->spc_cur_gyro_scaleferror->setText(line_items[0]);
//            break;
//        case 5:
//            ui->spc_cur_gyro_quant->setText(line_items[0]);
//            break;
//        case 6:
//            ui->spc_cur_gyro_angrwalk->setText(line_items[0]);
//            break;
//        case 7:
//            ui->spc_cur_gyro_bias_stab->setText(line_items[0]);
//            ui->spc_cur_gyro_bias_tspan->setText(line_items[1]);
//            break;
//        case 8:
//            ui->spc_cur_gyro_angnoise->setText(line_items[0]);
//            break;
//        case 9:
//            ui->spc_cur_gyro_initbias->setText(line_items[0]);
//            break;
//        case 10:
//            ui->spc_cur_gyro_node->setValue(line_items[0].toInt());
//            break;
//        }
//    }

//    tmp_line_item = spc_data[reset_ind_mag + 1 - 1].split(QRegExp("\\s"), Qt::SkipEmptyParts);
//    mags = tmp_line_item[0].toInt();

//    ui->spc_cur_mag_list->clear();
//    tmp_list.clear();

//    for (int i=0; i<mags; i++) tmp_list.append("Axis " + QString::number(i));
//    ui->spc_cur_mag_list->addItems(tmp_list);

//    if (mags == 0) mags = 1;
//    reset_ind_css = reset_ind_mag + mag_headers + mag_entries*mags;

//    for (int line_num = reset_ind_mag + mag_headers; line_num<reset_ind_css; line_num++)
//    {
//        line_string = spc_string[line_num-1];
//        line_items = spc_data[line_num-1].split(QRegExp("\\s"), Qt::SkipEmptyParts);

//        long mag_line_num = line_num - reset_ind_mag - mag_headers;
//        cur_item = floor(mag_line_num/mag_entries);
//        cur_entry = mag_line_num % mag_entries;

//        ui->spc_cur_mag_list->setCurrentRow(cur_item);

//        switch (cur_entry){
//        case 0:
//            break; // header
//        case 1:
//            ui->spc_cur_mag_samptime->setText(line_items[0]);
//            break;
//        case 2:
//            ui->spc_cur_mag_axis_1->setText(line_items[0]);
//            ui->spc_cur_mag_axis_2->setText(line_items[1]);
//            ui->spc_cur_mag_axis_3->setText(line_items[2]);
//            break;
//        case 3:
//            ui->spc_cur_mag_sat->setText(line_items[0]);
//            break;
//        case 4:
//            ui->spc_cur_mag_scaleferror->setText(line_items[0]);
//            break;
//        case 5:
//            ui->spc_cur_mag_quant->setText(line_items[0]);
//            break;
//        case 6:
//            ui->spc_cur_mag_noise->setText(line_items[0]);
//            break;
//        case 7:
//            ui->spc_cur_mag_node->setValue(line_items[0].toInt());
//            break;
//        }
//    }

//    tmp_line_item = spc_data[reset_ind_css + 1 - 1].split(QRegExp("\\s"), Qt::SkipEmptyParts);
//    css_s = tmp_line_item[0].toInt();

//    ui->spc_cur_css_list->clear();
//    tmp_list.clear();

//    for (int i=0; i<css_s; i++) tmp_list.append("CSS " + QString::number(i));
//    ui->spc_cur_css_list->addItems(tmp_list);

//    if (css_s == 0) css_s = 1;

//    reset_ind_fss = reset_ind_css + css_headers + css_entries*css_s;

//    for (int line_num = reset_ind_css + css_headers; line_num<reset_ind_fss; line_num++)
//    {
//        line_string = spc_string[line_num-1];
//        line_items = spc_data[line_num-1].split(QRegExp("\\s"), Qt::SkipEmptyParts);

//        long css_line_num = line_num - reset_ind_css - css_headers;
//        cur_item = floor(css_line_num/css_entries);
//        cur_entry = css_line_num % css_entries;

//        ui->spc_cur_css_list->setCurrentRow(cur_item);

//        switch (cur_entry){
//        case 0:
//            break; // header
//        case 1:
//            ui->spc_cur_css_samptime->setText(line_items[0]);
//            break;
//        case 2:
//            ui->spc_cur_css_axis_1->setText(line_items[0]);
//            ui->spc_cur_css_axis_2->setText(line_items[1]);
//            ui->spc_cur_css_axis_3->setText(line_items[2]);
//            break;
//        case 3:
//            ui->spc_cur_css_halfcone->setText(line_items[0]);
//            break;
//        case 4:
//            ui->spc_cur_css_scale->setText(line_items[0]);
//            break;
//        case 5:
//            ui->spc_cur_css_quant->setText(line_items[0]);
//            break;
//        case 6:
//            ui->spc_cur_css_body->setValue(line_items[0].toInt());
//            break;
//        case 7:
//            ui->spc_cur_css_node->setValue(line_items[0].toInt());
//            break;
//        }
//    }

//    tmp_line_item = spc_data[reset_ind_fss + 1 - 1].split(QRegExp("\\s"), Qt::SkipEmptyParts);
//    fss_s = tmp_line_item[0].toInt();

//    ui->spc_cur_fss_list->clear();
//    tmp_list.clear();

//    for (int i=0; i<fss_s; i++) tmp_list.append("FSS " + QString::number(i));
//    ui->spc_cur_fss_list->addItems(tmp_list);

//    if (fss_s == 0) fss_s = 1;

//    reset_ind_strack = reset_ind_fss + fss_headers + fss_entries*fss_s;

//    for (int line_num = reset_ind_fss + fss_headers; line_num<reset_ind_strack; line_num++)
//    {
//        line_string = spc_string[line_num-1];
//        line_items = spc_data[line_num-1].split(QRegExp("\\s"), Qt::SkipEmptyParts);

//        long fss_line_num = line_num - reset_ind_fss - fss_headers;
//        cur_item = floor(fss_line_num/fss_entries);
//        cur_entry = fss_line_num % fss_entries;

//        ui->spc_cur_fss_list->setCurrentRow(cur_item);

//        switch (cur_entry){
//        case 0:
//            break; // header
//        case 1:
//            ui->spc_cur_fss_samptime->setText(line_items[0]);
//            break;
//        case 2:
//            ui->spc_cur_fss_mount_1->setText(line_items[0]);
//            ui->spc_cur_fss_mount_2->setText(line_items[1]);
//            ui->spc_cur_fss_mount_3->setText(line_items[2]);

//            setQComboBox(ui->spc_cur_fss_mountseq, line_items[0]);
//            break;
//        case 3:
//            setQComboBox(ui->spc_cur_fss_boreaxis, line_items[0]);
//            break;
//        case 4:
//            ui->spc_cur_fss_hfov->setText(line_items[0]);
//            ui->spc_cur_fss_vfov->setText(line_items[1]);
//            break;
//        case 5:
//            ui->spc_cur_fss_noiseang->setText(line_items[0]);
//            break;
//        case 6:
//            ui->spc_cur_fss_quant->setText(line_items[0]);
//            break;
//        case 7:
//            ui->spc_cur_fss_node->setValue(line_items[0].toInt());
//            break;
//        }
//    }

//    tmp_line_item = spc_data[reset_ind_strack + 1 - 1].split(QRegExp("\\s"), Qt::SkipEmptyParts);
//    stracks = tmp_line_item[0].toInt();

//    ui->spc_cur_strack_list->clear();
//    tmp_list.clear();

//    for (int i=0; i<stracks; i++) tmp_list.append("ST " + QString::number(i));
//    ui->spc_cur_strack_list->addItems(tmp_list);

//    if (stracks == 0) stracks = 1;

//    reset_ind_gps = reset_ind_strack + strack_headers +strack_entries*stracks;

//    for (int line_num = reset_ind_strack + strack_headers; line_num<reset_ind_gps; line_num++)
//    {
//        line_string = spc_string[line_num-1];
//        line_items = spc_data[line_num-1].split(QRegExp("\\s"), Qt::SkipEmptyParts);

//        long strack_line_num = line_num - reset_ind_strack - strack_headers;
//        cur_item = floor(strack_line_num/strack_entries);
//        cur_entry = strack_line_num % strack_entries;

//        ui->spc_cur_strack_list->setCurrentRow(cur_item);

//        switch (cur_entry){
//        case 0:
//            break; // header
//        case 1:
//            ui->spc_cur_strack_samptime->setText(line_items[0]);
//            break;
//        case 2:
//            ui->spc_cur_strack_mount_1->setText(line_items[0]);
//            ui->spc_cur_strack_mount_2->setText(line_items[1]);
//            ui->spc_cur_strack_mount_3->setText(line_items[2]);

//            setQComboBox(ui->spc_cur_strack_mountseq, line_items[3]);
//            break;
//        case 3:
//            setQComboBox(ui->spc_cur_strack_boreaxis, line_items[3]);
//            break;
//        case 4:
//            ui->spc_cur_strack_hfov->setText(line_items[0]);
//            ui->spc_cur_strack_vfov->setText(line_items[1]);
//            break;
//        case 5:
//            ui->spc_cur_strack_sun->setText(line_items[0]);
//            ui->spc_cur_strack_earth->setText(line_items[1]);
//            ui->spc_cur_strack_moon->setText(line_items[2]);
//            break;
//        case 6:
//            ui->spc_cur_strack_noiseang_1->setText(line_items[0]);
//            ui->spc_cur_strack_noiseang_2->setText(line_items[1]);
//            ui->spc_cur_strack_noiseang_3->setText(line_items[2]);
//            break;
//        case 7:
//            ui->spc_cur_strack_node->setValue(line_items[0].toInt());
//            break;
//        }
//    }

//    tmp_line_item = spc_data[reset_ind_gps + 1 - 1].split(QRegExp("\\s"), Qt::SkipEmptyParts);
//    gps_s = tmp_line_item[0].toInt();

//    ui->spc_cur_gps_list->clear();
//    tmp_list.clear();

//    for (int i=0; i<gps_s; i++) tmp_list.append("GPSR " + QString::number(i));
//    ui->spc_cur_gps_list->addItems(tmp_list);

//    if (gps_s == 0) gps_s = 1;

//    reset_ind_acc = reset_ind_gps + gps_headers + gps_entries*gps_s;

//    for (int line_num = reset_ind_gps + gps_headers; line_num<reset_ind_acc; line_num++)
//    {
//        line_string = spc_string[line_num-1];
//        line_items = spc_data[line_num-1].split(QRegExp("\\s"), Qt::SkipEmptyParts);

//        long gps_line_num = line_num - reset_ind_gps - gps_headers;
//        cur_item = floor(gps_line_num/gps_entries);
//        cur_entry = gps_line_num % gps_entries;

//        ui->spc_cur_gps_list->setCurrentRow(cur_item);

//        switch (cur_entry){
//        case 0:
//            break; // header
//        case 1:
//            ui->spc_cur_gps_samptime->setText(line_items[0]);
//            break;
//        case 2:
//            ui->spc_cur_gps_posnoise->setText(line_items[0]);
//            break;
//        case 3:
//            ui->spc_cur_gps_velnoise->setText(line_items[0]);
//            break;
//        case 4:
//            ui->spc_cur_gps_timenoise->setText(line_items[0]);
//            break;
//        case 5:
//            ui->spc_cur_gps_node->setValue(line_items[0].toInt());
//            break;
//        }
//    }

//    tmp_line_item = spc_data[reset_ind_acc + 1 - 1].split(QRegExp("\\s"), Qt::SkipEmptyParts);
//    accels = tmp_line_item[0].toInt();

//    ui->spc_cur_accel_list->clear();
//    tmp_list.clear();

//    for (int i=0; i<accels; i++) tmp_list.append("Acc " + QString::number(i));
//    ui->spc_cur_accel_list->addItems(tmp_list);

//    if (accels == 0) accels = 1;

//    reset_ind_end = reset_ind_acc + accel_headers + acc_entries*accels;

//    for (int line_num = reset_ind_acc+accel_headers; line_num<reset_ind_end; line_num++)
//    {
//        line_string = spc_string[line_num-1];
//        line_items = spc_data[line_num-1].split(QRegExp("\\s"), Qt::SkipEmptyParts);

//        long acc_line_num = line_num - reset_ind_acc - accel_headers;
//        cur_item = floor(acc_line_num/acc_entries);
//        cur_entry = acc_line_num % acc_entries;

//        ui->spc_cur_accel_list->setCurrentRow(cur_item);
//        switch (cur_entry){
//        case 0:
//            break; // header
//        case 1:
//            ui->spc_cur_acc_samptime->setText(line_items[0]);
//            break;
//        case 2:
//            ui->spc_cur_acc_axis_1->setText(line_items[0]);
//            ui->spc_cur_acc_axis_2->setText(line_items[1]);
//            ui->spc_cur_acc_axis_3->setText(line_items[2]);
//            break;
//        case 3:
//            ui->spc_cur_acc_maxacc->setText(line_items[0]);
//            break;
//        case 4:
//            ui->spc_cur_acc_scaleerror->setText(line_items[0]);
//            break;
//        case 5:
//            ui->spc_cur_acc_quant->setText(line_items[0]);
//            break;
//        case 6:
//            ui->spc_cur_acc_dvrandwalk->setText(line_items[0]);
//            break;
//        case 7:
//            ui->spc_cur_acc_bias_stab->setText(line_items[0]);
//            ui->spc_cur_acc_bias_tspan->setText(line_items[1]);
//            break;
//        case 8:
//            ui->spc_cur_acc_dvnoise->setText(line_items[0]);
//            break;
//        case 9:
//            ui->spc_cur_acc_initbias->setText(line_items[0]);
//            break;
//        case 10:
//            ui->spc_cur_acc_node->setValue(line_items[0].toInt());
//            break;
//        }
//    }
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


//// S/C Buttons

void SPC_submenu::on_spc_cur_save_clicked()
{
    int response = dsm_gui_lib::warning_message("Overwrite Default SC file?");
    if (response == QMessageBox::Ok) {
        QFile::remove(inout_path+"__default__/SC_Simple.txt");
        QFile::copy(file_path,inout_path+"__default__/SC_Simple.txt");
        receive_data();
        apply_data();
    }
}


void SPC_submenu::on_spc_cur_close_clicked()
{
    SPC_submenu::close();
}


void SPC_submenu::on_spc_cur_apply_clicked()
{

    QFile file(file_path);
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(0, "error", file.errorString());
    }

    QTextStream in(&file);
    long counter = 1;
    while(!in.atEnd()) {
        QString line = in.readLine();
        if (counter < 8)
        {
            spc_update.append(line);
            spc_update.append("\n");
        }
        counter++;
    }
    file.close();

    for(int line_num=8; line_num<reset_ind_body; line_num++)
    {
        QString data_inp = "";
        switch (line_num) {
            /******************************************* HEADER ***************************************************/
            // Orbit Parameters
        case 8: // Orbit Prop
            data_inp = ui->spc_cur_orb_type->currentText();
            break;
        case 9: // Pos of CM or ORIGIN wrt F
            data_inp = ui->spc_cur_pos_ref->currentText();
            break;
        case 10: // Pos wrt Formation (m) expressed in F
            data_inp = ui->spc_cur_xpos_form->text() + " " + ui->spc_cur_ypos_form->text() + " " + ui->spc_cur_zpos_form->text();
            break;
        case 11: // Vel wrt Formation (m) expressed in F
            data_inp = ui->spc_cur_xvel_form->text() + " " + ui->spc_cur_yvel_form->text() + " " + ui->spc_cur_zvel_form->text();
            break;
        case 12: // Initial Attitude Header
            break;
        case 13: // Ang Vel wrt [NL], Att [QA] wrt [NLF]
            data_inp = ui->spc_cur_angvel_frame1->currentText() + ui->spc_cur_att_param->currentText() + ui->spc_cur_angvel_frame2->currentText();
            break;
        case 14:
            data_inp = ui->spc_cur_angvel_x->text() + " " + ui->spc_cur_angvel_y->text() + " " + ui->spc_cur_angvel_z->text();
            break;
        case 15:
            data_inp = ui->spc_cur_q1->text() + " " +  ui->spc_cur_q2->text() + " " + ui->spc_cur_q3->text() + " " + ui->spc_cur_q4->text();

            if (!QString::compare(ui->spc_cur_att_param->currentText(), "Q"))
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
        case 16:
            data_inp = ui->spc_cur_initeul_1->text() + " " + ui->spc_cur_initeul_2->text() + " " + ui->spc_cur_initeul_3->text() + " " + ui->spc_cur_initeul_seq->currentText();

            if (!QString::compare(ui->spc_cur_att_param->currentText(), "Q"))
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
        case 17: // Dynamics Flags Header
            break;
        case 18:
            data_inp = ui->spc_cur_solver->currentText();
            break;
        case 19: // Compute constraint forces and torques
            if (ui->spc_cur_con_on->isChecked()) data_inp = "TRUE";
            else data_inp = "FALSE";
            break;
        case 20: // mass props referenced to ...
            data_inp = ui->spc_cur_masspropref->currentText();
        case 21: // Flex active
            if (ui->spc_cur_flex_on->isChecked()) data_inp = "TRUE";
            else data_inp = "FALSE";
            break;
        case 22: // include 2nd order flex terms
            if (ui->spc_cur_2flex_on->isChecked()) data_inp = "TRUE";
            else data_inp = "FALSE";

        case 23: // Shaker file name
            if (ui->spc_cur_shaker_enab->isChecked()) data_inp = "NONE";
            else data_inp = ui->spc_cur_shaker_file->text();
            break;
        case 24: // Drag coefficient
            data_inp = ui->spc_cur_drag->text();
            break;
        case 25: // header
            break;
        case 26: // header
            break;
        case 27: // header
            break;
        case 28: // number of bodies
            data_inp = QString::number(bodies);
            break;
        }

        if(spc_file_headers[line_num-1].isEmpty())
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+spc_file_descrip[line_num-1]);
        else
            spc_update.append(spc_file_headers[line_num-1]);
    }

    /************************************* BODY SECTION ***********************************/

    reset_ind_joint = reset_ind_body + body_entries*bodies;

    QStringList tmp_data = {}; // Update the list widget data of the body tab we are currently on.
    if (ui->sections->currentIndex() == 3)
    {
        for (int line_num = reset_ind_body; line_num<reset_ind_joint; line_num++)
        {

            long body_line_num = line_num - reset_ind_body;
            long cur_item = floor(body_line_num/body_entries);
            long cur_entry = body_line_num % body_entries;

            if (ui->spc_cur_body_list->currentRow() == cur_item)
            {
                switch (cur_entry) {
                case 0: // Body X Header
                    tmp_data.append("blankline");
                    break;
                case 1:
                    tmp_data.append(ui->spc_cur_body_mass->text());
                    break;
                case 2: // Moments of Inertia (kg-m^2)
                    tmp_data.append(ui->spc_cur_body_pmoi_x->text());
                    tmp_data.append(ui->spc_cur_body_pmoi_y->text());
                    tmp_data.append(ui->spc_cur_body_pmoi_z->text());
                    break;
                case 3: // Products of Inertia
                    tmp_data.append(ui->spc_cur_body_poi_x->text());
                    tmp_data.append(ui->spc_cur_body_poi_y->text());
                    tmp_data.append(ui->spc_cur_body_poi_z->text());

                    break;
                case 4: // Location of mass center
                    tmp_data.append(ui->spc_cur_body_com_x->text());
                    tmp_data.append(ui->spc_cur_body_com_y->text());
                    tmp_data.append(ui->spc_cur_body_com_z->text());
                    break;
                case 5: // constant embedded momentum
                    tmp_data.append(ui->spc_cur_body_cem_x->text());
                    tmp_data.append(ui->spc_cur_body_cem_y->text());
                    tmp_data.append(ui->spc_cur_body_cem_z->text());
                    break;
                case 6: // constant embedded momentum
                    tmp_data.append(ui->spc_cur_body_cemd_x->text());
                    tmp_data.append(ui->spc_cur_body_cemd_y->text());
                    tmp_data.append(ui->spc_cur_body_cemd_z->text());
                    break;
                case 7:
                    tmp_data.append(ui->spc_cur_body_geom->text());
                    break;
                case 8:
                    if (ui->spc_cur_node_enab->isChecked())
                    {
                        tmp_data.append("NONE");
                        ui->spc_cur_node_file->setEnabled(false);
                    }
                    else
                    {
                        tmp_data.append(ui->spc_cur_node_file->text());
                        ui->spc_cur_node_file->setEnabled(true);
                        ui->spc_cur_node_file->setText(ui->spc_cur_node_file->text());
                    }
                    break;
                case 9:
                    tmp_data.append(ui->spc_cur_flex_file->text());
                    if (ui->spc_cur_flex_enab->isChecked())
                    {
                        tmp_data.append("NONE");
                        ui->spc_cur_flex_file->setEnabled(false);
                    }
                    else
                    {
                        tmp_data.append(ui->spc_cur_flex_file->text());
                        ui->spc_cur_flex_file->setEnabled(true);
                        ui->spc_cur_flex_file->setText(ui->spc_cur_flex_file->text());
                    }
                    break;
                }
            }
            ui->spc_cur_body_list->currentItem()->setData(1, tmp_data);
        }
    }

    tmp_data.clear();

    for (int line_num = reset_ind_body; line_num<reset_ind_joint; line_num++)
    {
        QString data_inp = "";

        long body_line_num = line_num - reset_ind_body;
        long cur_item = floor(body_line_num/body_entries);
        long cur_entry = body_line_num % body_entries;

        ui->spc_cur_body_list->setCurrentRow(cur_item);
        QStringList current_data = ui->spc_cur_body_list->currentItem()->data(1).toStringList();

        switch (cur_entry) {
        case 0: // Body X Header
            spc_update.append("================================ " + ui->spc_cur_body_list->currentItem()->text() + " ================================\n");
            break;
        case 1:
            data_inp = current_data[1];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Mass\n");
            break;
        case 2: // Moments of Inertia (kg-m^2)
            data_inp = current_data[2] + " " + current_data[3]  + " " + current_data[4];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Moments of Inertia (kg-m^2)\n");
            break;
        case 3: // Products of Inertia
            data_inp = current_data[5] + " " + current_data[6] + " " + current_data[7];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Products of Inertia (xy,xz,yz)\n");
            break;
        case 4: // Location of mass center
            data_inp = current_data[8] + " " + current_data[9] + " " + current_data[10];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Location of mass center, m\n");
            break;
        case 5: // constant embedded momentum
            data_inp = current_data[11] + " " + current_data[12] + " " + current_data[13];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Constant Embedded Momentum (Nms)\n");
            break;
        case 6: // constant embedded momentum
            data_inp = current_data[14] + " " + current_data[15] + " " + current_data[16];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Constant Embedded Magnetic Dipole (A-m^2)\n");
            break;
        case 7:
            data_inp = current_data[17];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Geometry Input File Name\n");
            break;
        case 8:
            data_inp = current_data[18];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Node File Name\n");
            break;
        case 9:
            data_inp = current_data[19];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Flex File Name\n");
            break;
        }
    }

    /************************************* JOINT SECTION ***********************************/

    spc_update.append("************************************************************************\n");
    spc_update.append("*************************** Joint Parameters ***************************\n");
    spc_update.append("************************************************************************\n");
    spc_update.append("         (Number of Joints is Number of Bodies minus one)\n");

    reset_ind_wheel = reset_ind_joint + joint_headers + joint_entries*joints;

    if (ui->sections->currentIndex() == 4)
    {
        for (int line_num = reset_ind_joint + joint_headers; line_num<reset_ind_wheel; line_num++)
        {

            long joint_line_num = line_num - reset_ind_joint;
            long cur_item = floor(joint_line_num/joint_entries);
            long cur_entry = joint_line_num % joint_entries;

            if (ui->spc_cur_joint_list->count() > 0 && ui->spc_cur_joint_list->currentRow() == cur_item)
            {
                switch (cur_entry) {
                case 0: // Joint X Header
                    tmp_data.append("blankline");
                    break;
                case 1: // Joint Type
                    tmp_data.append(ui->spc_cur_joint_type->currentText());
                    break;
                case 2: // Joint Connections (inner and outer body)
                    tmp_data.append(ui->spc_cur_joint_in->text());
                    tmp_data.append(ui->spc_cur_joint_out->text());
                    break;
                case 3: // RotDOF, RotDOF Seq, Rot Type
                    tmp_data.append(ui->spc_cur_joint_rotdof->text());
                    tmp_data.append(ui->spc_cur_joint_rotdof_seq->currentText());
                    tmp_data.append(ui->spc_cur_joint_rottype->currentText());
                    break;
                case 4: // Trn DOF, Trn Seq
                    tmp_data.append(ui->spc_cur_joint_trndof->text());
                    tmp_data.append(ui->spc_cur_joint_trndof_seq->currentText());
                    break;
                case 5: // Rotational Axes Locked?
                    if (ui->spc_cur_joint_rlock1->isChecked()) tmp_data.append("TRUE");
                    else tmp_data.append("FALSE");

                    if (ui->spc_cur_joint_rlock2->isChecked()) tmp_data.append("TRUE");
                    else tmp_data.append("FALSE");


                    if (ui->spc_cur_joint_rlock3->isChecked()) tmp_data.append("TRUE");
                    else tmp_data.append("FALSE");

                    break;

                case 6: // Translational Axes Locked?
                    if (ui->spc_cur_joint_tlock1->isChecked()) tmp_data.append("TRUE");
                    else tmp_data.append("FALSE");

                    if (ui->spc_cur_joint_tlock2->isChecked()) tmp_data.append("TRUE");
                    else tmp_data.append("FALSE");


                    if (ui->spc_cur_joint_tlock3->isChecked()) tmp_data.append("TRUE");
                    else tmp_data.append("FALSE");
                    break;
                case 7: // Joint initial angle
                    tmp_data.append(ui->spc_cur_joint_ang0_1->text());
                    tmp_data.append(ui->spc_cur_joint_ang0_2->text());
                    tmp_data.append(ui->spc_cur_joint_ang0_3->text());

                    break;
                case 8: // Joint initial angle rate
                    tmp_data.append(ui->spc_cur_joint_angrate0_1->text());
                    tmp_data.append(ui->spc_cur_joint_angrate0_2->text());
                    tmp_data.append(ui->spc_cur_joint_angrate0_3->text());

                    break;
                case 9: // Joint initial displacement
                    tmp_data.append(ui->spc_cur_joint_disp0_1->text());
                    tmp_data.append(ui->spc_cur_joint_disp0_2->text());
                    tmp_data.append(ui->spc_cur_joint_disp0_3->text());

                    break;
                case 10: // Joint initial velocity
                    tmp_data.append(ui->spc_cur_joint_dispr0_1->text());
                    tmp_data.append(ui->spc_cur_joint_dispr0_2->text());
                    tmp_data.append(ui->spc_cur_joint_dispr0_3->text());

                    break;
                case 11: // Joint Bi Gi angles, sequence
                    tmp_data.append(ui->spc_cur_joint_bigi_1->text());
                    tmp_data.append(ui->spc_cur_joint_bigi_2->text());
                    tmp_data.append(ui->spc_cur_joint_bigi_3->text());
                    tmp_data.append(ui->spc_cur_joint_bigi_seq->currentText());

                    break;
                case 12: // Joint Go Bo angles, sequence
                    tmp_data.append(ui->spc_cur_joint_bogo_1->text());
                    tmp_data.append(ui->spc_cur_joint_bogo_2->text());
                    tmp_data.append(ui->spc_cur_joint_bogo_3->text());
                    tmp_data.append(ui->spc_cur_joint_bogo_seq->currentText());

                    break;
                case 13: // Joint Position wrt inner body
                    tmp_data.append(ui->spc_cur_joint_poswrt_in_1->text());
                    tmp_data.append(ui->spc_cur_joint_poswrt_in_2->text());
                    tmp_data.append(ui->spc_cur_joint_poswrt_in_3->text());

                    break;
                case 14: // Joint Position wrt outer body
                    tmp_data.append(ui->spc_cur_joint_poswrt_out_1->text());
                    tmp_data.append(ui->spc_cur_joint_poswrt_out_2->text());
                    tmp_data.append(ui->spc_cur_joint_poswrt_out_3->text());

                    break;
                case 15: // Joint parameter file
                    tmp_data.append(ui->spc_cur_joint_param_file->text());
                    break;
                }
            }
            ui->spc_cur_joint_list->currentItem()->setData(1, tmp_data);
        }
    }

    tmp_data.clear();

    for (int line_num = reset_ind_joint + joint_headers; line_num<reset_ind_wheel; line_num++)
    {
        QString data_inp = "";

        long joint_line_num = line_num - reset_ind_joint - joint_headers;
        long cur_item = floor(joint_line_num/joint_entries);
        long cur_entry = joint_line_num % joint_entries;

        ui->spc_cur_joint_list->setCurrentRow(cur_item);
        QStringList current_data = ui->spc_cur_joint_list->currentItem()->data(1).toStringList();

        switch (cur_entry) {
        case 0: // Joint X Header
            if (ui->spc_cur_joint_list->count() > 0)
            {
                spc_update.append("============================== " + ui->spc_cur_joint_list->currentItem()->text() + " ================================\n");
            }
            else
            {
                spc_update.append("============================== Joint 0 ================================\n");
            }
            break;
        case 1:
            data_inp = current_data[1];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Type of joint (PASSIVE, ACTUATED, others)\n");
            break;
        case 2:
            data_inp = current_data[2] + " " + current_data[3];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Inner, outer body indices\n");
            break;
        case 3:
            data_inp = current_data[4] + " " + current_data[5] + " " + current_data[6];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! RotDOF, Seq, GIMBAL or SPHERICAL\n");
            break;
        case 4:
            data_inp = current_data[7] + " " + current_data[8];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! TrnDOF, Seq\n");
            break;
        case 5:
            if (ui->spc_cur_joint_rlock1->isChecked()) data_inp = "TRUE";
            else data_inp = "FALSE";

            if (ui->spc_cur_joint_rlock2->isChecked()) data_inp = "TRUE";
            else data_inp = "FALSE";

            if (ui->spc_cur_joint_rlock3->isChecked()) data_inp = "TRUE";
            else data_inp = "FALSE";
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! RotDOF Locked\n");
            break;

        case 6:
            if (ui->spc_cur_joint_tlock1->isChecked()) data_inp = "TRUE";
            else data_inp = "FALSE";

            if (ui->spc_cur_joint_tlock2->isChecked()) data_inp = "TRUE";
            else data_inp = "FALSE";

            if (ui->spc_cur_joint_tlock3->isChecked()) data_inp = "TRUE";
            else data_inp = "FALSE";
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! TrnDOF Locked\n");
            break;
        case 7:
            data_inp = current_data[15] + " " + current_data[16] + " " + current_data[17];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Initial Angles [deg]\n");
            break;
        case 8:
            data_inp = current_data[18] + " " + current_data[19] + " " + current_data[20];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Initial Rates, deg/sec\n");
            break;
        case 9:
            data_inp = current_data[21] + " " + current_data[22] + " " + current_data[23];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Initial Displacements [m]\n");
            break;
        case 10:
            data_inp = current_data[24] + " " + current_data[25] + " " + current_data[26];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Initial Displacement Rates, m/sec\n");
            break;
        case 11:
            data_inp = current_data[27] + " " + current_data[28] + " " + current_data[29] + " " + current_data[30];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Bi to Gi Static Angles [deg] & Seq\n");
            break;
        case 12:
            data_inp = current_data[31] + " " + current_data[32] + " " + current_data[33] + " " + current_data[34];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Go to Bo Static Angles [deg] & Seq\n");
            break;
        case 13:
            data_inp = current_data[35] + " " + current_data[36] + " " + current_data[37];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Position wrt inner body origin, m\n");
            break;
        case 14:
            data_inp = current_data[38] + " " + current_data[39] + " " + current_data[40];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Position wrt outer body origin, m\n");
            break;
        case 15:
            data_inp = current_data[41];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Parameter File Name\n");
            break;
        }
    }

    /*************************** WHEELS SECTION ****************************/

    reset_ind_mtb = reset_ind_wheel + wheel_headers + wheel_entries*wheels;

    long num_drjit = 0;

    for (int i=0; i<ui->spc_cur_wheel_list->count(); i++)
    {
        QString is_none = ui->spc_cur_wheel_list->item(i)->data(1).toStringList()[10];
        if (QString::compare(is_none, "NONE"))
        {
            num_drjit += 1;
        }
    }

    if (num_drjit == 0)
    {
        spc_update.append("*************************** Wheel Parameters ***************************\n");
        spc_update.append("FALSE                         ! Wheel Drag Active\n");
        spc_update.append("FALSE                         ! Wheel Jitter Active\n");
    }
    else
    {
        spc_update.append("*************************** Wheel Parameters ***************************\n");
        spc_update.append("TRUE                          ! Wheel Drag Active\n");
        spc_update.append("TRUE                          ! Wheel Jitter Active\n");
    }

    spc_update.append(dsm_gui_lib::whitespace(QString::number(wheels)) + "! Number of wheels\n");

    if (ui->sections->currentIndex() == 5 && ui->actuator_sections->currentIndex()==0)
    {
        for (int line_num = reset_ind_wheel + wheel_headers; line_num<reset_ind_mtb; line_num++)
        {

            long wheel_line_num = line_num - reset_ind_wheel - wheel_headers;
            long cur_item = floor(wheel_line_num/wheel_entries);
            long cur_entry = wheel_line_num % wheel_entries;

            if (ui->spc_cur_wheel_list->count() > 0 && ui->spc_cur_wheel_list->currentRow() == cur_item)
            {
                switch (cur_entry){
                case 0:
                    tmp_data.append("blankline");

                    break; // header
                case 1:
                    tmp_data.append(ui->spc_cur_wheel_initmom->text());
                    break;
                case 2:
                    tmp_data.append(ui->spc_cur_wheel_axis_1->text());
                    tmp_data.append(ui->spc_cur_wheel_axis_2->text());
                    tmp_data.append(ui->spc_cur_wheel_axis_3->text());
                    break;
                case 3:
                    tmp_data.append(ui->spc_cur_wheel_maxtrq->text());
                    tmp_data.append(ui->spc_cur_wheel_maxmom->text());
                    break;
                case 4:
                    tmp_data.append(ui->spc_cur_wheel_inertia->text());

                    break;
                case 5:
                    tmp_data.append(ui->spc_cur_wheel_body->cleanText());

                    break;
                case 6:
                    tmp_data.append(ui->spc_cur_wheel_node->cleanText());

                    break;
                case 7:
                    if (ui->spc_cur_wheel_drjit_on->isChecked()) tmp_data.append("NONE");
                    else tmp_data.append(ui->spc_cur_wheel_drjit_on->text());
                    break;
                }
            }
            ui->spc_cur_wheel_list->currentItem()->setData(1, tmp_data);
        }
    }

    tmp_data.clear();

    for (int line_num = reset_ind_wheel + wheel_headers; line_num<reset_ind_mtb; line_num++)
    {
        QString data_inp = "";

        long wheel_line_num = line_num - reset_ind_wheel - wheel_headers;
        long cur_item = floor(wheel_line_num/wheel_entries);
        long cur_entry = wheel_line_num % wheel_entries;

        ui->spc_cur_wheel_list->setCurrentRow(cur_item);
        QStringList current_data = ui->spc_cur_wheel_list->item(cur_item)->data(1).toStringList();


        switch (cur_entry){
        case 0:
            if (ui->spc_cur_wheel_list->count() > 0)
            {
                spc_update.append("=============================  " + ui->spc_cur_wheel_list->currentItem()->text() + "  ================================\n");
            }
            else
            {
                spc_update.append("=============================  Wheel 0  ================================\n");
            }

            break; // header
        case 1:
            data_inp = current_data[1];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Initial Momentum, N-m-sec\n");
            break;
        case 2:
            data_inp = current_data[2] + " " +  current_data[3] + " " +  current_data[4];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Wheel Axis Components, [X, Y, Z]\n");
            break;
        case 3:
            data_inp = current_data[5] + " " +  current_data[6];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Max Torque (N-m), Momentum (N-m-sec)\n");
            break;
        case 4:
            data_inp =  current_data[7];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Wheel Rotor Inertia, kg-m^2\n");
            break;
        case 5:
            data_inp =  current_data[8];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Body\n");
            break;
        case 6:
            data_inp =  current_data[9];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Node\n");
            break;
        case 7:
            data_inp =  current_data[10];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Drag/Jitter Input File Name\n");
            break;
        }
    }

    /***************************** MTB SECTION ******************************/
    reset_ind_thr = reset_ind_mtb + mtb_headers + mtb_entries*mtbs;

    spc_update.append("**************************** MTB Parameters ****************************\n");
    spc_update.append(dsm_gui_lib::whitespace(QString::number(mtbs)) + "! Number of MTBs\n");
    if (ui->sections->currentIndex() == 5 && ui->actuator_sections->currentIndex()==1)
    {
        for (int line_num = reset_ind_mtb + mtb_headers; line_num<reset_ind_thr; line_num++)
        {

            long mtb_line_num = line_num - reset_ind_mtb - mtb_headers;
            long cur_item = floor(mtb_line_num/mtb_entries);
            long cur_entry = mtb_line_num % mtb_entries;

            if (ui->spc_cur_mtb_list->count() > 0 && ui->spc_cur_mtb_list->currentRow() == cur_item)
            {
                switch (cur_entry){
                case 0:
                    tmp_data.append("blankline");

                    break; // header
                case 1:
                    tmp_data.append(ui->spc_cur_mtb_sat->text());
                    break;
                case 2:
                    tmp_data.append(ui->spc_cur_mtb_axis_1->text());
                    tmp_data.append(ui->spc_cur_mtb_axis_2->text());
                    tmp_data.append(ui->spc_cur_mtb_axis_3->text());
                    break;
                case 3:
                    tmp_data.append(ui->spc_cur_mtb_node->text());
                    break;
                }
            }
            ui->spc_cur_mtb_list->currentItem()->setData(1, tmp_data);
        }
    }

    tmp_data.clear();


    for (int line_num = reset_ind_mtb + mtb_headers; line_num<reset_ind_thr; line_num++)
    {
        QString data_inp = "";

        long mtb_line_num = line_num - reset_ind_mtb - mtb_headers;
        long cur_item = floor(mtb_line_num/mtb_entries);
        long cur_entry = mtb_line_num % mtb_entries;

        ui->spc_cur_mtb_list->setCurrentRow(cur_item);
        QStringList current_data = ui->spc_cur_mtb_list->currentItem()->data(1).toStringList();

        switch (cur_entry){
        case 0:
            if (ui->spc_cur_mtb_list->count() > 0)
            {
                spc_update.append("=============================  " + ui->spc_cur_mtb_list->currentItem()->text() + "  ================================\n");
            }
            else
            {
                spc_update.append("=============================  MTB 0  ================================\n");
            }

            break; // header
        case 1:
            data_inp = current_data[1];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Saturation (A-m^2)\n");
            break;
        case 2:
            data_inp =  current_data[2] + " " +  current_data[3] + " " +  current_data[4];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! MTB Axis Components, [X, Y, Z]\n");
            break;
        case 3:
            data_inp =  current_data[5];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Node\n");
            break;
        }
    }

    /***************************** THRUSTER SECTION ******************************/
    reset_ind_gyro = reset_ind_thr + thr_headers + thr_entries*thrusters;

    spc_update.append("************************* Thruster Parameters **************************\n");
    spc_update.append(dsm_gui_lib::whitespace(QString::number(thrusters)) + "! Number of Thrusters\n");
    if (ui->sections->currentIndex() == 5 && ui->actuator_sections->currentIndex()==2)
    {
        for (int line_num = reset_ind_thr + thr_headers; line_num<reset_ind_gyro; line_num++)
        {

            long thr_line_num = line_num - reset_ind_thr - thr_headers;
            long cur_item = floor(thr_line_num/thr_entries);
            long cur_entry = thr_line_num % thr_entries;

            if (ui->spc_cur_thruster_list->count() > 0 && ui->spc_cur_thruster_list->currentRow() == cur_item)
            {
                switch (cur_entry){
                case 0:
                    tmp_data.append("blankline");

                    break; // header
                case 1:
                    tmp_data.append(ui->spc_cur_thruster_mode->currentText());
                    break;
                case 2:
                    tmp_data.append(ui->spc_cur_thruster_force->text());
                    break;
                case 3:
                    tmp_data.append(ui->spc_cur_thruster_axis_1->text());
                    tmp_data.append(ui->spc_cur_thruster_axis_2->text());
                    tmp_data.append(ui->spc_cur_thruster_axis_3->text());
                    break;
                case 4:
                    tmp_data.append(ui->spc_cur_thruster_body->text());
                case 5:
                    tmp_data.append(ui->spc_cur_thruster_node->text());
                    break;
                }
            }
            ui->spc_cur_thruster_list->currentItem()->setData(1, tmp_data);
        }
    }

    tmp_data.clear();


    for (int line_num = reset_ind_thr + thr_headers; line_num<reset_ind_gyro; line_num++)
    {
        QString data_inp = "";

        long thr_line_num = line_num - reset_ind_thr - thr_headers;
        long cur_item = floor(thr_line_num/thr_entries);
        long cur_entry = thr_line_num % thr_entries;

        ui->spc_cur_thruster_list->setCurrentRow(cur_item);
        QStringList current_data = ui->spc_cur_thruster_list->currentItem()->data(1).toStringList();

        switch (cur_entry){
        case 0:
            if (ui->spc_cur_thruster_list->count() > 0)
            {
                spc_update.append("=============================  " + ui->spc_cur_thruster_list->currentItem()->text() + "  ================================\n");
            }
            else
            {
                spc_update.append("=============================  Thr 0  ================================\n");
            }

            break; // header
        case 1:
            data_inp = current_data[1];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+" Mode (PULSED or PROPORTIONAL)\n");
            break;
        case 2:
            data_inp = current_data[2];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Thrust Force (N)\n");
            break;
        case 3:
            data_inp =  current_data[3] + " " +  current_data[4] + " " +  current_data[5];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Thrust Axis \n");
            break;
        case 4:
            data_inp =  current_data[6];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Body\n");
            break;
        case 5:
            data_inp =  current_data[7];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Node\n");
            break;
        }
    }

//    spc_update.append("************************* Thruster Parameters **************************\n");
//    spc_update.append(dsm_gui_lib::whitespace(QString::number(thrusters)) + "! Number of Thrusters\n");

//    for (int line_num = reset_ind_thr + thr_headers; line_num<reset_ind_gyro; line_num++)
//    {
//        QString data_inp = "";

//        long thr_line_num = line_num - reset_ind_thr - thr_headers;
//        long cur_item = floor(thr_line_num/thr_entries);
//        long cur_entry = thr_line_num % thr_entries;

//        ui->spc_cur_thruster_list->setCurrentRow(cur_item);

//        switch (cur_entry){
//        case 0:
//            if (ui->spc_cur_thruster_list->count() > 0)
//            {
//                spc_update.append("=============================  " + ui->spc_cur_thruster_list->currentItem()->text() + "  ================================\n");
//                thrusters = 0;
//            }
//            else
//            {
//                spc_update.append("=============================  Thr 0  ================================\n");
//            }

//            break; // header
//        case 1:
//            data_inp = ui->spc_cur_thruster_mode->currentText();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Mode (PULSED or PROPORTIONAL)\n");
//            break;
//        case 2:
//            data_inp = ui->spc_cur_thruster_force->text();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Thrust Force (N)\n");
//            break;
//        case 3:
//            data_inp = ui->spc_cur_thruster_axis_1->text() + " " + ui->spc_cur_thruster_axis_2->text() + " " + ui->spc_cur_thruster_axis_3->text();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Thrust Axis\n");
//            break;
//        case 4:
//            data_inp = ui->spc_cur_thruster_body->cleanText();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Body\n");
//            break;
//        case 5:
//            data_inp = ui->spc_cur_thruster_body->cleanText();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Node\n");
//            break;
//        }
//    }

//    spc_update.append("******************************* Gyro ************************************\n");
//    spc_update.append(dsm_gui_lib::whitespace(QString::number(gyros)) + "! Number of Gyro Axes\n");

//    for (int line_num = reset_ind_gyro + gyro_headers; line_num<reset_ind_mag; line_num++)
//    {
//        QString data_inp = "";

//        long gyro_line_num = line_num - reset_ind_gyro - gyro_headers;
//        long cur_item = floor(gyro_line_num/gyro_entries);
//        long cur_entry = gyro_line_num % gyro_entries;

//        ui->spc_cur_gyro_list->setCurrentRow(cur_item);

//        switch (cur_entry){
//        case 0:
//            if (ui->spc_cur_gyro_list->count() > 0)
//            {
//                spc_update.append("=============================  " + ui->spc_cur_gyro_list->currentItem()->text() + "  ================================\n");
//                gyros = 0;
//            }
//            else
//            {
//                spc_update.append("=============================  Axis 0  ================================\n");
//            }

//            break; // header
//        case 1:
//            data_inp = ui->spc_cur_gyro_samptime->text();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Sample Time,sec\n");
//            break;
//        case 2:
//            data_inp = ui->spc_cur_gyro_axis_1->text() + " " + ui->spc_cur_gyro_axis_2->text() + " " + ui->spc_cur_gyro_axis_3->text();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Axis expressed in Body Frame\n");
//            break;
//        case 3:
//            data_inp = ui->spc_cur_gyro_maxrate->text();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Max Rate, deg/sec\n");
//            break;
//        case 4:
//            data_inp = ui->spc_cur_gyro_scaleferror->text();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Scale Factor Error, ppm\n");
//            break;
//        case 5:
//            data_inp = ui->spc_cur_gyro_quant->text();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Quantization, arcsec \n");
//            break;
//        case 6:
//            data_inp = ui->spc_cur_gyro_bias_stab->text() + " " + ui->spc_cur_gyro_bias_tspan->text();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Bias Stability (deg/hr) over timespan (hr)\n");
//            break;
//        case 7:
//            data_inp = ui->spc_cur_gyro_initbias->text();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Initial Bias (deg/hr)\n");
//            break;
//        case 8:
//            data_inp = ui->spc_cur_gyro_node->text();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Node\n");
//            break;
//        }
//    }

//    spc_update.append("*************************** Magnetometer ********************************\n");
//    spc_update.append(dsm_gui_lib::whitespace(QString::number(mags)) + "! Number of Magnetometer Axes\n");

//    for (int line_num = reset_ind_mag + mag_headers; line_num<reset_ind_css; line_num++)
//    {
//        QString data_inp = "";

//        long mag_line_num = line_num - reset_ind_mag - mag_headers;
//        long cur_item = floor(mag_line_num/mag_entries);
//        long cur_entry = mag_line_num % mag_entries;

//        ui->spc_cur_mag_list->setCurrentRow(cur_item);

//        switch (cur_entry){
//        case 0:
//            if (ui->spc_cur_mag_list->count() > 0)
//            {
//                spc_update.append("=============================  " + ui->spc_cur_mag_list->currentItem()->text() + "  ================================\n");
//                mags = 0;
//            }
//            else
//            {
//                spc_update.append("=============================  Axis 0  ================================\n");
//            }

//            break; // header
//        case 1:
//            data_inp = ui->spc_cur_mag_samptime->text();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Sample Time,sec\n");
//            break;
//        case 2:
//            data_inp = ui->spc_cur_mag_axis_1->text() + " " + ui->spc_cur_mag_axis_2->text() + " " + ui->spc_cur_mag_axis_3->text();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Axis expressed in Body Frame\n");
//            break;
//        case 3:
//            data_inp = ui->spc_cur_mag_sat->text();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Saturation, Tesla\n");
//            break;
//        case 4:
//            data_inp = ui->spc_cur_mag_scaleferror->text();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Scale Factor Error, ppm\n");
//            break;
//        case 5:
//            data_inp = ui->spc_cur_mag_quant->text();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Quantization, Tesla \n");
//            break;
//        case 6:
//            data_inp = ui->spc_cur_mag_noise->text();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Noise, Tesla RMS\n");
//            break;
//        case 7:
//            data_inp = ui->spc_cur_mag_node->text();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Node\n");
//            break;
//        }
//    }

//    spc_update.append("*********************** Coarse Sun Sensor *******************************\n");
//    spc_update.append(dsm_gui_lib::whitespace(QString::number(css_s)) + "! Number of Coarse Sun Sensors\n");

//    for (int line_num = reset_ind_css + css_headers; line_num<reset_ind_fss; line_num++)
//    {
//        QString data_inp = "";

//        long css_line_num = line_num - reset_ind_css - css_headers;
//        long cur_item = floor(css_line_num/css_entries);
//        long cur_entry = css_line_num % css_entries;

//        ui->spc_cur_css_list->setCurrentRow(cur_item);

//        switch (cur_entry){
//        case 0:
//            if (ui->spc_cur_css_list->count() > 0)
//            {
//                spc_update.append("=============================  " + ui->spc_cur_css_list->currentItem()->text() + "  ================================\n");
//                css_s = 0;
//            }
//            else
//            {
//                spc_update.append("=============================  CSS 0  ================================\n");
//            }

//            break; // header
//        case 1:
//            data_inp = ui->spc_cur_css_samptime->text();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Sample Time,sec\n");
//            break;
//        case 2:
//            data_inp = ui->spc_cur_css_axis_1->text() + " " + ui->spc_cur_css_axis_2->text() + " " + ui->spc_cur_css_axis_3->text();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Axis expressed in Body Frame\n");
//            break;
//        case 3:
//            data_inp = ui->spc_cur_css_halfcone->text();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Half-cone Angle, deg\n");
//            break;
//        case 4:
//            data_inp = ui->spc_cur_css_scale->text();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Scale Factor\n");
//            break;
//        case 5:
//            data_inp = ui->spc_cur_css_quant->text();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Quantization\n");
//            break;
//        case 8:
//            data_inp = ui->spc_cur_css_body->text();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Body\n");
//            break;
//        }
//    }

//    spc_update.append("************************* Fine Sun Sensor *******************************\n");
//    spc_update.append(dsm_gui_lib::whitespace(QString::number(fss_s)) + "! Number of Fine Sun Sensors\n");

//    for (int line_num = reset_ind_fss + fss_headers; line_num<reset_ind_strack; line_num++)
//    {
//        QString data_inp = "";

//        long fss_line_num = line_num - reset_ind_fss - fss_headers;
//        long cur_item = floor(fss_line_num/fss_entries);
//        long cur_entry = fss_line_num % fss_entries;

//        ui->spc_cur_fss_list->setCurrentRow(cur_item);

//        switch (cur_entry){
//        case 0:
//            if (ui->spc_cur_css_list->count() > 0)
//            {
//                spc_update.append("=============================  " + ui->spc_cur_fss_list->currentItem()->text() + "  ================================\n");
//                fss_s = 0;
//            }
//            else
//            {
//                spc_update.append("=============================  FSS 0  ================================\n");
//            }

//            break; // header
//        case 1:
//            data_inp = ui->spc_cur_fss_samptime->text();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Sample Time,sec\n");
//            break;
//        case 2:
//            data_inp = ui->spc_cur_fss_mount_1->text() + " " + ui->spc_cur_fss_mount_2->text() + " " + ui->spc_cur_fss_mount_3->text() + ui->spc_cur_fss_mountseq->currentText();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Mounting Angles (deg), Seq in Body\n");
//            break;
//        case 3:
//            data_inp = ui->spc_cur_fss_boreaxis->currentText();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Boresight Axis X_AXIS, Y_AXIS, or Z_AXIS\n");
//            break;
//        case 4:
//            data_inp = ui->spc_cur_fss_hfov->text() + " " + ui->spc_cur_fss_vfov->text();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! H, V FOV Size, deg\n");
//            break;
//        case 5:
//            data_inp = ui->spc_cur_fss_noiseang->text();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Noise Equivalent Angle, deg RMS\n");
//            break;
//        case 6:
//            data_inp = ui->spc_cur_fss_quant->text();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Quantization, deg\n");
//            break;
//        case 7:
//            data_inp = ui->spc_cur_fss_node->text();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Node\n");
//            break;
//        }
//    }

//    spc_update.append("************************** Star Tracker *********************************\n");
//    spc_update.append(dsm_gui_lib::whitespace(QString::number(stracks)) + "! Number of Star Trackers\n");

//    for (int line_num = reset_ind_strack + strack_headers; line_num<reset_ind_gps; line_num++)
//    {
//        QString data_inp = "";

//        long strack_line_num = line_num - reset_ind_strack - strack_headers;
//        long cur_item = floor(strack_line_num/strack_entries);
//        long cur_entry = strack_line_num % strack_entries;

//        ui->spc_cur_fss_list->setCurrentRow(cur_item);

//        switch (cur_entry){
//        case 0:
//            if (ui->spc_cur_strack_list->count() > 0)
//            {
//                spc_update.append("=============================  " + ui->spc_cur_strack_list->currentItem()->text() + "  ================================\n");
//                stracks = 0;
//            }
//            else
//            {
//                spc_update.append("=============================  ST 0  ================================\n");
//            }

//            break; // header
//        case 1:
//            data_inp = ui->spc_cur_strack_samptime->text();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Sample Time,sec\n");
//            break;
//        case 2:
//            data_inp = ui->spc_cur_strack_mount_1->text() + " " + ui->spc_cur_strack_mount_2->text() + " " + ui->spc_cur_strack_mount_3->text() + ui->spc_cur_strack_mountseq->currentText();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Mounting Angles (deg), Seq in Body\n");
//            break;
//        case 3:
//            data_inp = ui->spc_cur_strack_boreaxis->currentText();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Boresight Axis X_AXIS, Y_AXIS, or Z_AXIS\n");
//            break;
//        case 4:
//            data_inp = ui->spc_cur_strack_hfov->text() + " " + ui->spc_cur_fss_vfov->text();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! H, V FOV Size, deg\n");
//            break;
//        case 5:
//            data_inp = ui->spc_cur_strack_sun->text() + " " + ui->spc_cur_strack_earth->text() + " " + ui->spc_cur_strack_moon->text();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Sun, Earth, Moon Exclusion Angles, deg\n");
//            break;
//        case 6:
//            data_inp = ui->spc_cur_strack_noiseang_1->text() + " " + ui->spc_cur_strack_noiseang_2->text() + " " + ui->spc_cur_strack_noiseang_3->text();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Noise Equivalent Angle, arcsec RMS\n");
//            break;
//        case 7:
//            data_inp = ui->spc_cur_strack_node->text();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Node\n");
//            break;
//        }
//    }

//    spc_update.append("****************************** GPS **************************************\n");
//    spc_update.append(dsm_gui_lib::whitespace(QString::number(stracks)) + "! Number of GPS Receivers\n");

//    for (int line_num = reset_ind_gps + gps_headers; line_num<reset_ind_acc; line_num++)
//    {
//        QString data_inp = "";

//        long gps_line_num = line_num - reset_ind_gps - gps_headers;
//        long cur_item = floor(gps_line_num/gps_entries);
//        long cur_entry = gps_line_num % gps_entries;

//        ui->spc_cur_gps_list->setCurrentRow(cur_item);

//        switch (cur_entry){
//        case 0:
//            if (ui->spc_cur_gps_list->count() > 0)
//            {
//                spc_update.append("=============================  " + ui->spc_cur_gps_list->currentItem()->text() + "  ================================\n");
//                gps_s = 0;
//            }
//            else
//            {
//                spc_update.append("=============================  GPSR 0  ================================\n");
//            }

//            break; // header
//        case 1:
//            data_inp = ui->spc_cur_gps_samptime->text();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Sample Time,sec\n");
//            break;
//        case 2:
//            data_inp = ui->spc_cur_gps_posnoise->text();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Position Noise, m RMS\n");
//            break;
//        case 3:
//            data_inp = ui->spc_cur_gps_velnoise->text();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Velocity Noise, m/sec RMS\n");
//            break;
//        case 4:
//            data_inp = ui->spc_cur_gps_timenoise->text();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Time Noise, sec RMS\n");
//            break;
//        case 5:
//            data_inp = ui->spc_cur_strack_node->text();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Node\n");
//            break;
//        }
//    }

//    spc_update.append("*************************** Accelerometer *******************************\n");
//    spc_update.append(dsm_gui_lib::whitespace(QString::number(accels)) + "! Number of Accel Axes\n");

//    for (int line_num = reset_ind_acc + accel_headers; line_num<spc_data.length(); line_num++)
//    {
//        QString data_inp = "";

//        long acc_line_num = line_num - reset_ind_acc - accel_headers;
//        long cur_item = floor(acc_line_num/acc_entries);
//        long cur_entry = acc_line_num % acc_entries;

//        ui->spc_cur_accel_list->setCurrentRow(cur_item);

//        switch (cur_entry){
//        case 0:
//            if (ui->spc_cur_accel_list->count() > 0)
//            {
//                spc_update.append("=============================  " + ui->spc_cur_accel_list->currentItem()->text() + "  ================================\n");
//                accels = 0;
//            }
//            else
//            {
//                spc_update.append("=============================  Axis 0  ================================\n");
//            }

//            break; // header
//        case 1:
//            data_inp = ui->spc_cur_acc_samptime->text();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Sample Time,sec\n");
//            break;
//        case 2:
//            data_inp = ui->spc_cur_acc_axis_1->text() + " " + ui->spc_cur_acc_axis_2->text() + " " + ui->spc_cur_acc_axis_3->text();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Axis expressed in Body Frame\n");
//            break;
//        case 3:
//            data_inp = ui->spc_cur_acc_maxacc->text();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Max Acceleration (m/s^2)\n");
//            break;
//        case 4:
//            data_inp = ui->spc_cur_acc_scaleerror->text();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Scale Factor Error, ppm\n");
//            break;
//        case 5:
//            data_inp = ui->spc_cur_acc_quant->text();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Quantization, m/s^2\n");
//            break;
//        case 6:
//            data_inp = ui->spc_cur_acc_dvrandwalk->text();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! DV Random Walk (m/s/rt-hr)\n");
//            break;
//        case 7:
//            data_inp = ui->spc_cur_acc_bias_stab->text() + " " + ui->spc_cur_acc_bias_tspan->text();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Bias Stability (m/s^2) over timespan (hr)\n");
//            break;
//        case 8:
//            data_inp = ui->spc_cur_acc_dvnoise->text();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! DV Noise, m/s\n");
//            break;
//        case 9:
//            data_inp = ui->spc_cur_acc_dvnoise->text();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! DV Noise, m/s\n");
//            break;
//        case 10:
//            data_inp = ui->spc_cur_acc_initbias->text();
//            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Initial Bias (m/s^2)\n");
//            break;
//        }
//    }

    write_data();
}

void SPC_submenu::setQComboBox(QComboBox *comboBox, QString string) {
    comboBox->setCurrentIndex(comboBox->findText(string));
}


// Body -/+/Duplicate/Item Clicked

void SPC_submenu::on_spc_cur_body_remove_clicked()
{
    delete ui->spc_cur_body_list->currentItem();
    bodies -= 1;

}


void SPC_submenu::on_spc_cur_body_add_clicked()
{
    bodies += 1;

    QStringList tmp_data = {};

    tmp_data.append("blankline");
    tmp_data.append("100.0");
    tmp_data.append("100.0");
    tmp_data.append("200.0");
    tmp_data.append("300.0");
    tmp_data.append("0.0");
    tmp_data.append("0.0");
    tmp_data.append("0.0");
    tmp_data.append("0.0");
    tmp_data.append("0.0");
    tmp_data.append("0.0");
    tmp_data.append("0.0");
    tmp_data.append("0.0");
    tmp_data.append("0.0");
    tmp_data.append("0.0");
    tmp_data.append("0.0");
    tmp_data.append("0.0");
    tmp_data.append("IonCruiser.obj");
    tmp_data.append("NONE");
    tmp_data.append("NONE");

    ui->spc_cur_body_list->addItem("Body " + QString::number(bodies - 1));
    ui->spc_cur_body_list->setCurrentRow(ui->spc_cur_body_list->count()-1);

    ui->spc_cur_body_list->currentItem()->setData(0, "Body " + QString::number(bodies - 1));
    ui->spc_cur_body_list->currentItem()->setData(1, tmp_data);
    on_spc_cur_body_list_itemClicked(ui->spc_cur_body_list->currentItem());
}


void SPC_submenu::on_spc_cur_body_duplicate_clicked()
{
    bodies += 1;

    QStringList old_data = ui->spc_cur_body_list->currentItem()->data(1).toStringList();

    ui->spc_cur_body_list->addItem("Body " + QString::number(bodies - 1));
    ui->spc_cur_body_list->setCurrentRow(ui->spc_cur_body_list->count()-1);
    ui->spc_cur_body_list->currentItem()->setData(1, old_data);
}


void SPC_submenu::on_spc_cur_body_list_itemClicked(QListWidgetItem *item)
{
    receive_data();

    QStringList current_data = item->data(1).toStringList();
    item->setText(item->data(0).toString());

    ui->spc_cur_body_mass->setText(current_data[1]);

    ui->spc_cur_body_pmoi_x->setText(current_data[2]);
    ui->spc_cur_body_pmoi_y->setText(current_data[3]);
    ui->spc_cur_body_pmoi_z->setText(current_data[4]);

    ui->spc_cur_body_poi_x->setText(current_data[5]);
    ui->spc_cur_body_poi_y->setText(current_data[6]);
    ui->spc_cur_body_poi_z->setText(current_data[7]);

    ui->spc_cur_body_com_x->setText(current_data[8]);
    ui->spc_cur_body_com_y->setText(current_data[9]);
    ui->spc_cur_body_com_z->setText(current_data[10]);

    ui->spc_cur_body_cem_x->setText(current_data[11]);
    ui->spc_cur_body_cem_y->setText(current_data[12]);
    ui->spc_cur_body_cem_z->setText(current_data[13]);

    ui->spc_cur_body_cemd_x->setText(current_data[14]);
    ui->spc_cur_body_cemd_y->setText(current_data[15]);
    ui->spc_cur_body_cemd_z->setText(current_data[16]);

    ui->spc_cur_body_geom->setText(current_data[17]);

    if (!QString::compare(current_data[18], "NONE"))
    {
        ui->spc_cur_node_enab->setCheckState(Qt::Checked);
    }
    else
    {
        ui->spc_cur_node_enab->setCheckState(Qt::Unchecked);
        ui->spc_cur_node_file->setText(current_data[18]);
    }

    if (!QString::compare(current_data[19], "NONE"))
    {
        ui->spc_cur_flex_enab->setCheckState(Qt::Checked);
    }
    else
    {
        ui->spc_cur_flex_enab->setCheckState(Qt::Unchecked);
        ui->spc_cur_flex_file->setText(current_data[19]);
    }
}

// Joint (item clicked)

void SPC_submenu::on_spc_cur_joint_list_itemClicked(QListWidgetItem *item)
{
    receive_data();

    QStringList current_data = item->data(1).toStringList();
    item->setText(item->data(0).toString());

    setQComboBox(ui->spc_cur_joint_type, current_data[1]);

    ui->spc_cur_joint_in->setValue(current_data[2].toInt());
    ui->spc_cur_joint_out->setValue(current_data[3].toInt());

    ui->spc_cur_joint_rotdof->setValue(current_data[4].toInt());
    setQComboBox(ui->spc_cur_joint_rotdof_seq, current_data[5]);
    setQComboBox(ui->spc_cur_joint_rottype, current_data[6]);

    ui->spc_cur_joint_trndof->setValue(current_data[7].toInt());
    setQComboBox(ui->spc_cur_joint_trndof_seq, current_data[8]);

    if (!QString::compare(current_data[9], "TRUE")) ui->spc_cur_joint_rlock1->setCheckState(Qt::Checked);
    else ui->spc_cur_joint_rlock1->setCheckState(Qt::Unchecked);

    if (!QString::compare(current_data[10], "TRUE")) ui->spc_cur_joint_rlock2->setCheckState(Qt::Checked);
    else ui->spc_cur_joint_rlock1->setCheckState(Qt::Unchecked);

    if (!QString::compare(current_data[11], "TRUE")) ui->spc_cur_joint_rlock3->setCheckState(Qt::Checked);
    else ui->spc_cur_joint_rlock1->setCheckState(Qt::Unchecked);



    if (!QString::compare(current_data[12], "TRUE")) ui->spc_cur_joint_tlock1->setCheckState(Qt::Checked);
    else ui->spc_cur_joint_rlock1->setCheckState(Qt::Unchecked);

    if (!QString::compare(current_data[13], "TRUE")) ui->spc_cur_joint_tlock2->setCheckState(Qt::Checked);
    else ui->spc_cur_joint_rlock1->setCheckState(Qt::Unchecked);

    if (!QString::compare(current_data[14], "TRUE")) ui->spc_cur_joint_tlock3->setCheckState(Qt::Checked);
    else ui->spc_cur_joint_rlock1->setCheckState(Qt::Unchecked);

    ui->spc_cur_joint_ang0_1->setText(current_data[15]);
    ui->spc_cur_joint_ang0_2->setText(current_data[16]);
    ui->spc_cur_joint_ang0_3->setText(current_data[17]);

    ui->spc_cur_joint_angrate0_1->setText(current_data[18]);
    ui->spc_cur_joint_angrate0_2->setText(current_data[19]);
    ui->spc_cur_joint_angrate0_3->setText(current_data[20]);

    ui->spc_cur_joint_disp0_1->setText(current_data[21]);
    ui->spc_cur_joint_disp0_2->setText(current_data[22]);
    ui->spc_cur_joint_disp0_3->setText(current_data[23]);

    ui->spc_cur_joint_dispr0_1->setText(current_data[24]);
    ui->spc_cur_joint_dispr0_2->setText(current_data[25]);
    ui->spc_cur_joint_dispr0_3->setText(current_data[26]);

    ui->spc_cur_joint_bigi_1->setText(current_data[27]);
    ui->spc_cur_joint_bigi_2->setText(current_data[28]);
    ui->spc_cur_joint_bigi_3->setText(current_data[29]);
    setQComboBox(ui->spc_cur_joint_bigi_seq, current_data[30]);

    ui->spc_cur_joint_bogo_1->setText(current_data[31]);
    ui->spc_cur_joint_bogo_2->setText(current_data[32]);
    ui->spc_cur_joint_bogo_3->setText(current_data[33]);
    setQComboBox(ui->spc_cur_joint_bogo_seq, current_data[34]);

    ui->spc_cur_joint_poswrt_in_1->setText(current_data[35]);
    ui->spc_cur_joint_poswrt_in_2->setText(current_data[36]);
    ui->spc_cur_joint_poswrt_in_3->setText(current_data[37]);

    ui->spc_cur_joint_poswrt_out_1->setText(current_data[38]);
    ui->spc_cur_joint_poswrt_out_2->setText(current_data[39]);
    ui->spc_cur_joint_poswrt_out_3->setText(current_data[40]);

    ui->spc_cur_joint_param_file->setText(current_data[41]);

}

void SPC_submenu::on_sections_tabBarClicked(int index)
{
    if (index == 3) on_spc_cur_body_list_itemClicked(ui->spc_cur_body_list->currentItem());
    else if (index == 4) on_spc_cur_joint_list_itemClicked(ui->spc_cur_joint_list->currentItem());
}

// Wheels -/+/Duplicate/Item Clicked

void SPC_submenu::on_spc_cur_wheel_remove_clicked()
{
    ui->spc_cur_wheel_list->removeItemWidget(ui->spc_cur_wheel_list->currentItem());
    wheels -= 1;
    ui->spc_cur_wheel_list->setCurrentRow(ui->spc_cur_wheel_list->count()-1);
}

void SPC_submenu::on_spc_cur_wheel_add_clicked()
{
    wheels += 1;

    QStringList tmp_data = {};

    tmp_data.append("blankline");
    tmp_data.append("0.0");
    tmp_data.append("1.0");
    tmp_data.append("0.0");
    tmp_data.append("0.0");
    tmp_data.append("0.14");
    tmp_data.append("50.0");
    tmp_data.append("0.012");
    tmp_data.append("0");
    tmp_data.append("0");
    tmp_data.append("NONE");

    ui->spc_cur_wheel_list->addItem("Wheel " + QString::number(wheels - 1));
    ui->spc_cur_wheel_list->setCurrentRow(ui->spc_cur_wheel_list->count()-1);

    ui->spc_cur_wheel_list->currentItem()->setData(0, "Wheel " + QString::number(wheels - 1));
    ui->spc_cur_wheel_list->currentItem()->setData(1, tmp_data);
    on_spc_cur_wheel_list_itemClicked(ui->spc_cur_wheel_list->currentItem());
}

void SPC_submenu::on_spc_cur_wheel_duplicate_clicked()
{
    wheels += 1;

    QStringList old_data = ui->spc_cur_wheel_list->currentItem()->data(1).toStringList();

    ui->spc_cur_wheel_list->addItem("Wheel " + QString::number(wheels - 1));
    ui->spc_cur_wheel_list->setCurrentRow(ui->spc_cur_wheel_list->count()-1);
    ui->spc_cur_wheel_list->currentItem()->setData(1, old_data);
}


void SPC_submenu::on_spc_cur_wheel_list_itemClicked(QListWidgetItem *item)
{
    receive_data();

    QStringList current_data = item->data(1).toStringList();
    item->setText(item->data(0).toString());

    ui->spc_cur_wheel_initmom->setText(current_data[1]);

    ui->spc_cur_wheel_axis_1->setText(current_data[2]);
    ui->spc_cur_wheel_axis_2->setText(current_data[3]);
    ui->spc_cur_wheel_axis_3->setText(current_data[4]);

    ui->spc_cur_wheel_maxtrq->setText(current_data[5]);
    ui->spc_cur_wheel_maxmom->setText(current_data[6]);

    ui->spc_cur_wheel_inertia->setText(current_data[7]);

    ui->spc_cur_wheel_body->setValue(current_data[8].toInt());
    ui->spc_cur_wheel_node->setValue(current_data[9].toInt());

    ui->spc_cur_wheel_drjit_file->setText(current_data[10]);
}

// MTB buttons


void SPC_submenu::on_spc_cur_mtb_remove_clicked()
{
    delete ui->spc_cur_mtb_list->currentItem();
    mtbs -= 1;
}


void SPC_submenu::on_spc_cur_mtb_add_clicked()
{
    mtbs += 1;

    QStringList tmp_data = {};

    tmp_data.append("blankline");
    tmp_data.append("180.0");
    tmp_data.append("1.0");
    tmp_data.append("0.0");
    tmp_data.append("0.0");
    tmp_data.append("0");

    ui->spc_cur_mtb_list->addItem("MTB " + QString::number(mtbs - 1));
    ui->spc_cur_mtb_list->setCurrentRow(ui->spc_cur_mtb_list->count()-1);

    ui->spc_cur_mtb_list->currentItem()->setData(0, "MTB " + QString::number(mtbs - 1));
    ui->spc_cur_mtb_list->currentItem()->setData(1, tmp_data);
    on_spc_cur_mtb_list_itemClicked(ui->spc_cur_mtb_list->currentItem());
}


void SPC_submenu::on_spc_cur_mtb_duplicate_clicked()
{
    mtbs += 1;

    QStringList old_data = ui->spc_cur_mtb_list->currentItem()->data(1).toStringList();

    ui->spc_cur_mtb_list->addItem("MTB " + QString::number(mtbs - 1));
    ui->spc_cur_mtb_list->setCurrentRow(ui->spc_cur_mtb_list->count()-1);
    ui->spc_cur_mtb_list->currentItem()->setData(1, old_data);
}


void SPC_submenu::on_spc_cur_mtb_list_itemClicked(QListWidgetItem *item)
{
    receive_data();

    QStringList current_data = item->data(1).toStringList();
    item->setText(item->data(0).toString());

    ui->spc_cur_mtb_sat->setText(current_data[1]);

    ui->spc_cur_mtb_axis_1->setText(current_data[2]);
    ui->spc_cur_mtb_axis_2->setText(current_data[3]);
    ui->spc_cur_mtb_axis_3->setText(current_data[4]);

    ui->spc_cur_mtb_node->setValue(current_data[5].toInt());
}

// Thruster buttons


void SPC_submenu::on_spc_cur_thruster_remove_clicked()
{

}


void SPC_submenu::on_spc_cur_thruster_add_clicked()
{
    thrusters += 1;

    QStringList tmp_data = {};

    tmp_data.append("blankline");
    tmp_data.append("PULSED");
    tmp_data.append("1.0");
    tmp_data.append("-1.0");
    tmp_data.append("0.0");
    tmp_data.append("0.0");
    tmp_data.append("0");
    tmp_data.append("0");

    ui->spc_cur_thruster_list->addItem("Thr " + QString::number(thrusters - 1));
    ui->spc_cur_thruster_list->setCurrentRow(ui->spc_cur_thruster_list->count()-1);

    ui->spc_cur_thruster_list->currentItem()->setData(0, "Thr " + QString::number(thrusters - 1));
    ui->spc_cur_thruster_list->currentItem()->setData(1, tmp_data);
    on_spc_cur_thruster_list_itemClicked(ui->spc_cur_thruster_list->currentItem());
}


void SPC_submenu::on_spc_cur_thruster_duplicate_clicked()
{
    thrusters += 1;

    QStringList old_data = ui->spc_cur_thruster_list->currentItem()->data(1).toStringList();

    ui->spc_cur_thruster_list->addItem("Thr " + QString::number(mtbs - 1));
    ui->spc_cur_thruster_list->setCurrentRow(ui->spc_cur_thruster_list->count()-1);
    ui->spc_cur_thruster_list->currentItem()->setData(1, old_data);
}


void SPC_submenu::on_spc_cur_thruster_list_itemClicked(QListWidgetItem *item)
{
    receive_data();

    QStringList current_data = item->data(1).toStringList();
    item->setText(item->data(0).toString());

    setQComboBox(ui->spc_cur_thruster_mode, current_data[1]);

    ui->spc_cur_thruster_force->setText(current_data[2]);

    ui->spc_cur_thruster_axis_1->setText(current_data[3]);
    ui->spc_cur_thruster_axis_2->setText(current_data[4]);
    ui->spc_cur_thruster_axis_3->setText(current_data[5]);

    ui->spc_cur_thruster_body->setValue(current_data[6].toInt());
    ui->spc_cur_thruster_node->setValue(current_data[7].toInt());
}

