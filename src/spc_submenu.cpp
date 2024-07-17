#include "spc_submenu.h"
#include "dsm_gui_lib.h"
#include "spc_menu.h">
#include "ui_spc_submenu.h"
#include <QDebug>

SPC_submenu::SPC_submenu(QWidget *parent)
    : QDialog(parent), ui(new Ui::SPC_submenu) {
   ui->setupUi(this);
   set_validators();

   ui->sections->setCurrentIndex(0);
   ui->actuator_sections->setCurrentIndex(0);
   ui->sensor_sections->setCurrentIndex(0);
}

SPC_submenu::~SPC_submenu() {
   delete ui;
}

void SPC_submenu::receive_spc_sm_path(QString name, QString path) {
   spc_cur_name = name; // store name
   spc_cur_file = path + "SC_" + spc_cur_name + ".yaml";
   file_path    = spc_cur_file;

   inout_path = path;

   ui->spc_cur_name_sub->setText(spc_cur_name);

   receive_data();
   on_sections_tabBarClicked(ui->sections->currentIndex());
}

void SPC_submenu::receive_apppath(QString path, QString junk) {
   appPath = path;
}

void SPC_submenu::receive_pythoncmd(QString cmd, QString junk) {
   pythonCmd = cmd;
}

void SPC_submenu::set_validators() {
   QRegularExpression rx("[^\"]*");
   QRegularExpression rx1("[^\" ]*");

   QValidator *noQuotes       = new QRegularExpressionValidator(rx, this);
   QValidator *noQuotesSpaces = new QRegularExpressionValidator(rx1, this);

   // Combo Boxes
   QComboBox *combo_boxes_euler[6] = {
       ui->spc_cur_joint_rotdof_seq,
       ui->spc_cur_joint_bigi_seq,
       ui->spc_cur_joint_bogo_seq,
       ui->spc_cur_joint_trndof_seq,
       ui->spc_cur_fss_mountseq,
       ui->spc_cur_strack_mountseq}; // All Euler combo boxes

   dsm_gui_lib::set_mult_cbox_validators(combo_boxes_euler, 6,
                                         dsm_gui_lib::eulerInputs);

   // All custom comboboxes
   ui->spc_cur_solver->addItems(dsm_gui_lib::sortStringList(solve_dynamics));
   ui->spc_cur_masspropref->addItems(
       dsm_gui_lib::sortStringList(mass_prop_ref));
   ui->spc_cur_joint_type->addItems(dsm_gui_lib::sortStringList(joint_type));
   ui->spc_cur_joint_rottype->addItems(
       dsm_gui_lib::sortStringList(rot_joint_seqtype));
   ui->spc_cur_thruster_mode->addItems(
       dsm_gui_lib::sortStringList(thruster_mode));
   ui->spc_cur_fss_boreaxis->addItems(dsm_gui_lib::sortStringList(axis));
   ui->spc_cur_strack_boreaxis->addItems(dsm_gui_lib::sortStringList(axis));

   // Numerical Validators

   QLineEdit *ninf_pinf[47] = {
       ui->spc_cur_drag,
       ui->spc_cur_body_pmoi_x,
       ui->spc_cur_body_pmoi_y,
       ui->spc_cur_body_pmoi_z,
       ui->spc_cur_body_poi_x,
       ui->spc_cur_body_poi_y,
       ui->spc_cur_body_poi_z,
       ui->spc_cur_body_com_x,
       ui->spc_cur_body_com_y,
       ui->spc_cur_body_com_z,
       ui->spc_cur_body_cem_x,
       ui->spc_cur_body_cem_y,
       ui->spc_cur_body_cem_z,
       ui->spc_cur_body_cemd_x,
       ui->spc_cur_body_cemd_y,
       ui->spc_cur_body_cemd_z,
       ui->spc_cur_joint_ang0_1,
       ui->spc_cur_joint_ang0_2,
       ui->spc_cur_joint_ang0_3,
       ui->spc_cur_joint_angrate0_1,
       ui->spc_cur_joint_angrate0_2,
       ui->spc_cur_joint_angrate0_3,
       ui->spc_cur_joint_disp0_1,
       ui->spc_cur_joint_disp0_2,
       ui->spc_cur_joint_disp0_3,
       ui->spc_cur_joint_dispr0_1,
       ui->spc_cur_joint_dispr0_2,
       ui->spc_cur_joint_dispr0_3,
       ui->spc_cur_joint_bigi_1,
       ui->spc_cur_joint_bigi_2,
       ui->spc_cur_joint_bigi_3,
       ui->spc_cur_joint_bogo_1,
       ui->spc_cur_joint_bogo_2,
       ui->spc_cur_joint_bogo_3,
       ui->spc_cur_joint_poswrt_in_1,
       ui->spc_cur_joint_poswrt_in_2,
       ui->spc_cur_joint_poswrt_in_3,
       ui->spc_cur_wheel_maxtrq,
       ui->spc_cur_wheel_maxmom,
       ui->spc_cur_gyro_initbias,
       ui->spc_cur_fss_mount_1,
       ui->spc_cur_fss_mount_2,
       ui->spc_cur_fss_mount_3,
       ui->spc_cur_strack_mount_1,
       ui->spc_cur_strack_mount_2,
       ui->spc_cur_strack_mount_3,
       ui->spc_cur_acc_initbias}; // All validators with (-INFINITY, INFINITY)

   QLineEdit *zero_pinf[50] = {
       ui->spc_cur_body_mass,
       ui->spc_cur_body_pmoi_x,
       ui->spc_cur_body_pmoi_y,
       ui->spc_cur_body_pmoi_z,
       ui->spc_cur_wheel_inertia,
       ui->spc_cur_mtb_sat,
       ui->spc_cur_thruster_force,
       ui->spc_cur_gyro_samptime,
       ui->spc_cur_gyro_maxrate,
       ui->spc_cur_gyro_scaleferror,
       ui->spc_cur_gyro_quant,
       ui->spc_cur_gyro_angrwalk,
       ui->spc_cur_gyro_angnoise,
       ui->spc_cur_gyro_bias_stab,
       ui->spc_cur_gyro_bias_tspan,
       ui->spc_cur_mag_samptime,
       ui->spc_cur_mag_sat,
       ui->spc_cur_mag_scaleferror,
       ui->spc_cur_mag_quant,
       ui->spc_cur_mag_noise,
       ui->spc_cur_css_samptime,
       ui->spc_cur_css_halfcone,
       ui->spc_cur_css_scale,
       ui->spc_cur_css_quant,
       ui->spc_cur_fss_samptime,
       ui->spc_cur_fss_hfov,
       ui->spc_cur_fss_vfov,
       ui->spc_cur_fss_noiseang,
       ui->spc_cur_fss_quant,
       ui->spc_cur_strack_samptime,
       ui->spc_cur_strack_hfov,
       ui->spc_cur_strack_vfov,
       ui->spc_cur_strack_sun,
       ui->spc_cur_strack_earth,
       ui->spc_cur_strack_moon,
       ui->spc_cur_strack_noiseang_1,
       ui->spc_cur_strack_noiseang_2,
       ui->spc_cur_strack_noiseang_3,
       ui->spc_cur_gps_samptime,
       ui->spc_cur_gps_posnoise,
       ui->spc_cur_gps_velnoise,
       ui->spc_cur_gps_timenoise,
       ui->spc_cur_acc_samptime,
       ui->spc_cur_acc_maxacc,
       ui->spc_cur_acc_scaleerror,
       ui->spc_cur_acc_quant,
       ui->spc_cur_acc_dvrandwalk,
       ui->spc_cur_acc_bias_stab,
       ui->spc_cur_acc_bias_tspan,
       ui->spc_cur_acc_dvnoise}; // All validators with [0, INFINITY)

   QLineEdit *none_one[21] = {
       ui->spc_cur_wheel_axis_1,    ui->spc_cur_wheel_axis_2,
       ui->spc_cur_wheel_axis_3,    ui->spc_cur_mtb_axis_1,
       ui->spc_cur_mtb_axis_2,      ui->spc_cur_mtb_axis_3,
       ui->spc_cur_thruster_axis_1, ui->spc_cur_thruster_axis_2,
       ui->spc_cur_thruster_axis_3, ui->spc_cur_gyro_axis_1,
       ui->spc_cur_gyro_axis_2,     ui->spc_cur_gyro_axis_3,
       ui->spc_cur_mag_axis_1,      ui->spc_cur_mag_axis_2,
       ui->spc_cur_mag_axis_3,      ui->spc_cur_css_axis_1,
       ui->spc_cur_css_axis_2,      ui->spc_cur_css_axis_3,
       ui->spc_cur_acc_axis_1,      ui->spc_cur_acc_axis_2,
       ui->spc_cur_acc_axis_3}; // All validators with [-1, 1]

   dsm_gui_lib::set_mult_validators(ninf_pinf, 47, -INFINITY, INFINITY, 5);
   dsm_gui_lib::set_mult_validators(zero_pinf, 50, 0, INFINITY, 5);
   dsm_gui_lib::set_mult_validators(none_one, 21, -1, 1, 5);

   // Name Validators

   QLineEdit *item_names[12] = {
       ui->spc_cur_body_name,     ui->spc_cur_joint_name,
       ui->spc_cur_wheel_name,    ui->spc_cur_mtb_name,
       ui->spc_cur_thruster_name, ui->spc_cur_gyro_name,
       ui->spc_cur_mag_name,      ui->spc_cur_css_name,
       ui->spc_cur_fss_name,      ui->spc_cur_strack_name,
       ui->spc_cur_gps_name,      ui->spc_cur_acc_name};

   dsm_gui_lib::set_mult_name_validators(item_names, 12, noQuotes);

   ui->spc_cur_body_geom->setValidator(noQuotesSpaces);
}

