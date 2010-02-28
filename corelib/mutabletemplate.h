/*
  This file is part of the Grantlee template system.

  Copyright (c) 2009,2010 Stephen Kelly <steveire@gmail.com>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either version
  2 of the Licence, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef GRANTLEE_MUTABLE_TEMPLATE_H
#define GRANTLEE_MUTABLE_TEMPLATE_H

#include "template.h"

namespace Grantlee
{
class MutableTemplateImpl;

typedef QWeakPointer<MutableTemplateImpl> MutableTemplateWeakPtr;
typedef QSharedPointer<MutableTemplateImpl> MutableTemplate;

/**
  A Template which is mutated each time it is rendered.
*/
class MutableTemplateImpl : public TemplateImpl
{
  Q_OBJECT
public:
  /**
    Constructor.
  */
  explicit MutableTemplateImpl( Engine const *engine, QObject *parent = 0 );

  /**
    Renders the Template, possibly mutating it.
  */
  /* reimp */ QString render( Context *c );

};

}

Q_DECLARE_METATYPE( Grantlee::MutableTemplate )

#endif
