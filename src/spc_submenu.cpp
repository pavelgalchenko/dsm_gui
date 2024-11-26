#include "spc_submenu.h"
#include "dsm_gui_lib.h"
#include "spc_menu.h"
#include "ui_spc_submenu.h"
#include <QDebug>

SPC_submenu::SPC_submenu(QWidget *parent)
    : QDialog(parent), ui(new Ui::SPC_submenu) {
   ui->setupUi(this);

   ninf_pinf_valid = std::make_unique<QDoubleValidator>(-INFINITY, INFINITY, 5);
   zero_pinf_valid = std::make_unique<QDoubleValidator>(0, INFINITY, 5);
   none_one_valid  = std::make_unique<QDoubleValidator>(-1, 1, 5);
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

   QFile target(inout_path + "yaml_comments/SC_comments.yaml");
   QString comment_path = inout_path + "yaml_comments/SC_comments.yaml";

   if (target.exists())
      target.remove();

   QFile::copy(":/data/yaml_comments/SC_comments.yaml", comment_path);

   spc_comments = YAML::LoadFile(comment_path.toStdString());
}

void SPC_submenu::set_validators() {
   QRegularExpression rx("[^\"]*");
   QRegularExpression rx1("[^\" ]*");

   no_quotes_valid        = std::make_unique<QRegularExpressionValidator>(rx);
   no_quotes_spaces_valid = std::make_unique<QRegularExpressionValidator>(rx1);

   // Combo Boxes
   QList<QComboBox *> combo_boxes_euler = {
       ui->spc_cur_joint_rotdof_seq,
       ui->spc_cur_joint_bigi_seq,
       ui->spc_cur_joint_bogo_seq,
       ui->spc_cur_joint_trndof_seq,
       ui->spc_cur_fss_mountseq,
       ui->spc_cur_strack_mountseq}; // All Euler combo boxes

   dsm_gui_lib::set_mult_cbox_validators(combo_boxes_euler,
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
   // All validators with (-INFINITY, INFINITY)
   QList<QLineEdit *> ninf_pinf = {ui->spc_cur_drag,
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
                                   ui->spc_cur_acc_initbias};

   // All validators with [0, INFINITY)
   QList<QLineEdit *> zero_pinf = {
       ui->spc_cur_body_mass,         ui->spc_cur_body_pmoi_x,
       ui->spc_cur_body_pmoi_y,       ui->spc_cur_body_pmoi_z,
       ui->spc_cur_wheel_inertia,     ui->spc_cur_mtb_sat,
       ui->spc_cur_thruster_force,    ui->spc_cur_gyro_samptime,
       ui->spc_cur_gyro_maxrate,      ui->spc_cur_gyro_scaleferror,
       ui->spc_cur_gyro_quant,        ui->spc_cur_gyro_angrwalk,
       ui->spc_cur_gyro_angnoise,     ui->spc_cur_gyro_bias_stab,
       ui->spc_cur_gyro_bias_tspan,   ui->spc_cur_mag_samptime,
       ui->spc_cur_mag_sat,           ui->spc_cur_mag_scaleferror,
       ui->spc_cur_mag_quant,         ui->spc_cur_mag_noise,
       ui->spc_cur_css_samptime,      ui->spc_cur_css_halfcone,
       ui->spc_cur_css_scale,         ui->spc_cur_css_quant,
       ui->spc_cur_fss_samptime,      ui->spc_cur_fss_hfov,
       ui->spc_cur_fss_vfov,          ui->spc_cur_fss_noiseang,
       ui->spc_cur_fss_quant,         ui->spc_cur_strack_samptime,
       ui->spc_cur_strack_hfov,       ui->spc_cur_strack_vfov,
       ui->spc_cur_strack_sun,        ui->spc_cur_strack_earth,
       ui->spc_cur_strack_moon,       ui->spc_cur_strack_noiseang_1,
       ui->spc_cur_strack_noiseang_2, ui->spc_cur_strack_noiseang_3,
       ui->spc_cur_gps_samptime,      ui->spc_cur_gps_posnoise,
       ui->spc_cur_gps_velnoise,      ui->spc_cur_gps_timenoise,
       ui->spc_cur_acc_samptime,      ui->spc_cur_acc_maxacc,
       ui->spc_cur_acc_scaleerror,    ui->spc_cur_acc_quant,
       ui->spc_cur_acc_dvrandwalk,    ui->spc_cur_acc_bias_stab,
       ui->spc_cur_acc_bias_tspan,    ui->spc_cur_acc_dvnoise};

   // All validators with [-1, 1]
   QList<QLineEdit *> none_one = {
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
       ui->spc_cur_acc_axis_3};

   dsm_gui_lib::set_mult_validators(ninf_pinf, ninf_pinf_valid.get());
   dsm_gui_lib::set_mult_validators(zero_pinf, zero_pinf_valid.get());
   dsm_gui_lib::set_mult_validators(none_one, none_one_valid.get());

   // Name Validators
   QList<QLineEdit *> item_names = {
       ui->spc_cur_body_name,     ui->spc_cur_joint_name,
       ui->spc_cur_wheel_name,    ui->spc_cur_mtb_name,
       ui->spc_cur_thruster_name, ui->spc_cur_gyro_name,
       ui->spc_cur_mag_name,      ui->spc_cur_css_name,
       ui->spc_cur_fss_name,      ui->spc_cur_strack_name,
       ui->spc_cur_gps_name,      ui->spc_cur_acc_name};

   dsm_gui_lib::set_mult_validators(item_names, no_quotes_valid.get());

   ui->spc_cur_body_geom->setValidator(no_quotes_spaces_valid.get());
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
      YAML::Node node_seq = *it;
      YAML::Node cur_node = node_seq["Body"];
      cur_node.SetStyle(YAML::EmitterStyle::Block);

      QString item_name;

      if (cur_node["Name"])
         item_name = cur_node["Name"].as<QString>();
      else
         item_name = "Body_" + QString::number(index);
      ui->spc_cur_body_list->addItem(item_name);

      tmp_data.append(cur_node["Mass"].as<QString>());
      data_vector = cur_node["MOI"].as<QVector<QString>>();
      for (int i = 0; i < 3; i++)
         tmp_data.append(data_vector[i]);

      data_vector = cur_node["POI"].as<QVector<QString>>();
      for (int i = 0; i < 3; i++)
         tmp_data.append(data_vector[i]);

      data_vector = cur_node["Pos of CM"].as<QVector<QString>>();
      for (int i = 0; i < 3; i++)
         tmp_data.append(data_vector[i]);

      data_vector = cur_node["Constant Momentum"].as<QVector<QString>>();
      for (int i = 0; i < 3; i++)
         tmp_data.append(data_vector[i]);

      data_vector = cur_node["Constant Dipole"].as<QVector<QString>>();
      for (int i = 0; i < 3; i++)
         tmp_data.append(data_vector[i]);

      tmp_data.append(cur_node["Geometry File Name"].as<QString>());
      tmp_data.append(cur_node["Node File Name"].as<QString>());
      tmp_data.append(cur_node["Flex File Name"].as<QString>());

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
      YAML::Node node_seq = *it;
      YAML::Node cur_node = node_seq["Joint"];

      QString item_name;

      if (cur_node["Name"])
         item_name = cur_node["Name"].as<QString>();
      else
         item_name = "Joint_" + QString::number(index);
      ui->spc_cur_joint_list->addItem(item_name);

      tmp_data.append(cur_node["Joint Type"].as<QString>());

      data_vector = cur_node["Body Indicies"].as<QVector<QString>>();
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
          index, ui->spc_cur_joint_list, tmp_data, item_name);
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
      YAML::Node node_seq = *it;
      YAML::Node cur_node = node_seq["Wheel"];

      QString item_name;

      if (cur_node["Name"])
         item_name = cur_node["Name"].as<QString>();
      else
         item_name = "Wheel_" + QString::number(index);
      ui->spc_cur_wheel_list->addItem(item_name);

      tmp_data.append(cur_node["Initial Momentum"].as<QString>());
      data_vector = cur_node["Axis"].as<QVector<QString>>();
      for (int i = 0; i < 3; i++)
         tmp_data.append(data_vector[i]);

      tmp_data.append(cur_node["Max Torque"].as<QString>());
      tmp_data.append(cur_node["Max Momentum"].as<QString>());
      tmp_data.append(cur_node["Rotor Inertia"].as<QString>());
      tmp_data.append(cur_node["Body"]["Index"].as<QString>());
      tmp_data.append(cur_node["Node"].as<QString>());

      tmp_data.append(cur_node["Drag-Jitter File Name"].as<QString>());

      tmp_data = dsm_gui_lib::apply_data_section_end(
          index, ui->spc_cur_wheel_list, tmp_data, item_name);
      index++;
   }

   /* MTBs */
   ui->spc_cur_mtb_list->clear();

   YAML::Node mtb_node = cur_spc_yaml["MTBs"];
   index               = 0;

   mtbs = mtb_node.size();

   for (YAML::const_iterator it = mtb_node.begin(); it != mtb_node.end();
        ++it) {
      YAML::Node node_seq = *it;
      YAML::Node cur_node = node_seq["MTB"];

      QString item_name;

      if (cur_node["Name"])
         item_name = cur_node["Name"].as<QString>();
      else
         item_name = "MTB_" + QString::number(index);
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
      YAML::Node node_seq = *it;
      YAML::Node cur_node = node_seq["Thruster"];

      QString item_name;

      if (cur_node["Name"])
         item_name = cur_node["Name"].as<QString>();
      else
         item_name = "Thr_" + QString::number(index);
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
      YAML::Node node_seq = *it;
      YAML::Node cur_node = node_seq["Gyro"];

      QString item_name;
      if (cur_node["Name"])
         item_name = cur_node["Name"].as<QString>();
      else
         item_name = "Gyro_" + QString::number(index);
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
      YAML::Node node_seq = *it;
      YAML::Node cur_node = node_seq["Magnetometer"];

      QString item_name;

      if (cur_node["Name"])
         item_name = cur_node["Name"].as<QString>();
      else
         item_name = "Mag_" + QString::number(index);
      ui->spc_cur_mag_list->addItem(item_name);

      tmp_data.append(cur_node["Sample Time"].as<QString>());
      data_vector = cur_node["Axis"].as<QVector<QString>>();
      for (int i = 0; i < 3; i++)
         tmp_data.append(data_vector[i]);

      tmp_data.append(cur_node["Saturation"].as<QString>());
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
      YAML::Node node_seq = *it;
      YAML::Node cur_node = node_seq["CSS"];

      QString item_name;

      if (cur_node["Name"])
         item_name = cur_node["Name"].as<QString>();
      else
         item_name = "CSS_" + QString::number(index);
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
      YAML::Node node_seq = *it;
      YAML::Node cur_node = node_seq["FSS"];

      QString item_name;

      if (cur_node["Name"])
         item_name = cur_node["Name"].as<QString>();
      else
         item_name = "FSS_" + QString::number(index);
      ui->spc_cur_fss_list->addItem(item_name);

      tmp_data.append(cur_node["Sample Time"].as<QString>());

      data_vector =
          cur_node["Mounting Angles"]["Angles"].as<QVector<QString>>();
      for (int i = 0; i < 3; i++)
         tmp_data.append(data_vector[i]);

      tmp_data.append(cur_node["Mounting Angles"]["Sequence"].as<QString>());
      tmp_data.append(cur_node["Boresight Axis"].as<QString>());
      data_vector = cur_node["FOV Size"].as<QVector<QString>>();
      for (int i = 0; i < 2; i++)
         tmp_data.append(data_vector[i]);

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
      YAML::Node node_seq = *it;
      YAML::Node cur_node = node_seq["ST"];

      QString item_name;

      if (cur_node["Name"])
         item_name = cur_node["Name"].as<QString>();
      else
         item_name = "ST_" + QString::number(index);
      ui->spc_cur_strack_list->addItem(item_name);

      tmp_data.append(cur_node["Sample Time"].as<QString>());

      data_vector =
          cur_node["Mounting Angles"]["Angles"].as<QVector<QString>>();
      for (int i = 0; i < 3; i++)
         tmp_data.append(data_vector[i]);

      tmp_data.append(cur_node["Mounting Angles"]["Sequence"].as<QString>());
      tmp_data.append(cur_node["Boresight Axis"].as<QString>());
      data_vector = cur_node["FOV Size"].as<QVector<QString>>();
      for (int i = 0; i < 2; i++)
         tmp_data.append(data_vector[i]);
      tmp_data.append(cur_node["Exclusion Angles"]["Sun"].as<QString>());
      tmp_data.append(cur_node["Exclusion Angles"]["Earth"].as<QString>());
      tmp_data.append(cur_node["Exclusion Angles"]["Luna"].as<QString>());
      data_vector = cur_node["Noise Equivalent Angle"].as<QVector<QString>>();
      for (int i = 0; i < 3; i++)
         tmp_data.append(data_vector[i]);

      tmp_data.append(cur_node["Node"].as<QString>());

      tmp_data = dsm_gui_lib::apply_data_section_end(
          index, ui->spc_cur_strack_list, tmp_data, item_name);
      index++;
   }

   /* GPSs */
   ui->spc_cur_gps_list->clear();

   YAML::Node gps_node = cur_spc_yaml["GPSs"];
   index               = 0;

   gps_s = gps_node.size();

   for (YAML::const_iterator it = gps_node.begin(); it != gps_node.end();
        ++it) {
      YAML::Node node_seq = *it;
      YAML::Node cur_node = node_seq["GPS"];

      QString item_name;

      if (cur_node["Name"])
         item_name = cur_node["Name"].as<QString>();
      else
         item_name = "GPS_" + QString::number(index);
      ui->spc_cur_gps_list->addItem(item_name);

      tmp_data.append(cur_node["Sample Time"].as<QString>());
      tmp_data.append(cur_node["Position Noise"].as<QString>());
      tmp_data.append(cur_node["Velocity Noise"].as<QString>());
      tmp_data.append(cur_node["Time Noise"].as<QString>());
      tmp_data.append(cur_node["Node"].as<QString>());

      tmp_data = dsm_gui_lib::apply_data_section_end(
          index, ui->spc_cur_gps_list, tmp_data, item_name);
      index++;
   }

   /* Accelerometers */
   ui->spc_cur_accel_list->clear();

   YAML::Node acc_node = cur_spc_yaml["Accelerometers"];
   index               = 0;

   accels = acc_node.size();

   for (YAML::const_iterator it = acc_node.begin(); it != acc_node.end();
        ++it) {
      YAML::Node node_seq = *it;
      YAML::Node cur_node = node_seq["Accelerometer"];

      QString item_name;

      if (cur_node["Name"])
         item_name = cur_node["Name"].as<QString>();
      else
         item_name = "Acc_" + QString::number(index);
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
          index, ui->spc_cur_accel_list, tmp_data, item_name);
      index++;
   }
}