void SPC_submenu::receive_data() {
   QFile file(file_path);
   if (!file.open(QIODevice::ReadOnly)) {
      QMessageBox::information(0, "error", file.errorString());
   }

   spc_data.clear();
   spc_string.clear();

   /* Load Yaml File */
   cur_spc_yaml = YAML::LoadFile(file_path.toStdString());

   QStringList tmp_data;
   QVector<QString> data_vector;
   long index = 0;

   /* Dynamics Flags */

   setQComboBox(ui->spc_cur_solver,
                cur_spc_yaml["Dynamics Flags"]["Method"].as<QString>());
   if (!QString::compare(
           cur_spc_yaml["Dynamics Flags"]["Compute Constraints"].as<QString>(),
           "true"))
      ui->spc_cur_con_on->setChecked(true);
   else
      ui->spc_cur_con_off->setChecked(true);

   if (!QString::compare(
           cur_spc_yaml["Dynamics Flags"]["Mass Reference Point"].as<QString>(),
           "true"))
      ui->spc_cur_flex_on->setChecked(true);
   else
      ui->spc_cur_flex_off->setChecked(true);

   if (!QString::compare(
           cur_spc_yaml["Dynamics Flags"]["2nd Order Flex"].as<QString>(),
           "true"))
      ui->spc_cur_2flex_on->setChecked(true);
   else
      ui->spc_cur_2flex_off->setChecked(true);
   ui->spc_cur_shaker_file->setText(
       cur_spc_yaml["Dynamics Flags"]["Shaker File Name"].as<QString>());
   ui->spc_cur_drag->setText(
       cur_spc_yaml["Dynamics Flags"]["Drag Coefficient"].as<QString>());

   /* Bodies */
   ui->spc_cur_body_list->clear();
   YAML::Node body_node = cur_spc_yaml["Bodies"];
   index                = 0;

   bodies = body_node.size();
   for (YAML::const_iterator it = body_node.begin(); it != body_node.end();
        ++it) {
      QMap<QString, QString> cur_node = it->as<QMap<QString, QString>>();

      QString item_name = cur_node["Name"];
      ui->spc_cur_body_list->addItem(item_name);

      // tmp_data.append(cur_node["Mass"]);
      // data_vector = cur_node["MOI"];
      // for (int i = 0; i < 3; i++)
      //    tmp_data.append(data_vector[i]);

      // data_vector = cur_node["POI"];
      // for (int i = 0; i < 3; i++)
      //    tmp_data.append(data_vector[i]);

      // data_vector = cur_node["Pos of CM"];
      // for (int i = 0; i < 3; i++)
      //    tmp_data.append(data_vector[i]);

      // data_vector = cur_node["Constant Momentum"];
      // for (int i = 0; i < 3; i++)
      //    tmp_data.append(data_vector[i]);

      // data_vector = cur_node["Constant Dipole"];
      // for (int i = 0; i < 3; i++)
      //    tmp_data.append(data_vector[i]);

      tmp_data.append(cur_node["Geometry File Name"]);
      tmp_data.append(cur_node["Node File Name"]);
      tmp_data.append(cur_node["Flex File Name"]);

      tmp_data = dsm_gui_lib::apply_data_section_end(
          index, ui->spc_cur_body_list, tmp_data, item_name);
      index++;
   }

   /* Joints */
   ui->spc_cur_joint_list->clear();
   YAML::Node joint_node = cur_spc_yaml["Joints"];
   index                 = 0;

   joints = joint_node.size();
   for (YAML::const_iterator it = joint_node.begin(); it != joint_node.end();
        ++it) {
      YAML::Node cur_node = *it;
      QString item_name   = cur_node["Name"].as<QString>();
      ui->spc_cur_joint_list->addItem(item_name);

      tmp_data.append(cur_node["Joint Type"].as<QString>());
      data_vector = cur_node["Joint Connections"].as<QVector<QString>>();
      for (int i = 0; i < 2; i++)
         tmp_data.append(data_vector[i]);

      tmp_data.append(cur_node["Rot DOF"].as<QString>());
      tmp_data.append(cur_node["Rot Sequence"].as<QString>());
      tmp_data.append(cur_node["Rot Type"].as<QString>());
      tmp_data.append(cur_node["Trn DOF"].as<QString>());
      tmp_data.append(cur_node["Trn Sequence"].as<QString>());

      data_vector = cur_node["Rot DOF Locked"].as<QVector<QString>>();
      for (int i = 0; i < 3; i++)
         tmp_data.append(data_vector[i]);

      data_vector = cur_node["Trn DOF Locked"].as<QVector<QString>>();
      for (int i = 0; i < 3; i++)
         tmp_data.append(data_vector[i]);

      data_vector = cur_node["Init Angles"].as<QVector<QString>>();
      for (int i = 0; i < 3; i++)
         tmp_data.append(data_vector[i]);

      data_vector = cur_node["Init Angle Rates"].as<QVector<QString>>();
      for (int i = 0; i < 3; i++)
         tmp_data.append(data_vector[i]);

      data_vector = cur_node["Init Displacement"].as<QVector<QString>>();
      for (int i = 0; i < 3; i++)
         tmp_data.append(data_vector[i]);

      data_vector = cur_node["Init Displacement Rates"].as<QVector<QString>>();
      for (int i = 0; i < 3; i++)
         tmp_data.append(data_vector[i]);

      data_vector = cur_node["Bi-Gi Angles"]["Angles"].as<QVector<QString>>();
      for (int i = 0; i < 3; i++)
         tmp_data.append(data_vector[i]);

      tmp_data.append(cur_node["Bi-Gi Angles"]["Sequence"].as<QString>());

      data_vector = cur_node["Bo-Go Angles"]["Angles"].as<QVector<QString>>();
      for (int i = 0; i < 3; i++)
         tmp_data.append(data_vector[i]);

      tmp_data.append(cur_node["Bo-Go Angles"]["Sequence"].as<QString>());

      data_vector = cur_node["Pos wrt Inner Body"].as<QVector<QString>>();
      for (int i = 0; i < 3; i++)
         tmp_data.append(data_vector[i]);
      data_vector = cur_node["Pos wrt Outer Body"].as<QVector<QString>>();
      for (int i = 0; i < 3; i++)
         tmp_data.append(data_vector[i]);
      tmp_data.append(cur_node["Parm File Name"].as<QString>());

      tmp_data = dsm_gui_lib::apply_data_section_end(
          index, ui->spc_cur_body_list, tmp_data, item_name);
      index++;
   }

   /* Wheels */
   ui->spc_cur_wheel_list->clear();

   QMap<QString, QString> wheel_params =
       cur_spc_yaml["Wheel Params"].as<QMap<QString, QString>>();

   wheel_drag   = wheel_params["Drag"];
   wheel_jitter = wheel_params["Jitter"];

   if (!QString::compare(wheel_drag, "false"))
      ui->spc_cur_wheel_glob_drag_off->setChecked(Qt::Checked);
   else
      ui->spc_cur_wheel_glob_drag_on->setChecked(Qt::Checked);

   if (!QString::compare(wheel_jitter, "false"))
      ui->spc_cur_wheel_glob_jitter_off->setChecked(Qt::Checked);
   else
      ui->spc_cur_wheel_glob_jitter_on->setChecked(Qt::Checked);

   YAML::Node wheel_node = cur_spc_yaml["Wheels"];
   index                 = 0;

   wheels = wheel_node.size();

   for (YAML::const_iterator it = wheel_node.begin(); it != wheel_node.end();
        ++it) {
      YAML::Node cur_node = *it;
      QString item_name   = cur_node["Name"].as<QString>();
      ui->spc_cur_wheel_list->addItem(item_name);

      tmp_data.append(cur_node["Initial Momentum"].as<QString>());
      data_vector = cur_node["Axis"].as<QVector<QString>>();
      for (int i = 0; i < 2; i++)
         tmp_data.append(data_vector[i]);

      tmp_data.append(cur_node["Max Torque"].as<QString>());
      tmp_data.append(cur_node["Max Momentum"].as<QString>());
      tmp_data.append(cur_node["Rotor Inertia"].as<QString>());
      tmp_data.append(cur_node["Body"]["Index"].as<QString>());
      tmp_data.append(cur_node["Node"].as<QString>());

      tmp_data.append(cur_node["Drag-Jitter File Name"].as<QString>());

      tmp_data = dsm_gui_lib::apply_data_section_end(
          index, ui->spc_cur_joint_list, tmp_data, item_name);
      index++;
   }

   /* MTBs */
   ui->spc_cur_mtb_list->clear();

   YAML::Node mtb_node = cur_spc_yaml["MTBs"];
   index               = 0;

   mtbs = mtb_node.size();

   for (YAML::const_iterator it = mtb_node.begin(); it != mtb_node.end();
        ++it) {
      YAML::Node cur_node = *it;
      QString item_name   = cur_node["Name"].as<QString>();
      ui->spc_cur_mtb_list->addItem(item_name);

      tmp_data.append(cur_node["Saturation"].as<QString>());
      data_vector = cur_node["Axis"].as<QVector<QString>>();
      for (int i = 0; i < 3; i++)
         tmp_data.append(data_vector[i]);

      tmp_data.append(cur_node["Node"].as<QString>());

      tmp_data = dsm_gui_lib::apply_data_section_end(
          index, ui->spc_cur_mtb_list, tmp_data, item_name);
      index++;
   }

   /* Thrusters */
   ui->spc_cur_thruster_list->clear();

   YAML::Node thruster_node = cur_spc_yaml["Thrusters"];
   index                    = 0;

   thrusters = thruster_node.size();

   for (YAML::const_iterator it = thruster_node.begin();
        it != thruster_node.end(); ++it) {
      YAML::Node cur_node = *it;
      QString item_name   = cur_node["Name"].as<QString>();
      ui->spc_cur_thruster_list->addItem(item_name);

      tmp_data.append(cur_node["Mode"].as<QString>());
      tmp_data.append(cur_node["Force"].as<QString>());
      data_vector = cur_node["Axis"].as<QVector<QString>>();
      for (int i = 0; i < 3; i++)
         tmp_data.append(data_vector[i]);

      tmp_data.append(cur_node["Body"]["Index"].as<QString>());
      tmp_data.append(cur_node["Node"].as<QString>());

      tmp_data = dsm_gui_lib::apply_data_section_end(
          index, ui->spc_cur_thruster_list, tmp_data, item_name);
      index++;
   }

   /* Gyros */
   ui->spc_cur_gyro_list->clear();

   YAML::Node gyro_node = cur_spc_yaml["Gyros"];
   index                = 0;

   gyros = gyro_node.size();

   for (YAML::const_iterator it = gyro_node.begin(); it != gyro_node.end();
        ++it) {
      YAML::Node cur_node = *it;
      QString item_name   = cur_node["Name"].as<QString>();
      ui->spc_cur_gyro_list->addItem(item_name);

      tmp_data.append(cur_node["Sample Time"].as<QString>());
      data_vector = cur_node["Axis"].as<QVector<QString>>();
      for (int i = 0; i < 3; i++)
         tmp_data.append(data_vector[i]);

      tmp_data.append(cur_node["Max Rate"].as<QString>());
      tmp_data.append(cur_node["Scale Factor"].as<QString>());
      tmp_data.append(cur_node["Quantization"].as<QString>());
      tmp_data.append(cur_node["Angle Random Walk"].as<QString>());
      tmp_data.append(cur_node["Bias Stability"].as<QString>());
      tmp_data.append(cur_node["Bias Stability Timespan"].as<QString>());
      tmp_data.append(cur_node["Angle Noise"].as<QString>());
      tmp_data.append(cur_node["Initial Bias"].as<QString>());
      tmp_data.append(cur_node["Node"].as<QString>());

      tmp_data = dsm_gui_lib::apply_data_section_end(
          index, ui->spc_cur_gyro_list, tmp_data, item_name);
      index++;
   }

   /* Magnetometers */
   ui->spc_cur_mag_list->clear();

   YAML::Node mag_node = cur_spc_yaml["Magnetometers"];
   index               = 0;

   mags = mag_node.size();

   for (YAML::const_iterator it = mag_node.begin(); it != mag_node.end();
        ++it) {
      YAML::Node cur_node = *it;
      QString item_name   = cur_node["Name"].as<QString>();
      ui->spc_cur_mag_list->addItem(item_name);

      tmp_data.append(cur_node["Sample Time"].as<QString>());
      data_vector = cur_node["Axis"].as<QVector<QString>>();
      for (int i = 0; i < 3; i++)
         tmp_data.append(data_vector[i]);

      tmp_data.append(cur_node["Max Rate"].as<QString>());
      tmp_data.append(cur_node["Scale Factor"].as<QString>());
      tmp_data.append(cur_node["Quantization"].as<QString>());
      tmp_data.append(cur_node["Noise"].as<QString>());
      tmp_data.append(cur_node["Node"].as<QString>());

      tmp_data = dsm_gui_lib::apply_data_section_end(
          index, ui->spc_cur_mag_list, tmp_data, item_name);
      index++;
   }

   /* CSSs */
   ui->spc_cur_css_list->clear();

   YAML::Node css_node = cur_spc_yaml["CSSs"];
   index               = 0;

   css_s = css_node.size();

   for (YAML::const_iterator it = css_node.begin(); it != css_node.end();
        ++it) {
      YAML::Node cur_node = *it;
      QString item_name   = cur_node["Name"].as<QString>();
      ui->spc_cur_css_list->addItem(item_name);

      tmp_data.append(cur_node["Sample Time"].as<QString>());
      data_vector = cur_node["Axis"].as<QVector<QString>>();
      for (int i = 0; i < 3; i++)
         tmp_data.append(data_vector[i]);

      tmp_data.append(cur_node["Half Cone Angle"].as<QString>());
      tmp_data.append(cur_node["Scale Factor"].as<QString>());
      tmp_data.append(cur_node["Quantization"].as<QString>());
      tmp_data.append(cur_node["Body"]["Index"].as<QString>());
      tmp_data.append(cur_node["Node"].as<QString>());

      tmp_data = dsm_gui_lib::apply_data_section_end(
          index, ui->spc_cur_css_list, tmp_data, item_name);
      index++;
   }

   /* FSSs */
   ui->spc_cur_fss_list->clear();

   YAML::Node fss_node = cur_spc_yaml["FSSs"];
   index               = 0;

   fss_s = fss_node.size();

   for (YAML::const_iterator it = fss_node.begin(); it != fss_node.end();
        ++it) {
      YAML::Node cur_node = *it;
      QString item_name   = cur_node["Name"].as<QString>();
      ui->spc_cur_fss_list->addItem(item_name);

      tmp_data.append(cur_node["Sample Time"].as<QString>());

      data_vector = cur_node["Mounting Angle"]["Angles"].as<QVector<QString>>();
      for (int i = 0; i < 3; i++)
         tmp_data.append(data_vector[i]);

      tmp_data.append(cur_node["Mounting Angles"]["Sequence"].as<QString>());
      tmp_data.append(cur_node["Boresight Axis"].as<QString>());
      tmp_data.append(cur_node["FOV Size"].as<QString>());
      tmp_data.append(cur_node["Noise Equivalent Angle"].as<QString>());
      tmp_data.append(cur_node["Quantization"].as<QString>());
      tmp_data.append(cur_node["Node"].as<QString>());

      tmp_data = dsm_gui_lib::apply_data_section_end(
          index, ui->spc_cur_fss_list, tmp_data, item_name);
      index++;
   }

   /* STs */
   ui->spc_cur_strack_list->clear();

   YAML::Node strack_node = cur_spc_yaml["STs"];
   index                  = 0;

   stracks = strack_node.size();

   for (YAML::const_iterator it = strack_node.begin(); it != strack_node.end();
        ++it) {
      YAML::Node cur_node = *it;
      QString item_name   = cur_node["Name"].as<QString>();
      ui->spc_cur_strack_list->addItem(item_name);

      tmp_data.append(cur_node["Sample Time"].as<QString>());

      data_vector = cur_node["Mounting Angle"]["Angles"].as<QVector<QString>>();
      for (int i = 0; i < 3; i++)
         tmp_data.append(data_vector[i]);

      tmp_data.append(cur_node["Mounting Angles"]["Sequence"].as<QString>());
      tmp_data.append(cur_node["Boresight Axis"].as<QString>());
      tmp_data.append(cur_node["FOV Size"].as<QString>());
      tmp_data.append(cur_node["Exclusion Angles"]["Sun"].as<QString>());
      tmp_data.append(cur_node["Exclusion Angles"]["Earth"].as<QString>());
      tmp_data.append(cur_node["Exclusion Angles"]["Luna"].as<QString>());
      tmp_data.append(cur_node["Noise Equivalent Angle"].as<QString>());
      tmp_data.append(cur_node["Node"].as<QString>());

      tmp_data = dsm_gui_lib::apply_data_section_end(
          index, ui->spc_cur_strack_list, tmp_data, item_name);
      index++;
   }

   /* GPSs */
   ui->spc_cur_strack_list->clear();

   YAML::Node gps_node = cur_spc_yaml["GPSs"];
   index               = 0;

   gps_s = gps_node.size();

   for (YAML::const_iterator it = gps_node.begin(); it != gps_node.end();
        ++it) {
      YAML::Node cur_node = *it;
      QString item_name   = cur_node["Name"].as<QString>();
      ui->spc_cur_gps_list->addItem(item_name);

      tmp_data.append(cur_node["Sample Time"].as<QString>());
      tmp_data.append(cur_node["Position Noise"].as<QString>());
      tmp_data.append(cur_node["Velocity Noise"].as<QString>());
      tmp_data.append(cur_node["Time Noise"]["Sun"].as<QString>());
      tmp_data.append(cur_node["Node"].as<QString>());

      tmp_data = dsm_gui_lib::apply_data_section_end(
          index, ui->spc_cur_strack_list, tmp_data, item_name);
      index++;
   }

   /* Accelerometers */
   ui->spc_cur_accel_list->clear();

   YAML::Node acc_node = cur_spc_yaml["Accelerometers"];
   index               = 0;

   accels = acc_node.size();

   for (YAML::const_iterator it = acc_node.begin(); it != acc_node.end();
        ++it) {
      YAML::Node cur_node = *it;
      QString item_name   = cur_node["Name"].as<QString>();
      ui->spc_cur_accel_list->addItem(item_name);

      tmp_data.append(cur_node["Sample Time"].as<QString>());
      data_vector = cur_node["Axis"].as<QVector<QString>>();
      for (int i = 0; i < 3; i++)
         tmp_data.append(data_vector[i]);
      tmp_data.append(cur_node["Max Acceleration"].as<QString>());
      tmp_data.append(cur_node["Scale Factor"].as<QString>());
      tmp_data.append(cur_node["Quantization"].as<QString>());
      tmp_data.append(cur_node["DV Random Walk"].as<QString>());
      tmp_data.append(cur_node["Bias Stability"].as<QString>());
      tmp_data.append(cur_node["Bias Stability Timespan"].as<QString>());
      tmp_data.append(cur_node["DV Noise"].as<QString>());
      tmp_data.append(cur_node["Initial Bias"].as<QString>());
      tmp_data.append(cur_node["Node"].as<QString>());

      tmp_data = dsm_gui_lib::apply_data_section_end(
          index, ui->spc_cur_strack_list, tmp_data, item_name);
      index++;
   }
}

