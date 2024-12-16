#ifndef IPC_MENU_H
#define IPC_MENU_H

#include <QDebug>
#include <QDialog>
#include <QFile>
#include <QInputDialog>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QRegularExpression>
#include <QTextStream>
#include <dsm_gui_lib.h>

class Host {
   private:
   QString _name = "localhost";
   long _port    = 10001;

   public:
   Host(const QString name = "localhost", const long port = 10001) {
      setName(name);
      setPort(port);
   }
   void setName(const QString name) {
      _name = name;
   }
   void setPort(const long port) {
      _port = port;
   }
   QString name() const {
      return _name;
   }
   long port() const {
      return _port;
   }
};

class Socket {
   protected:
   enum roles { SERVER = 0, CLIENT, GMSEC_CLIENT };
   const static inline QMap<QString, enum roles> role_strs = {
       {"SERVER", SERVER}, {"CLIENT", CLIENT}, {"GMSEC_CLIENT", GMSEC_CLIENT}};

   private:
   enum roles _role = CLIENT;
   Host _host       = Host();
   bool _blocking   = true;

   public:
   Socket(const QString role = role_strs.key(CLIENT), const Host &host = Host(),
          const bool blocking = true) {
      setRole(role);
      setHost(host);
      setBlocking(blocking);
   }
   void setRole(const QString role) {
      _role = role_strs.value(role, CLIENT);
   }
   void setHost(const Host &host) {
      _host = host;
   }
   void setHost(const QString host_name) {
      _host.setName(host_name);
   }
   void setHost(const long port) {
      _host.setPort(port);
   }
   void setBlocking(const bool blocking) {
      _blocking = blocking;
   }
   QString role() const {
      return role_strs.key(_role);
   }
   Host host() const {
      return _host;
   }
   bool blocking() const {
      return _blocking;
   }
};

class IPC {
   protected:
   enum modes { OFF = 0, TX, RX, TXRX, ACS, WRITEFILE, READFILE };
   const static inline QMap<QString, enum modes> mode_strs = {
       {"OFF", OFF},          {"TX", TX},   {"RX", RX},
       {"TXRX", TXRX},        {"ACS", ACS}, {"WRITEFILE", WRITEFILE},
       {"READFILE", READFILE}};

   private:
   enum modes _mode      = OFF;
   long _ac_id           = 0;
   QString _file_name    = "State00.42";
   Socket _socket        = Socket();
   bool _echo            = false;
   QStringList _prefixes = {};

   public:
   IPC(const QString mode = mode_strs.key(OFF), const long ac_id = 0,
       const QString file_name = "State00.42", const Socket socket = Socket(),
       const bool echo = false, const QStringList prefixes = {}) {
      setMode(mode);
      setACID(ac_id);
      setFileName(file_name);
      setSocket(socket);
      setEcho(echo);
      setPrefixes(prefixes);
   }
   void setMode(const QString mode) {
      _mode = mode_strs.value(mode, OFF);
   }
   void setACID(const long ac_id) {
      _ac_id = std::max(ac_id, 0l);
   }
   void setFileName(const QString file_name) {
      _file_name = file_name;
   }
   void setEcho(const bool echo) {
      _echo = echo;
   }
   void setPrefixes(const QStringList prefixes) {
      _prefixes = prefixes;
   }
   void addPrefix(const QString prefix) {
      _prefixes.append(prefix);
   }
   void removePrefix(const QString prefix) {
      _prefixes.removeOne(prefix);
   }
   void removePrefix(const long prefix_ind) {
      _prefixes.removeAt(prefix_ind);
   }
   void setSocket(const Socket socket) {
      _socket = socket;
   }
   void setSocketRole(const QString role) {
      _socket.setRole(role);
   }
   void setSocketHost(const Host &host) {
      _socket.setHost(host);
   }
   void setSocketHost(const QString host_name) {
      _socket.setHost(host_name);
   }
   void setSocketHost(const long port) {
      _socket.setHost(port);
   }
   void setSocketBlocking(const bool blocking) {
      _socket.setBlocking(blocking);
   }
   QString mode() const {
      return mode_strs.key(_mode);
   }
   long ac_id() const {
      return _ac_id;
   }
   QString file_name() const {
      return _file_name;
   }
   Socket socket() const {
      return _socket;
   }
   bool echo() const {
      return _echo;
   }
   QStringList prefixes() const {
      return _prefixes;
   }
};

