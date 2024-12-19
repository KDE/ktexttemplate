/*
  This file is part of the KTextTemplate library

  SPDX-FileCopyrightText: 2010 Stephen Kelly <steveire@gmail.com>

  SPDX-License-Identifier: LGPL-2.1-or-later

*/

#ifndef KTEXTTEMPLATE_RENDERCONTEXT_H
#define KTEXTTEMPLATE_RENDERCONTEXT_H

#include "ktexttemplate_export.h"

#include <QVariantHash>

namespace KTextTemplate
{

class ContextPrivate;
class TemplateImpl;
class Node;

class RenderContextPrivate;

/*!
   \class KTextTemplate::RenderContext
   \inheaderfile KTextTemplate/RenderContext
   \inmodule KTextTemplate

   \brief Provides storage facility for state while rendering a template.

   This class can not be instatiated by users of it. An instance
   of RenderContext is available through Context::renderContext() for use
   with implementations of re-entrant template tags.
 */
class KTEXTTEMPLATE_EXPORT RenderContext
{
public:
    /*!
      Returns the data for the node \a scopeNode.
     */
    QVariant &data(const Node *const scopeNode);

    /*!
      Returns whether the RenderContext contains data for \a scopeNode.
     */
    bool contains(Node *const scopeNode) const;

    ~RenderContext();

private:
    RenderContext();

    void push();

    void pop();

private:
    friend class ContextPrivate;
    friend class TemplateImpl;

    Q_DISABLE_COPY(RenderContext)
    Q_DECLARE_PRIVATE(RenderContext)
    RenderContextPrivate *const d_ptr;
};
}

#endif
