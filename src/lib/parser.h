/*
  This file is part of the KTextTemplate library

  SPDX-FileCopyrightText: 2009, 2010 Stephen Kelly <steveire@gmail.com>

  SPDX-License-Identifier: LGPL-2.1-or-later

*/

#ifndef KTEXTTEMPLATE_PARSER_H
#define KTEXTTEMPLATE_PARSER_H

#include "filter.h"
#include "node.h"
#include "token.h"

#include <QStringList>

namespace KTextTemplate
{
class TemplateImpl;

class ParserPrivate;

/// @headerfile parser.h <KTextTemplate/Parser>

/**
  @brief The **%Parser** class processes a string template into a tree of nodes.

  For application developers, this class is internal.

  For template tag authors it may be necessary to advance the parser and process
  contained tags if the tag works in a tag -- endtag fashion.

  @author Stephen Kelly <steveire@gmail.com>
*/
class KTEXTTEMPLATE_EXPORT Parser : public QObject
{
    Q_OBJECT
public:
    /**
      Constructor.

      Initialises the **%Parser** with the @p tokenList.
    */
    Parser(const QList<Token> &tokenList, QObject *parent);

    /**
      Destructor.
    */
    ~Parser() override;

    /**
      Advance the parser, using @p parent as the parent of new Nodes. The parser
      will stop if it encounters a tag which is contained in the list @p stopAt.

      For example, the @gr_tag{if} tag would stopAt both @gr_tag{endif} and
      @gr_tag{else} tags.

      @see AbstractNodeFactory::getNode
    */
    NodeList parse(Node *parent, const QStringList &stopAt = {});

    /**
      This is an overloaded method.
      @see parse.
    */
    NodeList parse(TemplateImpl *parent, const QStringList &stopAt = {});

    /**
      This is an overloaded method.
      @see parse.
    */
    NodeList parse(Node *parent, const QString &stopAt);

    /**
      Returns the filter object called @p name or an invalid object if no filter
      with that name is loaded.
    */
    QSharedPointer<Filter> getFilter(const QString &name) const;

    /**
      Advances the parser to the tag @p tag. This method is similar to @ref parse,
      but it does not create nodes for tags encountered.
    */
    void skipPast(const QString &tag);

    /**
      Returns the next token to be processed by the parser. This can be examined
      in template tag implementations to determine why parsing stopped.

      For example, if the @gr_tag{if} tag, parsing may stop at an @gr_tag{else}
      tag, in which case parsing should be restarted, or it could stop at an
      @gr_tag{endif} tag, in which case parsing is finished for that node.
    */
    Token takeNextToken();

    /**
      Returns whether the parser has another token to process.
    */
    bool hasNextToken() const;

    /**
      Deletes the next token available to the parser.
    */
    void removeNextToken();

    void invalidBlockTag(const Token &token, const QString &command, const QStringList &stopAt = {});

#ifndef K_DOXYGEN
    /**
      @internal

      Used by the @gr_tag{load} tag to load libraries.
    */
    void loadLib(const QString &name);
#endif

protected:
    /**
      Puts the token @p token to the front of the list to be processed by the
      parser.
    */
    void prependToken(const Token &token);

private:
    Q_DECLARE_PRIVATE(Parser)
    ParserPrivate *const d_ptr;
};
}

#endif
