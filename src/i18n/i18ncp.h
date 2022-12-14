/*
  This file is part of the KTextTemplate library

  Copyright (c) 2010 Stephen Kelly <steveire@gmail.com>

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

#ifndef I18NCPNODE_H
#define I18NCPNODE_H

#include "node.h"

namespace KTextTemplate
{
class Parser;
}

using namespace KTextTemplate;

class I18ncpNodeFactory : public AbstractNodeFactory
{
    Q_OBJECT
public:
    I18ncpNodeFactory();

    Node *getNode(const QString &tagContent, Parser *p) const override;
};

class I18ncpVarNodeFactory : public AbstractNodeFactory
{
    Q_OBJECT
public:
    I18ncpVarNodeFactory();

    Node *getNode(const QString &tagContent, Parser *p) const override;
};

class I18ncpNode : public Node
{
    Q_OBJECT
public:
    I18ncpNode(const QString &contextText, const QString &sourceText, const QString &pluralTex, const QList<FilterExpression> &feList, QObject *parent = {});
    void render(OutputStream *stream, Context *c) const override;

private:
    QString m_contextText;
    QString m_sourceText;
    QString m_pluralText;
    QList<FilterExpression> m_filterExpressionList;
};

class I18ncpVarNode : public Node
{
    Q_OBJECT
public:
    I18ncpVarNode(const QString &contextText,
                  const QString &sourceText,
                  const QString &pluralText,
                  const QList<FilterExpression> &feList,
                  const QString &resultName,
                  QObject *parent = {});
    void render(OutputStream *stream, Context *c) const override;

private:
    QString m_contextText;
    QString m_sourceText;
    QString m_pluralText;
    QList<FilterExpression> m_filterExpressionList;
    QString m_resultName;
};

#endif
