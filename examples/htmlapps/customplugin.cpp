/*
  This file is part of the KTextTemplate library

  SPDX-FileCopyrightText: 2011 Stephen Kelly <steveire@gmail.com>

  SPDX-License-Identifier: LGPL-2.1-or-later

*/

#include "customplugin.h"

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#include <QtPlugin>

Q_EXPORT_PLUGIN2(customplugin, CustomPlugin)
#endif
