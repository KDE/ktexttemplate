/*
  This file is part of the KTextTemplate library

  SPDX-FileCopyrightText: 2009, 2010 Stephen Kelly <steveire@gmail.com>

  SPDX-License-Identifier: LGPL-2.1-or-later

*/

#ifndef KTEXTTEMPLATE_TEMPLATE_H
#define KTEXTTEMPLATE_TEMPLATE_H

#include "exception.h"
#include "ktexttemplate_export.h"
#include "node.h"

#include <QSharedPointer>
#include <QStringList>

namespace KTextTemplate
{
class Context;
class Engine;
class TemplateImpl;
class OutputStream;

/*!
 * \typedef KTextTemplate::Template
 * \relates KTextTemplate::TemplateImpl
 */
typedef QSharedPointer<TemplateImpl> Template;

class TemplatePrivate;

/*!
  \class KTextTemplate::TemplateImpl
  \inheaderfile KTextTemplate/Template
  \inmodule KTextTemplate
  \brief The Template class is a tree of nodes which may be rendered.

  All Templates are created through the KTextTemplate::Engine class.
  A Template is created by parsing some text markup passed into the Engine,
  or by reading it from a file.

  Note that Template is actually a typedef for a
  QSharedPointer<TemplateImpl>; so all of its members should be
  accessed with operator->().

  The result of parsing is a Template object which can be rendered multiple
  times with multiple different Contexts.

  \code
    auto engine = getEngine();
    auto t = engine->newTemplate(
        "{{ name }} is aged {{ age }}",
        "simple template" );
    if ( t->error() )
    {
      // Tokenizing or parsing error, or couldn't find custom tags or filters.
      qDebug() << t->errorString();
      return;
    }
    QTextStream textStream( stdout );
    OutputStream stream( textStream );

    for ( ... )
    {
      Context c;
      // ... c.insert
      t->render( stream, c );

      if (t->error())
      {
        // Rendering error.
        qDebug() << t->errorString();
      }
    }
  \endcode

  If there is an error in parsing or rendering, the error and
  errorString methods can be used to check the source of the error.
*/
class KTEXTTEMPLATE_EXPORT TemplateImpl : public QObject
{
    Q_OBJECT
public:
    ~TemplateImpl() override;

    /*!
      Renders the Template to a string given the Context \a c.
    */
    QString render(Context *c) const;

    /*!
      Renders the Template to the OutputStream \a stream given the Context c.
    */
    OutputStream *render(OutputStream *stream, Context *c) const;

    /*!
      \internal
    */
    NodeList nodeList() const;

    /*!
      \internal
    */
    void setNodeList(const NodeList &list);

    /*!
      Returns an error code for the error encountered.
    */
    Error error() const;

    /*!
      Returns more information to developers in the form of a string.
    */
    QString errorString() const;

    /*!
      Returns the Engine that created this Template.
    */
    Engine const *engine() const;

protected:
    TemplateImpl(Engine const *engine, QObject *parent = {});
    TemplateImpl(Engine const *engine, bool smartTrim, QObject *parent = {});

    void setContent(const QString &templateString);

private:
    // Don't allow setting the parent on a Template, which is memory managed as
    // a QSharedPointer.
    using QObject::setParent;

private:
    Q_DECLARE_PRIVATE(Template)
    TemplatePrivate *const d_ptr;
    friend class Engine;
    friend class Parser;
};
}

#endif