void SPC_submenu::apply_data() {}

void SPC_submenu::write_data(YAML::Node inp_spc) {
   QStringList params;
   QProcess p;
   QFile::remove(file_path);
   QFile file(file_path);
   if (!file.open(QFile::WriteOnly)) {
      QMessageBox::information(0, "error", file.errorString());
   } else {
      QTextStream in(&file);
      YAML::Emitter out;
      out.SetIndent(4);
      out.SetMapFormat(YAML::EMITTER_MANIP::Block);
      out << inp_spc;
      in << out.c_str();
   }
   file.close();
   params << appPath + "/__python__/AddYAMLComments.py" << appPath << inout_path
          << "SC_" << ui->spc_cur_name_sub->text() << ".yaml";
   p.start(pythonCmd, params);
   p.waitForFinished(-1);
}

void SPC_submenu::on_spc_cur_load_clicked() {
   file_path    = spc_cur_file;
   int response = dsm_gui_lib::warning_message("Overwrite SC file?");
   if (response == QMessageBox::Ok) {
      QString spc_cur_name =
          spc_cur_file.replace(inout_path, "").chopped(4).mid(3);

      connect(this, SIGNAL(refresh_spc_menu(QString)), this->parent(),
              SLOT(load_1SC_default(QString)));
      emit refresh_spc_menu(spc_cur_name);
      disconnect(this, SIGNAL(refresh_spc_menu(QString)), 0, 0);
   }
}

//// S/C Buttons

void SPC_submenu::on_spc_cur_save_clicked() {
   on_spc_cur_apply_clicked();
   file_path    = spc_cur_file;
   int response = dsm_gui_lib::warning_message("Overwrite Default SC file?");
   if (response == QMessageBox::Ok) {
      QFile::remove(inout_path + "__default__/__SCDEFAULT__.yaml");
      QFile::copy(file_path, inout_path + "__default__/__SCDEFAULT__.yaml");
      receive_data();
      apply_data();
   }
}

void SPC_submenu::on_spc_cur_close_clicked() {
   if (joints_valid == 1) {
      SPC_submenu::close();
   } else {
      int response = dsm_gui_lib::warning_message(
          "The number of joints must equal number of bodies minus one. Click "
          "\"OK\" to disregard this warning and close.");
      if (response == QMessageBox::Cancel) {
         return;
      } else if (response == QMessageBox::Ok) {
         SPC_submenu::close();
      }
   }
}

