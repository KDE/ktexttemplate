/*
  This file is part of the KTextTemplate library

  SPDX-FileCopyrightText: 2009, 2010 Stephen Kelly <steveire@gmail.com>

  SPDX-License-Identifier: LGPL-2.1-or-later

*/

#include "filter.h"

using namespace KTextTemplate;

class KTextTemplate::FilterPrivate
{
public:
    OutputStream *m_stream = nullptr;
    Context *m_context = nullptr;
};

Filter::Filter() = default;
Filter::~Filter()
{
    delete d_ptr;
}

void Filter::forgottenBaseCtorRemoveInKF7()
{
    d_ptr = nullptr;
}

void Filter::setStream(KTextTemplate::OutputStream *stream)
{
    if (!d_ptr) {
        d_ptr = new FilterPrivate;
    }
    d_ptr->m_stream = stream;
}

SafeString Filter::escape(const QString &input) const
{
    return d_ptr->m_stream->escape(input);
}

SafeString Filter::escape(const SafeString &input) const
{
    if (input.isSafe()) {
        return {d_ptr->m_stream->escape(input), SafeString::IsSafe};
    }
    return d_ptr->m_stream->escape(input);
}

SafeString Filter::conditionalEscape(const SafeString &input) const
{
    if (!input.isSafe()) {
        return d_ptr->m_stream->escape(input);
    }
    return input;
}

bool Filter::isSafe() const
{
    return false;
}

Context *Filter::context() const
{
    return d_ptr ? d_ptr->m_context : nullptr;
}

void Filter::setContext(KTextTemplate::Context *context)
{
    if (!d_ptr) {
        d_ptr = new FilterPrivate;
    }
    d_ptr->m_context = context;
}
