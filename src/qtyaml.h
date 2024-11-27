/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 2.0
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * ***** END LICENSE BLOCK ***** */

/*
 * Copyright (c) 2014, Filip Brcic <brcha@gna.org>. All rights reserved.
 *
 * This file is part of lusim
 */

#ifndef QTYAML_H
#define QTYAML_H

#include <yaml-cpp/yaml.h>

#include <QDate>
#include <QTime>
#include <QVector3D>
#include <QVector4D>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QPair>
#include <QtCore/QString>
#include <QtCore/QVector>

namespace YAML {

// QString
template <> struct convert<QString> {
   static Node encode(const QString &rhs) {
      return Node(rhs.toStdString());
   }

   static bool decode(const Node &node, QString &rhs) {
      if (!node.IsScalar())
         return false;
      rhs = QString::fromStdString(node.Scalar());
      return true;
   }
};

// QMap
template <typename Key, typename Value> struct convert<QMap<Key, Value>> {
   static Node encode(const QMap<Key, Value> &rhs) {
      Node node(NodeType::Map);
      auto it = rhs.constBegin();
      while (it != rhs.constEnd()) {
         node.force_insert(it.key(), it.value());
         ++it;
      }
      return node;
   }

   static bool decode(const Node &node, QMap<Key, Value> &rhs) {
      if (!node.IsMap())
         return false;

      rhs.clear();
      const_iterator it = node.begin();
      while (it != node.end()) {
         rhs[it->first.as<Key>()] = it->second.as<Value>();
         ++it;
      }
      return true;
   }
};

// QVector
template <typename T> struct convert<QVector<T>> {
   static Node encode(const QVector<T> &rhs) {
      Node node(NodeType::Sequence);
      foreach (T value, rhs) {
         node.push_back(value);
      }
      if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
         node.SetStyle(EmitterStyle::Flow);
      } else {
         node.SetStyle(EmitterStyle::Block);
      }
      return node;
   }

   static bool decode(const Node &node, QVector<T> &rhs) {
      if (!node.IsSequence())
         return false;

      rhs.clear();
      const_iterator it = node.begin();
      while (it != node.end()) {
         rhs.push_back(it->as<T>());
         ++it;
      }
      return true;
   }
};

// QList
template <typename T> struct convert<QList<T>> {
   static Node encode(const QList<T> &rhs) {
      Node node(NodeType::Sequence);
      foreach (T value, rhs) {
         node.push_back(value);
      }
      if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
         node.SetStyle(EmitterStyle::Flow);
      } else {
         node.SetStyle(EmitterStyle::Block);
      }
      return node;
   }

   static bool decode(const Node &node, QList<T> &rhs) {
      if (!node.IsSequence())
         return false;

      rhs.clear();
      const_iterator it = node.begin();
      while (it != node.end()) {
         rhs.push_back(it->as<T>());
         ++it;
      }
      return true;
   }
};

// QPair
template <typename T, typename U> struct convert<QPair<T, U>> {
   static Node encode(const QPair<T, U> &rhs) {
      Node node(NodeType::Sequence);
      node.push_back(rhs.first);
      node.push_back(rhs.second);
      return node;
   }

   static bool decode(const Node &node, QPair<T, U> &rhs) {
      if (!node.IsSequence())
         return false;
      if (node.size() != 2)
         return false;

      rhs.first  = node[0].as<T>();
      rhs.second = node[1].as<U>();
      return true;
   }
};

// QStringList
template <> struct convert<QStringList> {
   static Node encode(const QStringList &rhs) {
      Node node(NodeType::Sequence);
      for (const auto &value : rhs) {
         node.push_back(value);
      }
      return node;
   }

   static bool decode(const Node &node, QStringList &rhs) {
      if (!node.IsSequence())
         return false;

      rhs.clear();
      const_iterator it = node.begin();
      while (it != node.end()) {
         rhs.push_back(it->as<QString>());
         ++it;
      }
      return true;
   }
};

// QDate
template <> struct convert<QDate> {
   static Node encode(const QDate &rhs) {
      Node node(NodeType::Map);
      node["Year"]  = rhs.year();
      node["Month"] = rhs.month();
      node["Day"]   = rhs.day();
      return node;
   }
   static bool decode(const Node &node, QDate &rhs) {
      if (!node.IsMap())
         return false;

      rhs.setDate(node["Year"].as<int>(), node["Month"].as<int>(),
                  node["Day"].as<int>());
      return true;
   }
};

// QTime
template <> struct convert<QTime> {
   static Node encode(const QTime &rhs) {
      Node node(NodeType::Map);
      node["Hour"]        = rhs.hour();
      node["Minute"]      = rhs.minute();
      node["Second"]      = rhs.second();
      node["Millisecond"] = rhs.msec();
      return node;
   }
   static bool decode(const Node &node, QTime &rhs) {
      if (!node.IsMap())
         return false;

      rhs.setHMS(node["Hour"].as<int>(), node["Minute"].as<int>(),
                 node["Second"].as<int>(), node["Millisecond"].as<int>());
      return true;
   }
};

// TODO: Add the rest of the container classes
// QLinkedList, QStack, QQueue, QSet, QMultiMap, QHash, QMultiHash,
// ...

// Added by Daniel Newberry, NASA WFF Intern, summer '24
//  QVector3D
template <> struct convert<QVector3D> {
   static Node encode(const QVector3D &rhs) {
      Node node(NodeType::Sequence);
      for (int i = 0; i < 3; i++) {
         node.push_back(rhs[i]);
      }
      node.SetStyle(EmitterStyle::Flow);
      return node;
   }
   static bool decode(const Node &node, QVector3D &rhs) {
      if (!node.IsSequence() || node.size() != 3)
         return false;

      const_iterator it = node.begin();
      for (int i = 0; i < 3; i++) {
         rhs[i] = it->as<double>();
         ++it;
      }
      return true;
   }
};

// QHash
template <class Key, class Value> struct convert<QHash<Key, Value>> {
   static Node encode(const QHash<Key, Value> &rhs) {
      Node node(NodeType::Map);
      for (auto it = rhs.constBegin(); it != rhs.constEnd(); ++it) {
         node.force_insert(it.key(), it.value());
      }
      return node;
   }
   static bool decode(const Node &node, QHash<Key, Value> &rhs) {
      if (!node.IsMap())
         return false;

      rhs.clear();
      for (const_iterator it = node.begin(); it != node.end(); ++it) {
         rhs[it->first.as<Key>()] = it->second.as<Value>();
      }
      return true;
   }
};

// QVector4D
template <> struct convert<QVector4D> {
   static Node encode(const QVector4D &rhs) {
      Node node(NodeType::Sequence);
      for (int i = 0; i < 4; i++) {
         node.push_back(rhs[i]);
      }
      node.SetStyle(EmitterStyle::Flow);
      return node;
   }
   static bool decode(const Node &node, QVector4D &rhs) {
      if (!node.IsSequence() || node.size() != 4)
         return false;

      const_iterator it = node.begin();
      for (int i = 0; i < 4; i++) {
         rhs[i] = it->as<double>();
         ++it;
      }
      return true;
   }
};

} // end namespace YAML

#endif // QTYAML_H