void SPC_submenu::on_spc_cur_apply_clicked() {
   QFile file(file_path);
   if (!file.open(QIODevice::ReadOnly)) {
      QMessageBox::information(0, "error", file.errorString());
   }

   file.close();

   QVector<QString> data_vector = {};
   QStringList tmp_data;

   /* Dynamics Flags */
   cur_spc_yaml["Dynamics Flags"]["Method"] = ui->spc_cur_solver->currentText();
   if (ui->spc_cur_con_on->isChecked())
      cur_spc_yaml["Dynamics Flags"]["Compute Constraints"] = "true";
   else
      cur_spc_yaml["Dynamics Flags"]["Compute Constraints"] = "false";

   cur_spc_yaml["Dynamics Flags"]["Mass Reference Point"] =
       ui->spc_cur_masspropref->currentText();

   if (ui->spc_cur_flex_on->isChecked())
      cur_spc_yaml["Dynamics Flags"]["Flex Active"] = "true";
   else
      cur_spc_yaml["Dynamics Flags"]["Flex Active"] = "false";

   if (ui->spc_cur_2flex_on->isChecked())
      cur_spc_yaml["Dynamics Flags"]["2nd Order Flex"] = "true";
   else
      cur_spc_yaml["Dynamics Flags"]["2nd Order Flex"] = "false";

   cur_spc_yaml["Dynamics Flags"]["Shaker File Name"] =
       ui->spc_cur_shaker_file->text();
   cur_spc_yaml["Dynamics Flags"]["Drag Coefficient"] =
       ui->spc_cur_drag->text();

   /* Bodies */
   for (int i = 0; i < bodies; i++) {
      ui->spc_cur_body_list->setCurrentRow(i);
      YAML::Node cur_node = cur_spc_yaml["Bodies"][i];

      cur_node["Name"]  = ui->spc_cur_body_list->currentItem()->text();
      cur_node["Index"] = i;
      cur_node["Mass"]  = ui->spc_cur_body_mass->text();

      cur_node["MOI"] = dsm_gui_lib::create_QVec3(
          ui->spc_cur_body_pmoi_x->text(), ui->spc_cur_body_pmoi_y->text(),
          ui->spc_cur_body_pmoi_z->text());

      cur_node["POI"] = dsm_gui_lib::create_QVec3(
          ui->spc_cur_body_poi_x->text(), ui->spc_cur_body_poi_y->text(),
          ui->spc_cur_body_poi_z->text());

      cur_node["Pos of COM"] = dsm_gui_lib::create_QVec3(
          ui->spc_cur_body_com_x->text(), ui->spc_cur_body_com_y->text(),
          ui->spc_cur_body_com_z->text());

      cur_node["Constant Momentum"] = dsm_gui_lib::create_QVec3(
          ui->spc_cur_body_cem_x->text(), ui->spc_cur_body_cem_y->text(),
          ui->spc_cur_body_cem_z->text());

      cur_node["Constant Dipole"] = dsm_gui_lib::create_QVec3(
          ui->spc_cur_body_cemd_x->text(), ui->spc_cur_body_cemd_y->text(),
          ui->spc_cur_body_cemd_z->text());

      cur_node["Geometry File Name"] = ui->spc_cur_body_geom->text();
      cur_node["Node File Name"]     = ui->spc_cur_node_file->text();
      cur_node["Flex File Name"]     = ui->spc_cur_flex_file->text();

      tmp_data.append(ui->spc_cur_body_pmoi_x->text());
      tmp_data.append(ui->spc_cur_body_pmoi_y->text());
      tmp_data.append(ui->spc_cur_body_pmoi_z->text());
      tmp_data.append(ui->spc_cur_body_poi_x->text());
      tmp_data.append(ui->spc_cur_body_poi_y->text());
      tmp_data.append(ui->spc_cur_body_poi_z->text());
      tmp_data.append(ui->spc_cur_body_com_x->text());
      tmp_data.append(ui->spc_cur_body_com_y->text());
      tmp_data.append(ui->spc_cur_body_com_z->text());
      tmp_data.append(ui->spc_cur_body_cem_x->text());
      tmp_data.append(ui->spc_cur_body_cem_y->text());
      tmp_data.append(ui->spc_cur_body_cem_z->text());
      tmp_data.append(ui->spc_cur_body_cemd_x->text());
      tmp_data.append(ui->spc_cur_body_cemd_y->text());
      tmp_data.append(ui->spc_cur_body_cemd_z->text());
      tmp_data.append(ui->spc_cur_body_geom->text());
      tmp_data.append(ui->spc_cur_node_file->text());
      tmp_data.append(ui->spc_cur_flex_file->text());

      ui->spc_cur_body_list->currentItem()->setData(
          256, ui->spc_cur_body_name->text());
      ui->spc_cur_body_list->currentItem()->setData(257, tmp_data);
      tmp_data.clear();
   }

   /* Joints */
   for (int i = 0; i < joints; i++) {
      ui->spc_cur_joint_list->setCurrentRow(i);
      YAML::Node cur_node = cur_spc_yaml["Bodies"][i];

      cur_node["Name"]  = ui->spc_cur_joint_list->currentItem()->text();
      cur_node["Index"] = i;

      cur_node["Body Indicies"] = dsm_gui_lib::create_QVec2(
          ui->spc_cur_joint_in->text(), ui->spc_cur_joint_out->text());
      ;

      cur_node["Joint Type"]   = ui->spc_cur_joint_type->currentText();
      cur_node["Rot DOF"]      = ui->spc_cur_joint_rotdof->text();
      cur_node["Rot Sequence"] = ui->spc_cur_joint_rotdof_seq->currentText();
      cur_node["Rot Type"]     = ui->spc_cur_joint_rottype->currentText();

      cur_node["Trn DOF"]      = ui->spc_cur_joint_trndof->text();
      cur_node["Trn Sequence"] = ui->spc_cur_joint_trndof_seq->currentText();

      QString data1, data2, data3;

      if (ui->spc_cur_joint_rlock1->isChecked())
         data1 = "true";
      else
         data1 = "false";

      if (ui->spc_cur_joint_rlock2->isChecked())
         data2 = "true";
      else
         data2 = "false";

      if (ui->spc_cur_joint_rlock3->isChecked())
         data3 = "true";
      else
         data3 = "false";

      cur_node["Rot DOF Locked"] =
          dsm_gui_lib::create_QVec3(data1, data2, data3);

      if (ui->spc_cur_joint_tlock1->isChecked())
         data1 = "true";
      else
         data1 = "false";

      if (ui->spc_cur_joint_tlock2->isChecked())
         data2 = "true";
      else
         data2 = "false";

      if (ui->spc_cur_joint_tlock3->isChecked())
         data3 = "true";
      else
         data3 = "false";

      cur_node["Trn DOF Locked"] =
          dsm_gui_lib::create_QVec3(data1, data2, data3);

      cur_node["Init Angles"] = dsm_gui_lib::create_QVec3(
          ui->spc_cur_joint_ang0_1->text(), ui->spc_cur_joint_ang0_2->text(),
          ui->spc_cur_joint_ang0_3->text());

      cur_node["Init Angle Rates"] =
          dsm_gui_lib::create_QVec3(ui->spc_cur_joint_angrate0_1->text(),
                                    ui->spc_cur_joint_angrate0_2->text(),
                                    ui->spc_cur_joint_angrate0_3->text());

      cur_node["Init Displacement"] = dsm_gui_lib::create_QVec3(
          ui->spc_cur_joint_disp0_1->text(), ui->spc_cur_joint_disp0_2->text(),
          ui->spc_cur_joint_disp0_3->text());

      cur_node["Init Displacement Rates"] =
          dsm_gui_lib::create_QVec3(ui->spc_cur_joint_dispr0_1->text(),
                                    ui->spc_cur_joint_dispr0_2->text(),
                                    ui->spc_cur_joint_dispr0_3->text());

      cur_node["Bi-Gi Angles"]["Angles"] = dsm_gui_lib::create_QVec3(
          ui->spc_cur_joint_bigi_1->text(), ui->spc_cur_joint_bigi_2->text(),
          ui->spc_cur_joint_bigi_3->text());

      cur_node["Bi-Gi Angles"]["Sequence"] =
          ui->spc_cur_joint_bigi_seq->currentText();

      cur_node["Bo-Go Angles"]["Angles"] = dsm_gui_lib::create_QVec3(
          ui->spc_cur_joint_bogo_1->text(), ui->spc_cur_joint_bogo_2->text(),
          ui->spc_cur_joint_bogo_3->text());
      cur_node["Bo-Go Angles"]["Sequence"] =
          ui->spc_cur_joint_bogo_seq->currentText();

      cur_node["Pos wrt Inner Body"] =
          dsm_gui_lib::create_QVec3(ui->spc_cur_joint_poswrt_in_1->text(),
                                    ui->spc_cur_joint_poswrt_in_2->text(),
                                    ui->spc_cur_joint_poswrt_in_3->text());

      cur_node["Pos wrt Outer Body"] =
          dsm_gui_lib::create_QVec3(ui->spc_cur_joint_poswrt_out_1->text(),
                                    ui->spc_cur_joint_poswrt_out_2->text(),
                                    ui->spc_cur_joint_poswrt_out_3->text());

      tmp_data.append(ui->spc_cur_joint_type->currentText());
      tmp_data.append(ui->spc_cur_joint_in->text());
      tmp_data.append(ui->spc_cur_joint_out->text());
      tmp_data.append(ui->spc_cur_joint_rotdof->text());
      tmp_data.append(ui->spc_cur_joint_rotdof_seq->currentText());
      tmp_data.append(ui->spc_cur_joint_rottype->currentText());
      tmp_data.append(ui->spc_cur_joint_trndof->text());
      tmp_data.append(ui->spc_cur_joint_trndof_seq->currentText());

      if (ui->spc_cur_joint_rlock1->isChecked())
         tmp_data.append("true");
      else
         tmp_data.append("false");

      if (ui->spc_cur_joint_rlock2->isChecked())
         tmp_data.append("true");
      else
         tmp_data.append("false");

      if (ui->spc_cur_joint_rlock3->isChecked())
         tmp_data.append("true");
      else
         tmp_data.append("FALSE");

      if (ui->spc_cur_joint_tlock1->isChecked())
         tmp_data.append("true");
      else
         tmp_data.append("false");

      if (ui->spc_cur_joint_tlock2->isChecked())
         tmp_data.append("true");
      else
         tmp_data.append("false");

      if (ui->spc_cur_joint_tlock3->isChecked())
         tmp_data.append("true");
      else
         tmp_data.append("FALSE");

      tmp_data.append(ui->spc_cur_joint_ang0_1->text());
      tmp_data.append(ui->spc_cur_joint_ang0_2->text());
      tmp_data.append(ui->spc_cur_joint_ang0_3->text());
      tmp_data.append(ui->spc_cur_joint_angrate0_1->text());
      tmp_data.append(ui->spc_cur_joint_angrate0_2->text());
      tmp_data.append(ui->spc_cur_joint_angrate0_3->text());
      tmp_data.append(ui->spc_cur_joint_disp0_1->text());
      tmp_data.append(ui->spc_cur_joint_disp0_2->text());
      tmp_data.append(ui->spc_cur_joint_disp0_3->text());
      tmp_data.append(ui->spc_cur_joint_dispr0_1->text());
      tmp_data.append(ui->spc_cur_joint_dispr0_2->text());
      tmp_data.append(ui->spc_cur_joint_dispr0_3->text());
      tmp_data.append(ui->spc_cur_joint_bigi_1->text());
      tmp_data.append(ui->spc_cur_joint_bigi_2->text());
      tmp_data.append(ui->spc_cur_joint_bigi_3->text());
      tmp_data.append(ui->spc_cur_joint_bigi_seq->currentText());
      tmp_data.append(ui->spc_cur_joint_bogo_1->text());
      tmp_data.append(ui->spc_cur_joint_bogo_2->text());
      tmp_data.append(ui->spc_cur_joint_bogo_3->text());
      tmp_data.append(ui->spc_cur_joint_bogo_seq->currentText());
      tmp_data.append(ui->spc_cur_joint_poswrt_in_1->text());
      tmp_data.append(ui->spc_cur_joint_poswrt_in_2->text());
      tmp_data.append(ui->spc_cur_joint_poswrt_in_3->text());
      tmp_data.append(ui->spc_cur_joint_poswrt_out_1->text());
      tmp_data.append(ui->spc_cur_joint_poswrt_out_2->text());
      tmp_data.append(ui->spc_cur_joint_poswrt_out_3->text());
      tmp_data.append(ui->spc_cur_joint_param_file->text());

      ui->spc_cur_joint_list->currentItem()->setData(
          256, ui->spc_cur_joint_name->text());
      ui->spc_cur_joint_list->currentItem()->setData(257, tmp_data);
      tmp_data.clear();
   }

   /* Wheels */
   QMap<QString, QString> global_wheel_params = {{"Drag", wheel_drag},
                                                 {"Jitter", wheel_jitter}};
   cur_spc_yaml["Wheel Params"]               = global_wheel_params;

   for (int i = 0; i < wheels; i++) {
      ui->spc_cur_body_list->setCurrentRow(i);
      YAML::Node cur_node = cur_spc_yaml["Wheels"][i];

      cur_node["Name"]  = ui->spc_cur_wheel_list->currentItem()->text();
      cur_node["Index"] = i;

      cur_node["Initial Momentum"] = ui->spc_cur_wheel_initmom->text();

      cur_node["Axis"] = dsm_gui_lib::create_QVec3(
          ui->spc_cur_wheel_axis_1->text(), ui->spc_cur_wheel_axis_2->text(),
          ui->spc_cur_wheel_axis_3->text());
      ;

      cur_node["Max Torque"]   = ui->spc_cur_wheel_initmom->text();
      cur_node["Max Momentum"] = ui->spc_cur_wheel_maxmom->text();

      cur_node["Rotor Inertia"] = ui->spc_cur_wheel_inertia->text();
      cur_node["Body"] =
          cur_spc_yaml["Bodies"][ui->spc_cur_wheel_body->cleanText().toInt()];

      cur_node["Node"] = ui->spc_cur_wheel_node->cleanText();

      tmp_data.append(ui->spc_cur_wheel_initmom->text());
      tmp_data.append(ui->spc_cur_wheel_axis_1->text());
      tmp_data.append(ui->spc_cur_wheel_axis_2->text());
      tmp_data.append(ui->spc_cur_wheel_axis_3->text());
      tmp_data.append(ui->spc_cur_wheel_maxtrq->text());
      tmp_data.append(ui->spc_cur_wheel_maxmom->text());
      tmp_data.append(ui->spc_cur_wheel_inertia->text());
      tmp_data.append(ui->spc_cur_wheel_body->cleanText());
      tmp_data.append(ui->spc_cur_wheel_node->cleanText());
      tmp_data.append(ui->spc_cur_wheel_drjit_file->text());

      ui->spc_cur_wheel_list->currentItem()->setData(
          256, ui->spc_cur_wheel_name->text());
      ui->spc_cur_wheel_list->currentItem()->setData(257, tmp_data);
      tmp_data.clear();
   }

   /* MTBs */

   for (int i = 0; i < mtbs; i++) {
      ui->spc_cur_mtb_list->setCurrentRow(i);
      YAML::Node cur_node = cur_spc_yaml["MTBs"][i];

      cur_node["Name"]  = ui->spc_cur_mtb_list->currentItem()->text();
      cur_node["Index"] = i;

      cur_node["Saturation"] = ui->spc_cur_mtb_sat->text();

      cur_node["Axis"] = dsm_gui_lib::create_QVec3(
          ui->spc_cur_mtb_axis_1->text(), ui->spc_cur_mtb_axis_2->text(),
          ui->spc_cur_mtb_axis_3->text());
      cur_node["Node"] = ui->spc_cur_mtb_node->cleanText();

      tmp_data.append(ui->spc_cur_mtb_sat->text());
      tmp_data.append(ui->spc_cur_mtb_axis_1->text());
      tmp_data.append(ui->spc_cur_mtb_axis_2->text());
      tmp_data.append(ui->spc_cur_mtb_axis_3->text());
      tmp_data.append(ui->spc_cur_mtb_node->text());

      ui->spc_cur_mtb_list->currentItem()->setData(
          256, ui->spc_cur_mtb_name->text());
      ui->spc_cur_mtb_list->currentItem()->setData(257, tmp_data);
      tmp_data.clear();
   }

   /* Thrusters */

   for (int i = 0; i < thrusters; i++) {
      ui->spc_cur_thruster_list->setCurrentRow(i);
      YAML::Node cur_node = cur_spc_yaml["Thrusters"][i];

      cur_node["Name"]  = ui->spc_cur_thruster_list->currentItem()->text();
      cur_node["Index"] = i;

      cur_node["Mode"]  = ui->spc_cur_thruster_mode->currentText();
      cur_node["Force"] = ui->spc_cur_thruster_force->text();

      cur_node["Axis"] =
          dsm_gui_lib::create_QVec3(ui->spc_cur_thruster_axis_1->text(),
                                    ui->spc_cur_thruster_axis_2->text(),
                                    ui->spc_cur_thruster_axis_3->text());
      cur_node["Node"] = ui->spc_cur_thruster_node->cleanText();

      tmp_data.append(ui->spc_cur_thruster_mode->currentText());
      tmp_data.append(ui->spc_cur_thruster_force->text());
      tmp_data.append(ui->spc_cur_thruster_axis_1->text());
      tmp_data.append(ui->spc_cur_thruster_axis_2->text());
      tmp_data.append(ui->spc_cur_thruster_axis_3->text());
      tmp_data.append(ui->spc_cur_thruster_body->text());
      tmp_data.append(ui->spc_cur_thruster_node->text());

      ui->spc_cur_thruster_list->currentItem()->setData(
          256, ui->spc_cur_thruster_name->text());
      ui->spc_cur_thruster_list->currentItem()->setData(257, tmp_data);
      tmp_data.clear();
   }

   /* Gyros */

   for (int i = 0; i < gyros; i++) {
      ui->spc_cur_gyro_list->setCurrentRow(i);
      YAML::Node cur_node = cur_spc_yaml["Gyros"][i];

      cur_node["Name"]  = ui->spc_cur_gyro_list->currentItem()->text();
      cur_node["Index"] = i;

      cur_node["Mode"] = ui->spc_cur_gyro_samptime->text();

      cur_node["Axis"] = dsm_gui_lib::create_QVec3(
          ui->spc_cur_gyro_axis_1->text(), ui->spc_cur_gyro_axis_2->text(),
          ui->spc_cur_gyro_axis_3->text());
      ;
      cur_node["Max Rate"]          = ui->spc_cur_gyro_maxrate->text();
      cur_node["Scale Factor"]      = ui->spc_cur_gyro_scaleferror->text();
      cur_node["Quantization"]      = ui->spc_cur_gyro_quant->text();
      cur_node["Angle Random Walk"] = ui->spc_cur_gyro_angrwalk->text();
      cur_node["Bias Stability"]    = ui->spc_cur_gyro_bias_stab->text();
      cur_node["Bias Stability Timespan"] = ui->spc_cur_gyro_bias_tspan->text();
      cur_node["Angle Noise"]             = ui->spc_cur_gyro_angnoise->text();
      cur_node["Initial Bias"]            = ui->spc_cur_gyro_initbias->text();

      cur_node["Node"] = ui->spc_cur_gyro_node->cleanText();

      tmp_data.append(ui->spc_cur_gyro_samptime->text());
      tmp_data.append(ui->spc_cur_gyro_axis_1->text());
      tmp_data.append(ui->spc_cur_gyro_axis_2->text());
      tmp_data.append(ui->spc_cur_gyro_axis_3->text());
      tmp_data.append(ui->spc_cur_gyro_maxrate->text());
      tmp_data.append(ui->spc_cur_gyro_scaleferror->text());
      tmp_data.append(ui->spc_cur_gyro_quant->text());
      tmp_data.append(ui->spc_cur_gyro_angrwalk->text());
      tmp_data.append(ui->spc_cur_gyro_bias_stab->text());
      tmp_data.append(ui->spc_cur_gyro_bias_tspan->text());
      tmp_data.append(ui->spc_cur_gyro_angnoise->text());
      tmp_data.append(ui->spc_cur_gyro_initbias->text());
      tmp_data.append(ui->spc_cur_gyro_node->text());

      ui->spc_cur_gyro_list->currentItem()->setData(
          256, ui->spc_cur_gyro_name->text());
      ui->spc_cur_gyro_list->currentItem()->setData(257, tmp_data);
      tmp_data.clear();
   }

   /* Magnetometers */
   for (int i = 0; i < mags; i++) {
      ui->spc_cur_mag_list->setCurrentRow(i);
      YAML::Node cur_node = cur_spc_yaml["Magnetometers"][i];

      cur_node["Name"]  = ui->spc_cur_mag_list->currentItem()->text();
      cur_node["Index"] = i;

      cur_node["Sample Time"] = ui->spc_cur_mag_samptime->text();

      cur_node["Axis"] = dsm_gui_lib::create_QVec3(
          ui->spc_cur_mag_axis_1->text(), ui->spc_cur_mag_axis_2->text(),
          ui->spc_cur_mag_axis_3->text());
      ;
      cur_node["Saturation"]   = ui->spc_cur_mag_sat->text();
      cur_node["Scale Factor"] = ui->spc_cur_mag_scaleferror->text();
      cur_node["Quantization"] = ui->spc_cur_mag_quant->text();
      cur_node["Noise"]        = ui->spc_cur_mag_noise->text();

      cur_node["Node"] = ui->spc_cur_mag_node->cleanText();

      tmp_data.append(ui->spc_cur_mag_samptime->text());
      tmp_data.append(ui->spc_cur_mag_axis_1->text());
      tmp_data.append(ui->spc_cur_mag_axis_2->text());
      tmp_data.append(ui->spc_cur_mag_axis_3->text());
      tmp_data.append(ui->spc_cur_mag_sat->text());
      tmp_data.append(ui->spc_cur_mag_scaleferror->text());
      tmp_data.append(ui->spc_cur_mag_quant->text());
      tmp_data.append(ui->spc_cur_mag_noise->text());
      tmp_data.append(ui->spc_cur_mag_node->text());

      ui->spc_cur_mag_list->currentItem()->setData(
          256, ui->spc_cur_mag_name->text());
      ui->spc_cur_mag_list->currentItem()->setData(257, tmp_data);
      tmp_data.clear();
   }

   /* CSSs */
   for (int i = 0; i < css_s; i++) {
      ui->spc_cur_css_list->setCurrentRow(i);
      YAML::Node cur_node = cur_spc_yaml["CSSs"][i];

      cur_node["Name"]  = ui->spc_cur_css_list->currentItem()->text();
      cur_node["Index"] = i;

      cur_node["Sample Time"] = ui->spc_cur_css_samptime->text();

      cur_node["Axis"] = dsm_gui_lib::create_QVec3(
          ui->spc_cur_css_axis_1->text(), ui->spc_cur_css_axis_2->text(),
          ui->spc_cur_css_axis_3->text());
      ;
      cur_node["Half Cone Angle"] = ui->spc_cur_css_halfcone->text();
      cur_node["Scale Factor"]    = ui->spc_cur_css_scale->text();
      cur_node["Quantization"]    = ui->spc_cur_css_quant->text();
      cur_node["Body"] =
          cur_spc_yaml["Bodies"][ui->spc_cur_css_body->text().toInt()];

      cur_node["Node"] = ui->spc_cur_css_node->cleanText();

      tmp_data.append(ui->spc_cur_css_samptime->text());
      tmp_data.append(ui->spc_cur_css_axis_1->text());
      tmp_data.append(ui->spc_cur_css_axis_2->text());
      tmp_data.append(ui->spc_cur_css_axis_3->text());
      tmp_data.append(ui->spc_cur_css_halfcone->text());
      tmp_data.append(ui->spc_cur_css_scale->text());
      tmp_data.append(ui->spc_cur_css_quant->text());
      tmp_data.append(ui->spc_cur_css_body->text());
      tmp_data.append(ui->spc_cur_css_node->text());

      ui->spc_cur_css_list->currentItem()->setData(
          256, ui->spc_cur_css_name->text());
      ui->spc_cur_css_list->currentItem()->setData(257, tmp_data);
      tmp_data.clear();
   }

   /* FSSs */
   for (int i = 0; i < fss_s; i++) {
      ui->spc_cur_fss_list->setCurrentRow(i);
      YAML::Node cur_node = cur_spc_yaml["FSSs"][i];

      cur_node["Name"]  = ui->spc_cur_fss_list->currentItem()->text();
      cur_node["Index"] = i;

      cur_node["Sample Time"] = ui->spc_cur_fss_samptime->text();

      cur_node["Mounting Angles"]["Angles"] = dsm_gui_lib::create_QVec3(
          ui->spc_cur_fss_mount_1->text(), ui->spc_cur_fss_mount_2->text(),
          ui->spc_cur_fss_mount_3->text());
      ;
      cur_node["Mounting Angles"]["Sequence"] =
          ui->spc_cur_fss_mountseq->currentText();
      cur_node["Boresight Axis"] = ui->spc_cur_fss_boreaxis->currentText();

      cur_node["FOV Size"] = dsm_gui_lib::create_QVec2(
          ui->spc_cur_fss_hfov->text(), ui->spc_cur_fss_vfov->text());
      ;
      cur_node["Noise Equivalent Angle"] = ui->spc_cur_fss_noiseang->text();
      cur_node["Quantization"]           = ui->spc_cur_fss_quant->text();
      cur_node["Node"]                   = ui->spc_cur_fss_node->cleanText();

      tmp_data.append(ui->spc_cur_fss_samptime->text());
      tmp_data.append(ui->spc_cur_fss_mount_1->text());
      tmp_data.append(ui->spc_cur_fss_mount_2->text());
      tmp_data.append(ui->spc_cur_fss_mount_3->text());
      tmp_data.append(ui->spc_cur_fss_mountseq->currentText());
      tmp_data.append(ui->spc_cur_fss_boreaxis->currentText());
      tmp_data.append(ui->spc_cur_fss_hfov->text());
      tmp_data.append(ui->spc_cur_fss_vfov->text());
      tmp_data.append(ui->spc_cur_fss_noiseang->text());
      tmp_data.append(ui->spc_cur_fss_quant->text());
      tmp_data.append(ui->spc_cur_fss_node->text());

      ui->spc_cur_fss_list->currentItem()->setData(
          256, ui->spc_cur_fss_name->text());
      ui->spc_cur_fss_list->currentItem()->setData(257, tmp_data);
      tmp_data.clear();
   }

   /* STs */
   for (int i = 0; i < stracks; i++) {
      ui->spc_cur_strack_list->setCurrentRow(i);
      YAML::Node cur_node = cur_spc_yaml["STs"][i];

      cur_node["Name"]  = ui->spc_cur_strack_list->currentItem()->text();
      cur_node["Index"] = i;

      cur_node["Sample Time"] = ui->spc_cur_strack_samptime->text();

      cur_node["Mounting Angles"]["Angles"] =
          dsm_gui_lib::create_QVec3(ui->spc_cur_strack_mount_1->text(),
                                    ui->spc_cur_strack_mount_2->text(),
                                    ui->spc_cur_strack_mount_3->text());
      ;
      cur_node["Mounting Angles"]["Sequence"] =
          ui->spc_cur_strack_mountseq->currentText();
      cur_node["Boresight Axis"] = ui->spc_cur_strack_boreaxis->currentText();

      data_vector = {ui->spc_cur_strack_hfov->text(),
                     ui->spc_cur_strack_vfov->text()};

      cur_node["FOV Size"] = dsm_gui_lib::create_QVec2(
          ui->spc_cur_strack_hfov->text(), ui->spc_cur_strack_vfov->text());
      ;
      cur_node["Exclusion Angles"]["Sun"]   = ui->spc_cur_strack_sun->text();
      cur_node["Exclusion Angles"]["Earth"] = ui->spc_cur_strack_earth->text();
      cur_node["Exclusion Angles"]["Luna"]  = ui->spc_cur_strack_moon->text();

      data_vector = {ui->spc_cur_strack_noiseang_1->text(),
                     ui->spc_cur_strack_noiseang_2->text(),
                     ui->spc_cur_strack_noiseang_3->text()};

      cur_node["Noise Equivalent Angle"] =
          dsm_gui_lib::create_QVec3(ui->spc_cur_strack_noiseang_1->text(),
                                    ui->spc_cur_strack_noiseang_2->text(),
                                    ui->spc_cur_strack_noiseang_3->text());
      cur_node["Node"] = ui->spc_cur_strack_node->cleanText();

      tmp_data.append(ui->spc_cur_strack_samptime->text());
      tmp_data.append(ui->spc_cur_strack_mount_1->text());
      tmp_data.append(ui->spc_cur_strack_mount_2->text());
      tmp_data.append(ui->spc_cur_strack_mount_3->text());
      tmp_data.append(ui->spc_cur_strack_mountseq->currentText());
      tmp_data.append(ui->spc_cur_strack_boreaxis->currentText());
      tmp_data.append(ui->spc_cur_strack_hfov->text());
      tmp_data.append(ui->spc_cur_strack_vfov->text());
      tmp_data.append(ui->spc_cur_strack_sun->text());
      tmp_data.append(ui->spc_cur_strack_earth->text());
      tmp_data.append(ui->spc_cur_strack_moon->text());
      tmp_data.append(ui->spc_cur_strack_noiseang_1->text());
      tmp_data.append(ui->spc_cur_strack_noiseang_2->text());
      tmp_data.append(ui->spc_cur_strack_noiseang_3->text());
      tmp_data.append(ui->spc_cur_strack_node->text());

      ui->spc_cur_strack_list->currentItem()->setData(
          256, ui->spc_cur_strack_name->text());
      ui->spc_cur_strack_list->currentItem()->setData(257, tmp_data);
   }

   /* GPSs */

   for (int i = 0; i < gps_s; i++) {
      ui->spc_cur_gps_list->setCurrentRow(i);
      YAML::Node cur_node = cur_spc_yaml["GPSs"][i];

      cur_node["Name"]  = ui->spc_cur_gps_list->currentItem()->text();
      cur_node["Index"] = i;

      cur_node["Sample Time"]    = ui->spc_cur_gps_samptime->text();
      cur_node["Position Noise"] = ui->spc_cur_gps_posnoise->text();
      cur_node["Velocity Noise"] = ui->spc_cur_gps_velnoise->text();
      cur_node["Time Noise"]     = ui->spc_cur_gps_timenoise->text();
      cur_node["Node"]           = ui->spc_cur_gps_node->cleanText();

      tmp_data.append(ui->spc_cur_gps_samptime->text());
      tmp_data.append(ui->spc_cur_gps_posnoise->text());
      tmp_data.append(ui->spc_cur_gps_velnoise->text());
      tmp_data.append(ui->spc_cur_gps_timenoise->text());
      tmp_data.append(ui->spc_cur_gps_node->text());

      ui->spc_cur_gps_list->currentItem()->setData(
          256, ui->spc_cur_gps_name->text());
      ui->spc_cur_gps_list->currentItem()->setData(257, tmp_data);
      tmp_data.clear();
   }

   /* Accelerometers */

   for (int i = 0; i < accels; i++) {
      ui->spc_cur_accel_list->setCurrentRow(i);
      YAML::Node cur_node = cur_spc_yaml["Accelerometers"][i];

      cur_node["Name"]  = ui->spc_cur_accel_list->currentItem()->text();
      cur_node["Index"] = i;

      cur_node["Sample Time"] = ui->spc_cur_acc_samptime->text();

      cur_node["Axis"] = dsm_gui_lib::create_QVec3(
          ui->spc_cur_acc_axis_1->text(), ui->spc_cur_acc_axis_2->text(),
          ui->spc_cur_acc_axis_3->text());
      ;

      cur_node["Max Acceleration"]        = ui->spc_cur_acc_maxacc->text();
      cur_node["Scale Factor"]            = ui->spc_cur_acc_scaleerror->text();
      cur_node["Quantization"]            = ui->spc_cur_acc_quant->text();
      cur_node["DV Random Walk"]          = ui->spc_cur_acc_dvrandwalk->text();
      cur_node["Bias Stability"]          = ui->spc_cur_acc_bias_stab->text();
      cur_node["Bias Stability Timespan"] = ui->spc_cur_acc_bias_tspan->text();
      cur_node["DV Noise"]                = ui->spc_cur_acc_dvnoise->text();
      cur_node["Initial Bias"]            = ui->spc_cur_acc_initbias->text();
      cur_node["Node"]                    = ui->spc_cur_acc_node->cleanText();

      tmp_data.append(ui->spc_cur_acc_samptime->text());
      tmp_data.append(ui->spc_cur_acc_axis_1->text());
      tmp_data.append(ui->spc_cur_acc_axis_2->text());
      tmp_data.append(ui->spc_cur_acc_axis_3->text());
      tmp_data.append(ui->spc_cur_acc_maxacc->text());
      tmp_data.append(ui->spc_cur_acc_scaleerror->text());
      tmp_data.append(ui->spc_cur_acc_quant->text());
      tmp_data.append(ui->spc_cur_acc_dvrandwalk->text());
      tmp_data.append(ui->spc_cur_acc_bias_stab->text());
      tmp_data.append(ui->spc_cur_acc_bias_tspan->text());
      tmp_data.append(ui->spc_cur_acc_dvnoise->text());
      tmp_data.append(ui->spc_cur_acc_initbias->text());
      tmp_data.append(ui->spc_cur_acc_node->text());

      ui->spc_cur_accel_list->currentItem()->setData(
          256, ui->spc_cur_acc_name->text());
      ui->spc_cur_accel_list->currentItem()->setData(257, tmp_data);
      tmp_data.clear();
   }

   write_data(cur_spc_yaml);
}

