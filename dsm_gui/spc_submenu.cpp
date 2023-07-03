#include "spc_submenu.h"
#include "ui_spc_submenu.h"
#include "dsm_gui_lib.h"
#include "spc_menu.h"

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

    inout_path = path;

    receive_data();
    apply_data();
}

void SPC_submenu::set_validators()
{

    // Fixed
    ui->spc_cur_orb_type->addItems(dsm_gui_lib::sortStringList(orbit_type));
    ui->spc_cur_pos_ref->addItems(dsm_gui_lib::sortStringList(pos_wrt_F));

    ui->spc_cur_angvel_frame1->addItems(dsm_gui_lib::sortStringList(angvel_wrt));
    ui->spc_cur_angvel_frame2->addItems(dsm_gui_lib::sortStringList(att_wrt));
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

    // used to capture the name of the item between equal signs (any number of equal signs, one space, then any string, then one space, then any number of equal signs)
    static QRegularExpression rx4("=+\\s([A-Za-z0-9 ]+)\\s=+");

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

        QRegularExpressionMatch match4 = rx4.match(line);
        spc_item_names.append(match4.captured(1));

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

    QString cur_item_name;

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
            ui->spc_cur_angvel_1->setText(line_items[0]);
            ui->spc_cur_angvel_2->setText(line_items[1]);
            ui->spc_cur_angvel_3->setText(line_items[2]);
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
            break;
        case 21: // Flex active
            if (!QString::compare(line_items[0], "TRUE")) ui->spc_cur_flex_on->setChecked(true);
            else ui->spc_cur_flex_off->setChecked(true);
            break;
        case 22: // include 2nd order flex terms
            if (!QString::compare(line_items[0], "TRUE")) ui->spc_cur_2flex_on->setChecked(true);
            else ui->spc_cur_2flex_off->setChecked(true);
            break;
        case 23: // Shaker file name
            if (!QString::compare(line_items[0], "NONE")) ui->spc_cur_shaker_file->setText("");
            else ui->spc_cur_shaker_file->setText(line_items[0]);
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

        switch (cur_entry) {
        case 0: // Body X Header
            cur_item_name = spc_item_names[line_num - 1];
            ui->spc_cur_body_list->addItem(cur_item_name);
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
            break;
        case 9: // Flex
            tmp_data.append(line_items[0]);
        }
        if (cur_entry==body_entries-1){
            ui->spc_cur_body_list->setCurrentRow(cur_item);
            ui->spc_cur_body_list->currentItem()->setData(0, cur_item_name);
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

            switch (cur_entry) {
            case 0: // Joint X Header
                cur_item_name = spc_item_names[line_num - 1];
                ui->spc_cur_joint_list->addItem(cur_item_name);
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
                break;
            }
            if (cur_entry==joint_entries-1){
                ui->spc_cur_joint_list->setCurrentRow(cur_item);
                ui->spc_cur_joint_list->currentItem()->setData(0, cur_item_name);
                ui->spc_cur_joint_list->currentItem()->setData(1, tmp_data);
                tmp_data.clear();
            }
        }
    }

    /********************** WHEELS *************************/

    wheels = spc_data[reset_ind_wheel + 2].toInt();

    wheel_drag = spc_data[reset_ind_wheel + 0].split(QRegExp("\\s"), Qt::SkipEmptyParts)[0];
    wheel_jitter = spc_data[reset_ind_wheel + 1].split(QRegExp("\\s"), Qt::SkipEmptyParts)[0];

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

            switch (cur_entry){
            case 0:
                cur_item_name = spc_item_names[line_num - 1];
                ui->spc_cur_wheel_list->addItem(cur_item_name);
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
                break;
            }
            if (cur_entry==wheel_entries-1){

                ui->spc_cur_wheel_list->setCurrentRow(cur_item);
                ui->spc_cur_wheel_list->currentItem()->setData(0, cur_item_name);
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

            switch (cur_entry){
            case 0:
                cur_item_name = spc_item_names[line_num - 1];
                ui->spc_cur_mtb_list->addItem(cur_item_name);
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
                ui->spc_cur_mtb_list->currentItem()->setData(0, cur_item_name);
                ui->spc_cur_mtb_list->currentItem()->setData(1, tmp_data);
                tmp_data.clear();
            }
        }
    }

    /**************** THRUSTERS ************************/

    thrusters = spc_data[reset_ind_thr].toInt();

    if (thrusters == 0) reset_ind_gyro = reset_ind_thr + thr_headers + thr_entries; // SC_Simple has an example wheel
    else reset_ind_gyro = reset_ind_thr + thr_headers + thr_entries*thrusters;


    ui->spc_cur_thruster_list->clear();
    tmp_data.clear();
    if (thrusters > 0){
        for (int line_num = reset_ind_thr + thr_headers; line_num<reset_ind_gyro; line_num++)
        {
            line_string = spc_string[line_num-1];
            line_items = spc_data[line_num-1].split(QRegExp("\\s"), Qt::SkipEmptyParts);

            long thr_line_num = line_num - reset_ind_thr - thr_headers;
            cur_item = floor(thr_line_num/thr_entries);
            cur_entry = thr_line_num % thr_entries;

            switch (cur_entry){
            case 0:
                cur_item_name = spc_item_names[line_num - 1];
                ui->spc_cur_thruster_list->addItem(cur_item_name);
                tmp_data.append("blankline");
                break; // header
            case 1:
                tmp_data.append(line_items[0]);
                break;
            case 2:
                tmp_data.append(line_items[0]);
                break;
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
            if (cur_entry==thr_entries-1){
                ui->spc_cur_thruster_list->setCurrentRow(cur_item);
                ui->spc_cur_thruster_list->currentItem()->setData(0, cur_item_name);
                ui->spc_cur_thruster_list->currentItem()->setData(1, tmp_data);
                tmp_data.clear();
            }
        }
    }

    /**************** GYROS ************************/

    gyros = spc_data[reset_ind_gyro].toInt();

    if (gyros == 0) reset_ind_mag = reset_ind_gyro + gyro_headers + gyro_entries; // SC_Simple has an example wheel
    else reset_ind_gyro = reset_ind_gyro + gyro_headers + gyro_entries*gyros;


    ui->spc_cur_gyro_list->clear();
    tmp_data.clear();
    if (gyros > 0){
        for (int line_num = reset_ind_gyro + gyro_headers; line_num<reset_ind_mag; line_num++)
        {
            line_string = spc_string[line_num-1];
            line_items = spc_data[line_num-1].split(QRegExp("\\s"), Qt::SkipEmptyParts);

            long gyro_line_num = line_num - reset_ind_gyro - gyro_headers;
            cur_item = floor(gyro_line_num/gyro_entries);
            cur_entry = gyro_line_num % gyro_entries;

            switch (cur_entry){
            case 0:
                cur_item_name = spc_item_names[line_num - 1];
                ui->spc_cur_gyro_list->addItem(cur_item_name);
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
            case 4:
                tmp_data.append(line_items[0]);
                break;
            case 5:
                tmp_data.append(line_items[0]);
                break;
            case 6:
                tmp_data.append(line_items[0]);
                break;
            case 7:
                tmp_data.append(line_items[0]);
                tmp_data.append(line_items[1]);
                break;
            case 8:
                tmp_data.append(line_items[0]);
                break;
            case 9:
                tmp_data.append(line_items[0]);
                break;
            case 10:
                tmp_data.append(line_items[0]);
                break;
            }
            if (cur_entry==gyro_entries-1){
                ui->spc_cur_gyro_list->setCurrentRow(cur_item);
                ui->spc_cur_gyro_list->currentItem()->setData(0, cur_item_name);
                ui->spc_cur_gyro_list->currentItem()->setData(1, tmp_data);
                tmp_data.clear();
            }
        }
    }

    /**************** MAGNETOMETERS ************************/

    mags = spc_data[reset_ind_mag].toInt();

    if (mags == 0) reset_ind_css = reset_ind_mag + mag_headers + mag_entries; // SC_Simple has an example wheel
    else reset_ind_css = reset_ind_mag + mag_headers + mag_entries*mags;


    ui->spc_cur_mag_list->clear();
    tmp_data.clear();
    if (mags > 0){
        for (int line_num = reset_ind_mag + mag_headers; line_num<reset_ind_css; line_num++)
        {
            line_string = spc_string[line_num-1];
            line_items = spc_data[line_num-1].split(QRegExp("\\s"), Qt::SkipEmptyParts);

            long mag_line_num = line_num - reset_ind_mag - mag_headers;
            cur_item = floor(mag_line_num/mag_entries);
            cur_entry = mag_line_num % mag_entries;

            switch (cur_entry){
            case 0:
                cur_item_name = spc_item_names[line_num - 1];
                ui->spc_cur_mag_list->addItem("Axis " + QString::number(cur_item));
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
            case 4:
                tmp_data.append(line_items[0]);
                break;
            case 5:
                tmp_data.append(line_items[0]);
                break;
            case 6:
                tmp_data.append(line_items[0]);
                break;
            case 7:
                tmp_data.append(line_items[0]);
                break;
            }
            if (cur_entry==mag_entries-1){
                ui->spc_cur_mag_list->setCurrentRow(cur_item);
                ui->spc_cur_mag_list->currentItem()->setData(0, cur_item_name);
                ui->spc_cur_mag_list->currentItem()->setData(1, tmp_data);
                tmp_data.clear();
            }
        }
    }

    /**************** CSS ************************/

    css_s = spc_data[reset_ind_css].toInt();

    if (css_s == 0) reset_ind_fss = reset_ind_css + css_headers + css_entries; // SC_Simple has an example wheel
    else reset_ind_fss = reset_ind_css + css_headers + css_entries*css_s;


    ui->spc_cur_css_list->clear();
    tmp_data.clear();
    if (css_s > 0){
        for (int line_num = reset_ind_css + css_headers; line_num<reset_ind_fss; line_num++)
        {
            line_string = spc_string[line_num-1];
            line_items = spc_data[line_num-1].split(QRegExp("\\s"), Qt::SkipEmptyParts);

            long css_line_num = line_num - reset_ind_css - css_headers;
            cur_item = floor(css_line_num/css_entries);
            cur_entry = css_line_num % css_entries;

            switch (cur_entry){
            case 0:
                cur_item_name = spc_item_names[line_num - 1];
                ui->spc_cur_css_list->addItem(cur_item_name);
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
            case 4:
                tmp_data.append(line_items[0]);
                break;
            case 5:
                tmp_data.append(line_items[0]);
                break;
            case 6:
                tmp_data.append(line_items[0]);
                break;
            case 7:
                tmp_data.append(line_items[0]);
                break;
            }
            if (cur_entry==css_entries-1){
                ui->spc_cur_css_list->setCurrentRow(cur_item);
                ui->spc_cur_css_list->currentItem()->setData(0, cur_item_name);
                ui->spc_cur_css_list->currentItem()->setData(1, tmp_data);
                tmp_data.clear();
            }
        }
    }

    /**************** FSS ************************/

    fss_s = spc_data[reset_ind_fss].toInt();

    if (fss_s == 0) reset_ind_strack = reset_ind_fss + fss_headers + fss_entries; // SC_Simple has an example wheel
    else reset_ind_strack = reset_ind_fss + fss_headers + fss_entries*fss_s;


    ui->spc_cur_fss_list->clear();
    tmp_data.clear();
    if (css_s > 0){
        for (int line_num = reset_ind_fss + fss_headers; line_num<reset_ind_strack; line_num++)
        {
            line_string = spc_string[line_num-1];
            line_items = spc_data[line_num-1].split(QRegExp("\\s"), Qt::SkipEmptyParts);

            long fss_line_num = line_num - reset_ind_fss - fss_headers;
            cur_item = floor(fss_line_num/fss_entries);
            cur_entry = fss_line_num % fss_entries;

            switch (cur_entry){
            case 0:
                cur_item_name = spc_item_names[line_num - 1];
                ui->spc_cur_fss_list->addItem(cur_item_name);
                tmp_data.append("blankline");
                break; // header
            case 1:
                tmp_data.append(line_items[0]);
                break;
            case 2:
                tmp_data.append(line_items[0]);
                tmp_data.append(line_items[1]);
                tmp_data.append(line_items[2]);
                tmp_data.append(line_items[3]);
                break;
            case 3:
                tmp_data.append(line_items[0]);
                break;
            case 4:
                tmp_data.append(line_items[0]);
                tmp_data.append(line_items[1]);
                break;
            case 5:
                tmp_data.append(line_items[0]);
                break;
            case 6:
                tmp_data.append(line_items[0]);
                break;
            case 7:
                tmp_data.append(line_items[0]);
                break;
            }
            if (cur_entry==fss_entries-1){
                ui->spc_cur_fss_list->setCurrentRow(cur_item);
                ui->spc_cur_fss_list->currentItem()->setData(0, cur_item_name);
                ui->spc_cur_fss_list->currentItem()->setData(1, tmp_data);
                tmp_data.clear();
            }
        }
    }

    /**************** STAR TRACKER ************************/

    stracks = spc_data[reset_ind_strack].toInt();

    if (stracks == 0) reset_ind_gps = reset_ind_strack + strack_headers + strack_entries; // SC_Simple has an example wheel
    else reset_ind_gps = reset_ind_strack + strack_headers + strack_entries*stracks;


    ui->spc_cur_strack_list->clear();
    tmp_data.clear();
    if (stracks > 0){
        for (int line_num = reset_ind_strack + strack_headers; line_num<reset_ind_gps; line_num++)
        {
            line_string = spc_string[line_num-1];
            line_items = spc_data[line_num-1].split(QRegExp("\\s"), Qt::SkipEmptyParts);

            long strack_line_num = line_num - reset_ind_strack - strack_headers;
            cur_item = floor(strack_line_num/strack_entries);
            cur_entry = strack_line_num % strack_entries;

            switch (cur_entry){
            case 0:
                cur_item_name = spc_item_names[line_num - 1];
                ui->spc_cur_strack_list->addItem(cur_item_name);
                tmp_data.append("blankline");
                break; // header
            case 1:
                tmp_data.append(line_items[0]);
                break;
            case 2:
                tmp_data.append(line_items[0]);
                tmp_data.append(line_items[1]);
                tmp_data.append(line_items[2]);
                tmp_data.append(line_items[3]);
                break;
            case 3:
                tmp_data.append(line_items[0]);
                break;
            case 4:
                tmp_data.append(line_items[0]);
                tmp_data.append(line_items[1]);
                break;
            case 5:
                tmp_data.append(line_items[0]);
                tmp_data.append(line_items[1]);
                tmp_data.append(line_items[2]);
                break;
            case 6:
                tmp_data.append(line_items[0]);
                tmp_data.append(line_items[1]);
                tmp_data.append(line_items[2]);
                break;
            case 7:
                tmp_data.append(line_items[0]);
                break;
            }
            if (cur_entry==strack_entries-1){
                ui->spc_cur_strack_list->setCurrentRow(cur_item);
                ui->spc_cur_strack_list->currentItem()->setData(0, cur_item_name);
                ui->spc_cur_strack_list->currentItem()->setData(1, tmp_data);
                tmp_data.clear();
            }
        }
    }

    /**************** GPS ************************/

    gps_s = spc_data[reset_ind_gps].toInt();

    if (gps_s == 0) reset_ind_acc = reset_ind_gps + gps_headers + gps_entries; // SC_Simple has an example wheel
    else reset_ind_acc = reset_ind_gps + gps_headers + gps_entries*gps_s;


    ui->spc_cur_gps_list->clear();
    tmp_data.clear();
    if (gps_s > 0){
        for (int line_num = reset_ind_gps + gps_headers; line_num<reset_ind_acc; line_num++)
        {
            line_string = spc_string[line_num-1];
            line_items = spc_data[line_num-1].split(QRegExp("\\s"), Qt::SkipEmptyParts);

            long gps_line_num = line_num - reset_ind_gps - gps_headers;
            cur_item = floor(gps_line_num/gps_entries);
            cur_entry = gps_line_num % gps_entries;

            switch (cur_entry){
            case 0:
                cur_item_name = spc_item_names[line_num - 1];
                ui->spc_cur_gps_list->addItem(cur_item_name);
                tmp_data.append("blankline");
                break; // header
            case 1:
                tmp_data.append(line_items[0]);
                break;
            case 2:
                tmp_data.append(line_items[0]);
                break;
            case 3:
                tmp_data.append(line_items[0]);
                break;
            case 4:
                tmp_data.append(line_items[0]);
                break;
            case 5:
                tmp_data.append(line_items[0]);
                break;
            }
            if (cur_entry==gps_entries-1){
                ui->spc_cur_gps_list->setCurrentRow(cur_item);
                ui->spc_cur_gps_list->currentItem()->setData(0, cur_item_name);
                ui->spc_cur_gps_list->currentItem()->setData(1, tmp_data);
                tmp_data.clear();
            }
        }
    }

    /**************** ACCELEROMETER ************************/

    accels = spc_data[reset_ind_acc].toInt();

    if (accels == 0) reset_ind_end = reset_ind_acc + accel_headers + acc_entries; // SC_Simple has an example wheel
    else reset_ind_end = reset_ind_acc + accel_headers + acc_entries*accels;


    ui->spc_cur_accel_list->clear();
    tmp_data.clear();
    if (gps_s > 0){
        for (int line_num = reset_ind_acc + accel_headers; line_num<reset_ind_end; line_num++)
        {
            line_string = spc_string[line_num-1];
            line_items = spc_data[line_num-1].split(QRegExp("\\s"), Qt::SkipEmptyParts);

            long acc_line_num = line_num - reset_ind_acc - accel_headers;
            cur_item = floor(acc_line_num/acc_entries);
            cur_entry = acc_line_num % acc_entries;

            switch (cur_entry){
            case 0:
                cur_item_name = spc_item_names[line_num - 1];
                ui->spc_cur_accel_list->addItem(cur_item_name);
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
            case 4:
                tmp_data.append(line_items[0]);
                break;
            case 5:
                tmp_data.append(line_items[0]);
                break;
            case 6:
                tmp_data.append(line_items[0]);
                break;
            case 7:
                tmp_data.append(line_items[0]);
                tmp_data.append(line_items[1]);
                break;
            case 8:
                tmp_data.append(line_items[0]);
                break;
            case 9:
                tmp_data.append(line_items[0]);
                break;
            case 10:
                tmp_data.append(line_items[0]);
                break;
            }
            if (cur_entry==acc_entries-1){
                ui->spc_cur_accel_list->setCurrentRow(cur_item);
                ui->spc_cur_accel_list->currentItem()->setData(0, cur_item_name);
                ui->spc_cur_accel_list->currentItem()->setData(1, tmp_data);
                tmp_data.clear();
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
    if (joints_valid == 1) {
        SPC_submenu::close();
    }
    else {
        int response = dsm_gui_lib::warning_message("The number of joints must equal number of bodies minus one. Click \"OK\" to disregard this warning and close.");
        if (response == QMessageBox::Cancel) {
            return;
        }
        else if (response == QMessageBox::Ok) {
            SPC_submenu::close();

        }
    }
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
            data_inp = ui->spc_cur_angvel_1->text() + " " + ui->spc_cur_angvel_2->text() + " " + ui->spc_cur_angvel_3->text();
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
            break;
        case 21: // Flex active
            if (ui->spc_cur_flex_on->isChecked()) data_inp = "TRUE";
            else data_inp = "FALSE";
            break;
        case 22: // include 2nd order flex terms
            if (ui->spc_cur_2flex_on->isChecked()) data_inp = "TRUE";
            else data_inp = "FALSE";
            break;
        case 23: // Shaker file name
            if (!QString::compare(ui->spc_cur_shaker_file->text(), "")) data_inp = "NONE";
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

    /****************************** SETUP ***************/
    QStringList tmp_data = {}; // Update the list widget data of the body tab we are currently on.
    /************************************* BODY SECTION ***********************************/

    reset_ind_joint = reset_ind_body + body_entries*bodies;

    if (ui->sections->currentIndex() == 3)
    {
        cur_item_row = ui->spc_cur_body_list->currentRow();
        if (cur_item_row == -1) return;

        for (int line_num = reset_ind_body; line_num<reset_ind_joint; line_num++)
        {

            long body_line_num = line_num - reset_ind_body;
            long cur_item = floor(body_line_num/body_entries);
            long cur_entry = body_line_num % body_entries;

            if (ui->spc_cur_body_list->currentRow() == cur_item)
            {
                switch (cur_entry) {
                case 0: // Body X Header
                    ui->spc_cur_body_list->currentItem()->setText(ui->spc_cur_body_name->text());
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
                    if (!QString::compare(ui->spc_cur_node_file->text(), "")) tmp_data.append("NONE");
                    else tmp_data.append(ui->spc_cur_node_file->text());

                    break;
                case 9:
                    if (!QString::compare(ui->spc_cur_flex_file->text(), "")) tmp_data.append("NONE");
                    else tmp_data.append(ui->spc_cur_flex_file->text());

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

    if (ui->sections->currentIndex() == 3) {
        ui->spc_cur_body_list->setCurrentRow(cur_item_row);
        on_spc_cur_body_list_itemClicked(ui->spc_cur_body_list->item(cur_item_row));
    }

    /************************************* JOINT SECTION ***********************************/

    spc_update.append("************************************************************************\n");
    spc_update.append("*************************** Joint Parameters ***************************\n");
    spc_update.append("************************************************************************\n");
    spc_update.append("         (Number of Joints is Number of Bodies minus one)\n");

    if (joints != bodies - 1){
        joints_valid = 0;
        dsm_gui_lib::error_message("The number of joints must equal number of bodies minus one.");
    }
    else joints_valid = 1;

    reset_ind_wheel = reset_ind_joint + joint_headers + joint_entries*joints;

    if (joints == 0)
    {
        spc_update.append("============================== Joint 0 ================================\n");
        spc_update.append("PASSIVE                     ! Type of joint (PASSIVE, ACTUATED, others)\n");
        spc_update.append("0 1                         ! Inner, outer body indices\n");
        spc_update.append("1   213   GIMBAL            ! RotDOF, Seq, GIMBAL or SPHERICAL\n");
        spc_update.append("0   123                     ! TrnDOF, Seq\n");
        spc_update.append("FALSE  FALSE  FALSE         ! RotDOF Locked\n");
        spc_update.append("FALSE  FALSE  FALSE         ! TrnDOF Locked\n");
        spc_update.append("0.0    0.0    0.0           ! Initial Angles [deg]\n");
        spc_update.append("0.0    0.0    0.0           ! Initial Rates, deg/sec\n");
        spc_update.append("0.0    0.0    0.0           ! Initial Displacements [m]\n");
        spc_update.append("0.0    0.0    0.0           ! Initial Displacement Rates, m/sec\n");
        spc_update.append("0.0   0.0  0.0  312         ! Bi to Gi Static Angles [deg] & Seq\n");
        spc_update.append("0.0   0.0  0.0  312         ! Go to Bo Static Angles [deg] & Seq\n");
        spc_update.append("0.0   0.0  0.0              ! Position wrt inner body origin, m\n");
        spc_update.append("0.0   0.0  0.0              ! Position wrt outer body origin, m\n");
        spc_update.append("NONE                        ! Parameter File Name\n");
    }

    if (ui->sections->currentIndex() == 4)
    {

        cur_item_row = ui->spc_cur_joint_list->currentRow();
        if (cur_item_row == -1) return;

        for (int line_num = reset_ind_joint + joint_headers; line_num<reset_ind_wheel; line_num++)
        {

            long joint_line_num = line_num - reset_ind_joint - joint_headers;
            long cur_item = floor(joint_line_num/joint_entries);
            long cur_entry = joint_line_num % joint_entries;

            if (ui->spc_cur_joint_list->count() > 0 && ui->spc_cur_joint_list->currentRow() == cur_item)
            {
                switch (cur_entry) {
                case 0: // Joint X Header
                    ui->spc_cur_joint_list->currentItem()->setText(ui->spc_cur_joint_name->text());
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
                    if (!QString::compare(ui->spc_cur_joint_param_file->text(), "")) tmp_data.append("NONE");
                    else tmp_data.append(ui->spc_cur_joint_param_file->text());
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
            spc_update.append("============================== " + ui->spc_cur_joint_list->currentItem()->text() + " ================================\n");
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
            data_inp = current_data[9] + " " + current_data[10] + " " + current_data[11];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! RotDOF Locked\n");
            break;

        case 6:
            data_inp = current_data[12] + " " + current_data[13] + " " + current_data[14];
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

    if (ui->sections->currentIndex() == 3 && joints > 0) {
        ui->spc_cur_body_list->setCurrentRow(cur_item_row);
        on_spc_cur_body_list_itemClicked(ui->spc_cur_body_list->item(cur_item_row));
    }

    /*************************** WHEELS SECTION ****************************/
    reset_ind_mtb = reset_ind_wheel + wheel_headers + wheel_entries*wheels;

    spc_update.append("*************************** Wheel Parameters ***************************\n");

    if (!QString::compare(wheel_drag, "FALSE")) spc_update.append("FALSE                         ! Wheel Drag Active\n");
    else spc_update.append("TRUE                          ! Wheel Drag Active\n");

    if (!QString::compare(wheel_jitter, "FALSE")) spc_update.append("FALSE                         ! Wheel Jitter Active\n");
    else spc_update.append("TRUE                          ! Wheel Jitter Active\n");

    spc_update.append(dsm_gui_lib::whitespace(QString::number(wheels)) + "! Number of wheels\n");

    if (wheels == 0)
    {
        spc_update.append("=============================  Wheel 0  ================================\n");
        spc_update.append("0.0                         ! Initial Momentum, N-m-sec\n");
        spc_update.append("1.0   0.0   0.0             ! Wheel Axis Components, [X, Y, Z]\n");
        spc_update.append("0.14   50.0                 ! Max Torque (N-m), Momentum (N-m-sec)\n");
        spc_update.append("0.012                       ! Wheel Rotor Inertia, kg-m^2\n");
        spc_update.append("0                           ! Body\n");
        spc_update.append("0                           ! Node\n");
        spc_update.append("NONE                        ! Drag/Jitter Input File Name\n");
    }

    if (ui->sections->currentIndex() == 5 && ui->actuator_sections->currentIndex()==0)
    {

        cur_item_row = ui->spc_cur_wheel_list->currentRow();
        if (cur_item_row == -1) return;

        for (int line_num = reset_ind_wheel + wheel_headers; line_num<reset_ind_mtb; line_num++)
        {

            long wheel_line_num = line_num - reset_ind_wheel - wheel_headers;
            long cur_item = floor(wheel_line_num/wheel_entries);
            long cur_entry = wheel_line_num % wheel_entries;

            if (ui->spc_cur_wheel_list->count() > 0 && ui->spc_cur_wheel_list->currentRow() == cur_item)
            {

                switch (cur_entry){
                case 0:
                    ui->spc_cur_wheel_list->currentItem()->setText(ui->spc_cur_wheel_name->text());
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
                    if (!QString::compare(ui->spc_cur_wheel_drjit_file->text(), "")) tmp_data.append("NONE");
                    else tmp_data.append(ui->spc_cur_wheel_drjit_file->text());
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
            spc_update.append("=============================  " + ui->spc_cur_wheel_list->currentItem()->text() + "  ================================\n");
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

    if (ui->sections->currentIndex() == 5 && ui->actuator_sections->currentIndex()==0 && wheels > 0) {
        ui->spc_cur_wheel_list->setCurrentRow(cur_item_row);
        on_spc_cur_wheel_list_itemClicked(ui->spc_cur_wheel_list->item(cur_item_row));
    }

    /***************************** MTB SECTION ******************************/
    reset_ind_thr = reset_ind_mtb + mtb_headers + mtb_entries*mtbs;

    spc_update.append("**************************** MTB Parameters ****************************\n");
    spc_update.append(dsm_gui_lib::whitespace(QString::number(mtbs)) + "! Number of MTBs\n");


    if (mtbs == 0)
    {
        spc_update.append("==============================  MTB 0  =================================\n");
        spc_update.append("180.0                       ! Saturation (A-m^2)\n");
        spc_update.append("1.0   0.0   0.0             ! Wheel Axis Components, [X, Y, Z]\n");
        spc_update.append("1.0   0.0   0.0             ! MTB Axis Components, [X, Y, Z]\n");
        spc_update.append("0                           ! Node\n");
    }

    if (ui->sections->currentIndex() == 5 && ui->actuator_sections->currentIndex()==1)
    {

        cur_item_row = ui->spc_cur_mtb_list->currentRow();
        if (cur_item_row == -1) return;

        for (int line_num = reset_ind_mtb + mtb_headers; line_num<reset_ind_thr; line_num++)
        {

            long mtb_line_num = line_num - reset_ind_mtb - mtb_headers;
            long cur_item = floor(mtb_line_num/mtb_entries);
            long cur_entry = mtb_line_num % mtb_entries;

            if (ui->spc_cur_mtb_list->count() > 0 && ui->spc_cur_mtb_list->currentRow() == cur_item)
            {
                switch (cur_entry){
                case 0:
                    ui->spc_cur_mtb_list->currentItem()->setText(ui->spc_cur_mtb_name->text());
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
            spc_update.append("=============================  " + ui->spc_cur_mtb_list->currentItem()->text() + "  ================================\n");

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

    if (ui->sections->currentIndex() == 5 && ui->actuator_sections->currentIndex()==1 && mtbs > 0) {
        ui->spc_cur_mtb_list->setCurrentRow(cur_item_row);
        on_spc_cur_mtb_list_itemClicked(ui->spc_cur_mtb_list->item(cur_item_row));
    }

    /***************************** THRUSTER SECTION ******************************/
    reset_ind_gyro = reset_ind_thr + thr_headers + thr_entries*thrusters;

    spc_update.append("************************* Thruster Parameters **************************\n");
    spc_update.append(dsm_gui_lib::whitespace(QString::number(thrusters)) + "! Number of Thrusters\n");

    if (thrusters == 0)
    {
        spc_update.append("==============================  Thr 0  =================================\n");
        spc_update.append("PULSED                      ! Mode (PULSED or PROPORTIONAL)\n");
        spc_update.append("1.0                         ! Thrust Force (N)\n");
        spc_update.append("-1.0  0.0  0.0              ! Thrust Axis \n");
        spc_update.append("0                           ! Body\n");
        spc_update.append("0                           ! Node\n");
    }

    if (ui->sections->currentIndex() == 5 && ui->actuator_sections->currentIndex()==2)
    {

        cur_item_row = ui->spc_cur_thruster_list->currentRow();
        if (cur_item_row == -1) return;

        for (int line_num = reset_ind_thr + thr_headers; line_num<reset_ind_gyro; line_num++)
        {

            long thr_line_num = line_num - reset_ind_thr - thr_headers;
            long cur_item = floor(thr_line_num/thr_entries);
            long cur_entry = thr_line_num % thr_entries;

            if (ui->spc_cur_thruster_list->count() > 0 && ui->spc_cur_thruster_list->currentRow() == cur_item)
            {
                switch (cur_entry){
                case 0:
                    ui->spc_cur_thruster_list->currentItem()->setText(ui->spc_cur_thruster_name->text());
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
            spc_update.append("=============================  " + ui->spc_cur_thruster_list->currentItem()->text() + "  ================================\n");

            break; // header
        case 1:
            data_inp = current_data[1];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Mode (PULSED or PROPORTIONAL)\n");
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

    if (ui->sections->currentIndex() == 5 && ui->actuator_sections->currentIndex()==2 && thrusters > 0) {
        ui->spc_cur_thruster_list->setCurrentRow(cur_item_row);
        on_spc_cur_thruster_list_itemClicked(ui->spc_cur_thruster_list->item(cur_item_row));
    }

    /***************************** GYRO SECTION ******************************/
    reset_ind_mag = reset_ind_gyro + gyro_headers + gyro_entries*gyros;

    spc_update.append("******************************* Gyro ************************************\n");
    spc_update.append(dsm_gui_lib::whitespace(QString::number(gyros)) + "! Number of Gyro Axes\n");

    if (gyros == 0)
    {
        spc_update.append("============================== Axis 0 ===================================\n");
        spc_update.append("0.1                         ! Sample Time,sec\n");
        spc_update.append("1.0  0.0  0.0               ! Axis expressed in Body Frame\n");
        spc_update.append("1000.0                      ! Max Rate, deg/sec\n");
        spc_update.append("100.0                       ! Scale Factor Error, ppm\n");
        spc_update.append("1.0                         ! Quantization, arcsec \n");
        spc_update.append("0.07                        ! Angle Random Walk (deg/rt-hr)\n");
        spc_update.append("0.1  1.0                    ! Bias Stability (deg/hr) over timespan (hr)\n");
        spc_update.append("0.1                         ! Angle Noise, arcsec RMS\n");
        spc_update.append("0.1                         ! Initial Bias (deg/hr)\n");
        spc_update.append("0                           ! Node\n");
    }

    if (ui->sections->currentIndex() == 6 && ui->sensor_sections->currentIndex()==0)
    {

        cur_item_row = ui->spc_cur_gyro_list->currentRow();
        if (cur_item_row == -1) return;

        for (int line_num = reset_ind_gyro + gyro_headers; line_num<reset_ind_mag; line_num++)
        {

            long gyro_line_num = line_num - reset_ind_gyro - gyro_headers;
            long cur_item = floor(gyro_line_num/gyro_entries);
            long cur_entry = gyro_line_num % gyro_entries;

            if (ui->spc_cur_gyro_list->count() > 0 && ui->spc_cur_gyro_list->currentRow() == cur_item)
            {
                switch (cur_entry){
                case 0:
                    ui->spc_cur_gyro_list->currentItem()->setText(ui->spc_cur_gyro_name->text());
                    tmp_data.append("blankline");

                    break; // header
                case 1:
                    tmp_data.append(ui->spc_cur_gyro_samptime->text());
                    break;
                case 2:
                    tmp_data.append(ui->spc_cur_gyro_axis_1->text());
                    tmp_data.append(ui->spc_cur_gyro_axis_2->text());
                    tmp_data.append(ui->spc_cur_gyro_axis_3->text());
                    break;
                case 3:
                    tmp_data.append(ui->spc_cur_gyro_maxrate->text());
                    break;
                case 4:
                    tmp_data.append(ui->spc_cur_gyro_scaleferror->text());
                    break;
                case 5:
                    tmp_data.append(ui->spc_cur_gyro_quant->text());
                    break;
                case 6:
                    tmp_data.append(ui->spc_cur_gyro_angrwalk->text());
                    break;
                case 7:
                    tmp_data.append(ui->spc_cur_gyro_bias_stab->text());
                    tmp_data.append(ui->spc_cur_gyro_bias_tspan->text());
                    break;
                case 8:
                    tmp_data.append(ui->spc_cur_gyro_angnoise->text());
                    break;
                case 9:
                    tmp_data.append(ui->spc_cur_gyro_initbias->text());
                    break;
                case 10:
                    tmp_data.append(ui->spc_cur_gyro_node->text());
                    break;
                }
            }
            ui->spc_cur_gyro_list->currentItem()->setData(1, tmp_data);
        }
    }

    tmp_data.clear();


    for (int line_num = reset_ind_gyro + gyro_headers; line_num<reset_ind_mag; line_num++)
    {
        QString data_inp = "";

        long gyro_line_num = line_num - reset_ind_gyro - gyro_headers;
        long cur_item = floor(gyro_line_num/gyro_entries);
        long cur_entry = gyro_line_num % gyro_entries;

        ui->spc_cur_gyro_list->setCurrentRow(cur_item);
        QStringList current_data = ui->spc_cur_gyro_list->currentItem()->data(1).toStringList();

        switch (cur_entry){
        case 0:
            spc_update.append("=============================  " + ui->spc_cur_gyro_list->currentItem()->text() + "  ================================\n");

            break; // header
        case 1:
            data_inp = current_data[1];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Sample Time,sec\n");
            break;
        case 2:
            data_inp =  current_data[2] + " " +  current_data[3] + " " +  current_data[4];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Axis expressed in Body Frame\n");
            break;
        case 3:
            data_inp =  current_data[5];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Max Rate, deg/sec\n");
            break;
        case 4:
            data_inp =  current_data[6];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Scale Factor Error, ppm\n");
            break;
        case 5:
            data_inp =  current_data[7];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Quantization, arcsec \n");
            break;
        case 6:
            data_inp =  current_data[8];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Angle Random Walk (deg/rt-hr)\n");
            break;
        case 7:
            data_inp =  current_data[9] + " " + current_data[10];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Bias Stability (deg/hr) over timespan (hr)\n");
            break;
        case 8:
            data_inp =  current_data[11];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Angle Noise, arcsec RMS\n");
            break;
        case 9:
            data_inp =  current_data[12];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Initial Bias (deg/hr)\n");
            break;
        case 10:
            data_inp =  current_data[13];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Node\n");
            break;
        }
    }

    if (ui->sections->currentIndex() == 6 && ui->sensor_sections->currentIndex()==0 && gyros > 0) {
        ui->spc_cur_gyro_list->setCurrentRow(cur_item_row);
        on_spc_cur_gyro_list_itemClicked(ui->spc_cur_gyro_list->item(cur_item_row));
    }

    /***************************** MAGNETOMETER SECTION ******************************/
    reset_ind_css = reset_ind_mag + mag_headers + mag_entries*mags;

    spc_update.append("*************************** Magnetometer ********************************\n");
    spc_update.append(dsm_gui_lib::whitespace(QString::number(mags)) + "! Number of Magnetometer Axes\n");

    if (mags == 0)
    {
        spc_update.append("============================== Axis 0 ===================================\n");
        spc_update.append("0.1                         ! Sample Time,sec\n");
        spc_update.append("1.0  0.0  0.0               ! Axis expressed in Body Frame\n");
        spc_update.append("60.0E-6                     ! Saturation, Tesla\n");
        spc_update.append("0.0                         ! Scale Factor Error, ppm\n");
        spc_update.append("1.0                         ! Quantization, arcsec \n");
        spc_update.append("1.0E-6                      ! Quantization, Tesla \n");
        spc_update.append("1.0E-6                      ! Noise, Tesla RMS\n");
        spc_update.append("0                           ! Node\n");
    }


    if (ui->sections->currentIndex() == 6 && ui->sensor_sections->currentIndex()==1)
    {

        cur_item_row = ui->spc_cur_mag_list->currentRow();
        if (cur_item_row == -1) return;

        for (int line_num = reset_ind_mag + mag_headers; line_num<reset_ind_css; line_num++)
        {

            long mag_line_num = line_num - reset_ind_mag - mag_headers;
            long cur_item = floor(mag_line_num/mag_entries);
            long cur_entry = mag_line_num % mag_entries;

            if (ui->spc_cur_mag_list->count() > 0 && ui->spc_cur_mag_list->currentRow() == cur_item)
            {
                switch (cur_entry){
                case 0:
                    ui->spc_cur_mag_list->currentItem()->setText(ui->spc_cur_mag_name->text());
                    tmp_data.append("blankline");

                    break; // header
                case 1:
                    tmp_data.append(ui->spc_cur_mag_samptime->text());
                    break;
                case 2:
                    tmp_data.append(ui->spc_cur_mag_axis_1->text());
                    tmp_data.append(ui->spc_cur_mag_axis_2->text());
                    tmp_data.append(ui->spc_cur_mag_axis_3->text());
                    break;
                case 3:
                    tmp_data.append(ui->spc_cur_mag_sat->text());
                    break;
                case 4:
                    tmp_data.append(ui->spc_cur_mag_scaleferror->text());
                    break;
                case 5:
                    tmp_data.append(ui->spc_cur_mag_quant->text());
                    break;
                case 6:
                    tmp_data.append(ui->spc_cur_mag_noise->text());
                    break;
                case 7:
                    tmp_data.append(ui->spc_cur_mag_node->text());
                    break;
                }
            }
            ui->spc_cur_mag_list->currentItem()->setData(1, tmp_data);
        }
    }

    tmp_data.clear();


    for (int line_num = reset_ind_mag + mag_headers; line_num<reset_ind_css; line_num++)
    {
        QString data_inp = "";

        long mag_line_num = line_num - reset_ind_mag - mag_headers;
        long cur_item = floor(mag_line_num/mag_entries);
        long cur_entry = mag_line_num % mag_entries;

        ui->spc_cur_mag_list->setCurrentRow(cur_item);
        QStringList current_data = ui->spc_cur_mag_list->currentItem()->data(1).toStringList();

        switch (cur_entry){
        case 0:
            spc_update.append("=============================  " + ui->spc_cur_mag_list->currentItem()->text() + "  ================================\n");

            break; // header
        case 1:
            data_inp = current_data[1];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Sample Time,sec\n");
            break;
        case 2:
            data_inp =  current_data[2] + " " +  current_data[3] + " " +  current_data[4];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Axis expressed in Body Frame\n");
            break;
        case 3:
            data_inp =  current_data[5];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Saturation, Tesla\n");
            break;
        case 4:
            data_inp =  current_data[6];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Scale Factor Error, ppm\n");
            break;
        case 5:
            data_inp =  current_data[7];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Quantization, Tesla \n");
            break;
        case 6:
            data_inp =  current_data[8];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Noise, Tesla RMS\n");
            break;
        case 7:
            data_inp =  current_data[9];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Node\n");
            break;
        }
    }

    if (ui->sections->currentIndex() == 6 && ui->sensor_sections->currentIndex()==1 && mags > 0) {
        ui->spc_cur_mag_list->setCurrentRow(cur_item_row);
        on_spc_cur_mag_list_itemClicked(ui->spc_cur_mag_list->item(cur_item_row));
    }

    /***************************** CSS SECTION ******************************/
    reset_ind_fss = reset_ind_css + css_headers + css_entries*css_s;

    spc_update.append("*********************** Coarse Sun Sensor *******************************\n");
    spc_update.append(dsm_gui_lib::whitespace(QString::number(css_s)) + "! Number of Coarse Sun Sensors\n");

    if (css_s == 0)
    {
        spc_update.append("============================== CSS 0 ====================================\n");
        spc_update.append("0.1                         ! Sample Time,sec\n");
        spc_update.append("1.0  0.0  0.0               ! Axis expressed in Body Frame\n");
        spc_update.append("90.0                        ! Half-cone Angle, deg\n");
        spc_update.append("1.0                         ! Scale Factor\n");
        spc_update.append("0.001                       ! Quantization\n");
        spc_update.append("0                           ! Body\n");
        spc_update.append("0                           ! Node\n");
    }


    if (ui->sections->currentIndex() == 6 && ui->sensor_sections->currentIndex()==2)
    {

        cur_item_row = ui->spc_cur_css_list->currentRow();
        if (cur_item_row == -1) return;

        for (int line_num = reset_ind_css + css_headers; line_num<reset_ind_fss; line_num++)
        {

            long css_line_num = line_num - reset_ind_css - css_headers;
            long cur_item = floor(css_line_num/css_entries);
            long cur_entry = css_line_num % css_entries;

            if (ui->spc_cur_css_list->count() > 0 && ui->spc_cur_css_list->currentRow() == cur_item)
            {
                switch (cur_entry){
                case 0:
                    ui->spc_cur_css_list->currentItem()->setText(ui->spc_cur_css_name->text());
                    tmp_data.append("blankline");

                    break; // header
                case 1:
                    tmp_data.append(ui->spc_cur_css_samptime->text());
                    break;
                case 2:
                    tmp_data.append(ui->spc_cur_css_axis_1->text());
                    tmp_data.append(ui->spc_cur_css_axis_2->text());
                    tmp_data.append(ui->spc_cur_css_axis_3->text());
                    break;
                case 3:
                    tmp_data.append(ui->spc_cur_css_halfcone->text());
                    break;
                case 4:
                    tmp_data.append(ui->spc_cur_css_scale->text());
                    break;
                case 5:
                    tmp_data.append(ui->spc_cur_css_quant->text());
                    break;
                case 6:
                    tmp_data.append(ui->spc_cur_css_body->text());
                    break;
                case 7:
                    tmp_data.append(ui->spc_cur_css_node->text());
                    break;
                }
            }
            ui->spc_cur_css_list->currentItem()->setData(1, tmp_data);
        }
    }

    tmp_data.clear();


    for (int line_num = reset_ind_css + css_headers; line_num<reset_ind_fss; line_num++)
    {
        QString data_inp = "";

        long css_line_num = line_num - reset_ind_css - css_headers;
        long cur_item = floor(css_line_num/css_entries);
        long cur_entry = css_line_num % css_entries;

        ui->spc_cur_css_list->setCurrentRow(cur_item);
        QStringList current_data = ui->spc_cur_css_list->currentItem()->data(1).toStringList();

        switch (cur_entry){
        case 0:
            spc_update.append("=============================  " + ui->spc_cur_css_list->currentItem()->text() + "  ================================\n");

            break; // header
        case 1:
            data_inp = current_data[1];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Sample Time,sec\n");
            break;
        case 2:
            data_inp =  current_data[2] + " " +  current_data[3] + " " +  current_data[4];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Axis expressed in Body Frame\n");
            break;
        case 3:
            data_inp =  current_data[5];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Half-cone Angle, deg\n");
            break;
        case 4:
            data_inp =  current_data[6];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Scale Factor\n");
            break;
        case 5:
            data_inp =  current_data[7];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Quantization\n");
            break;
        case 6:
            data_inp =  current_data[8];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Body\n");
            break;
        case 7:
            data_inp =  current_data[9];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Node\n");
            break;
        }
    }

    if (ui->sections->currentIndex() == 6 && ui->sensor_sections->currentIndex()==2 && css_s > 0) {
        ui->spc_cur_css_list->setCurrentRow(cur_item_row);
        on_spc_cur_css_list_itemClicked(ui->spc_cur_css_list->item(cur_item_row));
    }

    /***************************** FSS SECTION ******************************/
    reset_ind_strack = reset_ind_fss + fss_headers + fss_entries*fss_s;

    spc_update.append("************************* Fine Sun Sensor *******************************\n");
    spc_update.append(dsm_gui_lib::whitespace(QString::number(fss_s)) + "! Number of Fine Sun Sensors\n");

    if (fss_s == 0)
    {
        spc_update.append("=============================== FSS 0 ===================================\n");
        spc_update.append("0.2                         ! Sample Time,sec\n");
        spc_update.append("70.0  0.0  0.0  231         ! Mounting Angles (deg), Seq in Body\n");
        spc_update.append("Z_AXIS                      ! Boresight Axis X_AXIS, Y_AXIS, or Z_AXIS\n");
        spc_update.append("32.0   32.0                 ! H, V FOV Size, deg\n");
        spc_update.append("0.1                         ! Noise Equivalent Angle, deg RMS\n");
        spc_update.append("0.5                         ! Quantization, deg\n");
        spc_update.append("0                           ! Node\n");
    }

    if (ui->sections->currentIndex() == 6 && ui->sensor_sections->currentIndex()==3)
    {

        cur_item_row = ui->spc_cur_fss_list->currentRow();
        if (cur_item_row == -1) return;

        for (int line_num = reset_ind_fss + fss_headers; line_num<reset_ind_strack; line_num++)
        {

            long fss_line_num = line_num - reset_ind_fss - fss_headers;
            long cur_item = floor(fss_line_num/fss_entries);
            long cur_entry = fss_line_num % fss_entries;

            if (ui->spc_cur_fss_list->count() > 0 && ui->spc_cur_fss_list->currentRow() == cur_item)
            {
                switch (cur_entry){
                case 0:
                    ui->spc_cur_fss_list->currentItem()->setText(ui->spc_cur_fss_name->text());
                    tmp_data.append("blankline");

                    break; // header
                case 1:
                    tmp_data.append(ui->spc_cur_fss_samptime->text());
                    break;
                case 2:
                    tmp_data.append(ui->spc_cur_fss_mount_1->text());
                    tmp_data.append(ui->spc_cur_fss_mount_2->text());
                    tmp_data.append(ui->spc_cur_fss_mount_3->text());
                    tmp_data.append(ui->spc_cur_fss_mountseq->currentText());
                    break;
                case 3:
                    tmp_data.append(ui->spc_cur_fss_boreaxis->currentText());
                    break;
                case 4:
                    tmp_data.append(ui->spc_cur_fss_hfov->text());
                    tmp_data.append(ui->spc_cur_fss_vfov->text());
                    break;
                case 5:
                    tmp_data.append(ui->spc_cur_fss_noiseang->text());
                    break;
                case 6:
                    tmp_data.append(ui->spc_cur_fss_quant->text());
                    break;
                case 7:
                    tmp_data.append(ui->spc_cur_fss_node->text());
                    break;
                }
            }
            ui->spc_cur_fss_list->currentItem()->setData(1, tmp_data);
        }
    }

    tmp_data.clear();


    for (int line_num = reset_ind_fss + fss_headers; line_num<reset_ind_strack; line_num++)
    {
        QString data_inp = "";

        long fss_line_num = line_num - reset_ind_fss - fss_headers;
        long cur_item = floor(fss_line_num/fss_entries);
        long cur_entry = fss_line_num % fss_entries;

        ui->spc_cur_fss_list->setCurrentRow(cur_item);
        QStringList current_data = ui->spc_cur_fss_list->currentItem()->data(1).toStringList();

        switch (cur_entry){
        case 0:
            spc_update.append("=============================  " + ui->spc_cur_fss_list->currentItem()->text() + "  ================================\n");

            break; // header
        case 1:
            data_inp = current_data[1];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Sample Time,sec\n");
            break;
        case 2:
            data_inp =  current_data[2] + " " +  current_data[3] + " " +  current_data[4] + " " + current_data[5];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Mounting Angles (deg), Seq in Body\n");
            break;
        case 3:
            data_inp =  current_data[6];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Boresight Axis X_AXIS, Y_AXIS, or Z_AXIS\n");
            break;
        case 4:
            data_inp =  current_data[7] + " " + current_data[8];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! H, V FOV Size, deg\n");
            break;
        case 5:
            data_inp =  current_data[8];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Noise Equivalent Angle, deg RMS\n");
            break;
        case 6:
            data_inp =  current_data[9];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Quantization, deg\n");
            break;
        case 7:
            data_inp =  current_data[10];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Node\n");
            break;
        }
    }

    if (ui->sections->currentIndex() == 6 && ui->sensor_sections->currentIndex()==3 && fss_s > 0) {
        ui->spc_cur_fss_list->setCurrentRow(cur_item_row);
        on_spc_cur_fss_list_itemClicked(ui->spc_cur_fss_list->item(cur_item_row));
    }

    /***************************** STAR TRACKER SECTION ******************************/
    reset_ind_gps = reset_ind_strack + strack_headers + strack_entries*stracks;

    spc_update.append("************************** Star Tracker *********************************\n");
    spc_update.append(dsm_gui_lib::whitespace(QString::number(stracks)) + "! Number of Star Trackers\n");

    if (stracks == 0)
    {
        spc_update.append("=============================== ST 0 ====================================\n");
        spc_update.append("0.25                        ! Sample Time,sec\n");
        spc_update.append("-90.0  90.0  00.0  321      ! Mounting Angles (deg), Seq in Body\n");
        spc_update.append("Z_AXIS                      ! Boresight Axis X_AXIS, Y_AXIS, or Z_AXIS\n");
        spc_update.append("8.0   8.0                   ! H, V FOV Size, deg\n");
        spc_update.append("30.0  10.0  10.0            ! Sun, Earth, Moon Exclusion Angles, deg\n");
        spc_update.append("2.0  2.0  20.0              ! Noise Equivalent Angle, arcsec RMS\n");
        spc_update.append("0                           ! Node\n");
    }

    if (ui->sections->currentIndex() == 6 && ui->sensor_sections->currentIndex()==4)
    {

        cur_item_row = ui->spc_cur_strack_list->currentRow();
        if (cur_item_row == -1) return;

        for (int line_num = reset_ind_strack + strack_headers; line_num<reset_ind_gps; line_num++)
        {

            long strack_line_num = line_num - reset_ind_strack - strack_headers;
            long cur_item = floor(strack_line_num/strack_entries);
            long cur_entry = strack_line_num % strack_entries;

            if (ui->spc_cur_strack_list->count() > 0 && ui->spc_cur_strack_list->currentRow() == cur_item)
            {
                switch (cur_entry){
                case 0:
                    ui->spc_cur_strack_list->currentItem()->setText(ui->spc_cur_strack_name->text());
                    tmp_data.append("blankline");

                    break; // header
                case 1:
                    tmp_data.append(ui->spc_cur_strack_samptime->text());
                    break;
                case 2:
                    tmp_data.append(ui->spc_cur_strack_mount_1->text());
                    tmp_data.append(ui->spc_cur_strack_mount_2->text());
                    tmp_data.append(ui->spc_cur_strack_mount_3->text());
                    tmp_data.append(ui->spc_cur_strack_mountseq->currentText());
                    break;
                case 3:
                    tmp_data.append(ui->spc_cur_strack_boreaxis->currentText());
                    break;
                case 4:
                    tmp_data.append(ui->spc_cur_strack_hfov->text());
                    tmp_data.append(ui->spc_cur_strack_vfov->text());
                    break;
                case 5:
                    tmp_data.append(ui->spc_cur_strack_sun->text());
                    tmp_data.append(ui->spc_cur_strack_earth->text());
                    tmp_data.append(ui->spc_cur_strack_moon->text());
                    break;
                case 6:
                    tmp_data.append(ui->spc_cur_strack_noiseang_1->text());
                    tmp_data.append(ui->spc_cur_strack_noiseang_2->text());
                    tmp_data.append(ui->spc_cur_strack_noiseang_3->text());
                    break;
                case 7:
                    tmp_data.append(ui->spc_cur_strack_node->text());
                    break;
                }
            }
            ui->spc_cur_strack_list->currentItem()->setData(1, tmp_data);
        }
    }

    tmp_data.clear();


    for (int line_num = reset_ind_strack + strack_headers; line_num<reset_ind_gps; line_num++)
    {
        QString data_inp = "";

        long strack_line_num = line_num - reset_ind_strack - strack_headers;
        long cur_item = floor(strack_line_num/strack_entries);
        long cur_entry = strack_line_num % strack_entries;

        ui->spc_cur_strack_list->setCurrentRow(cur_item);
        QStringList current_data = ui->spc_cur_strack_list->currentItem()->data(1).toStringList();

        switch (cur_entry){
        case 0:
            spc_update.append("=============================  " + ui->spc_cur_strack_list->currentItem()->text() + "  ================================\n");

            break; // header
        case 1:
            data_inp = current_data[1];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Sample Time,sec\n");
            break;
        case 2:
            data_inp =  current_data[2] + " " +  current_data[3] + " " +  current_data[4] + " " + current_data[5];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Mounting Angles (deg), Seq in Body\n");
            break;
        case 3:
            data_inp =  current_data[6];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Boresight Axis X_AXIS, Y_AXIS, or Z_AXIS\n");
            break;
        case 4:
            data_inp =  current_data[7] + " " + current_data[8];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! H, V FOV Size, deg\n");
            break;
        case 5:
            data_inp =  current_data[8] + " " + current_data[9] + " " + current_data[10];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Sun, Earth, Moon Exclusion Angles, deg\n");
            break;
        case 6:
            data_inp =  current_data[11] + " " + current_data[12] + " " + current_data[13];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Noise Equivalent Angle, arcsec RMS\n");
            break;
        case 7:
            data_inp =  current_data[14];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Node\n");
            break;
        }
    }

    if (ui->sections->currentIndex() == 6 && ui->sensor_sections->currentIndex()==4 && stracks > 0) {
        ui->spc_cur_strack_list->setCurrentRow(cur_item_row);
        on_spc_cur_strack_list_itemClicked(ui->spc_cur_strack_list->item(cur_item_row));
    }

    /***************************** GPS SECTION ******************************/
    reset_ind_acc = reset_ind_gps + gps_headers + gps_entries*gps_s;

    spc_update.append("****************************** GPS **************************************\n");
    spc_update.append(dsm_gui_lib::whitespace(QString::number(gps_s)) + "! Number of GPS Receivers\n");

    if (gps_s == 0)
    {
        spc_update.append("============================= GPSR 0 ====================================\n");
        spc_update.append("0.25                        ! Sample Time,sec\n");
        spc_update.append("4.0                         ! Position Noise, m RMS\n");
        spc_update.append("0.02                        ! Velocity Noise, m/sec RMS\n");
        spc_update.append("20.0E-9                     ! Time Noise, sec RMS\n");
        spc_update.append("0                           ! Node\n");
    }

    if (ui->sections->currentIndex() == 6 && ui->sensor_sections->currentIndex()==5)
    {

        cur_item_row = ui->spc_cur_gps_list->currentRow();
        if (cur_item_row == -1) return;

        for (int line_num = reset_ind_gps + gps_headers; line_num<reset_ind_acc; line_num++)
        {

            long gps_line_num = line_num - reset_ind_gps - gps_headers;
            long cur_item = floor(gps_line_num/gps_entries);
            long cur_entry = gps_line_num % gps_entries;

            if (ui->spc_cur_gps_list->count() > 0 && ui->spc_cur_gps_list->currentRow() == cur_item)
            {
                switch (cur_entry){
                case 0:
                    ui->spc_cur_gps_list->currentItem()->setText(ui->spc_cur_gps_name->text());
                    tmp_data.append("blankline");

                    break; // header
                case 1:
                    tmp_data.append(ui->spc_cur_gps_samptime->text());
                    break;
                case 2:
                    tmp_data.append(ui->spc_cur_gps_posnoise->text());
                    break;
                case 3:
                    tmp_data.append(ui->spc_cur_gps_velnoise->text());
                    break;
                case 4:
                    tmp_data.append(ui->spc_cur_gps_timenoise->text());
                    break;
                case 5:
                    tmp_data.append(ui->spc_cur_gps_node->text());
                    break;
                }
            }
            ui->spc_cur_gps_list->currentItem()->setData(1, tmp_data);
        }
    }

    tmp_data.clear();


    for (int line_num = reset_ind_gps + gps_headers; line_num<reset_ind_acc; line_num++)
    {
        QString data_inp = "";

        long gps_line_num = line_num - reset_ind_gps - gps_headers;
        long cur_item = floor(gps_line_num/gps_entries);
        long cur_entry = gps_line_num % gps_entries;

        ui->spc_cur_gps_list->setCurrentRow(cur_item);
        QStringList current_data = ui->spc_cur_gps_list->currentItem()->data(1).toStringList();

        switch (cur_entry){
        case 0:
            spc_update.append("=============================  " + ui->spc_cur_gps_list->currentItem()->text() + "  ================================\n");

            break; // header
        case 1:
            data_inp = current_data[1];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Sample Time,sec\n");
            break;
        case 2:
            data_inp =  current_data[2];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Position Noise, m RMS\n");
            break;
        case 3:
            data_inp =  current_data[3];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Boresight Axis X_AXIS, Y_AXIS, or Z_AXIS\n");
            break;
        case 4:
            data_inp =  current_data[4];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Velocity Noise, m/sec RMS\n");
            break;
        case 5:
            data_inp =  current_data[5];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Time Noise, sec RMS\n");
            break;
        case 6:
            data_inp =  current_data[6];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Node\n");
            break;
        }
    }

    if (ui->sections->currentIndex() == 6 && ui->sensor_sections->currentIndex()==5 && gps_s > 0) {
        ui->spc_cur_gps_list->setCurrentRow(cur_item_row);
        on_spc_cur_gps_list_itemClicked(ui->spc_cur_gps_list->item(cur_item_row));
    }

    /***************************** ACCELEROMETER SECTION ******************************/
    reset_ind_end = reset_ind_acc + accel_headers + acc_entries*accels;

    spc_update.append("*************************** Accelerometer *******************************\n");
    spc_update.append(dsm_gui_lib::whitespace(QString::number(accels)) + "! Number of Accel Axes\n");

    if (accels == 0)
    {
        spc_update.append("============================== Axis 0 ===================================\n");
        spc_update.append("0.1                         ! Sample Time,sec\n");
        spc_update.append("1.0  0.0  0.0               ! Axis expressed in Body Frame\n");
        spc_update.append("1.0                         ! Max Acceleration (m/s^2)\n");
        spc_update.append("0.0                         ! Scale Factor Error, ppm\n");
        spc_update.append("0.05                        ! Quantization, m/s^2\n");
        spc_update.append("0.0                         ! DV Random Walk (m/s/rt-hr)\n");
        spc_update.append("0.0 1.0                     ! Bias Stability (m/s^2) over timespan (hr)\n");
        spc_update.append("0.0                         ! DV Noise, m/s\n");
        spc_update.append("0.5                         ! Initial Bias (m/s^2)\n");
        spc_update.append("0                           ! Node\n");
    }

    if (ui->sections->currentIndex() == 6 && ui->sensor_sections->currentIndex()==6)
    {

        cur_item_row = ui->spc_cur_accel_list->currentRow();
        if (cur_item_row == -1) return;

        for (int line_num = reset_ind_acc + accel_headers; line_num<reset_ind_end; line_num++)
        {

            long acc_line_num = line_num - reset_ind_acc - accel_headers;
            long cur_item = floor(acc_line_num/acc_entries);
            long cur_entry = acc_line_num % acc_entries;

            if (ui->spc_cur_accel_list->count() > 0 && ui->spc_cur_accel_list->currentRow() == cur_item)
            {
                switch (cur_entry){
                case 0:
                    ui->spc_cur_accel_list->currentItem()->setText(ui->spc_cur_acc_name->text());
                    tmp_data.append("blankline");

                    break; // header
                case 1:
                    tmp_data.append(ui->spc_cur_acc_samptime->text());
                    break;
                case 2:
                    tmp_data.append(ui->spc_cur_acc_axis_1->text());
                    tmp_data.append(ui->spc_cur_acc_axis_2->text());
                    tmp_data.append(ui->spc_cur_acc_axis_3->text());
                    break;
                case 3:
                    tmp_data.append(ui->spc_cur_acc_maxacc->text());
                    break;
                case 4:
                    tmp_data.append(ui->spc_cur_acc_scaleerror->text());
                    break;
                case 5:
                    tmp_data.append(ui->spc_cur_acc_quant->text());
                    break;
                case 6:
                    tmp_data.append(ui->spc_cur_acc_dvrandwalk->text());
                    break;
                case 7:
                    tmp_data.append(ui->spc_cur_acc_bias_stab->text());
                    tmp_data.append(ui->spc_cur_acc_bias_tspan->text());
                    break;
                case 8:
                    tmp_data.append(ui->spc_cur_acc_dvnoise->text());
                    break;
                case 9:
                    tmp_data.append(ui->spc_cur_acc_initbias->text());
                    break;
                case 10:
                    tmp_data.append(ui->spc_cur_acc_node->text());
                    break;
                }
            }
            ui->spc_cur_accel_list->currentItem()->setData(1, tmp_data);
        }
    }

    tmp_data.clear();


    for (int line_num = reset_ind_acc + accel_headers; line_num<reset_ind_end; line_num++)
    {
        QString data_inp = "";

        long acc_line_num = line_num - reset_ind_acc - accel_headers;
        long cur_item = floor(acc_line_num/acc_entries);
        long cur_entry = acc_line_num % acc_entries;

        ui->spc_cur_accel_list->setCurrentRow(cur_item);
        QStringList current_data = ui->spc_cur_accel_list->currentItem()->data(1).toStringList();

        switch (cur_entry){
        case 0:
            spc_update.append("=============================  " + ui->spc_cur_accel_list->currentItem()->text() + "  ================================\n");

            break; // header
        case 1:
            data_inp = current_data[1];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Sample Time,sec\n");
            break;
        case 2:
            data_inp =  current_data[2] + " " + current_data[3] + " " + current_data[4];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Axis expressed in Body Frame\n");
            break;
        case 3:
            data_inp =  current_data[5];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Max Acceleration (m/s^2)\n");
            break;
        case 4:
            data_inp =  current_data[6];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Scale Factor Error, ppm\n");
            break;
        case 5:
            data_inp =  current_data[7];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Quantization, m/s^2\n");
            break;
        case 6:
            data_inp =  current_data[8];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! DV Random Walk (m/s/rt-hr)\n");
            break;
        case 7:
            data_inp =  current_data[9] + " " + current_data[10];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Bias Stability (m/s^2) over timespan (hr)\n");
            break;
        case 8:
            data_inp =  current_data[11];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! DV Noise, m/s\n");
            break;
        case 9:
            data_inp =  current_data[12];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Initial Bias (m/s^2)\n");
            break;
        case 10:
            data_inp =  current_data[13];
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+"! Node\n");
            break;
        }
    }

    if (ui->sections->currentIndex() == 6 && ui->sensor_sections->currentIndex()==6 && accels > 0) {
        ui->spc_cur_accel_list->setCurrentRow(cur_item_row);
        on_spc_cur_accel_list_itemClicked(ui->spc_cur_accel_list->item(cur_item_row));
    }

    write_data();
}

void SPC_submenu::setQComboBox(QComboBox *comboBox, QString string) {
    comboBox->setCurrentIndex(comboBox->findText(string));
}


// Body -/+/Duplicate/Item Clicked

void SPC_submenu::on_spc_cur_body_remove_clicked()
{
    delete ui->spc_cur_body_list->currentItem();
    if (bodies > 0) bodies -= 1;

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

    ui->spc_cur_body_list->addItem("New Body");
    ui->spc_cur_body_list->setCurrentRow(ui->spc_cur_body_list->count()-1);

    ui->spc_cur_body_list->currentItem()->setData(0, "New Body");
    ui->spc_cur_body_list->currentItem()->setData(1, tmp_data);
    on_spc_cur_body_list_itemClicked(ui->spc_cur_body_list->currentItem());
}


void SPC_submenu::on_spc_cur_body_duplicate_clicked()
{
    if (bodies == 0) return;

    bodies += 1;

    QStringList old_data = ui->spc_cur_body_list->currentItem()->data(1).toStringList();

    ui->spc_cur_body_list->addItem("New Body");
    ui->spc_cur_body_list->setCurrentRow(ui->spc_cur_body_list->count()-1);
    ui->spc_cur_body_list->currentItem()->setData(1, old_data);
    on_spc_cur_body_list_itemClicked(ui->spc_cur_body_list->currentItem());
}


void SPC_submenu::on_spc_cur_body_list_itemClicked(QListWidgetItem *item)
{

    receive_data();

    QStringList current_data = item->data(1).toStringList();
    item->setText(item->data(0).toString());

    ui->spc_cur_body_name->setText(item->data(0).toString());

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

    if (!QString::compare(current_data[18], "NONE")) ui->spc_cur_node_file->setText("");
    else ui->spc_cur_node_file->setText(current_data[18]);

    if (!QString::compare(current_data[19], "NONE")) ui->spc_cur_flex_file->setText("");
    else ui->spc_cur_node_file->setText(current_data[19]);
}

// Joint

void SPC_submenu::on_spc_cur_joint_remove_clicked()
{
    delete ui->spc_cur_joint_list->currentItem();
    if (joints > 0) joints -= 1;
}


void SPC_submenu::on_spc_cur_joint_add_clicked()
{
    joints += 1;

    QStringList tmp_data = {};

    tmp_data.append("blankline");
    tmp_data.append("PASSIVE");
    tmp_data.append("0");
    tmp_data.append("1");
    tmp_data.append("1");
    tmp_data.append("213");
    tmp_data.append("GIMBAL");
    tmp_data.append("0");
    tmp_data.append("123");
    tmp_data.append("FALSE");
    tmp_data.append("FALSE");
    tmp_data.append("FALSE");
    tmp_data.append("FALSE");
    tmp_data.append("FALSE");
    tmp_data.append("FALSE");
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
    tmp_data.append("0.0");
    tmp_data.append("0.0");
    tmp_data.append("0.0");
    tmp_data.append("312");
    tmp_data.append("0.0");
    tmp_data.append("0.0");
    tmp_data.append("0.0");
    tmp_data.append("312");
    tmp_data.append("0.0");
    tmp_data.append("0.0");
    tmp_data.append("0.0");
    tmp_data.append("0.0");
    tmp_data.append("0.0");
    tmp_data.append("0.0");
    tmp_data.append("NONE");

    ui->spc_cur_joint_list->addItem("New Joint");
    ui->spc_cur_joint_list->setCurrentRow(ui->spc_cur_joint_list->count()-1);

    ui->spc_cur_joint_list->currentItem()->setData(0, "New Joint");
    ui->spc_cur_joint_list->currentItem()->setData(1, tmp_data);
    on_spc_cur_body_list_itemClicked(ui->spc_cur_body_list->currentItem());
}


void SPC_submenu::on_spc_cur_joint_duplicate_clicked()
{
    if (joints == 0) return;
    joints += 1;

    QStringList old_data = ui->spc_cur_joint_list->currentItem()->data(1).toStringList();

    ui->spc_cur_joint_list->addItem("New Joint");
    ui->spc_cur_joint_list->setCurrentRow(ui->spc_cur_joint_list->count()-1);
    ui->spc_cur_joint_list->currentItem()->setData(1, old_data);
    on_spc_cur_joint_list_itemClicked(ui->spc_cur_joint_list->currentItem());
}

void SPC_submenu::on_spc_cur_joint_list_itemClicked(QListWidgetItem *item)
{
    receive_data();

    QStringList current_data = item->data(1).toStringList();
    item->setText(item->data(0).toString());

    ui->spc_cur_joint_name->setText(item->data(0).toString());

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

    if (!QString::compare(current_data[41], "NONE")) ui->spc_cur_joint_param_file->setText("");
    else ui->spc_cur_joint_param_file->setText(current_data[41]);

}

// Wheels -/+/Duplicate/Item Clicked

void SPC_submenu::on_spc_cur_wheel_remove_clicked()
{
    delete ui->spc_cur_wheel_list->currentItem();
    if (wheels > 0) wheels -= 1;
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

    ui->spc_cur_wheel_list->addItem("New Wheel");
    ui->spc_cur_wheel_list->setCurrentRow(ui->spc_cur_wheel_list->count()-1);

    ui->spc_cur_wheel_list->currentItem()->setData(0, "New Wheel");
    ui->spc_cur_wheel_list->currentItem()->setData(1, tmp_data);
    on_spc_cur_wheel_list_itemClicked(ui->spc_cur_wheel_list->currentItem());
}

void SPC_submenu::on_spc_cur_wheel_duplicate_clicked()
{
    if (wheels == 0) return;
    wheels += 1;

    QStringList old_data = ui->spc_cur_wheel_list->currentItem()->data(1).toStringList();

    ui->spc_cur_wheel_list->addItem("New Wheel");
    ui->spc_cur_wheel_list->setCurrentRow(ui->spc_cur_wheel_list->count()-1);
    ui->spc_cur_wheel_list->currentItem()->setData(1, old_data);
    on_spc_cur_wheel_list_itemClicked(ui->spc_cur_wheel_list->currentItem());
}


void SPC_submenu::on_spc_cur_wheel_list_itemClicked(QListWidgetItem *item)
{
    receive_data();

    QStringList current_data = item->data(1).toStringList();
    item->setText(item->data(0).toString());

    ui->spc_cur_wheel_name->setText(item->data(0).toString());

    ui->spc_cur_wheel_initmom->setText(current_data[1]);

    ui->spc_cur_wheel_axis_1->setText(current_data[2]);
    ui->spc_cur_wheel_axis_2->setText(current_data[3]);
    ui->spc_cur_wheel_axis_3->setText(current_data[4]);

    ui->spc_cur_wheel_maxtrq->setText(current_data[5]);
    ui->spc_cur_wheel_maxmom->setText(current_data[6]);

    ui->spc_cur_wheel_inertia->setText(current_data[7]);

    ui->spc_cur_wheel_body->setValue(current_data[8].toInt());
    ui->spc_cur_wheel_node->setValue(current_data[9].toInt());

    if (!QString::compare(current_data[10], "NONE")) ui->spc_cur_wheel_drjit_file->setText("");
    else ui->spc_cur_wheel_drjit_file->setText(current_data[10]);

    if (!QString::compare(wheel_drag, "FALSE")) ui->spc_cur_wheel_glob_drag_off->setChecked(Qt::Checked);
    else ui->spc_cur_wheel_glob_drag_on->setChecked(Qt::Checked);

    if (!QString::compare(wheel_jitter, "FALSE")) ui->spc_cur_wheel_glob_jitter_off->setChecked(Qt::Checked);
    else ui->spc_cur_wheel_glob_jitter_on->setChecked(Qt::Checked);
}

// MTB buttons


void SPC_submenu::on_spc_cur_mtb_remove_clicked()
{
    delete ui->spc_cur_mtb_list->currentItem();
    if (mtbs > 0) mtbs -= 1;
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

    ui->spc_cur_mtb_list->addItem("New MTB");
    ui->spc_cur_mtb_list->setCurrentRow(ui->spc_cur_mtb_list->count()-1);

    ui->spc_cur_mtb_list->currentItem()->setData(0, "New MTB");
    ui->spc_cur_mtb_list->currentItem()->setData(1, tmp_data);
    on_spc_cur_mtb_list_itemClicked(ui->spc_cur_mtb_list->currentItem());
}


void SPC_submenu::on_spc_cur_mtb_duplicate_clicked()
{
    if (mtbs == 0) return;
    mtbs += 1;

    QStringList old_data = ui->spc_cur_mtb_list->currentItem()->data(1).toStringList();

    ui->spc_cur_mtb_list->addItem("New MTB");
    ui->spc_cur_mtb_list->setCurrentRow(ui->spc_cur_mtb_list->count()-1);
    ui->spc_cur_mtb_list->currentItem()->setData(1, old_data);
    on_spc_cur_mtb_list_itemClicked(ui->spc_cur_mtb_list->currentItem());
}


void SPC_submenu::on_spc_cur_mtb_list_itemClicked(QListWidgetItem *item)
{
    receive_data();

    QStringList current_data = item->data(1).toStringList();
    item->setText(item->data(0).toString());

    ui->spc_cur_mtb_name->setText(item->data(0).toString());

    ui->spc_cur_mtb_sat->setText(current_data[1]);

    ui->spc_cur_mtb_axis_1->setText(current_data[2]);
    ui->spc_cur_mtb_axis_2->setText(current_data[3]);
    ui->spc_cur_mtb_axis_3->setText(current_data[4]);

    ui->spc_cur_mtb_node->setValue(current_data[5].toInt());
}

// Thruster buttons


void SPC_submenu::on_spc_cur_thruster_remove_clicked()
{
    delete ui->spc_cur_thruster_list->currentItem();
    if (thrusters > 0) thrusters -= 1;
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

    ui->spc_cur_thruster_list->addItem("New Thr");
    ui->spc_cur_thruster_list->setCurrentRow(ui->spc_cur_thruster_list->count()-1);

    ui->spc_cur_thruster_list->currentItem()->setData(0, "New Thr");
    ui->spc_cur_thruster_list->currentItem()->setData(1, tmp_data);
    on_spc_cur_thruster_list_itemClicked(ui->spc_cur_thruster_list->currentItem());
}


void SPC_submenu::on_spc_cur_thruster_duplicate_clicked()
{
    if (thrusters == 0) return;
    thrusters += 1;

    QStringList old_data = ui->spc_cur_thruster_list->currentItem()->data(1).toStringList();

    ui->spc_cur_thruster_list->addItem("Thr " + QString::number(mtbs - 1));
    ui->spc_cur_thruster_list->setCurrentRow(ui->spc_cur_thruster_list->count()-1);
    ui->spc_cur_thruster_list->currentItem()->setData(1, old_data);
    on_spc_cur_body_list_itemClicked(ui->spc_cur_thruster_list->currentItem());
}


void SPC_submenu::on_spc_cur_thruster_list_itemClicked(QListWidgetItem *item)
{
    receive_data();

    QStringList current_data = item->data(1).toStringList();
    item->setText(item->data(0).toString());

    ui->spc_cur_thruster_name->setText(item->data(0).toString());

    setQComboBox(ui->spc_cur_thruster_mode, current_data[1]);

    ui->spc_cur_thruster_force->setText(current_data[2]);

    ui->spc_cur_thruster_axis_1->setText(current_data[3]);
    ui->spc_cur_thruster_axis_2->setText(current_data[4]);
    ui->spc_cur_thruster_axis_3->setText(current_data[5]);

    ui->spc_cur_thruster_body->setValue(current_data[6].toInt());
    ui->spc_cur_thruster_node->setValue(current_data[7].toInt());
}

// Gyro buttons

void SPC_submenu::on_spc_cur_gyro_remove_clicked()
{
    delete ui->spc_cur_gyro_list->currentItem();
    if (gyros > 0) gyros -= 1;
}


void SPC_submenu::on_spc_cur_gyro_add_clicked()
{
    gyros += 1;

    QStringList tmp_data = {};

    tmp_data.append("blankline");
    tmp_data.append("0.1");
    tmp_data.append("1.0");
    tmp_data.append("0.0");
    tmp_data.append("0.0");
    tmp_data.append("1000.0");
    tmp_data.append("100.0");
    tmp_data.append("1.0");
    tmp_data.append("0.07");
    tmp_data.append("0.1");
    tmp_data.append("1.0");
    tmp_data.append("0.1");
    tmp_data.append("0.1");
    tmp_data.append("0");

    ui->spc_cur_gyro_list->addItem("New Gyro");
    ui->spc_cur_gyro_list->setCurrentRow(ui->spc_cur_gyro_list->count()-1);

    ui->spc_cur_gyro_list->currentItem()->setData(0, "New Gyro");
    ui->spc_cur_gyro_list->currentItem()->setData(1, tmp_data);
    on_spc_cur_gyro_list_itemClicked(ui->spc_cur_gyro_list->currentItem());
}


void SPC_submenu::on_spc_cur_gyro_duplicate_clicked()
{
    if (gyros == 0) return;
    gyros += 1;

    QStringList old_data = ui->spc_cur_gyro_list->currentItem()->data(1).toStringList();

    ui->spc_cur_gyro_list->addItem("New Gyro");
    ui->spc_cur_gyro_list->setCurrentRow(ui->spc_cur_gyro_list->count()-1);
    ui->spc_cur_gyro_list->currentItem()->setData(1, old_data);
    on_spc_cur_gyro_list_itemClicked(ui->spc_cur_gyro_list->currentItem());
}


void SPC_submenu::on_spc_cur_gyro_list_itemClicked(QListWidgetItem *item)
{
    receive_data();

    QStringList current_data = item->data(1).toStringList();
    item->setText(item->data(0).toString());

    ui->spc_cur_gyro_name->setText(item->data(0).toString());

    ui->spc_cur_gyro_samptime->setText(current_data[1]);

    ui->spc_cur_gyro_axis_1->setText(current_data[2]);
    ui->spc_cur_gyro_axis_2->setText(current_data[3]);
    ui->spc_cur_gyro_axis_3->setText(current_data[4]);

    ui->spc_cur_gyro_maxrate->setText(current_data[5]);

    ui->spc_cur_gyro_scaleferror->setText(current_data[6]);

    ui->spc_cur_gyro_quant->setText(current_data[7]);

    ui->spc_cur_gyro_angrwalk->setText(current_data[8]);

    ui->spc_cur_gyro_bias_stab->setText(current_data[9]);
    ui->spc_cur_gyro_bias_tspan->setText(current_data[10]);

    ui->spc_cur_gyro_angnoise->setText(current_data[11]);

    ui->spc_cur_gyro_initbias->setText(current_data[12]);

    ui->spc_cur_gyro_node->setValue(current_data[13].toInt());
}

// Magnetometer buttons

void SPC_submenu::on_spc_cur_mag_remove_clicked()
{
    delete ui->spc_cur_css_list->currentItem();
    if (mags > 0) mags -= 1;
}


void SPC_submenu::on_spc_cur_mag_add_clicked()
{
    mags += 1;

    QStringList tmp_data = {};

    tmp_data.append("blankline");
    tmp_data.append("0.1");
    tmp_data.append("1.0");
    tmp_data.append("0.0");
    tmp_data.append("0.0");
    tmp_data.append("60.0E-6");
    tmp_data.append("0.0");
    tmp_data.append("1.0E-6");
    tmp_data.append("1.0E-6");
    tmp_data.append("0");

    ui->spc_cur_mag_list->addItem("New Mag");
    ui->spc_cur_mag_list->setCurrentRow(ui->spc_cur_mag_list->count()-1);

    ui->spc_cur_mag_list->currentItem()->setData(0, "New Mag");
    ui->spc_cur_mag_list->currentItem()->setData(1, tmp_data);
    on_spc_cur_mag_list_itemClicked(ui->spc_cur_mag_list->currentItem());
}


void SPC_submenu::on_spc_cur_mag_duplicate_clicked()
{
    if (mags == 0) return;
    mags += 1;

    QStringList old_data = ui->spc_cur_mag_list->currentItem()->data(1).toStringList();

    ui->spc_cur_mag_list->addItem("New Mag");
    ui->spc_cur_mag_list->setCurrentRow(ui->spc_cur_mag_list->count()-1);
    ui->spc_cur_mag_list->currentItem()->setData(1, old_data);
    on_spc_cur_mag_list_itemClicked(ui->spc_cur_mag_list->currentItem());
}


void SPC_submenu::on_spc_cur_mag_list_itemClicked(QListWidgetItem *item)
{
    receive_data();

    QStringList current_data = item->data(1).toStringList();
    item->setText(item->data(0).toString());

    ui->spc_cur_mag_name->setText(item->data(0).toString());

    ui->spc_cur_mag_samptime->setText(current_data[1]);

    ui->spc_cur_mag_axis_1->setText(current_data[2]);
    ui->spc_cur_mag_axis_2->setText(current_data[3]);
    ui->spc_cur_mag_axis_3->setText(current_data[4]);

    ui->spc_cur_mag_sat->setText(current_data[5]);

    ui->spc_cur_mag_scaleferror->setText(current_data[6]);

    ui->spc_cur_mag_quant->setText(current_data[7]);

    ui->spc_cur_mag_noise->setText(current_data[8]);

    ui->spc_cur_mag_node->setValue(current_data[9].toInt());
}

// CSS buttons

void SPC_submenu::on_spc_cur_css_remove_clicked()
{
    delete ui->spc_cur_css_list->currentItem();
    if (css_s > 0) css_s -= 1;
}


void SPC_submenu::on_spc_cur_css_add_clicked()
{
    css_s += 1;

    QStringList tmp_data = {};

    tmp_data.append("blankline");
    tmp_data.append("0.1");
    tmp_data.append("1.0");
    tmp_data.append("0.0");
    tmp_data.append("0.0");
    tmp_data.append("90.0");
    tmp_data.append("1.0");
    tmp_data.append("0.001");
    tmp_data.append("0");
    tmp_data.append("0");

    ui->spc_cur_css_list->addItem("New CSS");
    ui->spc_cur_css_list->setCurrentRow(ui->spc_cur_css_list->count()-1);

    ui->spc_cur_css_list->currentItem()->setData(0, "New CSS");
    ui->spc_cur_css_list->currentItem()->setData(1, tmp_data);
    on_spc_cur_css_list_itemClicked(ui->spc_cur_css_list->currentItem());
}


void SPC_submenu::on_spc_cur_css_duplicate_clicked()
{
    if (css_s == 0) return;
    css_s += 1;

    QStringList old_data = ui->spc_cur_css_list->currentItem()->data(1).toStringList();

    ui->spc_cur_css_list->addItem("New CSS");
    ui->spc_cur_css_list->setCurrentRow(ui->spc_cur_css_list->count()-1);
    ui->spc_cur_css_list->currentItem()->setData(1, old_data);
    on_spc_cur_css_list_itemClicked(ui->spc_cur_css_list->currentItem());
}


void SPC_submenu::on_spc_cur_css_list_itemClicked(QListWidgetItem *item)
{
    receive_data();

    QStringList current_data = item->data(1).toStringList();
    item->setText(item->data(0).toString());

    ui->spc_cur_css_name->setText(item->data(0).toString());

    ui->spc_cur_css_samptime->setText(current_data[1]);

    ui->spc_cur_css_axis_1->setText(current_data[2]);
    ui->spc_cur_css_axis_2->setText(current_data[3]);
    ui->spc_cur_css_axis_3->setText(current_data[4]);

    ui->spc_cur_css_halfcone->setText(current_data[5]);

    ui->spc_cur_css_scale->setText(current_data[6]);

    ui->spc_cur_css_quant->setText(current_data[7]);

    ui->spc_cur_css_body->setValue(current_data[8].toInt());

    ui->spc_cur_css_node->setValue(current_data[9].toInt());
}

// FSS Buttons

void SPC_submenu::on_spc_cur_fss_remove_clicked()
{
    delete ui->spc_cur_fss_list->currentItem();
    if (fss_s > 0) fss_s -= 1;
}


void SPC_submenu::on_spc_cur_fss_add_clicked()
{
    fss_s += 1;

    QStringList tmp_data = {};

    tmp_data.append("blankline");
    tmp_data.append("0.2");
    tmp_data.append("70.0");
    tmp_data.append("0.0");
    tmp_data.append("0.0");
    tmp_data.append("321");
    tmp_data.append("Z_AXIS");
    tmp_data.append("32.0");
    tmp_data.append("32.0");
    tmp_data.append("0.1");
    tmp_data.append("0.5");
    tmp_data.append("0");

    ui->spc_cur_fss_list->addItem("New FSS");
    ui->spc_cur_fss_list->setCurrentRow(ui->spc_cur_fss_list->count()-1);

    ui->spc_cur_fss_list->currentItem()->setData(0, "New FSS");
    ui->spc_cur_fss_list->currentItem()->setData(1, tmp_data);
    on_spc_cur_fss_list_itemClicked(ui->spc_cur_fss_list->currentItem());
}


void SPC_submenu::on_spc_cur_fss_duplicate_clicked()
{
    if (fss_s == 0) return;
    fss_s += 1;

    QStringList old_data = ui->spc_cur_fss_list->currentItem()->data(1).toStringList();

    ui->spc_cur_fss_list->addItem("New FSS");
    ui->spc_cur_fss_list->setCurrentRow(ui->spc_cur_fss_list->count()-1);
    ui->spc_cur_fss_list->currentItem()->setData(1, old_data);
    on_spc_cur_fss_list_itemClicked(ui->spc_cur_fss_list->currentItem());
}


void SPC_submenu::on_spc_cur_fss_list_itemClicked(QListWidgetItem *item)
{
    receive_data();

    QStringList current_data = item->data(1).toStringList();
    item->setText(item->data(0).toString());

    ui->spc_cur_fss_name->setText(item->data(0).toString());

    ui->spc_cur_fss_samptime->setText(current_data[1]);

    ui->spc_cur_fss_mount_1->setText(current_data[2]);
    ui->spc_cur_fss_mount_2->setText(current_data[3]);
    ui->spc_cur_fss_mount_3->setText(current_data[4]);
    setQComboBox(ui->spc_cur_fss_mountseq, current_data[5]);

    setQComboBox(ui->spc_cur_fss_boreaxis, current_data[6]);

    ui->spc_cur_fss_hfov->setText(current_data[7]);
    ui->spc_cur_fss_vfov->setText(current_data[8]);

    ui->spc_cur_fss_noiseang->setText(current_data[9]);

    ui->spc_cur_fss_quant->setText(current_data[10]);

    ui->spc_cur_css_node->setValue(current_data[11].toInt());
}

// Star Tracker Buttons

void SPC_submenu::on_spc_cur_strack_remove_clicked()
{
    delete ui->spc_cur_strack_list->currentItem();
    if (stracks > 0) stracks -= 1;
}


void SPC_submenu::on_spc_cur_strack_add_clicked()
{
    stracks += 1;

    QStringList tmp_data = {};

    tmp_data.append("blankline");
    tmp_data.append("0.25");
    tmp_data.append("-90.0");
    tmp_data.append("90.0");
    tmp_data.append("00.0");
    tmp_data.append("321");
    tmp_data.append("Z_AXIS");
    tmp_data.append("8.0");
    tmp_data.append("8.0");
    tmp_data.append("30.0");
    tmp_data.append("10.0");
    tmp_data.append("10.0");
    tmp_data.append("2.0");
    tmp_data.append("2.0");
    tmp_data.append("20.0");
    tmp_data.append("0");

    ui->spc_cur_strack_list->addItem("New ST");
    ui->spc_cur_strack_list->setCurrentRow(ui->spc_cur_strack_list->count()-1);

    ui->spc_cur_strack_list->currentItem()->setData(0, "New ST");
    ui->spc_cur_strack_list->currentItem()->setData(1, tmp_data);
    on_spc_cur_strack_list_itemClicked(ui->spc_cur_strack_list->currentItem());
}


void SPC_submenu::on_spc_cur_strack_duplicate_clicked()
{
    if (stracks == 0) return;
    stracks += 1;

    QStringList old_data = ui->spc_cur_fss_list->currentItem()->data(1).toStringList();

    ui->spc_cur_strack_list->addItem("New ST");
    ui->spc_cur_strack_list->setCurrentRow(ui->spc_cur_strack_list->count()-1);
    ui->spc_cur_strack_list->currentItem()->setData(1, old_data);
    on_spc_cur_strack_list_itemClicked(ui->spc_cur_strack_list->currentItem());
}


void SPC_submenu::on_spc_cur_strack_list_itemClicked(QListWidgetItem *item)
{
    receive_data();

    QStringList current_data = item->data(1).toStringList();
    item->setText(item->data(0).toString());

    ui->spc_cur_strack_name->setText(item->data(0).toString());

    ui->spc_cur_strack_samptime->setText(current_data[1]);

    ui->spc_cur_strack_mount_1->setText(current_data[2]);
    ui->spc_cur_strack_mount_2->setText(current_data[3]);
    ui->spc_cur_strack_mount_3->setText(current_data[4]);
    setQComboBox(ui->spc_cur_strack_mountseq, current_data[5]);

    setQComboBox(ui->spc_cur_strack_boreaxis, current_data[6]);

    ui->spc_cur_strack_hfov->setText(current_data[7]);
    ui->spc_cur_strack_vfov->setText(current_data[8]);

    ui->spc_cur_strack_sun->setText(current_data[9]);
    ui->spc_cur_strack_earth->setText(current_data[10]);
    ui->spc_cur_strack_moon->setText(current_data[11]);

    ui->spc_cur_strack_noiseang_1->setText(current_data[12]);
    ui->spc_cur_strack_noiseang_2->setText(current_data[13]);
    ui->spc_cur_strack_noiseang_3->setText(current_data[14]);

    ui->spc_cur_strack_node->setValue(current_data[15].toInt());
}

// GPS Buttons


void SPC_submenu::on_spc_cur_gps_remove_clicked()
{
    delete ui->spc_cur_gps_list->currentItem();
    if (gps_s > 0) gps_s -= 1;
}


void SPC_submenu::on_spc_cur_gps_add_clicked()
{
    gps_s += 1;

    QStringList tmp_data = {};

    tmp_data.append("blankline");
    tmp_data.append("0.25");
    tmp_data.append("4.0");
    tmp_data.append("0.02");
    tmp_data.append("20.0E-9");
    tmp_data.append("0");

    ui->spc_cur_gps_list->addItem("New GPS");
    ui->spc_cur_gps_list->setCurrentRow(ui->spc_cur_gps_list->count()-1);

    ui->spc_cur_gps_list->currentItem()->setData(0, "New GPS");
    ui->spc_cur_gps_list->currentItem()->setData(1, tmp_data);
    on_spc_cur_gps_list_itemClicked(ui->spc_cur_gps_list->currentItem());
}


void SPC_submenu::on_spc_cur_gps_duplicate_clicked()
{
    if (gps_s == 0) return;
    gps_s += 1;

    QStringList old_data = ui->spc_cur_gps_list->currentItem()->data(1).toStringList();

    ui->spc_cur_gps_list->addItem("FSS " + QString::number(gps_s - 1));
    ui->spc_cur_gps_list->setCurrentRow(ui->spc_cur_gps_list->count()-1);
    ui->spc_cur_gps_list->currentItem()->setData(1, old_data);
    on_spc_cur_gps_list_itemClicked(ui->spc_cur_gps_list->currentItem());
}


void SPC_submenu::on_spc_cur_gps_list_itemClicked(QListWidgetItem *item)
{
    receive_data();

    QStringList current_data = item->data(1).toStringList();
    item->setText(item->data(0).toString());

     ui->spc_cur_gps_name->setText(item->data(0).toString());

    ui->spc_cur_gps_samptime->setText(current_data[1]);

    ui->spc_cur_gps_posnoise->setText(current_data[2]);

    ui->spc_cur_gps_velnoise->setText(current_data[3]);

    ui->spc_cur_gps_timenoise->setText(current_data[4]);

    ui->spc_cur_gps_node->setValue(current_data[5].toInt());
}

// Accelerometer Buttons



void SPC_submenu::on_spc_cur_accel_remove_clicked()
{
    delete ui->spc_cur_accel_list->currentItem();
    if (accels > 0) accels -= 1;
}


void SPC_submenu::on_spc_cur_accel_add_clicked()
{
    accels += 1;

    QStringList tmp_data = {};

    tmp_data.append("blankline");
    tmp_data.append("0.1");
    tmp_data.append("1.0");
    tmp_data.append("0.0");
    tmp_data.append("0.0");
    tmp_data.append("1.0");
    tmp_data.append("0.0");
    tmp_data.append("0.05");
    tmp_data.append("0.0");
    tmp_data.append("0.0");
    tmp_data.append("1.0");
    tmp_data.append("0.0");
    tmp_data.append("0.5");
    tmp_data.append("0");

    ui->spc_cur_accel_list->addItem("New Acc");
    ui->spc_cur_accel_list->setCurrentRow(ui->spc_cur_accel_list->count()-1);

    ui->spc_cur_accel_list->currentItem()->setData(0, "New Acc");
    ui->spc_cur_accel_list->currentItem()->setData(1, tmp_data);
    on_spc_cur_accel_list_itemClicked(ui->spc_cur_accel_list->currentItem());
}


void SPC_submenu::on_spc_cur_accel_duplicate_clicked()
{
    if (accels == 0) return;
    accels += 1;

    QStringList old_data = ui->spc_cur_accel_list->currentItem()->data(1).toStringList();

    ui->spc_cur_accel_list->addItem("New Acc");
    ui->spc_cur_accel_list->setCurrentRow(ui->spc_cur_accel_list->count()-1);
    ui->spc_cur_accel_list->currentItem()->setData(1, old_data);
    on_spc_cur_accel_list_itemClicked(ui->spc_cur_accel_list->currentItem());
}


void SPC_submenu::on_spc_cur_accel_list_itemClicked(QListWidgetItem *item)
{
    receive_data();

    QStringList current_data = item->data(1).toStringList();
    item->setText(item->data(0).toString());

    ui->spc_cur_acc_name->setText(item->data(0).toString());

    ui->spc_cur_acc_samptime->setText(current_data[1]);

    ui->spc_cur_acc_axis_1->setText(current_data[2]);
    ui->spc_cur_acc_axis_2->setText(current_data[3]);
    ui->spc_cur_acc_axis_3->setText(current_data[4]);

    ui->spc_cur_acc_maxacc->setText(current_data[5]);

    ui->spc_cur_acc_scaleerror->setText(current_data[6]);

    ui->spc_cur_acc_quant->setText(current_data[7]);

    ui->spc_cur_acc_dvrandwalk->setText(current_data[8]);

    ui->spc_cur_acc_bias_stab->setText(current_data[9]);
    ui->spc_cur_acc_bias_tspan->setText(current_data[10]);

    ui->spc_cur_acc_dvnoise->setText(current_data[11]);

    ui->spc_cur_acc_initbias->setText(current_data[12]);

    ui->spc_cur_gps_node->setValue(current_data[13].toInt());
}

// Misc
void SPC_submenu::on_sections_tabBarClicked(int index)
{
    if (index == 3) on_spc_cur_body_list_itemClicked(ui->spc_cur_body_list->currentItem());
    else if (index == 4 && ui->spc_cur_joint_list->count() > 0) on_spc_cur_joint_list_itemClicked(ui->spc_cur_joint_list->currentItem());
}

// File Selection Buttons

void SPC_submenu::on_spc_cur_shaker_select_clicked()
{
    QString file_name = QFileDialog::getOpenFileName(this, tr("Choose Folder"), inout_path, QString(), nullptr,  QFileDialog::DontUseNativeDialog);

    QDir dir(inout_path);
    QString rel_file_path = dir.relativeFilePath(file_name);

    if (file_name.isEmpty())
        return;

    ui->spc_cur_shaker_file->setText(rel_file_path);
}


void SPC_submenu::on_spc_cur_node_select_clicked()
{
    QString file_name = QFileDialog::getOpenFileName(this, tr("Choose Folder"), inout_path, QString(), nullptr,  QFileDialog::DontUseNativeDialog);

    QDir dir(inout_path);
    QString rel_file_path = dir.relativeFilePath(file_name);

    if (file_name.isEmpty())
        return;

    ui->spc_cur_node_file->setText(rel_file_path);
}

void SPC_submenu::on_spc_cur_flex_select_clicked()
{
    QString file_name = QFileDialog::getOpenFileName(this, tr("Choose Folder"), inout_path, QString(), nullptr,  QFileDialog::DontUseNativeDialog);

    QDir dir(inout_path);
    QString rel_file_path = dir.relativeFilePath(file_name);

    if (file_name.isEmpty())
        return;

    ui->spc_cur_flex_file->setText(rel_file_path);
}


void SPC_submenu::on_spc_cur_joint_param_select_clicked()
{
    QString file_name = QFileDialog::getOpenFileName(this, tr("Choose Folder"), inout_path, QString(), nullptr,  QFileDialog::DontUseNativeDialog);

    QDir dir(inout_path);
    QString rel_file_path = dir.relativeFilePath(file_name);

    if (file_name.isEmpty())
        return;

    ui->spc_cur_joint_param_file->setText(rel_file_path);
}


void SPC_submenu::on_spc_cur_wheel_drjit_select_clicked()
{
    QString file_name = QFileDialog::getOpenFileName(this, tr("Choose Folder"), inout_path, QString(), nullptr,  QFileDialog::DontUseNativeDialog);

    QDir dir(inout_path);
    QString rel_file_path = dir.relativeFilePath(file_name);

    if (file_name.isEmpty())
        return;

    ui->spc_cur_wheel_drjit_file->setText(rel_file_path);
}

// Q and A switcher in Attitude
void SPC_submenu::on_spc_cur_att_param_currentTextChanged(const QString &arg1)
{
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
}


void SPC_submenu::on_spc_cur_shaker_clear_clicked()
{
    ui->spc_cur_shaker_file->setText("");
}


void SPC_submenu::on_spc_cur_node_clear_clicked()
{
    ui->spc_cur_node_file->setText("");
}


void SPC_submenu::on_spc_cur_flex_clear_clicked()
{
    ui->spc_cur_flex_file->setText("");
}


void SPC_submenu::on_spc_cur_joint_param_clear_clicked()
{
    ui->spc_cur_joint_param_file->setText("");
}


void SPC_submenu::on_spc_cur_wheel_drjit_clear_clicked()
{
    ui->spc_cur_wheel_drjit_file->setText("");
}

