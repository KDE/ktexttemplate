/*
  This file is part of the KTextTemplate library

  SPDX-FileCopyrightText: 2009, 2010 Stephen Kelly <steveire@gmail.com>

  SPDX-License-Identifier: LGPL-2.1-or-later

*/

#ifndef KTEXTTEMPLATE_FILTEREXPRESSION_H
#define KTEXTTEMPLATE_FILTEREXPRESSION_H

#include "variable.h"

#include "ktexttemplate_export.h"

namespace KTextTemplate
{
class Filter;
class OutputStream;
class Parser;
struct Token;

class FilterExpressionPrivate;

/*!
  \class KTextTemplate::FilterExpression
  \inheaderfile KTextTemplate/FilterExpression
  \inmodule KTextTemplate

  \brief A FilterExpression object represents a filter expression in a
  template.

  This class is only relevant if implementing custom tags or filters. Most of
  the API here is internal.
  Usually when implementing tags or filters, filter expressions will just be
  created and resolved.

  In template markup, a filter expression is a variable followed by one or more
  filters separated by pipes:

  Filter expressions may appear in variable nodes:
  \code
    {{ some_var|upper_filter|lower_filter }}
  \endcode

  Or as arguments to tags
  \code
    {% some_tag some_arg1|filter1|filter2 some_arg2|filter3 %}
  \endcode

  The **%FilterExpression** class would be used in the getNode implementation of
  the AbstractNodeFactory implementation for the some_tag tag.

  \code
    Node* SomeTagFactory::getNode(const QString &tagContent, Parser *p) const {
      auto parts = smartSplit( tagContent );

      parts.removeFirst(); // Remove the "some_tag" part.

      FilterExpression arg1( parts.first(), p );
      FilterExpression arg2( parts.at( 1 ), p );

      return new SomeTagNode( arg1, arg2, p );
    }
  \endcode

  \sa AbstractNodeFactory::getFilterExpressionList

  When implementing the Node::render method, the resolve method may be used
  to process the filter expression.

  For example, if our SomeTagNode was to concatenate the resolved
  values given as arguments:

  \code
    void SomeTagNode::render( QTextStream *stream, Context *c ) {
      m_arg1.resolve( stream, c );
      m_arg2.resolve( stream, c );
    }
  \endcode

  Because Filters are highly generic, they do not all write data to the stream.
  For example, a Filter might take as input a string, and return a list by
  splitting the string on commas, or a Filter might compare an input to its
  argument and return whether they are the same, but not write anything to the
  stream. For that reason, the resolve method writes data to the given
  stream, and returns the same data in its returned QVariant.

  The suitability of either of the resolve methods will depend on the
  implementation and requirements of your custom tag. For example if the
  SomeTagNode ran a comparison of the arguments:

  \code
    void SomeTagNode::render( QTextStream *stream, Context *c ) {
      QString first = m_arg1.resolve( c ).toString();
      QString second = m_arg2.resolve( c ).toString();

      if ( first == second )
        m_trueList.render( stream, c );
      else
        m_falseList.render( stream, c );
    }
  \endcode
*/
class KTEXTTEMPLATE_EXPORT FilterExpression
{
public:
    /*!
      Constructs an invalid FilterExpression.
    */
    FilterExpression();

    /*!
      Constructs a filter expression from the string \a varString. The Parser \a
      parser is used to retrieve filters.
    */
    FilterExpression(const QString &varString, KTextTemplate::Parser *parser);

    FilterExpression(const FilterExpression &other);

    ~FilterExpression();

    FilterExpression &operator=(const FilterExpression &other);

    /*!
      Returns the initial variable in the FilterExpression.
    */
    Variable variable() const;

    /*!
      Resolves the FilterExpression in the Context \a c and writes it to the
      stream \a stream.
    */
    QVariant resolve(OutputStream *stream, Context *c) const;

    /*!
      Resolves the FilterExpression in the Context \a c.
    */
    QVariant resolve(Context *c) const;

    /*!
      Returns whether the Filter resolves to true in the Context \a c.
    */
    bool isTrue(Context *c) const;

    /*!
      Returns a list for the FilterExpression.

      If the FilterExpression can not be resolved to a list, an empty list
      will be returned.
    */
    QVariantList toList(Context *c) const;

    /*!
      Returns whether a filter expression is valid.

      A FilterExpression is valid if all filters in the expression exist and
      the initial variable being filtered is valid.
    */
    bool isValid() const;

    /*!
      \internal
      Returns the list of filters in the FilterExpression.
    */
    QStringList filters() const;

private:
    Q_DECLARE_PRIVATE(FilterExpression)
    FilterExpressionPrivate *const d_ptr;
};
}

#endif