void SPC_submenu::setQComboBox(QComboBox *comboBox, QString string) {
   comboBox->setCurrentIndex(comboBox->findText(string));
}

// Body -/+/Duplicate/Item Clicked

void SPC_submenu::on_spc_cur_body_remove_clicked() {
   if (bodies == 1) {
      dsm_gui_lib::error_message("Spacecraft must have at least one body!");
      return;
   }

   delete ui->spc_cur_body_list->currentItem();

   if (bodies > 1)
      bodies -= 1;

   if (bodies > 0) {
      QListWidgetItem cur_item = *ui->spc_cur_body_list->currentItem();
      on_spc_cur_body_list_itemClicked(&cur_item);
   }
}

void SPC_submenu::on_spc_cur_body_add_clicked() {
   bodies += 1;

   QStringList tmp_data = {"100.0",
                           "100.0",
                           "200.0",
                           "300.0",
                           "0.0",
                           "0.0",
                           "0.0",
                           "0.0",
                           "0.0",
                           "0.0",
                           "0.0",
                           "0.0",
                           "0.0",
                           "0.0",
                           "0.0",
                           "0.0",
                           "IonCruiser.obj",
                           "NONE",
                           "NONE"};

   proc_add(ui->spc_cur_body_list, tmp_data);
   on_spc_cur_body_list_itemClicked(ui->spc_cur_body_list->currentItem());
}

