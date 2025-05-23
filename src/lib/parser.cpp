/*
  This file is part of the KTextTemplate library

  SPDX-FileCopyrightText: 2009, 2010 Stephen Kelly <steveire@gmail.com>

  SPDX-License-Identifier: LGPL-2.1-or-later

*/

#include "parser.h"

#include "engine.h"
#include "exception.h"
#include "nodebuiltins_p.h"
#include "taglibraryinterface.h"
#include "template.h"

using namespace KTextTemplate;

namespace KTextTemplate
{

class ParserPrivate
{
public:
    ParserPrivate(Parser *parser, const QList<Token> &tokenList)
        : q_ptr(parser)
        , m_tokenList(tokenList)
    {
    }

    NodeList extendNodeList(NodeList list, Node *node);

    /*
      Parses the template to create a Nodelist.
      The given parent is the parent of each node in the returned list.
    */
    NodeList parse(QObject *parent, const QStringList &stopAt);

    void openLibrary(TagLibraryInterface *library);
    Q_DECLARE_PUBLIC(Parser)
    Parser *const q_ptr;

    QList<Token> m_tokenList;

    QHash<QString, AbstractNodeFactory *> m_nodeFactories;
    QHash<QString, QSharedPointer<Filter>> m_filters;

    NodeList m_nodeList;
};
}

void ParserPrivate::openLibrary(TagLibraryInterface *library)
{
    Q_Q(Parser);

    auto ti = qobject_cast<TemplateImpl *>(q->parent());

    auto cengine = ti->engine();
    Q_ASSERT(cengine);
    auto engine = const_cast<Engine *>(cengine);

    auto factories = library->nodeFactories();
    for (auto nodeIt = factories.begin(), nodeEnd = factories.end(); nodeIt != nodeEnd; ++nodeIt) {
        nodeIt.value()->setEngine(engine);
        m_nodeFactories.insert(nodeIt.key(), nodeIt.value());
    }
    auto filters = library->filters();
    for (auto filterIt = filters.begin(), filterEnd = filters.end(); filterIt != filterEnd; ++filterIt) {
        auto f = QSharedPointer<Filter>(filterIt.value());
        m_filters.insert(filterIt.key(), f);
    }
}

Parser::Parser(const QList<Token> &tokenList, QObject *parent)
    : QObject(parent)
    , d_ptr(new ParserPrivate(this, tokenList))
{
    Q_D(Parser);

    auto ti = qobject_cast<TemplateImpl *>(parent);

    auto cengine = ti->engine();
    Q_ASSERT(cengine);

    auto engine = const_cast<Engine *>(cengine);
    engine->loadDefaultLibraries();
    for (const QString &libraryName : engine->defaultLibraries()) {
        auto library = engine->loadLibrary(libraryName);
        if (!library)
            continue;
        d->openLibrary(library);
    }
}

Parser::~Parser()
{
    // Don't delete filters here because filters must out-live the parser in the
    // filter expressions.
    qDeleteAll(d_ptr->m_nodeFactories);
    delete d_ptr;
}

void Parser::loadLib(const QString &name)
{
    Q_D(Parser);
    auto ti = qobject_cast<TemplateImpl *>(parent());
    auto cengine = ti->engine();
    Q_ASSERT(cengine);
    auto engine = const_cast<Engine *>(cengine);
    auto library = engine->loadLibrary(name);
    if (!library)
        return;
    d->openLibrary(library);
}

NodeList ParserPrivate::extendNodeList(NodeList list, Node *node)
{
    if (node->mustBeFirst() && list.containsNonText()) {
        throw KTextTemplate::Exception(TagSyntaxError,
                                       QStringLiteral("Node appeared twice in template: %1").arg(QLatin1String(node->metaObject()->className())));
    }

    list.append(node);
    return list;
}

void Parser::skipPast(const QString &tag)
{
    while (hasNextToken()) {
        const auto token = takeNextToken();
        if (token.tokenType == BlockToken && token.content == tag)
            return;
    }
    throw KTextTemplate::Exception(UnclosedBlockTagError, QStringLiteral("No closing tag found for %1").arg(tag));
}

QSharedPointer<Filter> Parser::getFilter(const QString &name) const
{
    Q_D(const Parser);
    const auto it = d->m_filters.constFind(name);
    if (it != d->m_filters.constEnd()) {
        return it.value();
    }
    throw KTextTemplate::Exception(UnknownFilterError, QStringLiteral("Unknown filter: %1").arg(name));
}

NodeList Parser::parse(Node *parent, const QString &stopAt)
{
    Q_D(Parser);
    return d->parse(parent, {stopAt});
}

