/*
  This file is part of the KTextTemplate library

  SPDX-FileCopyrightText: 2009, 2010 Stephen Kelly <steveire@gmail.com>

  SPDX-License-Identifier: LGPL-2.1-or-later

*/

#ifndef KTEXTTEMPLATE_CONTEXT_H
#define KTEXTTEMPLATE_CONTEXT_H

#include "abstractlocalizer.h"
#include "ktexttemplate_export.h"

#include <QVariantHash>

namespace KTextTemplate
{

class RenderContext;

class ContextPrivate;

/*!
  \class KTextTemplate::Context
  \inheaderfile KTextTemplate/Context
  \inmodule KTextTemplate

  \brief The Context class holds the context to render a Template with.

  For application developers, using the Context class is a matter of
  inserting keys and values as appropriate for rendering a Template using the
  insert method.

  \code
    auto t = engine->newTemplate(
        "Name is {% name %} and age is {% age %}.", "some_template" );

    Context c1;
    c1.insert( "name", "Tom" );
    c1.insert( "age", 34 );

    Context c2;
    c2.insert( "name", "Harry" );
    c2.insert( "age", 43 );

    t->render(c1); // Returns "Name is Tom and age is 43."
    t->render(c2); // Returns "Name is Harry and age is 34."
  \endcode

  Note that one Template may be rendered multiple times with different contexts.
  Note also that any QVariant may be inserted into a Context object. Most
  commonly, QObjects will be used here.

  \section1 Context Stack

  For template tag developers, some other Context API is relevant.

  It is possible to push and pop layers of context while a template
  is being rendered.  This is useful if your template tag makes additional
  variables temporarily available in a part of a template.  %Template tags
  should only modify layers of context that they push themselves, and
  should pop any layers created before finishing its rendering step.

  See for example the \c {{% with %}} tag. In a template such as

  \code
    Some content
    {% with person.name|toUpper as lowerName %}
      Name is {% lowerName %}
    {% endwith %}
  \endcode

  In this case, lowerName is available in the context only between the
  \c {{% with %}} and \c {{% endwith %}} tags. The implementation of
  the \c {{% with %}} tag render method is:

  \code
    void WithNode::render( OutputStream *stream, Context *c ) const
    {
      c->push();
      // {% with m_filterExpression as m_name %}
      c->insert( m_name, m_filterExpression.resolve( c ) );
      m_list.render( stream, c );
      c->pop(); // The section of context defining m_name is removed.
    }
  \endcode

  Note that a Context may temporarily override a variable in a parent
  context. This is why it is important to push a new context when adding
  items to context and pop it when finished.

  \code
    Some content
    {% with "foo" as var %}
      Var is {% var %}         // Var is "foo"
      {% with "bar" as var %}
        Var is {% var %}       // Var is "bar"
      {% endwith %}
      Var is {% var %}         // Var is "foo"
    {% endwith %}
  \endcode
*/
class KTEXTTEMPLATE_EXPORT Context
{
public:
    /*!
      Creates an empty context
    */
    Context();
    /*!
      Sets every key in the hash as a property name with the variant as the
      value.
    */
    explicit Context(const QVariantHash &hash);

    Context(const Context &other);

    Context &operator=(const Context &other);

    /*!
      \internal

      Whether to automatically escape all context content. This is not usually
      used directly. Use the \c {{% autoescape %}} tag instead.
    */
    bool autoEscape() const;

    /*!
      \internal

      Sets whether to automatically escape all context content. This is not
      usually used directly. Use the \c {{% autoescape %}} tag instead.
    */
    void setAutoEscape(bool autoescape);

    ~Context();

    /*!
      Returns the context object identified by the key \a str
    */
    QVariant lookup(const QString &str) const;

    /*!
      Insert the context object \a object identified by \a name into
      the Context.
    */
    void insert(const QString &name, QObject *object);

    /*!
      Insert the context object \a variant identified by \a name into
      the Context.
    */
    void insert(const QString &name, const QVariant &variant);

    /*!
      Pushes a new context.
    */
    void push();

    /*!
      Pops the context.
    */
    void pop();

    /*!
      \internal
      Returns the context hash at depth \a depth.
    */
    QVariantHash stackHash(int depth) const;

    /*!
      \internal
      Returns whether template being rendered is being mutated.
    */
    bool isMutating() const;

    /*!
      \internal
      Sets whether template being rendered is being mutated to \a mutating.
    */
    void setMutating(bool mutating);

    /*!
      \internal
    */
    void addExternalMedia(const QString &absolutePart, const QString &relativePart);

    /*!
      \internal
    */
    void clearExternalMedia();

    /*!
      Sets the \a localizer to be used.

      The Context takes ownerwhip of the localizer.
    */
    void setLocalizer(QSharedPointer<AbstractLocalizer> localizer);

    /*!
      Returns the localizer currently in use.
    */
    QSharedPointer<AbstractLocalizer> localizer() const;

    /*!
      Returns the external media encountered in the Template while rendering.
    */
    QList<std::pair<QString, QString>> externalMedia() const;

    /*!
      The type of urls to external media that should be put in the template.

      \value AbsoluteUrls Absolute URLs should be put in the template.
      \value RelativeUrls Relative URLs should be put in the template.
    */
    enum UrlType {
        AbsoluteUrls,
        RelativeUrls,
    };

    /*!
      Sets the type of external media URL to be used in the template to \a type.
    */
    void setUrlType(UrlType type);

    /*!
      The type of URL used in the template.
    */
    UrlType urlType() const;

    /*!
      Sets the relative path to external media to be used in templates to \a
      relativePath
    */
    void setRelativeMediaPath(const QString &relativePath);

    /*!
      The relative path to external media to be used in templates.
    */
    QString relativeMediaPath() const;

    /*!
      Returns a modifiable RenderContext. This may be used to make
      Template rendering threadsafe so that render state does not need to be
      stored in the Node implementation itself.
     */
    RenderContext *renderContext() const;

private:
    Q_DECLARE_PRIVATE(Context)
    ContextPrivate *const d_ptr;
};
}

#endif