void SPC_submenu::on_spc_cur_body_duplicate_clicked() {
   if (bodies == 0)
      return;
   bodies += 1;

   proc_duplicates(ui->spc_cur_body_list);
   on_spc_cur_body_list_itemClicked(ui->spc_cur_body_list->currentItem());
}

void SPC_submenu::on_spc_cur_body_list_itemClicked(QListWidgetItem *item) {
   receive_data();

   QStringList current_data = item->data(257).toStringList();
   QLineEdit *textboxes[19] = {ui->spc_cur_body_mass,   ui->spc_cur_body_pmoi_x,
                               ui->spc_cur_body_pmoi_y, ui->spc_cur_body_pmoi_z,
                               ui->spc_cur_body_poi_x,  ui->spc_cur_body_poi_y,
                               ui->spc_cur_body_poi_z,  ui->spc_cur_body_com_x,
                               ui->spc_cur_body_com_y,  ui->spc_cur_body_com_z,
                               ui->spc_cur_body_cem_x,  ui->spc_cur_body_cem_y,
                               ui->spc_cur_body_cem_z,  ui->spc_cur_body_cemd_x,
                               ui->spc_cur_body_cemd_y, ui->spc_cur_body_cemd_z,
                               ui->spc_cur_body_geom,   ui->spc_cur_node_file,
                               ui->spc_cur_flex_file};
   int data_inds[19]        = {1,  2,  3,  4,  5,  6,  7,  8,  9, 10,
                               11, 12, 13, 14, 15, 16, 17, 18, 19};

   item->setText(item->data(256).toString());
   ui->spc_cur_body_name->setText(item->data(256).toString());

   dsm_gui_lib::mult_setText(textboxes, 19, current_data, data_inds);
}

// Joint

void SPC_submenu::on_spc_cur_joint_remove_clicked() {
   delete ui->spc_cur_joint_list->currentItem();
   if (joints > 0)
      joints -= 1;
   if (joints > 0) {
      QListWidgetItem cur_item = *ui->spc_cur_joint_list->currentItem();
      on_spc_cur_joint_list_itemClicked(&cur_item);
   }
}

void SPC_submenu::on_spc_cur_joint_add_clicked() {
   joints += 1;

   QStringList tmp_data = {
       "PASSIVE", "0",     "1",     "1",     "213",   "GIMBAL", "0",
       "123",     "FALSE", "FALSE", "FALSE", "FALSE", "FALSE",  "FALSE",
       "0.0",     "0.0",   "0.0",   "0.0",   "0.0",   "0.0",    "0.0",
       "0.0",     "0.0",   "0.0",   "0.0",   "0.0",   "0.0",    "0.0",
       "0.0",     "312",   "0.0",   "0.0",   "0.0",   "312",    "0.0",
       "0.0",     "0.0",   "0.0",   "0.0",   "0.0",   "NONE"};

   proc_add(ui->spc_cur_joint_list, tmp_data);
   on_spc_cur_joint_list_itemClicked(ui->spc_cur_joint_list->currentItem());
}

void SPC_submenu::on_spc_cur_joint_duplicate_clicked() {
   if (joints == 0)
      return;
   joints += 1;

   proc_duplicates(ui->spc_cur_joint_list);
   on_spc_cur_joint_list_itemClicked(ui->spc_cur_joint_list->currentItem());
}

void SPC_submenu::on_spc_cur_joint_list_itemClicked(QListWidgetItem *item) {
   receive_data();

   QStringList current_data = item->data(257).toStringList();
   QLineEdit *textboxes[25] = {
       ui->spc_cur_joint_ang0_1,       ui->spc_cur_joint_ang0_2,
       ui->spc_cur_joint_ang0_3,       ui->spc_cur_joint_angrate0_1,
       ui->spc_cur_joint_angrate0_2,   ui->spc_cur_joint_angrate0_3,
       ui->spc_cur_joint_disp0_1,      ui->spc_cur_joint_disp0_2,
       ui->spc_cur_joint_disp0_3,      ui->spc_cur_joint_dispr0_1,
       ui->spc_cur_joint_dispr0_2,     ui->spc_cur_joint_dispr0_3,
       ui->spc_cur_joint_bigi_1,       ui->spc_cur_joint_bigi_2,
       ui->spc_cur_joint_bigi_3,       ui->spc_cur_joint_bogo_1,
       ui->spc_cur_joint_bogo_2,       ui->spc_cur_joint_bogo_3,
       ui->spc_cur_joint_poswrt_in_1,  ui->spc_cur_joint_poswrt_in_2,
       ui->spc_cur_joint_poswrt_in_3,  ui->spc_cur_joint_poswrt_out_1,
       ui->spc_cur_joint_poswrt_out_2, ui->spc_cur_joint_poswrt_out_3,
       ui->spc_cur_joint_param_file};
   int data_inds[25] = {15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27,
                        28, 29, 31, 32, 33, 35, 36, 37, 38, 39, 40, 41};
   dsm_gui_lib::mult_setText(textboxes, 25, current_data, data_inds);

   item->setText(item->data(256).toString());
   ui->spc_cur_joint_name->setText(item->data(256).toString());

   setQComboBox(ui->spc_cur_joint_type, current_data[1]);

   ui->spc_cur_joint_in->setValue(current_data[2].toInt());
   ui->spc_cur_joint_out->setValue(current_data[3].toInt());

   ui->spc_cur_joint_rotdof->setValue(current_data[4].toInt());
   setQComboBox(ui->spc_cur_joint_rotdof_seq, current_data[5]);
   setQComboBox(ui->spc_cur_joint_rottype, current_data[6]);

   ui->spc_cur_joint_trndof->setValue(current_data[7].toInt());
   setQComboBox(ui->spc_cur_joint_trndof_seq, current_data[8]);

   if (!QString::compare(current_data[9], "true"))
      ui->spc_cur_joint_rlock1->setCheckState(Qt::Checked);
   else
      ui->spc_cur_joint_rlock1->setCheckState(Qt::Unchecked);

   if (!QString::compare(current_data[10], "true"))
      ui->spc_cur_joint_rlock2->setCheckState(Qt::Checked);
   else
      ui->spc_cur_joint_rlock2->setCheckState(Qt::Unchecked);

   if (!QString::compare(current_data[11], "true"))
      ui->spc_cur_joint_rlock3->setCheckState(Qt::Checked);
   else
      ui->spc_cur_joint_rlock3->setCheckState(Qt::Unchecked);

   if (!QString::compare(current_data[12], "true"))
      ui->spc_cur_joint_tlock1->setCheckState(Qt::Checked);
   else
      ui->spc_cur_joint_tlock1->setCheckState(Qt::Unchecked);

   if (!QString::compare(current_data[13], "true"))
      ui->spc_cur_joint_tlock2->setCheckState(Qt::Checked);
   else
      ui->spc_cur_joint_tlock2->setCheckState(Qt::Unchecked);

   if (!QString::compare(current_data[14], "true"))
      ui->spc_cur_joint_tlock3->setCheckState(Qt::Checked);
   else
      ui->spc_cur_joint_tlock3->setCheckState(Qt::Unchecked);

   setQComboBox(ui->spc_cur_joint_bigi_seq, current_data[30]);
   setQComboBox(ui->spc_cur_joint_bogo_seq, current_data[34]);
}

// Wheels -/+/Duplicate/Item Clicked

void SPC_submenu::on_spc_cur_wheel_remove_clicked() {
   delete ui->spc_cur_wheel_list->currentItem();
   if (wheels > 0)
      wheels -= 1;
   if (wheels > 0) {
      QListWidgetItem cur_item = *ui->spc_cur_wheel_list->currentItem();
      on_spc_cur_wheel_list_itemClicked(&cur_item);
   }
}

void SPC_submenu::on_spc_cur_wheel_add_clicked() {
   wheels += 1;

   QStringList tmp_data = {"0.0",  "1.0",   "0.0", "0.0", "0.14",
                           "50.0", "0.012", "0",   "0",   "NONE"};

   proc_add(ui->spc_cur_wheel_list, tmp_data);
   on_spc_cur_wheel_list_itemClicked(ui->spc_cur_wheel_list->currentItem());
}

void SPC_submenu::on_spc_cur_wheel_duplicate_clicked() {
   if (wheels == 0)
      return;
   wheels += 1;

   proc_duplicates(ui->spc_cur_wheel_list);
   on_spc_cur_wheel_list_itemClicked(ui->spc_cur_wheel_list->currentItem());
}

void SPC_submenu::on_spc_cur_wheel_list_itemClicked(QListWidgetItem *item) {
   receive_data();

   QStringList current_data = item->data(257).toStringList();
   QLineEdit *textboxes[8]  = {
       ui->spc_cur_wheel_initmom, ui->spc_cur_wheel_axis_1,
       ui->spc_cur_wheel_axis_2,  ui->spc_cur_wheel_axis_3,
       ui->spc_cur_wheel_maxtrq,  ui->spc_cur_wheel_maxmom,
       ui->spc_cur_wheel_inertia, ui->spc_cur_wheel_drjit_file};
   int data_inds[8] = {1, 2, 3, 4, 5, 6, 7, 10};
   dsm_gui_lib::mult_setText(textboxes, 8, current_data, data_inds);

   item->setText(item->data(256).toString());

   ui->spc_cur_wheel_name->setText(item->data(256).toString());

   ui->spc_cur_wheel_body->setValue(current_data[8].toInt());
   ui->spc_cur_wheel_node->setValue(current_data[9].toInt());

   if (!QString::compare(wheel_drag, "FALSE"))
      ui->spc_cur_wheel_glob_drag_off->setChecked(Qt::Checked);
   else
      ui->spc_cur_wheel_glob_drag_on->setChecked(Qt::Checked);

   if (!QString::compare(wheel_jitter, "FALSE"))
      ui->spc_cur_wheel_glob_jitter_off->setChecked(Qt::Checked);
   else
      ui->spc_cur_wheel_glob_jitter_on->setChecked(Qt::Checked);
}

// MTB buttons

void SPC_submenu::on_spc_cur_mtb_remove_clicked() {
   delete ui->spc_cur_mtb_list->currentItem();
   if (mtbs > 0)
      mtbs -= 1;
   if (mtbs > 0) {
      QListWidgetItem cur_item = *ui->spc_cur_mtb_list->currentItem();
      on_spc_cur_mtb_list_itemClicked(&cur_item);
   }
}

void SPC_submenu::on_spc_cur_mtb_add_clicked() {
   mtbs += 1;

   QStringList tmp_data = {"180.0", "1.0", "0.0", "0.0", "0"};

   proc_add(ui->spc_cur_mtb_list, tmp_data);
   on_spc_cur_mtb_list_itemClicked(ui->spc_cur_mtb_list->currentItem());
}

void SPC_submenu::on_spc_cur_mtb_duplicate_clicked() {
   if (mtbs == 0)
      return;
   mtbs += 1;

   proc_duplicates(ui->spc_cur_mtb_list);
   on_spc_cur_mtb_list_itemClicked(ui->spc_cur_mtb_list->currentItem());
}