NodeList Parser::parse(TemplateImpl *parent, const QStringList &stopAt)
{
    Q_D(Parser);
    return d->parse(parent, stopAt);
}

NodeList Parser::parse(Node *parent, const QStringList &stopAt)
{
    Q_D(Parser);
    return d->parse(parent, stopAt);
}

NodeList ParserPrivate::parse(QObject *parent, const QStringList &stopAt)
{
    Q_Q(Parser);
    NodeList nodeList;

    while (q->hasNextToken()) {
        const auto token = q->takeNextToken();
        if (token.tokenType == TextToken) {
            nodeList = extendNodeList(nodeList, new TextNode(token.content, parent));
        } else if (token.tokenType == VariableToken) {
            if (token.content.isEmpty()) {
                // Error. Empty variable
                QString message;
                Q_ASSERT(q->hasNextToken());
                message = QStringLiteral("Empty variable before \"%1\", line %2, %3")
                              .arg(q->takeNextToken().content.left(20))
                              .arg(token.linenumber)
                              .arg(q->parent()->objectName());
                throw KTextTemplate::Exception(EmptyVariableError, message);
            }

            FilterExpression filterExpression;
            try {
                filterExpression = FilterExpression(token.content, q);
            } catch (const KTextTemplate::Exception &e) {
                throw KTextTemplate::Exception(e.errorCode(),
                                               QStringLiteral("%1, line %2, %3").arg(e.what()).arg(token.linenumber).arg(q->parent()->objectName()));
            }

            nodeList = extendNodeList(nodeList, new VariableNode(filterExpression, parent));
        } else {
            Q_ASSERT(token.tokenType == BlockToken);
            const auto command = token.content.section(QLatin1Char(' '), 0, 0);
            if (stopAt.contains(command)) {
                // A matching token has been reached. Return control to
                // the caller. Put the token back on the token list so the
                // caller knows where it terminated.
                q->prependToken(token);
                return nodeList;
            }

            if (command.isEmpty()) {
                QString message;
                Q_ASSERT(q->hasNextToken());
                message = QStringLiteral("Empty block tag before \"%1\", line %2, %3")
                              .arg(token.content.left(20))
                              .arg(token.linenumber)
                              .arg(q->parent()->objectName());
                throw KTextTemplate::Exception(EmptyBlockTagError, message);
            }

            auto nodeFactory = m_nodeFactories[command];

            // unknown tag.
            if (!nodeFactory) {
                q->invalidBlockTag(token, command, stopAt);
            }

            // TODO: Make getNode take a Token instead?
            Node *n;
            try {
                n = nodeFactory->getNode(token.content, q);
            } catch (const KTextTemplate::Exception &e) {
                throw KTextTemplate::Exception(e.errorCode(),
                                               QStringLiteral("%1, line %2, %3").arg(e.what()).arg(token.linenumber).arg(q->parent()->objectName()));
            }
            if (!n) {
                throw KTextTemplate::Exception(
                    EmptyBlockTagError,
                    QStringLiteral("Failed to get node from %1, line %2, %3").arg(command).arg(token.linenumber).arg(q->parent()->objectName()));
            }

            n->setParent(parent);

            nodeList = extendNodeList(nodeList, n);
        }
    }

    if (!stopAt.isEmpty()) {
        const auto message =
            QStringLiteral("Unclosed tag in template %1. Expected one of: (%2)").arg(q->parent()->objectName(), stopAt.join(QChar::fromLatin1(' ')));
        throw KTextTemplate::Exception(UnclosedBlockTagError, message);
    }

    return nodeList;
}

bool Parser::hasNextToken() const
{
    Q_D(const Parser);
    return !d->m_tokenList.isEmpty();
}

Token Parser::takeNextToken()
{
    Q_D(Parser);
    return d->m_tokenList.takeFirst();
}

void Parser::removeNextToken()
{
    Q_D(Parser);
    d->m_tokenList.removeFirst();
}

void Parser::invalidBlockTag(const Token &token, const QString &command, const QStringList &stopAt)
{
    if (!stopAt.empty()) {
        throw KTextTemplate::Exception(
            InvalidBlockTagError,
            QStringLiteral("Invalid block tag on line %1: '%2', expected '%3'").arg(token.linenumber).arg(command, stopAt.join(QStringLiteral("', '"))));
    }
    throw KTextTemplate::Exception(InvalidBlockTagError,
                                   QStringLiteral("Invalid block tag on line %1: '%2\''. Did you forget "
                                                  "to register or load this tag?")
                                       .arg(token.linenumber)
                                       .arg(command));
}

void Parser::prependToken(const Token &token)
{
    Q_D(Parser);
    d->m_tokenList.prepend(token);
}

#include "moc_parser.cpp"
