/*
  This file is part of the KTextTemplate library

  SPDX-FileCopyrightText: 2009, 2010 Stephen Kelly <steveire@gmail.com>

  SPDX-License-Identifier: LGPL-2.1-or-later

*/

#include "range.h"

#include "exception.h"
#include "parser.h"

RangeNodeFactory::RangeNodeFactory() = default;

KTextTemplate::Node *RangeNodeFactory::getNode(const QString &tagContent, Parser *p) const
{
    auto expr = smartSplit(tagContent);

    expr.takeAt(0);
    auto numArgs = expr.size();
    if (numArgs != 1) {
        if (numArgs <= 2) {
            throw KTextTemplate::Exception(TagSyntaxError, QStringLiteral("'range' tag requires at least three arguments"));
        }

        if (expr.at(numArgs - 2) != QStringLiteral("as")) {
            throw KTextTemplate::Exception(TagSyntaxError, QStringLiteral("Invalid arguments to 'range' tag"));
        }
    }

    const auto name = (numArgs > 2) ? expr.at(numArgs - 1) : QString();
    if (numArgs > 2)
        numArgs -= 2;

    RangeNode *n = nullptr;

    switch (numArgs) {
    case 1:
        n = new RangeNode(name, FilterExpression(QChar::fromLatin1('0'), p), FilterExpression(expr.first(), p), p);
        break;
    case 2:
        n = new RangeNode(name, FilterExpression(expr.first(), p), FilterExpression(expr.at(1), p), p);
        break;
    case 3:
        n = new RangeNode(name, FilterExpression(expr.first(), p), FilterExpression(expr.at(1), p), FilterExpression(expr.at(2), p), p);
        break;
    default:
        return nullptr;
    }

    auto list = p->parse(n, QStringLiteral("endrange"));
    p->removeNextToken();

    n->setNodeList(list);
    return n;
}

RangeNode::RangeNode(const QString &name, const FilterExpression &startExpression, const FilterExpression &stopExpression, QObject *parent)
    : Node(parent)
    , m_name(name)
    , m_startExpression(startExpression)
    , m_stopExpression(stopExpression)
{
}

RangeNode::RangeNode(const QString &name,
                     const FilterExpression &startExpression,
                     const FilterExpression &stopExpression,
                     const FilterExpression &stepExpression,
                     QObject *parent)
    : Node(parent)
    , m_name(name)
    , m_startExpression(startExpression)
    , m_stopExpression(stopExpression)
    , m_stepExpression(stepExpression)
{
}

void RangeNode::setNodeList(const NodeList &list)
{
    m_list = list;
}

void RangeNode::render(OutputStream *stream, Context *c) const
{
    int start;
    int stop;
    int step;

    start = m_startExpression.resolve(c).value<int>();
    stop = m_stopExpression.resolve(c).value<int>();

    if (m_stepExpression.isValid()) {
        step = m_stepExpression.resolve(c).value<int>();
    } else {
        step = 1;
    }

    const auto insertContext = !m_name.isEmpty();

    Q_ASSERT(start < stop);

    for (auto i = start; i < stop; i += step) {
        if (insertContext) {
            c->push();
            c->insert(m_name, i);
        }
        m_list.render(stream, c);
        if (insertContext)
            c->pop();
    }
}

#include "moc_range.cpp"
