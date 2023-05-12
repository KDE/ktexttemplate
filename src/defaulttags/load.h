/*
  This file is part of the KTextTemplate library

  SPDX-FileCopyrightText: 2009, 2010 Stephen Kelly <steveire@gmail.com>

  SPDX-License-Identifier: LGPL-2.1-or-later

*/

#ifndef LOADNODE_H
#define LOADNODE_H

#include "node.h"

using namespace KTextTemplate;

class LoadNodeFactory : public AbstractNodeFactory
{
public:
    LoadNodeFactory();

    Node *getNode(const QString &tagContent, Parser *p) const override;
};

class LoadNode : public Node
{
    Q_OBJECT
public:
    explicit LoadNode(QObject *parent = {});

    void render(OutputStream *stream, Context *c) const override;
};

#endif