void SPC_submenu::on_spc_cur_mtb_list_itemClicked(QListWidgetItem *item) {
   receive_data();

   QStringList current_data = item->data(257).toStringList();
   QLineEdit *textboxes[4]  = {ui->spc_cur_mtb_sat, ui->spc_cur_mtb_axis_1,
                               ui->spc_cur_mtb_axis_2, ui->spc_cur_mtb_axis_3};
   int data_inds[4]         = {1, 2, 3, 4};
   dsm_gui_lib::mult_setText(textboxes, 4, current_data, data_inds);

   item->setText(item->data(256).toString());

   ui->spc_cur_mtb_name->setText(item->data(256).toString());

   ui->spc_cur_mtb_node->setValue(current_data[5].toInt());
}

// Thruster buttons

void SPC_submenu::on_spc_cur_thruster_remove_clicked() {
   delete ui->spc_cur_thruster_list->currentItem();
   if (thrusters > 0)
      thrusters -= 1;
   if (thrusters > 0) {
      QListWidgetItem cur_item = *ui->spc_cur_thruster_list->currentItem();
      on_spc_cur_thruster_list_itemClicked(&cur_item);
   }
}

void SPC_submenu::on_spc_cur_thruster_add_clicked() {
   thrusters += 1;

   QStringList tmp_data = {"PULSED", "1.0", "-1.0", "0.0", "0.0", "0", "0"};

   proc_add(ui->spc_cur_thruster_list, tmp_data);
   on_spc_cur_thruster_list_itemClicked(
       ui->spc_cur_thruster_list->currentItem());
}

void SPC_submenu::on_spc_cur_thruster_duplicate_clicked() {
   if (thrusters == 0)
      return;
   thrusters += 1;

   proc_duplicates(ui->spc_cur_thruster_list);
   on_spc_cur_thruster_list_itemClicked(
       ui->spc_cur_thruster_list->currentItem());
}

void SPC_submenu::on_spc_cur_thruster_list_itemClicked(QListWidgetItem *item) {
   receive_data();

   QStringList current_data = item->data(257).toStringList();
   QLineEdit *textboxes[4]  = {
       ui->spc_cur_thruster_force, ui->spc_cur_thruster_axis_1,
       ui->spc_cur_thruster_axis_2, ui->spc_cur_thruster_axis_3};
   int data_inds[4] = {2, 3, 4, 5};
   dsm_gui_lib::mult_setText(textboxes, 4, current_data, data_inds);

   item->setText(item->data(256).toString());
   ui->spc_cur_thruster_name->setText(item->data(256).toString());

   setQComboBox(ui->spc_cur_thruster_mode, current_data[1]);

   ui->spc_cur_thruster_body->setValue(current_data[6].toInt());
   ui->spc_cur_thruster_node->setValue(current_data[7].toInt());
}

// Gyro buttons

void SPC_submenu::on_spc_cur_gyro_remove_clicked() {
   delete ui->spc_cur_gyro_list->currentItem();
   if (gyros > 0)
      gyros -= 1;
   if (gyros > 0) {
      QListWidgetItem cur_item = *ui->spc_cur_gyro_list->currentItem();
      on_spc_cur_gyro_list_itemClicked(&cur_item);
   }
}

void SPC_submenu::on_spc_cur_gyro_add_clicked() {
   gyros += 1;

   QStringList tmp_data = {"0.1",   "1.0", "0.0",  "0.0", "1000.0",
                           "100.0", "1.0", "0.07", "0.1", "1.0",
                           "0.1",   "0.1", "0"};

   proc_add(ui->spc_cur_gyro_list, tmp_data);
   on_spc_cur_gyro_list_itemClicked(ui->spc_cur_gyro_list->currentItem());
}

void SPC_submenu::on_spc_cur_gyro_duplicate_clicked() {
   if (gyros == 0)
      return;
   gyros += 1;

   proc_duplicates(ui->spc_cur_gyro_list);
   on_spc_cur_gyro_list_itemClicked(ui->spc_cur_gyro_list->currentItem());
}

void SPC_submenu::on_spc_cur_gyro_list_itemClicked(QListWidgetItem *item) {
   receive_data();

   QStringList current_data = item->data(257).toStringList();
   QLineEdit *textboxes[12] = {
       ui->spc_cur_gyro_samptime,  ui->spc_cur_gyro_axis_1,
       ui->spc_cur_gyro_axis_2,    ui->spc_cur_gyro_axis_3,
       ui->spc_cur_gyro_maxrate,   ui->spc_cur_gyro_scaleferror,
       ui->spc_cur_gyro_quant,     ui->spc_cur_gyro_angrwalk,
       ui->spc_cur_gyro_bias_stab, ui->spc_cur_gyro_bias_tspan,
       ui->spc_cur_gyro_angnoise,  ui->spc_cur_gyro_initbias};
   int data_inds[12] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
   dsm_gui_lib::mult_setText(textboxes, 12, current_data, data_inds);

   item->setText(item->data(256).toString());
   ui->spc_cur_gyro_name->setText(item->data(256).toString());

   ui->spc_cur_gyro_node->setValue(current_data[13].toInt());
}

// Magnetometer buttons

void SPC_submenu::on_spc_cur_mag_remove_clicked() {
   delete ui->spc_cur_mag_list->currentItem();
   if (mags > 0)
      mags -= 1;
   if (mags > 0) {
      QListWidgetItem cur_item = *ui->spc_cur_mag_list->currentItem();
      on_spc_cur_mag_list_itemClicked(&cur_item);
   }
}

void SPC_submenu::on_spc_cur_mag_add_clicked() {
   mags += 1;

   QStringList tmp_data = {"0.1", "1.0",    "0.0",    "0.0", "60.0E-6",
                           "0.0", "1.0E-6", "1.0E-6", "0"};

   proc_add(ui->spc_cur_mag_list, tmp_data);
   on_spc_cur_mag_list_itemClicked(ui->spc_cur_mag_list->currentItem());
}

void SPC_submenu::on_spc_cur_mag_duplicate_clicked() {
   if (mags == 0)
      return;
   mags += 1;

   proc_duplicates(ui->spc_cur_mag_list);
   on_spc_cur_mag_list_itemClicked(ui->spc_cur_mag_list->currentItem());
}

void SPC_submenu::on_spc_cur_mag_list_itemClicked(QListWidgetItem *item) {
   receive_data();

   QStringList current_data = item->data(257).toStringList();
   QLineEdit *textboxes[8]  = {
       ui->spc_cur_mag_samptime, ui->spc_cur_mag_axis_1,
       ui->spc_cur_mag_axis_2,   ui->spc_cur_mag_axis_3,
       ui->spc_cur_mag_sat,      ui->spc_cur_mag_scaleferror,
       ui->spc_cur_mag_quant,    ui->spc_cur_mag_noise};
   int data_inds[8] = {1, 2, 3, 4, 5, 6, 7, 8};
   dsm_gui_lib::mult_setText(textboxes, 8, current_data, data_inds);

   item->setText(item->data(256).toString());
   ui->spc_cur_mag_name->setText(item->data(256).toString());

   ui->spc_cur_mag_node->setValue(current_data[9].toInt());
}

// CSS buttons

void SPC_submenu::on_spc_cur_css_remove_clicked() {
   delete ui->spc_cur_css_list->currentItem();
   if (css_s > 0)
      css_s -= 1;
   if (css_s > 0) {
      QListWidgetItem cur_item = *ui->spc_cur_css_list->currentItem();
      on_spc_cur_css_list_itemClicked(&cur_item);
   }
}

void SPC_submenu::on_spc_cur_css_add_clicked() {
   css_s += 1;

   QStringList tmp_data = {"0.1", "1.0",   "0.0", "0.0", "90.0",
                           "1.0", "0.001", "0",   "0"};

   proc_add(ui->spc_cur_css_list, tmp_data);
   on_spc_cur_css_list_itemClicked(ui->spc_cur_css_list->currentItem());
}

void SPC_submenu::on_spc_cur_css_duplicate_clicked() {
   if (css_s == 0)
      return;
   css_s += 1;

   proc_duplicates(ui->spc_cur_css_list);
   on_spc_cur_css_list_itemClicked(ui->spc_cur_css_list->currentItem());
}

void SPC_submenu::on_spc_cur_css_list_itemClicked(QListWidgetItem *item) {
   receive_data();

   QStringList current_data = item->data(257).toStringList();
   QLineEdit *textboxes[7]  = {ui->spc_cur_css_samptime, ui->spc_cur_css_axis_1,
                               ui->spc_cur_css_axis_2,   ui->spc_cur_css_axis_3,
                               ui->spc_cur_css_halfcone, ui->spc_cur_css_scale,
                               ui->spc_cur_css_quant};
   int data_inds[7]         = {1, 2, 3, 4, 5, 6, 7};
   dsm_gui_lib::mult_setText(textboxes, 7, current_data, data_inds);

   item->setText(item->data(256).toString());
   ui->spc_cur_css_name->setText(item->data(256).toString());

   ui->spc_cur_css_body->setValue(current_data[8].toInt());
   ui->spc_cur_css_node->setValue(current_data[9].toInt());
}

// FSS Buttons

void SPC_submenu::on_spc_cur_fss_remove_clicked() {
   delete ui->spc_cur_fss_list->currentItem();
   if (fss_s > 0)
      fss_s -= 1;
   if (fss_s > 0) {
      QListWidgetItem cur_item = *ui->spc_cur_fss_list->currentItem();
      on_spc_cur_fss_list_itemClicked(&cur_item);
   }
}

void SPC_submenu::on_spc_cur_fss_add_clicked() {
   fss_s += 1;

   QStringList tmp_data = {"0.2",  "70.0", "0.0", "0.0", "321", "Z_AXIS",
                           "32.0", "32.0", "0.1", "0.5", "0"};

   proc_add(ui->spc_cur_fss_list, tmp_data);
   on_spc_cur_fss_list_itemClicked(ui->spc_cur_fss_list->currentItem());
}

void SPC_submenu::on_spc_cur_fss_duplicate_clicked() {
   if (fss_s == 0)
      return;
   fss_s += 1;

   proc_duplicates(ui->spc_cur_fss_list);
   on_spc_cur_fss_list_itemClicked(ui->spc_cur_fss_list->currentItem());
}

void SPC_submenu::on_spc_cur_fss_list_itemClicked(QListWidgetItem *item) {
   receive_data();

   QStringList current_data = item->data(257).toStringList();
   QLineEdit *textboxes[8] = {ui->spc_cur_fss_samptime, ui->spc_cur_fss_mount_1,
                              ui->spc_cur_fss_mount_2,  ui->spc_cur_fss_mount_3,
                              ui->spc_cur_fss_hfov,     ui->spc_cur_fss_vfov,
                              ui->spc_cur_fss_noiseang, ui->spc_cur_fss_quant};
   int data_inds[8]        = {1, 2, 3, 4, 7, 8, 9, 10};
   dsm_gui_lib::mult_setText(textboxes, 8, current_data, data_inds);

   item->setText(item->data(256).toString());
   ui->spc_cur_fss_name->setText(item->data(256).toString());

   setQComboBox(ui->spc_cur_fss_mountseq, current_data[5]);
   setQComboBox(ui->spc_cur_fss_boreaxis, current_data[6]);

   ui->spc_cur_css_node->setValue(current_data[11].toInt());
}

// Star Tracker Buttons

void SPC_submenu::on_spc_cur_strack_remove_clicked() {
   delete ui->spc_cur_strack_list->currentItem();
   if (stracks > 0)
      stracks -= 1;
   if (stracks > 0) {
      QListWidgetItem cur_item = *ui->spc_cur_strack_list->currentItem();
      on_spc_cur_strack_list_itemClicked(&cur_item);
   }
}

void SPC_submenu::on_spc_cur_strack_add_clicked() {
   stracks += 1;

   QStringList tmp_data = {"0.25",   "-90.0", "90.0", "00.0", "321",
                           "Z_AXIS", "8.0",   "8.0",  "30.0", "10.0",
                           "10.0",   "2.0",   "2.0",  "20.0", "0"};

   proc_add(ui->spc_cur_strack_list, tmp_data);
   on_spc_cur_strack_list_itemClicked(ui->spc_cur_strack_list->currentItem());
}

void SPC_submenu::on_spc_cur_strack_duplicate_clicked() {
   if (stracks == 0)
      return;
   stracks += 1;

   proc_duplicates(ui->spc_cur_strack_list);
   on_spc_cur_strack_list_itemClicked(ui->spc_cur_strack_list->currentItem());
}

