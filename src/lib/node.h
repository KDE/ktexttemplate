/*
  This file is part of the KTextTemplate library

  SPDX-FileCopyrightText: 2009, 2010 Stephen Kelly <steveire@gmail.com>

  SPDX-License-Identifier: LGPL-2.1-or-later

*/

#ifndef KTEXTTEMPLATE_NODE_H
#define KTEXTTEMPLATE_NODE_H

// krazy:excludeall=dpointer

#include "context.h"
#include "filterexpression.h"
#include "ktexttemplate_export.h"
#include "outputstream.h"
#include "safestring.h"

#include <QStringList>

// Need these for inheriting from QList<T> to work
// http://lists.trolltech.com/qt-interest/2008-01/thread00578-0.html
#include <QList>
#include <QSet>

namespace KTextTemplate
{

class Engine;
class NodeList;
class TemplateImpl;

class NodePrivate;

/*!
  \class KTextTemplate::Node
  \inheaderfile KTextTemplate/Node
  \inmodule KTextTemplate

  \brief Base class for all nodes.

  The Node class can be implemented to make additional functionality
  available to Templates.

  A node is represented in template markup as content surrounded by percent
  signed tokens.

  \code
    text content
    {% some_tag arg1 arg2 %}
      text content
    {% some_other_tag arg1 arg2 %}
      text content
    {% end_some_other_tag %}
    text content
  \endcode

  This is parsed into a tree of **%Node** objects by an implementation of
  AbstractNodeFactory. The **%Node** objects can then later be rendered by their
  render method.

  Rendering a **%Node** will usually mean writing some output to the stream. The
  content written to the stream could be determined by the arguments to the tag,
  or by the content of child nodes between a start and end tag, or both.

  \sa FilterExpression
*/
class KTEXTTEMPLATE_EXPORT Node : public QObject
{
    Q_OBJECT
public:
    /*!
      Constructor.

      \a parent The parent QObject
    */
    explicit Node(QObject *parent = {});

    ~Node() override;

    /*!
      Reimplement this to render the template in the Context \a c.

      This will also involve calling render on and child nodes.
    */
    virtual void render(OutputStream *stream, Context *c) const = 0;

    /*!
      \internal
    */
    virtual bool mustBeFirst()
    { // krazy:exclude:inline
        return false;
    }

protected:
    /*!
      Renders the value \a input in the Context \a c. This will involve escaping
      \a input if necessary.

      This is only relevant to developing template tags.
    */
    void streamValueInContext(OutputStream *stream, const QVariant &input, KTextTemplate::Context *c) const;

    /*!
      Returns a raw pointer to the Template this **%Node** is in.
    */
    TemplateImpl *containerTemplate() const;

private:
    Q_DECLARE_PRIVATE(Node)
    NodePrivate *const d_ptr;
};

/*!
  \class KTextTemplate::NodeList
  \inheaderfile KTextTemplate/Node
  \inmodule KTextTemplate

  \brief A list of Nodes with some convenience API for rendering them.

  Typically, tags which have an end tag will create and later render a list of
  child nodes.

  This class contains API such as append and render to make creating
  such list easily.

  The findChildren method behaves similarly to the QObject::findChildren
  method, returning a list of nodes of a particular type from the Node objects
  contained in the list (and their children).
*/
class KTEXTTEMPLATE_EXPORT NodeList : public QList<KTextTemplate::Node *>
{
public:
    /*!
      Creates an empty NodeList.
    */
    NodeList();

    NodeList(const NodeList &list);

    NodeList &operator=(const NodeList &list);

    /*!
      Convenience constructor
    */
    /* implicit */ NodeList(const QList<KTextTemplate::Node *> &list);

    ~NodeList();

    /*!
      Appends \a node to the end of this **%NodeList**.
    */
    void append(KTextTemplate::Node *node);

    /*!
      Appends \a nodeList to the end of this **%NodeList**.
    */
    void append(const QList<KTextTemplate::Node *> &nodeList);

    /*!
      Returns true if this **%NodeList** contains non-text nodes.
    */
    bool containsNonText() const;

    /*!
      A recursive listing of nodes in this tree of type \a T.
    */
    template<typename T>
    QList<T> findChildren()
    {
        QList<T> children;
        QList<KTextTemplate::Node *>::const_iterator it;
        const QList<KTextTemplate::Node *>::const_iterator first = constBegin();
        const QList<KTextTemplate::Node *>::const_iterator last = constEnd();
        for (it = first; it != last; ++it) {
            T object = qobject_cast<T>(*it);
            if (object) {
                children << object;
            }
            children << (*it)->findChildren<T>();
        }
        return children;
    }

