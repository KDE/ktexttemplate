/*
  This file is part of the KTextTemplate library

  SPDX-FileCopyrightText: 2009, 2010 Stephen Kelly <steveire@gmail.com>

  SPDX-License-Identifier: LGPL-2.1-or-later

*/

// krazy:excludeall=dpointer

#ifndef KTEXTTEMPLATE_FILTER_H
#define KTEXTTEMPLATE_FILTER_H

#include "ktexttemplate_export.h"
#include "outputstream.h"
#include "safestring.h"

#include <QSharedPointer>
#include <QStringList>
#include <QVariant>

namespace KTextTemplate
{

/*!
  \class KTextTemplate::Filter
  \inheaderfile KTextTemplate/Filter
  \inmodule KTextTemplate

  \brief Base class for all filters.

  The Filter class can be implemented in plugin libraries to make
  additional functionality available to templates.

  Developers are required only to implement the doFilter method and
  integrate the filter as part of a custom plugin, but will never create or
  access filters directly in application code.

  The FilterExpression class is the access interface to a chain of Filter
  objects.

  The escape and conditionalEscape methods are available for escaping
  data where needed.
*/
class KTEXTTEMPLATE_EXPORT Filter
{
public:
    virtual ~Filter();

    /*!
      FilterExpression makes it possible to access stream methods like escape
      while resolving.
    */
    void setStream(OutputStream *stream);

    /*!
      Escapes and returns \a input. The OutputStream::escape method is used to
      escape \a input.
    */
    SafeString escape(const QString &input) const;

    /*!
      Escapes and returns \a input. The OutputStream::escape method is used to
      escape \a input.
    */
    SafeString escape(const SafeString &input) const;

    /*!
      Escapes \a input if not already safe from further escaping and returns it.
      The OutputStream::escape method is used to escape \a input.
    */
    SafeString conditionalEscape(const SafeString &input) const;

    /*!
      Reimplement to filter \a input given \a argument.

      \a autoescape determines whether the autoescape feature is currently on or
      off. Most filters will not use this.
    */
    virtual QVariant doFilter(const QVariant &input, const QVariant &argument = {}, bool autoescape = {}) const = 0;

    /*!
      Reimplement to return whether this filter is safe.
    */
    virtual bool isSafe() const;

private:
    OutputStream *m_stream;
};
}

#endif
