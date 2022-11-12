/*
  This file is part of the KTextTemplate library

  Copyright (c) 2009,2010 Stephen Kelly <steveire@gmail.com>

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

#ifndef KTEXTTEMPLATE_TEMPLATE_P_H
#define KTEXTTEMPLATE_TEMPLATE_P_H

#include "engine.h"
#include "template.h"

#include <QPointer>

namespace KTextTemplate
{

class Engine;

class TemplatePrivate
{
  TemplatePrivate(Engine const *engine, bool smartTrim, TemplateImpl *t)
      : q_ptr(t), m_error(NoError), m_smartTrim(smartTrim), m_engine(engine)
  {
  }

  void parse();
  NodeList compileString(const QString &str);
  void setError(Error type, const QString &message) const;

  Q_DECLARE_PUBLIC(TemplateImpl)
  TemplateImpl *const q_ptr;

  mutable Error m_error;
  mutable QString m_errorString;
  NodeList m_nodeList;
  bool m_smartTrim;
  QPointer<const Engine> m_engine;

  friend class KTextTemplate::Engine;
  friend class Parser;
};
}

#endif