    /*!
      Renders the list of Nodes in the Context @p c.
    */
    void render(OutputStream *stream, Context *c) const;

private:
    bool m_containsNonText;
};

class AbstractNodeFactoryPrivate;

/*!
  \class KTextTemplate::AbstractNodeFactory
  \inheaderfile KTextTemplate/Node
  \inmodule KTextTemplate

  \brief Base class for all NodeFactories.

  This class can be used to make custom tags available to templates.
  The getNode method should be implemented to return a Node to be rendered.

  A node is represented in template markup as content surrounded by percent
  signed tokens.

  \code
    text content
    {% some_tag arg1 arg2 %}
      text content
    {% some_other_tag arg1 arg2 %}
      text content
    {% end_some_other_tag %}
    text content
  \endcode

  It is the responsibility of an **%AbstractNodeFactory** implementation to
  process the contents of a tag and return a Node implementation from its
  getNode method.

  The getNode method would for example be called with the tagContent
  \"some_tag arg1 arg2\". That content could then be split up, the
  arguments processed and a Node created

  \code
    Node* SomeTagFactory::getNode(const QString &tagContent, Parser *p) {
      QStringList parts = smartSplit( tagContent );

      parts.removeFirst(); // Remove the "some_tag" part.

      FilterExpression arg1( parts.first(), p );
      FilterExpression arg2( parts.at( 1 ), p );

      return new SomeTagNode( arg1, arg2, p );
    }
  \endcode

  The getNode implementation might also advance the parser. For example if
  we had a \c {{% times %}} tag which rendered content the amount of times it was
  given in its argument, it could be used like this:

  \code
    Some text content.
    {% times 5 %}
      the bit to be repeated
    {% end_times %}
    End text content
  \endcode

  The argument to \c {{% times %}} might not be a simple number, but could be a
  FilterExpression such as \"someobject.some_property|getDigit:1\".

  The implementation could look like

  \code
    Node* SomeOtherTagFactory::getNode(const QString &tagContent, Parser *p) {
      QStringList parts = smartSplit( tagContent );

      parts.removeFirst(); // Remove the "times" part.

      FilterExpression arg( parts.first(), p );

      auto node = new SomeTagNode( arg, p );
      auto childNodes = p->parse( node, "end_times" );
      node->setChildNodes( childNodes );
      p->removeNextToken();

      return node;
    }
  \endcode

  Note that it is necessary to invoke the parser to create the child nodes only
  after creating the Node to return. That node must be passed to the Parser to
  perform as the parent QObject to the child nodes.

  \sa Parser::parse
*/
class KTEXTTEMPLATE_EXPORT AbstractNodeFactory : public QObject
{
    Q_OBJECT
public:
    /*!
      Constructor.

      \a parent The parent QObject
    */
    explicit AbstractNodeFactory(QObject *parent = {});

    ~AbstractNodeFactory() override;

    /*!
      This method should be reimplemented to return a Node which can be
      rendered.

      The \a tagContent is the content of the tag including the tag name and
      arguments. For example, if the template content is \c {{% my_tag arg1
      arg2 %}}, the tagContent will be "my_tag arg1 arg2".

      The Parser \a p is available and can be advanced if appropriate. For
      example, if the tag has an end tag, the parser can be advanced to the end
      tag.
    */
    virtual Node *getNode(const QString &tagContent, Parser *p) const = 0;

    /*!
      \internal

      Sets the Engine which created this NodeFactory. Used by the
      ScriptableNodeFactory.
    */
    virtual void setEngine(Engine *)
    {
    }

protected:
    /*!
      Splits \a str into a list, taking quote marks into account.

      This is typically used in the implementation of getNode with the
      tagContent.

      If \a str is 'one "two three" four 'five " six' seven', the
      returned list will contain the following strings:
      \list
      \li one
      \li "two three"
      \li four
      \li five " six
      \li seven
      \endlist
    */
    Q_INVOKABLE QStringList smartSplit(const QString &str) const;

protected:
    /*!
      Returns a list of FilterExpression objects created with Parser \a p as
      described by the content of \a list.

      This is used for convenience when handling the arguments to a tag.
    */
    QList<FilterExpression> getFilterExpressionList(const QStringList &list, Parser *p) const;

private:
    Q_DECLARE_PRIVATE(AbstractNodeFactory)
    AbstractNodeFactoryPrivate *const d_ptr;
};
}

#endif