namespace Ui {
class IPC_Menu;
}

class IPC_Menu : public QDialog {
   Q_OBJECT

   public:
   explicit IPC_Menu(QWidget *parent = nullptr);
   ~IPC_Menu();

   private slots:
   void set_validators();
   void receive_ipcpath(QString);
   void apply_data();
   void clear_fields();

   void on_ipc_remove_clicked();
   void on_ipc_add_clicked();
   void on_ipclist_itemClicked(QListWidgetItem *item);
   void on_loaddefaultButton_clicked();
   void on_savedefaultButton_clicked();
   void on_closeButton_clicked();
   void on_applyButton_clicked();

   void on_prefix_remove_clicked();
   void on_prefix_add_clicked();

   void on_prefix_rename_clicked();

   void on_ipc_duplicate_clicked();

   void server_changed();

   void on_ipcmode_currentTextChanged(const QString &arg1);

   void on_acs_id_textChanged(const QString &arg1);

   void on_filename_textChanged(const QString &arg1);

   void on_socketrole_currentTextChanged(const QString &arg1);

   void on_blocking_toggled(bool checked);

   void on_echo_toggled(bool checked);

   void on_prefixlist_currentRowChanged(int currentRow);
   void update_prefixes(QListWidgetItem *item);

   private:
   Ui::IPC_Menu *ui;

   QString inout_path;
   QString file_path;
   QVector<int> ipc_name_index;
   QVector<int> ipc_name_size;
   QVector<int> ipc_name_prefixes;

   QHash<QListWidgetItem *, IPC> ipc_list_hash;

   QHash<QString, QString> ipcmodeinputs     = {{"OFF", "Off"},
                                                {"TX", "TX"},
                                                {"RX", "RX"},
                                                {"TXRX", "TXRX"},
                                                {"ACS", "ACS"},
                                                {"WRITEFILE", "Write to File"},
                                                {"READFILE", "Read from File"}};
   QHash<QString, QString> socketrole_inputs = {
       {"SERVER", "Server"},
       {"CLIENT", "Client"},
       {"GMSEC_CLIENT", "GMSEC Client"}};
};

// Configure YAML conversions
namespace YAML {
// Host
template <> struct convert<Host> {
   static Node encode(const Host &rhs) {
      Node node(NodeType::Map);
      node["Name"] = rhs.name();
      node["Port"] = rhs.port();
      return node;
   }
   static bool decode(const Node &node, Host &rhs) {
      if (!node.IsMap())
         return false;
      rhs.setName(node["Name"].as<QString>());
      rhs.setPort(node["Port"].as<long>());
      return true;
   }
};

// Socket
template <> struct convert<Socket> {
   static Node encode(const Socket &rhs) {
      Node node(NodeType::Map);
      node["Role"]     = rhs.role();
      node["Blocking"] = rhs.blocking();
      node["Host"]     = rhs.host();
      return node;
   }
   static bool decode(const Node &node, Socket &rhs) {
      if (!node.IsMap())
         return false;
      rhs.setRole(node["Role"].as<QString>());
      rhs.setBlocking(node["Blocking"].as<bool>());
      rhs.setHost(node["Host"].as<Host>());
      return true;
   }
};

// IPC
template <> struct convert<IPC> {
   static Node encode(const IPC &rhs) {
      Node node(NodeType::Map);
      Node data_node              = node["IPC"];
      data_node["Mode"]           = rhs.mode();
      data_node["AC ID"]          = rhs.ac_id();
      data_node["Socket"]         = rhs.socket();
      data_node["Echo to stdout"] = rhs.echo();
      data_node["Prefixes"]       = rhs.prefixes();
      return node;
   }
   static bool decode(const Node &node, IPC &rhs) {
      if (!node.IsMap())
         return false;
      YAML::Node data_node = node["IPC"];
      if (!data_node.IsMap())
         return false;
      rhs.setMode(data_node["Mode"].as<QString>());
      rhs.setACID(data_node["AC ID"].as<long>());
      rhs.setFileName(data_node["File Name"].as<QString>());
      rhs.setSocket(data_node["Socket"].as<Socket>());
      rhs.setEcho(data_node["Echo to stdout"].as<bool>());
      rhs.setPrefixes(data_node["Prefixes"].as<QStringList>());
      return true;
   }
};
} // namespace YAML

#endif // IPC_MENU_H
