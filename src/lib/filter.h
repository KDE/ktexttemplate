/*
  This file is part of the KTextTemplate library

  SPDX-FileCopyrightText: 2009, 2010 Stephen Kelly <steveire@gmail.com>

  SPDX-License-Identifier: LGPL-2.1-or-later

*/

// krazy:excludeall=dpointer

#ifndef KTEXTTEMPLATE_FILTER_H
#define KTEXTTEMPLATE_FILTER_H

#include "context.h"
#include "ktexttemplate_export.h"
#include "outputstream.h"
#include "safestring.h"

#include <QSharedPointer>
#include <QStringList>
#include <QVariant>

namespace KTextTemplate
{

class FilterExpression;
class FilterPrivate;
class ScriptableLibraryContainer;

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
    explicit Filter();
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
    [[nodiscard]] SafeString escape(const QString &input) const;

    /*!
      Escapes and returns \a input. The OutputStream::escape method is used to
      escape \a input.
    */
    [[nodiscard]] SafeString escape(const SafeString &input) const;

    /*!
      Escapes \a input if not already safe from further escaping and returns it.
      The OutputStream::escape method is used to escape \a input.
    */
    [[nodiscard]] SafeString conditionalEscape(const SafeString &input) const;

    // TODO KF7: pass context as an argument to doFilter, rather than holding it temporarily as a member
    /*!
      Reimplement to filter \a input given \a argument.

      \a autoescape determines whether the autoescape feature is currently on or
      off. Most filters will not use this.
    */
    [[nodiscard]] virtual QVariant doFilter(const QVariant &input, const QVariant &argument = {}, bool autoescape = {}) const = 0;

    /*!
      Reimplement to return whether this filter is safe.
    */
    [[nodiscard]] virtual bool isSafe() const;

    /*!
       The context in which the filter is evaluated.
       \warning Calling this is only valid from within doFilter itself!
    */
    [[nodiscard]] Context *context() const;

private:
    Q_DISABLE_COPY(Filter)
    // TODO KF7 remove this
    // this is needed as prior to KF 6.29 what became d below was uninitialized
    // and the default ctor was inline, ie. older external subclasses would leave us
    // with an uninitialized member variable here
    friend class ParserPrivate;
    friend class ScriptableLibraryContainer;
    KTEXTTEMPLATE_NO_EXPORT void forgottenBaseCtorRemoveInKF7();

    // TODO KF7 remove this if Context becomes an argument to doFilter
    friend class FilterExpression;
    KTEXTTEMPLATE_NO_EXPORT void setContext(Context *context);

    // can become a std::unique_ptr in KF7, but not before due to the above issue
    FilterPrivate *d_ptr = nullptr;
};
}

#endif