void SPC_submenu::apply_data() {
   QFile file(file_path);
   if (!file.open(QIODevice::ReadOnly)) {
      QMessageBox::information(0, "error", file.errorString());
   }

   file.close();

   QVector<QString> data_vector = {};
   QStringList tmp_data;
   long index2, index, tmp_size;

   if (ui->sections->currentIndex() == 0) {
      /* Dynamics Flags */
      cur_spc_yaml["Dynamics Flags"]["Method"] =
          ui->spc_cur_solver->currentText();
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
   } else if (ui->sections->currentIndex() == 1) {
      /* Bodies */
      index2   = ui->spc_cur_body_list->currentRow();
      tmp_size = cur_spc_yaml["Bodies"].size();

      for (index = 0; index < bodies; index++) {
         YAML::Node cur_body_node = cur_spc_yaml["Bodies"][index]["Body"];

         QVector<int> wheel_body_inds    = {};
         QVector<int> thruster_body_inds = {};
         QVector<int> css_body_inds      = {};

         // Find components that referenced the previous definition of this body
         if (wheels > 0) {
            for (int i = 0; i < wheels; i++) {
               YAML::Node cur_tmp_node =
                   cur_spc_yaml["Wheels"][i]["Wheel"]["Body"];
               if (cur_tmp_node == cur_body_node) {
                  wheel_body_inds.append(i);
               }
            }
         }
         if (thrusters > 0) {
            for (int i = 0; i < thrusters; i++) {
               YAML::Node cur_tmp_node =
                   cur_spc_yaml["Thrusters"][i]["Thruster"]["Body"];
               if (cur_tmp_node == cur_body_node) {
                  thruster_body_inds.append(i);
               }
            }
         }
         if (css_s > 0) {
            for (int i = 0; i < css_s; i++) {
               YAML::Node cur_tmp_node = cur_spc_yaml["CSSs"][i]["CSS"]["Body"];
               if (cur_tmp_node == cur_body_node) {
                  css_body_inds.append(i);
               }
            }
         }

         YAML::Node top_node;
         YAML::Node cur_node;

         ui->spc_cur_body_list->setCurrentRow(index);
         on_spc_cur_body_list_itemClicked(ui->spc_cur_body_list->currentItem());

         cur_node["Name"]  = ui->spc_cur_body_name->text();
         cur_node["Index"] = index;
         cur_node["Mass"]  = ui->spc_cur_body_mass->text();

         cur_node["MOI"] = dsm_gui_lib::create_QVec3(
             ui->spc_cur_body_pmoi_x->text(), ui->spc_cur_body_pmoi_y->text(),
             ui->spc_cur_body_pmoi_z->text());

         cur_node["POI"] = dsm_gui_lib::create_QVec3(
             ui->spc_cur_body_poi_x->text(), ui->spc_cur_body_poi_y->text(),
             ui->spc_cur_body_poi_z->text());

         cur_node["Pos of CM"] = dsm_gui_lib::create_QVec3(
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

         top_node["Body"]              = cur_node;
         cur_spc_yaml["Bodies"][index] = top_node;

         tmp_data.append(ui->spc_cur_body_mass->text());
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

         on_spc_cur_body_list_itemClicked(ui->spc_cur_body_list->currentItem());

         // Set old body references to new one
         for (int i = 0; i < wheel_body_inds.length(); i++) {
            cur_spc_yaml["Wheels"][wheel_body_inds[i]]["Wheel"]["Body"] =
                cur_spc_yaml["Bodies"][index]["Body"];
         }
         for (int i = 0; i < thruster_body_inds.length(); i++) {
            cur_spc_yaml["Thrusters"][thruster_body_inds[i]]["Thruster"]
                        ["Body"] = cur_spc_yaml["Bodies"][index]["Body"];
         }
         for (int i = 0; i < css_body_inds.length(); i++) {
            cur_spc_yaml["CSSs"][css_body_inds[i]]["CSS"]["Body"] =
                cur_spc_yaml["Bodies"][index]["Body"];
         }
      }

      for (index = bodies; index < tmp_size; index++) {
         cur_spc_yaml["Bodies"].remove(index);
      }

      ui->spc_cur_body_list->setCurrentRow(index2);
      on_spc_cur_body_list_itemClicked(ui->spc_cur_body_list->item(index2));

      /* Joints */
      index2   = ui->spc_cur_joint_list->currentRow();
      tmp_size = cur_spc_yaml["Joints"].size();
      cur_spc_yaml["Joints"].SetStyle(YAML::EmitterStyle::Block);

      for (index = 0; index < joints; index++) {

         YAML::Node top_node2;
         YAML::Node cur_node2;

         ui->spc_cur_joint_list->setCurrentRow(index);
         on_spc_cur_joint_list_itemClicked(ui->spc_cur_joint_list->item(index));

         cur_node2["Name"]  = ui->spc_cur_joint_name->text();
         cur_node2["Index"] = index;

         cur_node2["Body Indicies"] = dsm_gui_lib::create_QVec2(
             ui->spc_cur_joint_in->text(), ui->spc_cur_joint_out->text());
         ;

         cur_node2["Joint Type"] = ui->spc_cur_joint_type->currentText();
         cur_node2["Rot DOF"]    = ui->spc_cur_joint_rotdof->text();
         cur_node2["Rot Sequence"] =
             ui->spc_cur_joint_rotdof_seq->currentText();
         cur_node2["Rot Type"] = ui->spc_cur_joint_rottype->currentText();

         cur_node2["Trn DOF"] = ui->spc_cur_joint_trndof->text();
         cur_node2["Trn Sequence"] =
             ui->spc_cur_joint_trndof_seq->currentText();

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

         cur_node2["Rot DOF Locked"] =
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

         cur_node2["Trn DOF Locked"] =
             dsm_gui_lib::create_QVec3(data1, data2, data3);

         cur_node2["Init Angles"] = dsm_gui_lib::create_QVec3(
             ui->spc_cur_joint_ang0_1->text(), ui->spc_cur_joint_ang0_2->text(),
             ui->spc_cur_joint_ang0_3->text());

         cur_node2["Init Angle Rates"] =
             dsm_gui_lib::create_QVec3(ui->spc_cur_joint_angrate0_1->text(),
                                       ui->spc_cur_joint_angrate0_2->text(),
                                       ui->spc_cur_joint_angrate0_3->text());

         cur_node2["Init Displacement"] =
             dsm_gui_lib::create_QVec3(ui->spc_cur_joint_disp0_1->text(),
                                       ui->spc_cur_joint_disp0_2->text(),
                                       ui->spc_cur_joint_disp0_3->text());

         cur_node2["Init Displacement Rates"] =
             dsm_gui_lib::create_QVec3(ui->spc_cur_joint_dispr0_1->text(),
                                       ui->spc_cur_joint_dispr0_2->text(),
                                       ui->spc_cur_joint_dispr0_3->text());

         cur_node2["Bi-Gi Angles"]["Angles"] = dsm_gui_lib::create_QVec3(
             ui->spc_cur_joint_bigi_1->text(), ui->spc_cur_joint_bigi_2->text(),
             ui->spc_cur_joint_bigi_3->text());

         cur_node2["Bi-Gi Angles"]["Sequence"] =
             ui->spc_cur_joint_bigi_seq->currentText();

         cur_node2["Bo-Go Angles"]["Angles"] = dsm_gui_lib::create_QVec3(
             ui->spc_cur_joint_bogo_1->text(), ui->spc_cur_joint_bogo_2->text(),
             ui->spc_cur_joint_bogo_3->text());
         cur_node2["Bo-Go Angles"]["Sequence"] =
             ui->spc_cur_joint_bogo_seq->currentText();

         cur_node2["Pos wrt Inner Body"] =
             dsm_gui_lib::create_QVec3(ui->spc_cur_joint_poswrt_in_1->text(),
                                       ui->spc_cur_joint_poswrt_in_2->text(),
                                       ui->spc_cur_joint_poswrt_in_3->text());

         cur_node2["Pos wrt Outer Body"] =
             dsm_gui_lib::create_QVec3(ui->spc_cur_joint_poswrt_out_1->text(),
                                       ui->spc_cur_joint_poswrt_out_2->text(),
                                       ui->spc_cur_joint_poswrt_out_3->text());

         cur_node2["Parm File Name"] = ui->spc_cur_joint_param_file->text();

         top_node2["Joint"]            = cur_node2;
         cur_spc_yaml["Joints"][index] = top_node2;

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
            tmp_data.append("false");

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
            tmp_data.append("false");

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

      for (index = joints; index < tmp_size; index++) {
         cur_spc_yaml["Joints"].remove(index);
      }

      if (joints > 0)
         on_spc_cur_joint_list_itemClicked(
             ui->spc_cur_joint_list->item(index2));
   } else if (ui->sections->currentIndex() == 2 &&
              ui->actuator_sections->currentIndex() == 0) {
      /* Wheels */
      QMap<QString, QString> global_wheel_params = {{"Drag", wheel_drag},
                                                    {"Jitter", wheel_jitter}};
      cur_spc_yaml["Wheel Params"]               = global_wheel_params;

      index2   = ui->spc_cur_wheel_list->currentRow();
      tmp_size = cur_spc_yaml["Wheels"].size();
      cur_spc_yaml["Wheels"].SetStyle(YAML::EmitterStyle::Block);

      for (index = 0; index < wheels; index++) {
         YAML::Node top_node;
         YAML::Node cur_node;

         ui->spc_cur_wheel_list->setCurrentRow(index);
         on_spc_cur_wheel_list_itemClicked(ui->spc_cur_wheel_list->item(index));

         cur_node["Name"]  = ui->spc_cur_wheel_name->text();
         cur_node["Index"] = index;

         cur_node["Initial Momentum"] = ui->spc_cur_wheel_initmom->text();

         cur_node["Axis"] = dsm_gui_lib::create_QVec3(
             ui->spc_cur_wheel_axis_1->text(), ui->spc_cur_wheel_axis_2->text(),
             ui->spc_cur_wheel_axis_3->text());
         ;

         cur_node["Max Torque"]   = ui->spc_cur_wheel_maxtrq->text();
         cur_node["Max Momentum"] = ui->spc_cur_wheel_maxmom->text();

         cur_node["Rotor Inertia"] = ui->spc_cur_wheel_inertia->text();
         cur_node["Body"] =
             cur_spc_yaml["Bodies"][ui->spc_cur_wheel_body->cleanText().toInt()]
                         ["Body"];

         cur_node["Node"] = ui->spc_cur_wheel_node->cleanText();
         cur_node["Drag-Jitter File Name"] =
             ui->spc_cur_wheel_drjit_file->text();

         top_node["Wheel"]             = cur_node;
         cur_spc_yaml["Wheels"][index] = top_node;

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
      for (index = wheels; index < tmp_size; index++) {
         cur_spc_yaml["Wheels"].remove(index);
      }
      if (wheels > 0)
         on_spc_cur_wheel_list_itemClicked(
             ui->spc_cur_wheel_list->item(index2));

   } else if (ui->sections->currentIndex() == 2 &&
              ui->actuator_sections->currentIndex() == 1) {
      /* MTBs */

      index2   = ui->spc_cur_mtb_list->currentRow();
      tmp_size = cur_spc_yaml["MTBs"].size();
      cur_spc_yaml["MTBs"].SetStyle(YAML::EmitterStyle::Block);

      for (index = 0; index < mtbs; index++) {
         YAML::Node top_node;
         YAML::Node cur_node;

         ui->spc_cur_mtb_list->setCurrentRow(index);
         on_spc_cur_mtb_list_itemClicked(ui->spc_cur_mtb_list->item(index));

         cur_node["Name"]  = ui->spc_cur_mtb_name->text();
         cur_node["Index"] = index;

         cur_node["Saturation"] = ui->spc_cur_mtb_sat->text();

         cur_node["Axis"] = dsm_gui_lib::create_QVec3(
             ui->spc_cur_mtb_axis_1->text(), ui->spc_cur_mtb_axis_2->text(),
             ui->spc_cur_mtb_axis_3->text());
         cur_node["Node"] = ui->spc_cur_mtb_node->cleanText();

         top_node["MTB"]             = cur_node;
         cur_spc_yaml["MTBs"][index] = top_node;

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
      for (index = mtbs; index < tmp_size; index++) {
         cur_spc_yaml["MTBs"].remove(index);
      }
      if (mtbs > 0)
         on_spc_cur_mtb_list_itemClicked(ui->spc_cur_mtb_list->item(index2));

   } else if (ui->sections->currentIndex() == 2 &&
              ui->actuator_sections->currentIndex() == 2) {
      /* Thrusters */
      index2   = ui->spc_cur_thruster_list->currentRow();
      tmp_size = cur_spc_yaml["Thrusters"].size();
      cur_spc_yaml["Thrusters"].SetStyle(YAML::EmitterStyle::Block);

      for (index = 0; index < thrusters; index++) {
         YAML::Node top_node;
         YAML::Node cur_node;

         ui->spc_cur_thruster_list->setCurrentRow(index);
         on_spc_cur_thruster_list_itemClicked(
             ui->spc_cur_thruster_list->item(index));

         cur_node["Name"]  = ui->spc_cur_thruster_name->text();
         cur_node["Index"] = index;

         cur_node["Mode"]  = ui->spc_cur_thruster_mode->currentText();
         cur_node["Force"] = ui->spc_cur_thruster_force->text();

         cur_node["Axis"] =
             dsm_gui_lib::create_QVec3(ui->spc_cur_thruster_axis_1->text(),
                                       ui->spc_cur_thruster_axis_2->text(),
                                       ui->spc_cur_thruster_axis_3->text());
         cur_node["Body"] =
             cur_spc_yaml["Bodies"][ui->spc_cur_thruster_body->cleanText()
                                        .toInt()]["Body"];
         cur_node["Node"] = ui->spc_cur_thruster_node->cleanText();

         top_node["Thruster"]             = cur_node;
         cur_spc_yaml["Thrusters"][index] = top_node;

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
      for (index = thrusters; index < tmp_size; index++) {
         cur_spc_yaml["Thrusters"].remove(index);
      }
      if (thrusters > 0)
         on_spc_cur_thruster_list_itemClicked(
             ui->spc_cur_thruster_list->item(index2));

   } else if (ui->sections->currentIndex() == 3 &&
              ui->sensor_sections->currentIndex() == 0) {
      /* Gyros */
      index2   = ui->spc_cur_gyro_list->currentRow();
      tmp_size = cur_spc_yaml["Gyros"].size();
      cur_spc_yaml["Gyros"].SetStyle(YAML::EmitterStyle::Block);

      for (index = 0; index < gyros; index++) {
         YAML::Node top_node;
         YAML::Node cur_node;

         ui->spc_cur_gyro_list->setCurrentRow(index);
         on_spc_cur_gyro_list_itemClicked(ui->spc_cur_gyro_list->item(index));

         cur_node["Name"]  = ui->spc_cur_gyro_name->text();
         cur_node["Index"] = index;

         cur_node["Sample Time"] = ui->spc_cur_gyro_samptime->text();

         cur_node["Axis"] = dsm_gui_lib::create_QVec3(
             ui->spc_cur_gyro_axis_1->text(), ui->spc_cur_gyro_axis_2->text(),
             ui->spc_cur_gyro_axis_3->text());

         cur_node["Max Rate"]          = ui->spc_cur_gyro_maxrate->text();
         cur_node["Scale Factor"]      = ui->spc_cur_gyro_scaleferror->text();
         cur_node["Quantization"]      = ui->spc_cur_gyro_quant->text();
         cur_node["Angle Random Walk"] = ui->spc_cur_gyro_angrwalk->text();
         cur_node["Bias Stability"]    = ui->spc_cur_gyro_bias_stab->text();
         cur_node["Bias Stability Timespan"] =
             ui->spc_cur_gyro_bias_tspan->text();
         cur_node["Angle Noise"]  = ui->spc_cur_gyro_angnoise->text();
         cur_node["Initial Bias"] = ui->spc_cur_gyro_initbias->text();

         cur_node["Node"] = ui->spc_cur_gyro_node->cleanText();

         top_node["Gyro"]             = cur_node;
         cur_spc_yaml["Gyros"][index] = top_node;

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

      for (index = gyros; index < tmp_size; index++) {
         cur_spc_yaml["Gyros"].remove(index);
      }
      if (gyros > 0)
         on_spc_cur_gyro_list_itemClicked(ui->spc_cur_gyro_list->item(index2));

   } else if (ui->sections->currentIndex() == 3 &&
              ui->sensor_sections->currentIndex() == 1) {
      /* Magnetometers */
      index2   = ui->spc_cur_mag_list->currentRow();
      tmp_size = cur_spc_yaml["Magnetometers"].size();
      cur_spc_yaml["Magnetometers"].SetStyle(YAML::EmitterStyle::Block);

      for (index = 0; index < mags; index++) {
         YAML::Node top_node;
         YAML::Node cur_node;

         ui->spc_cur_mag_list->setCurrentRow(index);
         on_spc_cur_mag_list_itemClicked(ui->spc_cur_mag_list->item(index));

         cur_node["Name"]  = ui->spc_cur_mag_name->text();
         cur_node["Index"] = index;

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

         top_node["Magnetometer"]             = cur_node;
         cur_spc_yaml["Magnetometers"][index] = top_node;

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
      for (index = mags; index < tmp_size; index++) {
         cur_spc_yaml["Magnetometers"].remove(index);
      }
      if (mags > 0)
         on_spc_cur_mag_list_itemClicked(ui->spc_cur_mag_list->item(index2));
   } else if (ui->sections->currentIndex() == 3 &&
              ui->sensor_sections->currentIndex() == 2) {
      /* CSSs */
      index2   = ui->spc_cur_css_list->currentRow();
      tmp_size = cur_spc_yaml["CSSs"].size();
      cur_spc_yaml["CSSs"].SetStyle(YAML::EmitterStyle::Block);

      for (index = 0; index < css_s; index++) {
         YAML::Node top_node;
         YAML::Node cur_node;

         ui->spc_cur_css_list->setCurrentRow(index);
         on_spc_cur_css_list_itemClicked(ui->spc_cur_css_list->item(index));

         cur_node["Name"]  = ui->spc_cur_css_name->text();
         cur_node["Index"] = index;

         cur_node["Sample Time"] = ui->spc_cur_css_samptime->text();

         cur_node["Axis"] = dsm_gui_lib::create_QVec3(
             ui->spc_cur_css_axis_1->text(), ui->spc_cur_css_axis_2->text(),
             ui->spc_cur_css_axis_3->text());
         ;
         cur_node["Half Cone Angle"] = ui->spc_cur_css_halfcone->text();
         cur_node["Scale Factor"]    = ui->spc_cur_css_scale->text();
         cur_node["Quantization"]    = ui->spc_cur_css_quant->text();
         cur_node["Body"] =
             cur_spc_yaml["Bodies"][ui->spc_cur_css_body->text().toInt()]
                         ["Body"];

         cur_node["Node"] = ui->spc_cur_css_node->cleanText();

         top_node["CSS"]             = cur_node;
         cur_spc_yaml["CSSs"][index] = top_node;

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
      for (index = css_s; index < tmp_size; index++) {
         cur_spc_yaml["CSSs"].remove(index);
      }
      if (css_s > 0)
         on_spc_cur_css_list_itemClicked(ui->spc_cur_css_list->item(index2));

   } else if (ui->sections->currentIndex() == 3 &&
              ui->sensor_sections->currentIndex() == 3) {
      /* FSSs */
      index2   = ui->spc_cur_fss_list->currentRow();
      tmp_size = cur_spc_yaml["FSSs"].size();
      cur_spc_yaml["FSSs"].SetStyle(YAML::EmitterStyle::Block);

      for (index = 0; index < fss_s; index++) {
         YAML::Node top_node;
         YAML::Node cur_node;

         ui->spc_cur_fss_list->setCurrentRow(index);
         on_spc_cur_fss_list_itemClicked(ui->spc_cur_fss_list->item(index));

         cur_node["Name"]  = ui->spc_cur_fss_name->text();
         cur_node["Index"] = index;

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

         top_node["FSS"]             = cur_node;
         cur_spc_yaml["FSSs"][index] = top_node;

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
      for (index = fss_s; index < tmp_size; index++) {
         cur_spc_yaml["FSSs"].remove(index);
      }
      if (fss_s > 0)
         on_spc_cur_fss_list_itemClicked(ui->spc_cur_fss_list->item(index2));

   } else if (ui->sections->currentIndex() == 3 &&
              ui->sensor_sections->currentIndex() == 4) {
      /* STs */
      index2   = ui->spc_cur_strack_list->currentRow();
      tmp_size = cur_spc_yaml["STs"].size();
      cur_spc_yaml["STs"].SetStyle(YAML::EmitterStyle::Block);

      for (index = 0; index < stracks; index++) {
         YAML::Node top_node;
         YAML::Node cur_node;

         ui->spc_cur_strack_list->setCurrentRow(index);
         on_spc_cur_strack_list_itemClicked(
             ui->spc_cur_strack_list->item(index));

         cur_node["Name"]  = ui->spc_cur_strack_name->text();
         cur_node["Index"] = index;

         cur_node["Sample Time"] = ui->spc_cur_strack_samptime->text();

         cur_node["Mounting Angles"]["Angles"] =
             dsm_gui_lib::create_QVec3(ui->spc_cur_strack_mount_1->text(),
                                       ui->spc_cur_strack_mount_2->text(),
                                       ui->spc_cur_strack_mount_3->text());
         ;
         cur_node["Mounting Angles"]["Sequence"] =
             ui->spc_cur_strack_mountseq->currentText();
         cur_node["Boresight Axis"] =
             ui->spc_cur_strack_boreaxis->currentText();

         data_vector = {ui->spc_cur_strack_hfov->text(),
                        ui->spc_cur_strack_vfov->text()};

         cur_node["FOV Size"] = dsm_gui_lib::create_QVec2(
             ui->spc_cur_strack_hfov->text(), ui->spc_cur_strack_vfov->text());
         ;
         cur_node["Exclusion Angles"]["Sun"] = ui->spc_cur_strack_sun->text();
         cur_node["Exclusion Angles"]["Earth"] =
             ui->spc_cur_strack_earth->text();
         cur_node["Exclusion Angles"]["Luna"] = ui->spc_cur_strack_moon->text();

         data_vector = {ui->spc_cur_strack_noiseang_1->text(),
                        ui->spc_cur_strack_noiseang_2->text(),
                        ui->spc_cur_strack_noiseang_3->text()};

         cur_node["Noise Equivalent Angle"] =
             dsm_gui_lib::create_QVec3(ui->spc_cur_strack_noiseang_1->text(),
                                       ui->spc_cur_strack_noiseang_2->text(),
                                       ui->spc_cur_strack_noiseang_3->text());
         cur_node["Node"] = ui->spc_cur_strack_node->cleanText();

         top_node["ST"]             = cur_node;
         cur_spc_yaml["STs"][index] = top_node;

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
      for (index = stracks; index < tmp_size; index++) {
         cur_spc_yaml["STs"].remove(index);
      }
      if (stracks > 0)
         on_spc_cur_strack_list_itemClicked(
             ui->spc_cur_strack_list->item(index2));

   } else if (ui->sections->currentIndex() == 3 &&
              ui->sensor_sections->currentIndex() == 5) {
      /* GPSs */
      index2   = ui->spc_cur_gps_list->currentRow();
      tmp_size = cur_spc_yaml["GPSs"].size();
      cur_spc_yaml["GPSs"].SetStyle(YAML::EmitterStyle::Block);

      for (index = 0; index < gps_s; index++) {
         YAML::Node top_node;
         YAML::Node cur_node;

         ui->spc_cur_gps_list->setCurrentRow(index);
         on_spc_cur_gps_list_itemClicked(ui->spc_cur_gps_list->item(index2));

         cur_node["Name"]  = ui->spc_cur_gps_name->text();
         cur_node["Index"] = index;

         cur_node["Sample Time"]    = ui->spc_cur_gps_samptime->text();
         cur_node["Position Noise"] = ui->spc_cur_gps_posnoise->text();
         cur_node["Velocity Noise"] = ui->spc_cur_gps_velnoise->text();
         cur_node["Time Noise"]     = ui->spc_cur_gps_timenoise->text();
         cur_node["Node"]           = ui->spc_cur_gps_node->cleanText();

         top_node["GPS"]             = cur_node;
         cur_spc_yaml["GPSs"][index] = top_node;

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
      for (index = gps_s; index < tmp_size; index++) {
         cur_spc_yaml["GPSs"].remove(index);
      }
      if (gps_s > 0)
         on_spc_cur_gps_list_itemClicked(ui->spc_cur_gps_list->item(index2));

   } else if (ui->sections->currentIndex() == 3 &&
              ui->sensor_sections->currentIndex() == 6) {
      /* Accelerometers */
      index2   = ui->spc_cur_accel_list->currentRow();
      tmp_size = cur_spc_yaml["Accelerometers"].size();
      cur_spc_yaml["Accelerometers"].SetStyle(YAML::EmitterStyle::Block);

      for (index = 0; index < accels; index++) {
         YAML::Node top_node;
         YAML::Node cur_node;

         ui->spc_cur_accel_list->setCurrentRow(index);
         on_spc_cur_accel_list_itemClicked(ui->spc_cur_accel_list->item(index));

         cur_node["Name"]  = ui->spc_cur_acc_name->text();
         cur_node["Index"] = index;

         cur_node["Sample Time"] = ui->spc_cur_acc_samptime->text();

         cur_node["Axis"] = dsm_gui_lib::create_QVec3(
             ui->spc_cur_acc_axis_1->text(), ui->spc_cur_acc_axis_2->text(),
             ui->spc_cur_acc_axis_3->text());

         cur_node["Max Acceleration"] = ui->spc_cur_acc_maxacc->text();
         cur_node["Scale Factor"]     = ui->spc_cur_acc_scaleerror->text();
         cur_node["Quantization"]     = ui->spc_cur_acc_quant->text();
         cur_node["DV Random Walk"]   = ui->spc_cur_acc_dvrandwalk->text();
         cur_node["Bias Stability"]   = ui->spc_cur_acc_bias_stab->text();
         cur_node["Bias Stability Timespan"] =
             ui->spc_cur_acc_bias_tspan->text();
         cur_node["DV Noise"]     = ui->spc_cur_acc_dvnoise->text();
         cur_node["Initial Bias"] = ui->spc_cur_acc_initbias->text();
         cur_node["Node"]         = ui->spc_cur_acc_node->cleanText();

         top_node["Accelerometer"]             = cur_node;
         cur_spc_yaml["Accelerometers"][index] = top_node;

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
      for (index = accels; index < tmp_size; index++) {
         cur_spc_yaml["Accelerometers"].remove(index);
      }
      if (accels > 0)
         on_spc_cur_accel_list_itemClicked(
             ui->spc_cur_accel_list->item(index2));
   }
   write_data(cur_spc_yaml);
}

void SPC_submenu::write_data(YAML::Node inp_spc) {
   QFile::remove(file_path);
   QFile file(file_path);
   if (!file.open(QFile::WriteOnly)) {
      QMessageBox::information(0, "error", file.errorString());
   } else {
      QString in_pc;
      QStringList in_pc_lines;
      QTextStream in(&file);
      YAML::Emitter out;

      QStringList sc_fields = {
          "Configuration", "Orbit",     "Attitude",     "Dynamics Flags",
          "Bodies",        "Joints",    "Wheel Params", "Wheels",
          "MTBs",          "Thrusters", "Gyros",        "Magnetometers",
          "CSSs",          "FSSs",      "STs",          "GPSs",
          "Accelerometers"};

      out.SetIndent(4);
      out.SetMapFormat(YAML::Block);

      out << inp_spc;

      in_pc       = out.c_str();
      in_pc_lines = in_pc.split("\n");
      in << "%YAML 1.2\n---\n";
      for (int i = 0; i < in_pc_lines.size(); i++) {
         in << in_pc_lines[i] + "\n";
      }
   }
   file.close();
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
   apply_data();
   file_path    = spc_cur_file;
   int response = dsm_gui_lib::warning_message("Overwrite Default SC file?");
   if (response == QMessageBox::Ok) {
      QFile::remove(inout_path + "__default__/SC_DEFAULT.yaml");
      QFile::copy(file_path, inout_path + "__default__/SC_DEFAULT.yaml");
      receive_data();
   }
}

void SPC_submenu::on_spc_cur_close_clicked() {
   if (joints == bodies - 1) {
      SPC_submenu::close();
   } else {
      int response = dsm_gui_lib::error_message(
          "The number of joints must equal number of bodies minus one.");
      if (response == QMessageBox::Cancel) {
         return;
      } else if (response == QMessageBox::Ok) {
         return;
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
   long index;

   if (ui->sections->currentIndex() == 0) {
      /* Dynamics Flags */
      cur_spc_yaml["Dynamics Flags"]["Method"] =
          ui->spc_cur_solver->currentText();
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
   } else if (ui->sections->currentIndex() == 1) {
      /* Bodies */
      index = ui->spc_cur_body_list->currentRow();

      YAML::Node cur_body_node = cur_spc_yaml["Bodies"][index]["Body"];

      QVector<int> wheel_body_inds    = {};
      QVector<int> thruster_body_inds = {};
      QVector<int> css_body_inds      = {};

      // Find components that referenced the previous definition of this body
      if (wheels > 0) {
         for (int i = 0; i < wheels; i++) {
            YAML::Node cur_tmp_node =
                cur_spc_yaml["Wheels"][i]["Wheel"]["Body"];
            if (cur_tmp_node == cur_body_node) {
               wheel_body_inds.append(i);
            }
         }
      }
      if (thrusters > 0) {
         for (int i = 0; i < thrusters; i++) {
            YAML::Node cur_tmp_node =
                cur_spc_yaml["Thrusters"][i]["Thruster"]["Body"];
            if (cur_tmp_node == cur_body_node) {
               thruster_body_inds.append(i);
            }
         }
      }
      if (css_s > 0) {
         for (int i = 0; i < css_s; i++) {
            YAML::Node cur_tmp_node = cur_spc_yaml["CSSs"][i]["CSS"]["Body"];
            if (cur_tmp_node == cur_body_node) {
               css_body_inds.append(i);
            }
         }
      }

      YAML::Node top_node;
      YAML::Node cur_node;

      ui->spc_cur_body_list->setCurrentRow(index);

      cur_node["Name"]  = ui->spc_cur_body_name->text();
      cur_node["Index"] = index;
      cur_node["Mass"]  = ui->spc_cur_body_mass->text();

      cur_node["MOI"] = dsm_gui_lib::create_QVec3(
          ui->spc_cur_body_pmoi_x->text(), ui->spc_cur_body_pmoi_y->text(),
          ui->spc_cur_body_pmoi_z->text());

      cur_node["POI"] = dsm_gui_lib::create_QVec3(
          ui->spc_cur_body_poi_x->text(), ui->spc_cur_body_poi_y->text(),
          ui->spc_cur_body_poi_z->text());

      cur_node["Pos of CM"] = dsm_gui_lib::create_QVec3(
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

      top_node["Body"]              = cur_node;
      cur_spc_yaml["Bodies"][index] = top_node;

      tmp_data.append(ui->spc_cur_body_mass->text());
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

      on_spc_cur_body_list_itemClicked(ui->spc_cur_body_list->currentItem());

      // Set old body references to new one
      for (int i = 0; i < wheel_body_inds.length(); i++) {
         cur_spc_yaml["Wheels"][wheel_body_inds[i]]["Wheel"]["Body"] =
             cur_spc_yaml["Bodies"][index]["Body"];
      }
      for (int i = 0; i < thruster_body_inds.length(); i++) {
         cur_spc_yaml["Thrusters"][thruster_body_inds[i]]["Thruster"]["Body"] =
             cur_spc_yaml["Bodies"][index]["Body"];
      }
      for (int i = 0; i < css_body_inds.length(); i++) {
         cur_spc_yaml["CSSs"][css_body_inds[i]]["CSS"]["Body"] =
             cur_spc_yaml["Bodies"][index]["Body"];
      }

      on_spc_cur_body_list_itemClicked(ui->spc_cur_body_list->item(index));

      /* Joints */
      if (joints > 0) {
         index = ui->spc_cur_joint_list->currentRow();

         YAML::Node top_node2;
         YAML::Node cur_node2;

         ui->spc_cur_joint_list->setCurrentRow(index);

         cur_node2["Name"]  = ui->spc_cur_joint_name->text();
         cur_node2["Index"] = index;

         cur_node2["Body Indicies"] = dsm_gui_lib::create_QVec2(
             ui->spc_cur_joint_in->text(), ui->spc_cur_joint_out->text());
         ;

         cur_node2["Joint Type"] = ui->spc_cur_joint_type->currentText();
         cur_node2["Rot DOF"]    = ui->spc_cur_joint_rotdof->text();
         cur_node2["Rot Sequence"] =
             ui->spc_cur_joint_rotdof_seq->currentText();
         cur_node2["Rot Type"] = ui->spc_cur_joint_rottype->currentText();

         cur_node2["Trn DOF"] = ui->spc_cur_joint_trndof->text();
         cur_node2["Trn Sequence"] =
             ui->spc_cur_joint_trndof_seq->currentText();

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

         cur_node2["Rot DOF Locked"] =
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

         cur_node2["Trn DOF Locked"] =
             dsm_gui_lib::create_QVec3(data1, data2, data3);

         cur_node2["Init Angles"] = dsm_gui_lib::create_QVec3(
             ui->spc_cur_joint_ang0_1->text(), ui->spc_cur_joint_ang0_2->text(),
             ui->spc_cur_joint_ang0_3->text());

         cur_node2["Init Angle Rates"] =
             dsm_gui_lib::create_QVec3(ui->spc_cur_joint_angrate0_1->text(),
                                       ui->spc_cur_joint_angrate0_2->text(),
                                       ui->spc_cur_joint_angrate0_3->text());

         cur_node2["Init Displacement"] =
             dsm_gui_lib::create_QVec3(ui->spc_cur_joint_disp0_1->text(),
                                       ui->spc_cur_joint_disp0_2->text(),
                                       ui->spc_cur_joint_disp0_3->text());

         cur_node2["Init Displacement Rates"] =
             dsm_gui_lib::create_QVec3(ui->spc_cur_joint_dispr0_1->text(),
                                       ui->spc_cur_joint_dispr0_2->text(),
                                       ui->spc_cur_joint_dispr0_3->text());

         cur_node2["Bi-Gi Angles"]["Angles"] = dsm_gui_lib::create_QVec3(
             ui->spc_cur_joint_bigi_1->text(), ui->spc_cur_joint_bigi_2->text(),
             ui->spc_cur_joint_bigi_3->text());

         cur_node2["Bi-Gi Angles"]["Sequence"] =
             ui->spc_cur_joint_bigi_seq->currentText();

         cur_node2["Bo-Go Angles"]["Angles"] = dsm_gui_lib::create_QVec3(
             ui->spc_cur_joint_bogo_1->text(), ui->spc_cur_joint_bogo_2->text(),
             ui->spc_cur_joint_bogo_3->text());
         cur_node2["Bo-Go Angles"]["Sequence"] =
             ui->spc_cur_joint_bogo_seq->currentText();

         cur_node2["Pos wrt Inner Body"] =
             dsm_gui_lib::create_QVec3(ui->spc_cur_joint_poswrt_in_1->text(),
                                       ui->spc_cur_joint_poswrt_in_2->text(),
                                       ui->spc_cur_joint_poswrt_in_3->text());

         cur_node2["Pos wrt Outer Body"] =
             dsm_gui_lib::create_QVec3(ui->spc_cur_joint_poswrt_out_1->text(),
                                       ui->spc_cur_joint_poswrt_out_2->text(),
                                       ui->spc_cur_joint_poswrt_out_3->text());

         cur_node2["Parm File Name"] = ui->spc_cur_joint_param_file->text();

         top_node2["Joint"]            = cur_node2;
         cur_spc_yaml["Joints"][index] = top_node2;

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
            tmp_data.append("false");

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
            tmp_data.append("false");

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

         on_spc_cur_joint_list_itemClicked(ui->spc_cur_joint_list->item(index));
      }
   } else if (ui->sections->currentIndex() == 2 &&
              ui->actuator_sections->currentIndex() == 0 && wheels > 0) {
      /* Wheels */
      QMap<QString, QString> global_wheel_params = {{"Drag", wheel_drag},
                                                    {"Jitter", wheel_jitter}};
      cur_spc_yaml["Wheel Params"]               = global_wheel_params;

      index = ui->spc_cur_wheel_list->currentRow();

      YAML::Node top_node;
      YAML::Node cur_node;

      ui->spc_cur_wheel_list->setCurrentRow(index);

      cur_node["Name"]  = ui->spc_cur_wheel_name->text();
      cur_node["Index"] = index;

      cur_node["Initial Momentum"] = ui->spc_cur_wheel_initmom->text();

      cur_node["Axis"] = dsm_gui_lib::create_QVec3(
          ui->spc_cur_wheel_axis_1->text(), ui->spc_cur_wheel_axis_2->text(),
          ui->spc_cur_wheel_axis_3->text());
      ;

      cur_node["Max Torque"]   = ui->spc_cur_wheel_maxtrq->text();
      cur_node["Max Momentum"] = ui->spc_cur_wheel_maxmom->text();

      cur_node["Rotor Inertia"] = ui->spc_cur_wheel_inertia->text();
      cur_node["Body"] =
          cur_spc_yaml["Bodies"][ui->spc_cur_wheel_body->cleanText().toInt()]
                      ["Body"];

      cur_node["Node"]                  = ui->spc_cur_wheel_node->cleanText();
      cur_node["Drag-Jitter File Name"] = ui->spc_cur_wheel_drjit_file->text();

      top_node["Wheel"]             = cur_node;
      cur_spc_yaml["Wheels"][index] = top_node;

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

      on_spc_cur_wheel_list_itemClicked(ui->spc_cur_wheel_list->item(index));

   } else if (ui->sections->currentIndex() == 2 &&
              ui->actuator_sections->currentIndex() == 1 && mtbs > 0) {
      /* MTBs */

      index = ui->spc_cur_mtb_list->currentRow();

      YAML::Node top_node;
      YAML::Node cur_node;

      ui->spc_cur_mtb_list->setCurrentRow(index);

      cur_node["Name"]  = ui->spc_cur_mtb_name->text();
      cur_node["Index"] = index;

      cur_node["Saturation"] = ui->spc_cur_mtb_sat->text();

      cur_node["Axis"] = dsm_gui_lib::create_QVec3(
          ui->spc_cur_mtb_axis_1->text(), ui->spc_cur_mtb_axis_2->text(),
          ui->spc_cur_mtb_axis_3->text());
      cur_node["Node"] = ui->spc_cur_mtb_node->cleanText();

      top_node["MTB"]             = cur_node;
      cur_spc_yaml["MTBs"][index] = top_node;

      tmp_data.append(ui->spc_cur_mtb_sat->text());
      tmp_data.append(ui->spc_cur_mtb_axis_1->text());
      tmp_data.append(ui->spc_cur_mtb_axis_2->text());
      tmp_data.append(ui->spc_cur_mtb_axis_3->text());
      tmp_data.append(ui->spc_cur_mtb_node->text());

      ui->spc_cur_mtb_list->currentItem()->setData(
          256, ui->spc_cur_mtb_name->text());
      ui->spc_cur_mtb_list->currentItem()->setData(257, tmp_data);
      tmp_data.clear();

      on_spc_cur_mtb_list_itemClicked(ui->spc_cur_mtb_list->item(index));

   } else if (ui->sections->currentIndex() == 2 &&
              ui->actuator_sections->currentIndex() == 2 && thrusters > 0) {
      /* Thrusters */
      index = ui->spc_cur_thruster_list->currentRow();

      YAML::Node top_node;
      YAML::Node cur_node;

      ui->spc_cur_thruster_list->setCurrentRow(index);

      cur_node["Name"]  = ui->spc_cur_thruster_name->text();
      cur_node["Index"] = index;

      cur_node["Mode"]  = ui->spc_cur_thruster_mode->currentText();
      cur_node["Force"] = ui->spc_cur_thruster_force->text();

      cur_node["Axis"] =
          dsm_gui_lib::create_QVec3(ui->spc_cur_thruster_axis_1->text(),
                                    ui->spc_cur_thruster_axis_2->text(),
                                    ui->spc_cur_thruster_axis_3->text());
      cur_node["Body"] =
          cur_spc_yaml["Bodies"][ui->spc_cur_thruster_body->cleanText().toInt()]
                      ["Body"];
      cur_node["Node"] = ui->spc_cur_thruster_node->cleanText();

      top_node["Thruster"]             = cur_node;
      cur_spc_yaml["Thrusters"][index] = top_node;

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

      on_spc_cur_thruster_list_itemClicked(
          ui->spc_cur_thruster_list->item(index));

   } else if (ui->sections->currentIndex() == 3 &&
              ui->sensor_sections->currentIndex() == 0 && gyros > 0) {
      /* Gyros */
      index = ui->spc_cur_gyro_list->currentRow();

      YAML::Node top_node;
      YAML::Node cur_node;

      ui->spc_cur_gyro_list->setCurrentRow(index);

      cur_node["Name"]  = ui->spc_cur_gyro_name->text();
      cur_node["Index"] = index;

      cur_node["Sample Time"] = ui->spc_cur_gyro_samptime->text();

      cur_node["Axis"] = dsm_gui_lib::create_QVec3(
          ui->spc_cur_gyro_axis_1->text(), ui->spc_cur_gyro_axis_2->text(),
          ui->spc_cur_gyro_axis_3->text());
      cur_node["Max Rate"]          = ui->spc_cur_gyro_maxrate->text();
      cur_node["Scale Factor"]      = ui->spc_cur_gyro_scaleferror->text();
      cur_node["Quantization"]      = ui->spc_cur_gyro_quant->text();
      cur_node["Angle Random Walk"] = ui->spc_cur_gyro_angrwalk->text();
      cur_node["Bias Stability"]    = ui->spc_cur_gyro_bias_stab->text();
      cur_node["Bias Stability Timespan"] = ui->spc_cur_gyro_bias_tspan->text();
      cur_node["Angle Noise"]             = ui->spc_cur_gyro_angnoise->text();
      cur_node["Initial Bias"]            = ui->spc_cur_gyro_initbias->text();

      cur_node["Node"] = ui->spc_cur_gyro_node->cleanText();

      top_node["Gyro"]             = cur_node;
      cur_spc_yaml["Gyros"][index] = top_node;

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

      on_spc_cur_gyro_list_itemClicked(ui->spc_cur_gyro_list->item(index));

   } else if (ui->sections->currentIndex() == 3 &&
              ui->sensor_sections->currentIndex() == 1 && mags > 0) {
      /* Magnetometers */
      index = ui->spc_cur_mag_list->currentRow();

      YAML::Node top_node;
      YAML::Node cur_node;

      ui->spc_cur_mag_list->setCurrentRow(index);

      cur_node["Name"]  = ui->spc_cur_mag_name->text();
      cur_node["Index"] = index;

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

      top_node["Magnetometer"]             = cur_node;
      cur_spc_yaml["Magnetometers"][index] = top_node;

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

      on_spc_cur_mag_list_itemClicked(ui->spc_cur_mag_list->item(index));
   } else if (ui->sections->currentIndex() == 3 &&
              ui->sensor_sections->currentIndex() == 2) {
      /* CSSs */
      index = ui->spc_cur_css_list->currentRow();

      YAML::Node top_node;
      YAML::Node cur_node;

      ui->spc_cur_css_list->setCurrentRow(index);

      cur_node["Name"]  = ui->spc_cur_css_name->text();
      cur_node["Index"] = index;

      cur_node["Sample Time"] = ui->spc_cur_css_samptime->text();

      cur_node["Axis"] = dsm_gui_lib::create_QVec3(
          ui->spc_cur_css_axis_1->text(), ui->spc_cur_css_axis_2->text(),
          ui->spc_cur_css_axis_3->text());
      ;
      cur_node["Half Cone Angle"] = ui->spc_cur_css_halfcone->text();
      cur_node["Scale Factor"]    = ui->spc_cur_css_scale->text();
      cur_node["Quantization"]    = ui->spc_cur_css_quant->text();
      cur_node["Body"] =
          cur_spc_yaml["Bodies"][ui->spc_cur_css_body->text().toInt()]["Body"];

      cur_node["Node"] = ui->spc_cur_css_node->cleanText();

      top_node["CSS"]             = cur_node;
      cur_spc_yaml["CSSs"][index] = top_node;

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

      on_spc_cur_css_list_itemClicked(ui->spc_cur_css_list->item(index));

   } else if (ui->sections->currentIndex() == 3 &&
              ui->sensor_sections->currentIndex() == 3 && fss_s > 0) {
      /* FSSs */
      index = ui->spc_cur_fss_list->currentRow();

      YAML::Node top_node;
      YAML::Node cur_node;

      ui->spc_cur_fss_list->setCurrentRow(index);

      cur_node["Name"]  = ui->spc_cur_fss_name->text();
      cur_node["Index"] = index;

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

      top_node["FSS"]             = cur_node;
      cur_spc_yaml["FSSs"][index] = top_node;

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

      on_spc_cur_fss_list_itemClicked(ui->spc_cur_fss_list->item(index));

   } else if (ui->sections->currentIndex() == 3 &&
              ui->sensor_sections->currentIndex() == 4 && stracks > 0) {
      /* STs */
      index = ui->spc_cur_strack_list->currentRow();

      YAML::Node top_node;
      YAML::Node cur_node;

      ui->spc_cur_strack_list->setCurrentRow(index);

      cur_node["Name"]  = ui->spc_cur_strack_name->text();
      cur_node["Index"] = index;

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

      top_node["ST"]             = cur_node;
      cur_spc_yaml["STs"][index] = top_node;

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

      on_spc_cur_strack_list_itemClicked(ui->spc_cur_strack_list->item(index));

   } else if (ui->sections->currentIndex() == 3 &&
              ui->sensor_sections->currentIndex() == 5 && gps_s > 0) {
      /* GPSs */
      index = ui->spc_cur_gps_list->currentRow();

      YAML::Node top_node;
      YAML::Node cur_node;

      ui->spc_cur_gps_list->setCurrentRow(index);

      cur_node["Name"]  = ui->spc_cur_gps_name->text();
      cur_node["Index"] = index;

      cur_node["Sample Time"]    = ui->spc_cur_gps_samptime->text();
      cur_node["Position Noise"] = ui->spc_cur_gps_posnoise->text();
      cur_node["Velocity Noise"] = ui->spc_cur_gps_velnoise->text();
      cur_node["Time Noise"]     = ui->spc_cur_gps_timenoise->text();
      cur_node["Node"]           = ui->spc_cur_gps_node->cleanText();

      top_node["GPS"]             = cur_node;
      cur_spc_yaml["GPSs"][index] = top_node;

      tmp_data.append(ui->spc_cur_gps_samptime->text());
      tmp_data.append(ui->spc_cur_gps_posnoise->text());
      tmp_data.append(ui->spc_cur_gps_velnoise->text());
      tmp_data.append(ui->spc_cur_gps_timenoise->text());
      tmp_data.append(ui->spc_cur_gps_node->text());

      ui->spc_cur_gps_list->currentItem()->setData(
          256, ui->spc_cur_gps_name->text());
      ui->spc_cur_gps_list->currentItem()->setData(257, tmp_data);
      tmp_data.clear();

      on_spc_cur_gps_list_itemClicked(ui->spc_cur_gps_list->item(index));

   } else if (ui->sections->currentIndex() == 3 &&
              ui->sensor_sections->currentIndex() == 6 && accels > 0) {
      /* Accelerometers */
      index = ui->spc_cur_accel_list->currentRow();

      YAML::Node top_node;
      YAML::Node cur_node;

      ui->spc_cur_accel_list->setCurrentRow(index);

      cur_node["Name"]  = ui->spc_cur_acc_name->text();
      cur_node["Index"] = index;

      cur_node["Sample Time"] = ui->spc_cur_acc_samptime->text();

      cur_node["Axis"] = dsm_gui_lib::create_QVec3(
          ui->spc_cur_acc_axis_1->text(), ui->spc_cur_acc_axis_2->text(),
          ui->spc_cur_acc_axis_3->text());

      cur_node["Max Acceleration"]        = ui->spc_cur_acc_maxacc->text();
      cur_node["Scale Factor"]            = ui->spc_cur_acc_scaleerror->text();
      cur_node["Quantization"]            = ui->spc_cur_acc_quant->text();
      cur_node["DV Random Walk"]          = ui->spc_cur_acc_dvrandwalk->text();
      cur_node["Bias Stability"]          = ui->spc_cur_acc_bias_stab->text();
      cur_node["Bias Stability Timespan"] = ui->spc_cur_acc_bias_tspan->text();
      cur_node["DV Noise"]                = ui->spc_cur_acc_dvnoise->text();
      cur_node["Initial Bias"]            = ui->spc_cur_acc_initbias->text();
      cur_node["Node"]                    = ui->spc_cur_acc_node->cleanText();

      top_node["Accelerometer"]             = cur_node;
      cur_spc_yaml["Accelerometers"][index] = top_node;

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

      on_spc_cur_accel_list_itemClicked(ui->spc_cur_accel_list->item(index));
   }

   if (joints != bodies - 1)
      dsm_gui_lib::error_message(
          "The number of joints must equal number of bodies minus one.");

   write_data(cur_spc_yaml);
}

void SPC_submenu::setQComboBox(QComboBox *comboBox, QString string) {
   comboBox->setCurrentIndex(comboBox->findText(string));
}

// Body -/+/Duplicate/Item Clicked

void SPC_submenu::on_spc_cur_body_remove_clicked() {
   if (bodies <= 1) {
      dsm_gui_lib::error_message("Spacecraft must have at least one body!");
      return;
   }

   if (bodies > 1) {
      bodies -= 1;
      cur_spc_yaml["Bodies"].remove(ui->spc_cur_body_list->currentRow());
      delete ui->spc_cur_body_list->currentItem();
   }
   apply_data();
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
   apply_data();
}

void SPC_submenu::on_spc_cur_body_duplicate_clicked() {
   if (bodies == 0)
      return;
   bodies += 1;

   proc_duplicates(ui->spc_cur_body_list);
   on_spc_cur_body_list_itemClicked(ui->spc_cur_body_list->currentItem());
   apply_data();
}

void SPC_submenu::on_spc_cur_body_list_itemClicked(QListWidgetItem *item) {
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
   int data_inds[19]        = {0,  1,  2,  3,  4,  5,  6,  7,  8, 9,
                               10, 11, 12, 13, 14, 15, 16, 17, 18};

   item->setText(item->data(256).toString());
   ui->spc_cur_body_name->setText(item->data(256).toString());

   dsm_gui_lib::mult_setText(textboxes, 19, current_data, data_inds);
}

// Joint

void SPC_submenu::on_spc_cur_joint_remove_clicked() {
   if (joints > 0) {
      joints -= 1;
      delete ui->spc_cur_joint_list->currentItem();
   }
   apply_data();
}

void SPC_submenu::on_spc_cur_joint_add_clicked() {
   joints += 1;

   QStringList tmp_data = {
       "PASSIVE", "0",     "1",     "1",     "213",   "GIMBAL", "0",
       "123",     "false", "false", "false", "false", "false",  "false",
       "0.0",     "0.0",   "0.0",   "0.0",   "0.0",   "0.0",    "0.0",
       "0.0",     "0.0",   "0.0",   "0.0",   "0.0",   "0.0",    "0.0",
       "0.0",     "312",   "0.0",   "0.0",   "0.0",   "312",    "0.0",
       "0.0",     "0.0",   "0.0",   "0.0",   "0.0",   "NONE"};

   proc_add(ui->spc_cur_joint_list, tmp_data);
   on_spc_cur_joint_list_itemClicked(ui->spc_cur_joint_list->currentItem());
   apply_data();
}

void SPC_submenu::on_spc_cur_joint_duplicate_clicked() {
   if (joints == 0)
      return;
   joints += 1;

   proc_duplicates(ui->spc_cur_joint_list);
   on_spc_cur_joint_list_itemClicked(ui->spc_cur_joint_list->currentItem());
   apply_data();
}

void SPC_submenu::on_spc_cur_joint_list_itemClicked(QListWidgetItem *item) {
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
   int data_inds[25] = {14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
                        27, 28, 30, 31, 32, 34, 35, 36, 37, 38, 39, 40};
   dsm_gui_lib::mult_setText(textboxes, 25, current_data, data_inds);

   item->setText(item->data(256).toString());
   ui->spc_cur_joint_name->setText(item->data(256).toString());

   setQComboBox(ui->spc_cur_joint_type, current_data[0]);

   ui->spc_cur_joint_in->setValue(current_data[1].toInt());
   ui->spc_cur_joint_out->setValue(current_data[2].toInt());

   ui->spc_cur_joint_rotdof->setValue(current_data[3].toInt());
   setQComboBox(ui->spc_cur_joint_rotdof_seq, current_data[4]);
   setQComboBox(ui->spc_cur_joint_rottype, current_data[5]);

   ui->spc_cur_joint_trndof->setValue(current_data[6].toInt());
   setQComboBox(ui->spc_cur_joint_trndof_seq, current_data[7]);

   if (!QString::compare(current_data[8], "true"))
      ui->spc_cur_joint_rlock1->setCheckState(Qt::Checked);
   else
      ui->spc_cur_joint_rlock1->setCheckState(Qt::Unchecked);

   if (!QString::compare(current_data[9], "true"))
      ui->spc_cur_joint_rlock2->setCheckState(Qt::Checked);
   else
      ui->spc_cur_joint_rlock2->setCheckState(Qt::Unchecked);

   if (!QString::compare(current_data[10], "true"))
      ui->spc_cur_joint_rlock3->setCheckState(Qt::Checked);
   else
      ui->spc_cur_joint_rlock3->setCheckState(Qt::Unchecked);

   if (!QString::compare(current_data[11], "true"))
      ui->spc_cur_joint_tlock1->setCheckState(Qt::Checked);
   else
      ui->spc_cur_joint_tlock1->setCheckState(Qt::Unchecked);

   if (!QString::compare(current_data[12], "true"))
      ui->spc_cur_joint_tlock2->setCheckState(Qt::Checked);
   else
      ui->spc_cur_joint_tlock2->setCheckState(Qt::Unchecked);

   if (!QString::compare(current_data[13], "true"))
      ui->spc_cur_joint_tlock3->setCheckState(Qt::Checked);
   else
      ui->spc_cur_joint_tlock3->setCheckState(Qt::Unchecked);

   setQComboBox(ui->spc_cur_joint_bigi_seq, current_data[29]);
   setQComboBox(ui->spc_cur_joint_bogo_seq, current_data[33]);
}

// Wheels -/+/Duplicate/Item Clicked

void SPC_submenu::on_spc_cur_wheel_remove_clicked() {
   if (wheels > 0) {
      wheels -= 1;
      delete ui->spc_cur_wheel_list->currentItem();
   }
   apply_data();
}

void SPC_submenu::on_spc_cur_wheel_add_clicked() {
   wheels += 1;

   QStringList tmp_data = {"0.0",  "1.0",   "0.0", "0.0", "0.14",
                           "50.0", "0.012", "0",   "0",   "NONE"};

   proc_add(ui->spc_cur_wheel_list, tmp_data);
   on_spc_cur_wheel_list_itemClicked(ui->spc_cur_wheel_list->currentItem());
   apply_data();
}

void SPC_submenu::on_spc_cur_wheel_duplicate_clicked() {
   if (wheels == 0)
      return;
   wheels += 1;

   proc_duplicates(ui->spc_cur_wheel_list);
   on_spc_cur_wheel_list_itemClicked(ui->spc_cur_wheel_list->currentItem());
   apply_data();
}

void SPC_submenu::on_spc_cur_wheel_list_itemClicked(QListWidgetItem *item) {
   QStringList current_data = item->data(257).toStringList();

   QLineEdit *textboxes[8] = {
       ui->spc_cur_wheel_initmom, ui->spc_cur_wheel_axis_1,
       ui->spc_cur_wheel_axis_2,  ui->spc_cur_wheel_axis_3,
       ui->spc_cur_wheel_maxtrq,  ui->spc_cur_wheel_maxmom,
       ui->spc_cur_wheel_inertia, ui->spc_cur_wheel_drjit_file};
   int data_inds[8] = {0, 1, 2, 3, 4, 5, 6, 9};
   dsm_gui_lib::mult_setText(textboxes, 8, current_data, data_inds);

   item->setText(item->data(256).toString());

   ui->spc_cur_wheel_name->setText(item->data(256).toString());

   ui->spc_cur_wheel_body->setValue(current_data[7].toInt());
   ui->spc_cur_wheel_node->setValue(current_data[8].toInt());

   if (!QString::compare(wheel_drag, "false"))
      ui->spc_cur_wheel_glob_drag_off->setChecked(Qt::Checked);
   else
      ui->spc_cur_wheel_glob_drag_on->setChecked(Qt::Checked);

   if (!QString::compare(wheel_jitter, "false"))
      ui->spc_cur_wheel_glob_jitter_off->setChecked(Qt::Checked);
   else
      ui->spc_cur_wheel_glob_jitter_on->setChecked(Qt::Checked);
}

// MTB buttons

void SPC_submenu::on_spc_cur_mtb_remove_clicked() {
   if (mtbs > 0) {
      mtbs -= 1;
      delete ui->spc_cur_mtb_list->currentItem();
   }
   apply_data();
}

void SPC_submenu::on_spc_cur_mtb_add_clicked() {
   mtbs += 1;

   QStringList tmp_data = {"180.0", "1.0", "0.0", "0.0", "0"};

   proc_add(ui->spc_cur_mtb_list, tmp_data);
   on_spc_cur_mtb_list_itemClicked(ui->spc_cur_mtb_list->currentItem());
   apply_data();
}

void SPC_submenu::on_spc_cur_mtb_duplicate_clicked() {
   if (mtbs == 0)
      return;
   mtbs += 1;

   proc_duplicates(ui->spc_cur_mtb_list);
   on_spc_cur_mtb_list_itemClicked(ui->spc_cur_mtb_list->currentItem());
   apply_data();
}

void SPC_submenu::on_spc_cur_mtb_list_itemClicked(QListWidgetItem *item) {
   QStringList current_data = item->data(257).toStringList();
   QLineEdit *textboxes[4]  = {ui->spc_cur_mtb_sat, ui->spc_cur_mtb_axis_1,
                               ui->spc_cur_mtb_axis_2, ui->spc_cur_mtb_axis_3};
   int data_inds[4]         = {0, 1, 2, 3};
   dsm_gui_lib::mult_setText(textboxes, 4, current_data, data_inds);

   item->setText(item->data(256).toString());

   ui->spc_cur_mtb_name->setText(item->data(256).toString());

   ui->spc_cur_mtb_node->setValue(current_data[4].toInt());
}

// Thruster buttons

void SPC_submenu::on_spc_cur_thruster_remove_clicked() {
   if (thrusters > 0) {
      thrusters -= 1;
      delete ui->spc_cur_thruster_list->currentItem();
   }
   apply_data();
}

void SPC_submenu::on_spc_cur_thruster_add_clicked() {
   thrusters += 1;

   QStringList tmp_data = {"PULSED", "1.0", "-1.0", "0.0", "0.0", "0", "0"};

   proc_add(ui->spc_cur_thruster_list, tmp_data);
   on_spc_cur_thruster_list_itemClicked(
       ui->spc_cur_thruster_list->currentItem());
   apply_data();
}

void SPC_submenu::on_spc_cur_thruster_duplicate_clicked() {
   if (thrusters == 0)
      return;
   thrusters += 1;

   proc_duplicates(ui->spc_cur_thruster_list);
   on_spc_cur_thruster_list_itemClicked(
       ui->spc_cur_thruster_list->currentItem());
   apply_data();
}

void SPC_submenu::on_spc_cur_thruster_list_itemClicked(QListWidgetItem *item) {
   QStringList current_data = item->data(257).toStringList();
   QLineEdit *textboxes[4]  = {
       ui->spc_cur_thruster_force, ui->spc_cur_thruster_axis_1,
       ui->spc_cur_thruster_axis_2, ui->spc_cur_thruster_axis_3};
   int data_inds[4] = {1, 2, 3, 4};
   dsm_gui_lib::mult_setText(textboxes, 4, current_data, data_inds);

   item->setText(item->data(256).toString());
   ui->spc_cur_thruster_name->setText(item->data(256).toString());

   setQComboBox(ui->spc_cur_thruster_mode, current_data[0]);

   ui->spc_cur_thruster_body->setValue(current_data[5].toInt());
   ui->spc_cur_thruster_node->setValue(current_data[6].toInt());
}

// Gyro buttons

void SPC_submenu::on_spc_cur_gyro_remove_clicked() {
   if (gyros > 0) {
      gyros -= 1;
      delete ui->spc_cur_gyro_list->currentItem();
   }
   apply_data();
}

void SPC_submenu::on_spc_cur_gyro_add_clicked() {
   gyros += 1;

   QStringList tmp_data = {"0.1",   "1.0", "0.0",  "0.0", "1000.0",
                           "100.0", "1.0", "0.07", "0.1", "1.0",
                           "0.1",   "0.1", "0"};

   proc_add(ui->spc_cur_gyro_list, tmp_data);
   on_spc_cur_gyro_list_itemClicked(ui->spc_cur_gyro_list->currentItem());
   apply_data();
}

void SPC_submenu::on_spc_cur_gyro_duplicate_clicked() {
   if (gyros == 0)
      return;
   gyros += 1;

   proc_duplicates(ui->spc_cur_gyro_list);
   on_spc_cur_gyro_list_itemClicked(ui->spc_cur_gyro_list->currentItem());
   apply_data();
}

void SPC_submenu::on_spc_cur_gyro_list_itemClicked(QListWidgetItem *item) {
   QStringList current_data = item->data(257).toStringList();
   QLineEdit *textboxes[12] = {
       ui->spc_cur_gyro_samptime,  ui->spc_cur_gyro_axis_1,
       ui->spc_cur_gyro_axis_2,    ui->spc_cur_gyro_axis_3,
       ui->spc_cur_gyro_maxrate,   ui->spc_cur_gyro_scaleferror,
       ui->spc_cur_gyro_quant,     ui->spc_cur_gyro_angrwalk,
       ui->spc_cur_gyro_bias_stab, ui->spc_cur_gyro_bias_tspan,
       ui->spc_cur_gyro_angnoise,  ui->spc_cur_gyro_initbias};
   int data_inds[12] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
   dsm_gui_lib::mult_setText(textboxes, 12, current_data, data_inds);

   item->setText(item->data(256).toString());
   ui->spc_cur_gyro_name->setText(item->data(256).toString());

   ui->spc_cur_gyro_node->setValue(current_data[12].toInt());
}

// Magnetometer buttons

void SPC_submenu::on_spc_cur_mag_remove_clicked() {
   if (mags > 0) {
      mags -= 1;
      delete ui->spc_cur_mag_list->currentItem();
   }
   apply_data();
}

void SPC_submenu::on_spc_cur_mag_add_clicked() {
   mags += 1;

   QStringList tmp_data = {"0.1", "1.0",    "0.0",    "0.0", "60.0E-6",
                           "0.0", "1.0E-6", "1.0E-6", "0"};

   proc_add(ui->spc_cur_mag_list, tmp_data);
   on_spc_cur_mag_list_itemClicked(ui->spc_cur_mag_list->currentItem());
   apply_data();
}

void SPC_submenu::on_spc_cur_mag_duplicate_clicked() {
   if (mags == 0)
      return;
   mags += 1;

   proc_duplicates(ui->spc_cur_mag_list);
   on_spc_cur_mag_list_itemClicked(ui->spc_cur_mag_list->currentItem());
   apply_data();
}

void SPC_submenu::on_spc_cur_mag_list_itemClicked(QListWidgetItem *item) {
   QStringList current_data = item->data(257).toStringList();
   QLineEdit *textboxes[8]  = {
       ui->spc_cur_mag_samptime, ui->spc_cur_mag_axis_1,
       ui->spc_cur_mag_axis_2,   ui->spc_cur_mag_axis_3,
       ui->spc_cur_mag_sat,      ui->spc_cur_mag_scaleferror,
       ui->spc_cur_mag_quant,    ui->spc_cur_mag_noise};
   int data_inds[8] = {0, 1, 2, 3, 4, 5, 6, 7};
   dsm_gui_lib::mult_setText(textboxes, 8, current_data, data_inds);

   item->setText(item->data(256).toString());
   ui->spc_cur_mag_name->setText(item->data(256).toString());

   ui->spc_cur_mag_node->setValue(current_data[8].toInt());
}

// CSS buttons

void SPC_submenu::on_spc_cur_css_remove_clicked() {
   if (css_s > 0) {
      css_s -= 1;
      delete ui->spc_cur_css_list->currentItem();
   }
   apply_data();
}

void SPC_submenu::on_spc_cur_css_add_clicked() {
   css_s += 1;

   QStringList tmp_data = {"0.1", "1.0",   "0.0", "0.0", "90.0",
                           "1.0", "0.001", "0",   "0"};

   proc_add(ui->spc_cur_css_list, tmp_data);
   on_spc_cur_css_list_itemClicked(ui->spc_cur_css_list->currentItem());
   apply_data();
}

void SPC_submenu::on_spc_cur_css_duplicate_clicked() {
   if (css_s == 0)
      return;
   css_s += 1;

   proc_duplicates(ui->spc_cur_css_list);
   on_spc_cur_css_list_itemClicked(ui->spc_cur_css_list->currentItem());
   apply_data();
}

void SPC_submenu::on_spc_cur_css_list_itemClicked(QListWidgetItem *item) {
   QStringList current_data = item->data(257).toStringList();
   QLineEdit *textboxes[7]  = {ui->spc_cur_css_samptime, ui->spc_cur_css_axis_1,
                               ui->spc_cur_css_axis_2,   ui->spc_cur_css_axis_3,
                               ui->spc_cur_css_halfcone, ui->spc_cur_css_scale,
                               ui->spc_cur_css_quant};
   int data_inds[7]         = {0, 1, 2, 3, 4, 5, 6};
   dsm_gui_lib::mult_setText(textboxes, 7, current_data, data_inds);

   item->setText(item->data(256).toString());
   ui->spc_cur_css_name->setText(item->data(256).toString());

   ui->spc_cur_css_body->setValue(current_data[7].toInt());
   ui->spc_cur_css_node->setValue(current_data[8].toInt());
}

// FSS Buttons

void SPC_submenu::on_spc_cur_fss_remove_clicked() {
   if (fss_s > 0) {
      fss_s -= 1;
      delete ui->spc_cur_fss_list->currentItem();
   }
   apply_data();
}

void SPC_submenu::on_spc_cur_fss_add_clicked() {
   fss_s += 1;

   QStringList tmp_data = {"0.2",  "70.0", "0.0", "0.0", "321", "Z_AXIS",
                           "32.0", "32.0", "0.1", "0.5", "0"};

   proc_add(ui->spc_cur_fss_list, tmp_data);
   on_spc_cur_fss_list_itemClicked(ui->spc_cur_fss_list->currentItem());
   apply_data();
}

void SPC_submenu::on_spc_cur_fss_duplicate_clicked() {
   if (fss_s == 0)
      return;
   fss_s += 1;

   proc_duplicates(ui->spc_cur_fss_list);
   on_spc_cur_fss_list_itemClicked(ui->spc_cur_fss_list->currentItem());
   apply_data();
}

void SPC_submenu::on_spc_cur_fss_list_itemClicked(QListWidgetItem *item) {
   QStringList current_data = item->data(257).toStringList();
   QLineEdit *textboxes[8] = {ui->spc_cur_fss_samptime, ui->spc_cur_fss_mount_1,
                              ui->spc_cur_fss_mount_2,  ui->spc_cur_fss_mount_3,
                              ui->spc_cur_fss_hfov,     ui->spc_cur_fss_vfov,
                              ui->spc_cur_fss_noiseang, ui->spc_cur_fss_quant};
   int data_inds[8]        = {0, 1, 2, 3, 6, 7, 8, 9};
   dsm_gui_lib::mult_setText(textboxes, 8, current_data, data_inds);

   item->setText(item->data(256).toString());
   ui->spc_cur_fss_name->setText(item->data(256).toString());

   setQComboBox(ui->spc_cur_fss_mountseq, current_data[4]);
   setQComboBox(ui->spc_cur_fss_boreaxis, current_data[5]);

   ui->spc_cur_css_node->setValue(current_data[10].toInt());
}

// Star Tracker Buttons

void SPC_submenu::on_spc_cur_strack_remove_clicked() {
   if (stracks > 0) {
      stracks -= 1;
      delete ui->spc_cur_strack_list->currentItem();
   }
   apply_data();
}

void SPC_submenu::on_spc_cur_strack_add_clicked() {
   stracks += 1;

   QStringList tmp_data = {"0.25",   "-90.0", "90.0", "00.0", "321",
                           "Z_AXIS", "8.0",   "8.0",  "30.0", "10.0",
                           "10.0",   "2.0",   "2.0",  "20.0", "0"};

   proc_add(ui->spc_cur_strack_list, tmp_data);
   on_spc_cur_strack_list_itemClicked(ui->spc_cur_strack_list->currentItem());
   apply_data();
}

void SPC_submenu::on_spc_cur_strack_duplicate_clicked() {
   if (stracks == 0)
      return;
   stracks += 1;

   proc_duplicates(ui->spc_cur_strack_list);
   on_spc_cur_strack_list_itemClicked(ui->spc_cur_strack_list->currentItem());
   apply_data();
}

void SPC_submenu::on_spc_cur_strack_list_itemClicked(QListWidgetItem *item) {
   QStringList current_data = item->data(257).toStringList();
   QLineEdit *textboxes[12] = {
       ui->spc_cur_strack_samptime,   ui->spc_cur_strack_mount_1,
       ui->spc_cur_strack_mount_2,    ui->spc_cur_strack_mount_3,
       ui->spc_cur_strack_hfov,       ui->spc_cur_strack_vfov,
       ui->spc_cur_strack_sun,        ui->spc_cur_strack_earth,
       ui->spc_cur_strack_moon,       ui->spc_cur_strack_noiseang_1,
       ui->spc_cur_strack_noiseang_2, ui->spc_cur_strack_noiseang_3};
   int data_inds[12] = {0, 1, 2, 3, 6, 7, 8, 9, 10, 11, 12, 13};
   dsm_gui_lib::mult_setText(textboxes, 12, current_data, data_inds);

   item->setText(item->data(256).toString());
   ui->spc_cur_strack_name->setText(item->data(256).toString());

   setQComboBox(ui->spc_cur_strack_mountseq, current_data[4]);
   setQComboBox(ui->spc_cur_strack_boreaxis, current_data[5]);

   ui->spc_cur_strack_node->setValue(current_data[14].toInt());
}

// GPS Buttons

void SPC_submenu::on_spc_cur_gps_remove_clicked() {
   if (gps_s > 0) {
      gps_s -= 1;
      delete ui->spc_cur_gps_list->currentItem();
   }
   apply_data();
}

void SPC_submenu::on_spc_cur_gps_add_clicked() {
   gps_s += 1;

   QStringList tmp_data = {"0.25", "4.0", "0.02", "20.0E-9", "0"};

   proc_add(ui->spc_cur_gps_list, tmp_data);
   on_spc_cur_gps_list_itemClicked(ui->spc_cur_gps_list->currentItem());
   apply_data();
}

void SPC_submenu::on_spc_cur_gps_duplicate_clicked() {
   if (gps_s == 0)
      return;
   gps_s += 1;

   proc_duplicates(ui->spc_cur_gps_list);
   on_spc_cur_gps_list_itemClicked(ui->spc_cur_gps_list->currentItem());
   apply_data();
}

void SPC_submenu::on_spc_cur_gps_list_itemClicked(QListWidgetItem *item) {
   QStringList current_data = item->data(257).toStringList();
   QLineEdit *textboxes[4]  = {
       ui->spc_cur_gps_samptime, ui->spc_cur_gps_posnoise,
       ui->spc_cur_gps_velnoise, ui->spc_cur_gps_timenoise};
   int data_inds[4] = {0, 1, 2, 3};
   dsm_gui_lib::mult_setText(textboxes, 4, current_data, data_inds);

   item->setText(item->data(256).toString());
   ui->spc_cur_gps_name->setText(item->data(256).toString());

   ui->spc_cur_gps_node->setValue(current_data[4].toInt());
}

// Accelerometer Buttons

void SPC_submenu::on_spc_cur_accel_remove_clicked() {
   if (accels > 0) {
      accels -= 1;
      delete ui->spc_cur_accel_list->currentItem();
   }
   apply_data();
}

void SPC_submenu::on_spc_cur_accel_add_clicked() {
   accels += 1;

   QStringList tmp_data = {"0.1", "1.0", "0.0", "0.0", "1.0", "0.0", "0.05",
                           "0.0", "0.0", "1.0", "0.0", "0.5", "0"};

   proc_add(ui->spc_cur_accel_list, tmp_data);
   on_spc_cur_accel_list_itemClicked(ui->spc_cur_accel_list->currentItem());
   apply_data();
}

void SPC_submenu::on_spc_cur_accel_duplicate_clicked() {
   if (accels == 0)
      return;
   accels += 1;

   proc_duplicates(ui->spc_cur_accel_list);
   on_spc_cur_accel_list_itemClicked(ui->spc_cur_accel_list->currentItem());
   apply_data();
}

void SPC_submenu::on_spc_cur_accel_list_itemClicked(QListWidgetItem *item) {
   QStringList current_data = item->data(257).toStringList();
   QLineEdit *textboxes[12] = {
       ui->spc_cur_acc_samptime,  ui->spc_cur_acc_axis_1,
       ui->spc_cur_acc_axis_2,    ui->spc_cur_acc_axis_3,
       ui->spc_cur_acc_maxacc,    ui->spc_cur_acc_scaleerror,
       ui->spc_cur_acc_quant,     ui->spc_cur_acc_dvrandwalk,
       ui->spc_cur_acc_bias_stab, ui->spc_cur_acc_bias_tspan,
       ui->spc_cur_acc_dvnoise,   ui->spc_cur_acc_initbias};
   int data_inds[12] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
   dsm_gui_lib::mult_setText(textboxes, 12, current_data, data_inds);

   item->setText(item->data(256).toString());
   ui->spc_cur_acc_name->setText(item->data(256).toString());

   ui->spc_cur_gps_node->setValue(current_data[12].toInt());
}

// Misc
void SPC_submenu::on_sections_tabBarClicked(int index) {
   if ((joints != bodies - 1) && (index != 1)) {
      ui->sections->widget(0)->setEnabled(false);
      ui->sections->widget(2)->setEnabled(false);
      ui->sections->widget(3)->setEnabled(false);

      dsm_gui_lib::error_message(
          "The number of joints must equal number of bodies minus one.");

   } else {
      ui->sections->widget(0)->setEnabled(true);
      ui->sections->widget(2)->setEnabled(true);
      ui->sections->widget(3)->setEnabled(true);
   }

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
}

void SPC_submenu::destroy_submenu(QString command, QString junk) {
   if (QString::compare(command, "Done") == 0) {
      QDialog::close();
   }
}