void SPC_submenu::on_spc_cur_strack_list_itemClicked(QListWidgetItem *item) {
   receive_data();

   QStringList current_data = item->data(257).toStringList();
   QLineEdit *textboxes[12] = {
       ui->spc_cur_strack_samptime,   ui->spc_cur_strack_mount_1,
       ui->spc_cur_strack_mount_2,    ui->spc_cur_strack_mount_3,
       ui->spc_cur_strack_hfov,       ui->spc_cur_strack_vfov,
       ui->spc_cur_strack_sun,        ui->spc_cur_strack_earth,
       ui->spc_cur_strack_moon,       ui->spc_cur_strack_noiseang_1,
       ui->spc_cur_strack_noiseang_2, ui->spc_cur_strack_noiseang_3};
   int data_inds[12] = {1, 2, 3, 4, 7, 8, 9, 10, 11, 12, 13, 14};
   dsm_gui_lib::mult_setText(textboxes, 12, current_data, data_inds);

   item->setText(item->data(256).toString());
   ui->spc_cur_strack_name->setText(item->data(256).toString());

   setQComboBox(ui->spc_cur_strack_mountseq, current_data[5]);
   setQComboBox(ui->spc_cur_strack_boreaxis, current_data[6]);

   ui->spc_cur_strack_node->setValue(current_data[15].toInt());
}

// GPS Buttons

void SPC_submenu::on_spc_cur_gps_remove_clicked() {
   delete ui->spc_cur_gps_list->currentItem();
   if (gps_s > 0)
      gps_s -= 1;
   if (gps_s > 0) {
      QListWidgetItem cur_item = *ui->spc_cur_gps_list->currentItem();
      on_spc_cur_gps_list_itemClicked(&cur_item);
   }
}

void SPC_submenu::on_spc_cur_gps_add_clicked() {
   gps_s += 1;

   QStringList tmp_data = {"0.25", "4.0", "0.02", "20.0E-9", "0"};

   proc_add(ui->spc_cur_gps_list, tmp_data);
   on_spc_cur_gps_list_itemClicked(ui->spc_cur_gps_list->currentItem());
}

void SPC_submenu::on_spc_cur_gps_duplicate_clicked() {
   if (gps_s == 0)
      return;
   gps_s += 1;

   proc_duplicates(ui->spc_cur_gps_list);
   on_spc_cur_gps_list_itemClicked(ui->spc_cur_gps_list->currentItem());
}

void SPC_submenu::on_spc_cur_gps_list_itemClicked(QListWidgetItem *item) {
   receive_data();

   QStringList current_data = item->data(257).toStringList();
   QLineEdit *textboxes[4]  = {
       ui->spc_cur_gps_samptime, ui->spc_cur_gps_posnoise,
       ui->spc_cur_gps_velnoise, ui->spc_cur_gps_timenoise};
   int data_inds[4] = {1, 2, 3, 4};
   dsm_gui_lib::mult_setText(textboxes, 4, current_data, data_inds);

   item->setText(item->data(256).toString());
   ui->spc_cur_gps_name->setText(item->data(256).toString());

   ui->spc_cur_gps_node->setValue(current_data[5].toInt());
}

// Accelerometer Buttons

void SPC_submenu::on_spc_cur_accel_remove_clicked() {
   delete ui->spc_cur_accel_list->currentItem();
   if (accels > 0)
      accels -= 1;
   if (accels > 0) {
      QListWidgetItem cur_item = *ui->spc_cur_accel_list->currentItem();
      on_spc_cur_accel_list_itemClicked(&cur_item);
   }
}

void SPC_submenu::on_spc_cur_accel_add_clicked() {
   accels += 1;

   QStringList tmp_data = {"0.1", "1.0", "0.0", "0.0", "1.0", "0.0", "0.05",
                           "0.0", "0.0", "1.0", "0.0", "0.5", "0"};

   proc_add(ui->spc_cur_accel_list, tmp_data);
   on_spc_cur_accel_list_itemClicked(ui->spc_cur_accel_list->currentItem());
}

void SPC_submenu::on_spc_cur_accel_duplicate_clicked() {
   if (accels == 0)
      return;
   accels += 1;

   proc_duplicates(ui->spc_cur_accel_list);
   on_spc_cur_accel_list_itemClicked(ui->spc_cur_accel_list->currentItem());
}

void SPC_submenu::on_spc_cur_accel_list_itemClicked(QListWidgetItem *item) {
   receive_data();

   QStringList current_data = item->data(257).toStringList();
   QLineEdit *textboxes[12] = {
       ui->spc_cur_acc_samptime,  ui->spc_cur_acc_axis_1,
       ui->spc_cur_acc_axis_2,    ui->spc_cur_acc_axis_3,
       ui->spc_cur_acc_maxacc,    ui->spc_cur_acc_scaleerror,
       ui->spc_cur_acc_quant,     ui->spc_cur_acc_dvrandwalk,
       ui->spc_cur_acc_bias_stab, ui->spc_cur_acc_bias_tspan,
       ui->spc_cur_acc_dvnoise,   ui->spc_cur_acc_initbias};
   int data_inds[12] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
   dsm_gui_lib::mult_setText(textboxes, 12, current_data, data_inds);

   item->setText(item->data(256).toString());
   ui->spc_cur_acc_name->setText(item->data(256).toString());

   ui->spc_cur_gps_node->setValue(current_data[13].toInt());
}

// Misc
void SPC_submenu::on_sections_tabBarClicked(int index) {
   if (index == 1) {
      on_spc_cur_body_list_itemClicked(ui->spc_cur_body_list->item(0));
      ui->spc_cur_body_list->setCurrentRow(0);
      if (ui->spc_cur_joint_list->count() > 0) {
         on_spc_cur_joint_list_itemClicked(ui->spc_cur_joint_list->item(0));
         ui->spc_cur_joint_list->setCurrentRow(0);
      }
   }
   if (index == 2) {
      on_actuator_sections_tabBarClicked(ui->actuator_sections->currentIndex());
   }
   if (index == 3) {
      on_sensor_sections_tabBarClicked(ui->sensor_sections->currentIndex());
   }
}

void SPC_submenu::on_actuator_sections_tabBarClicked(int index) {
   if (index == 0 && ui->spc_cur_wheel_list->count() > 0) {
      on_spc_cur_wheel_list_itemClicked(ui->spc_cur_wheel_list->item(0));
      ui->spc_cur_wheel_list->setCurrentRow(0);
   }
   if (index == 1 && ui->spc_cur_mtb_list->count() > 0) {
      on_spc_cur_mtb_list_itemClicked(ui->spc_cur_mtb_list->item(0));
      ui->spc_cur_mtb_list->setCurrentRow(0);
   }
   if (index == 2 && ui->spc_cur_thruster_list->count() > 0) {
      on_spc_cur_thruster_list_itemClicked(ui->spc_cur_thruster_list->item(0));
      ui->spc_cur_thruster_list->setCurrentRow(0);
   }
}

void SPC_submenu::on_sensor_sections_tabBarClicked(int index) {
   if (index == 0 && ui->spc_cur_gyro_list->count() > 0) {
      on_spc_cur_gyro_list_itemClicked(ui->spc_cur_gyro_list->item(0));
      ui->spc_cur_gyro_list->setCurrentRow(0);
   }
   if (index == 1 && ui->spc_cur_mag_list->count() > 0) {
      on_spc_cur_mag_list_itemClicked(ui->spc_cur_mag_list->item(0));
      ui->spc_cur_mag_list->setCurrentRow(0);
   }
   if (index == 2 && ui->spc_cur_css_list->count() > 0) {
      on_spc_cur_css_list_itemClicked(ui->spc_cur_css_list->item(0));
      ui->spc_cur_css_list->setCurrentRow(0);
   }
   if (index == 3 && ui->spc_cur_fss_list->count() > 0) {
      on_spc_cur_fss_list_itemClicked(ui->spc_cur_fss_list->item(0));
      ui->spc_cur_fss_list->setCurrentRow(0);
   }
   if (index == 4 && ui->spc_cur_strack_list->count() > 0) {
      on_spc_cur_strack_list_itemClicked(ui->spc_cur_strack_list->item(0));
      ui->spc_cur_strack_list->setCurrentRow(0);
   }
   if (index == 5 && ui->spc_cur_gps_list->count() > 0) {
      on_spc_cur_gps_list_itemClicked(ui->spc_cur_gps_list->item(0));
      ui->spc_cur_gps_list->setCurrentRow(0);
   }
   if (index == 6 && ui->spc_cur_accel_list->count() > 0) {
      on_spc_cur_accel_list_itemClicked(ui->spc_cur_accel_list->item(0));
      ui->spc_cur_accel_list->setCurrentRow(0);
   }
}

// File Selection Buttons

void SPC_submenu::on_spc_cur_shaker_select_clicked() {
   QString file_name = QFileDialog::getOpenFileName(
       this, tr("Choose Folder"), inout_path, QString(), nullptr,
       QFileDialog::DontUseNativeDialog);

   QDir dir(inout_path);
   QString rel_file_path = dir.relativeFilePath(file_name);

   if (file_name.isEmpty())
      return;

   ui->spc_cur_shaker_file->setText(rel_file_path);
}

void SPC_submenu::on_spc_cur_node_select_clicked() {
   QString file_name = QFileDialog::getOpenFileName(
       this, tr("Choose Folder"), inout_path, QString(), nullptr,
       QFileDialog::DontUseNativeDialog);

   QDir dir(inout_path);
   QString rel_file_path = dir.relativeFilePath(file_name);

   if (file_name.isEmpty())
      return;

   ui->spc_cur_node_file->setText(rel_file_path);
}

void SPC_submenu::on_spc_cur_flex_select_clicked() {
   QString file_name = QFileDialog::getOpenFileName(
       this, tr("Choose Folder"), inout_path, QString(), nullptr,
       QFileDialog::DontUseNativeDialog);

   QDir dir(inout_path);
   QString rel_file_path = dir.relativeFilePath(file_name);

   if (file_name.isEmpty())
      return;

   ui->spc_cur_flex_file->setText(rel_file_path);
}

void SPC_submenu::on_spc_cur_joint_param_select_clicked() {
   QString file_name = QFileDialog::getOpenFileName(
       this, tr("Choose Folder"), inout_path, QString(), nullptr,
       QFileDialog::DontUseNativeDialog);

   QDir dir(inout_path);
   QString rel_file_path = dir.relativeFilePath(file_name);

   if (file_name.isEmpty())
      return;

   ui->spc_cur_joint_param_file->setText(rel_file_path);
}

void SPC_submenu::on_spc_cur_wheel_drjit_select_clicked() {
   QString file_name = QFileDialog::getOpenFileName(
       this, tr("Choose Folder"), inout_path, QString(), nullptr,
       QFileDialog::DontUseNativeDialog);

   QDir dir(inout_path);
   QString rel_file_path = dir.relativeFilePath(file_name);

   if (file_name.isEmpty())
      return;

   ui->spc_cur_wheel_drjit_file->setText(rel_file_path);
}

void SPC_submenu::on_spc_cur_shaker_clear_clicked() {
   ui->spc_cur_shaker_file->setText("NONE");
}

void SPC_submenu::on_spc_cur_node_clear_clicked() {
   ui->spc_cur_node_file->setText("NONE");
}

void SPC_submenu::on_spc_cur_flex_clear_clicked() {
   ui->spc_cur_flex_file->setText("NONE");
}

void SPC_submenu::on_spc_cur_joint_param_clear_clicked() {
   ui->spc_cur_joint_param_file->setText("NONE");
}

void SPC_submenu::on_spc_cur_wheel_drjit_clear_clicked() {
   ui->spc_cur_wheel_drjit_file->setText("NONE");
}

// Simplification Functions for repeated actions

void SPC_submenu::proc_duplicates(QListWidget *cur_list) {
   QStringList old_data = cur_list->currentItem()->data(257).toStringList();

   int index = cur_list->currentRow();
   if (index == -1)
      return;

   QString old_item = cur_list->currentItem()->text();
   QString new_item = old_item + " Copy";
   for (int i = 0; i <= 30; i++) {
      QString newItemTest = new_item;
      if (i > 0)
         newItemTest += " " + QString::number(i);
      if (!spc_names.contains(newItemTest)) {
         new_item = newItemTest;
         break;
      }
   }

   cur_list->addItem(new_item);
   cur_list->setCurrentRow(cur_list->count() - 1);

   cur_list->currentItem()->setData(256, new_item);
   cur_list->currentItem()->setData(257, old_data);
   cur_list->sortItems();
}

void SPC_submenu::proc_add(QListWidget *cur_list, QStringList tmp_data) {
   QStringList all_names;
   for (int i = 0; i < cur_list->count(); i++) {
      all_names.append(cur_list->item(i)->text());
   }

   QString new_name = "New";
   if (cur_list->count() != 0) {
      for (int i = 0; i <= 50; i++) {
         QString newNameTest = new_name;
         if (i > 0)
            newNameTest += QString::number(i);
         if (!all_names.contains(newNameTest)) {
            new_name = newNameTest;
            break;
         }
         if (i == 50)
            return; // Nothing happens if too many
      }
   }

   cur_list->addItem(new_name);
   cur_list->setCurrentRow(cur_list->count() - 1);

   cur_list->currentItem()->setData(256, new_name);
   cur_list->currentItem()->setData(257, tmp_data);
   cur_list->sortItems();
}

void SPC_submenu::destroy_submenu(QString command, QString junk) {
   if (QString::compare(command, "Done") == 0) {
      QDialog::close();
   }
}
