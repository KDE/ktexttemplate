/*
  This file is part of the KTextTemplate library

  Copyright (c) 2010 Stephen Kelly <steveire@gmail.com>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either version
  2.1 of the Licence, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef KTEXTTEMPLATE_TYPEACCESSOR_H
#define KTEXTTEMPLATE_TYPEACCESSOR_H

#include "ktexttemplate_export.h"

#include <QObject>

/// @file

namespace KTextTemplate
{

#ifndef K_DOXYGEN
template<typename T>
struct TypeAccessor {
    static QVariant lookUp(const T object, const QString &property);
};

template<typename T>
struct TypeAccessor<T *> {
    static QVariant lookUp(const T *const object, const QString &property);
};

template<typename T>
struct TypeAccessor<T &> {
    static QVariant lookUp(const T &object, const QString &property);
};
#endif
}

#endif
