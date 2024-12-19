/*
  This file is part of the KTextTemplate library

  SPDX-FileCopyrightText: 2009, 2010 Stephen Kelly <steveire@gmail.com>

  SPDX-License-Identifier: LGPL-2.1-or-later

*/

#ifndef KTEXTTEMPLATE_UTIL_P_H
#define KTEXTTEMPLATE_UTIL_P_H

#include "context.h"
#include "ktexttemplate_export.h"
#include "safestring.h"

#include <QVariant>

/*!
 * \namespace KTextTemplate
 * \inheaderfile KTextTemplate/Util
 * \inmodule KTextTemplate
 */
namespace KTextTemplate
{

/*!
  Returns whether the \a variant is evaluated to true.
*/
KTEXTTEMPLATE_EXPORT bool variantIsTrue(const QVariant &variant);

/*!
  Converts the \a input into its unescaped form.

  Double slashes are converted to single slashes, \\&quot is converted to &quot
  etc.
*/
KTEXTTEMPLATE_EXPORT QString unescapeStringLiteral(const QString &input);

/*!
  Marks the \a input as safe.
*/
KTEXTTEMPLATE_EXPORT KTextTemplate::SafeString markSafe(const KTextTemplate::SafeString &input);

/*!
  Marks the \a input as requiring escaping.
*/
KTEXTTEMPLATE_EXPORT KTextTemplate::SafeString markForEscaping(const KTextTemplate::SafeString &input);

/*!
  Retrieves and returns a SafeString from the \a input.
*/
KTEXTTEMPLATE_EXPORT KTextTemplate::SafeString getSafeString(const QVariant &input);

/*!
  Returns whether \a input contains a SafeString.
*/
KTEXTTEMPLATE_EXPORT bool isSafeString(const QVariant &input);

/*!
  Returns true if the type of \a input can be inserted into a rendered template
  directly.

  Note that lists, hashes and QObject*s can not be.
*/
KTEXTTEMPLATE_EXPORT bool supportedOutputType(const QVariant &input);

/*!
  Compares \a lhs and \a rhs for equality. SafeStrings are compared as raw
  QStrings. Their safeness is not part of the comparison.
*/
KTEXTTEMPLATE_EXPORT bool equals(const QVariant &lhs, const QVariant &rhs);

/*!
  Converts \a size into the nearest file size unit like MB or MiB, based on the
  \a unitSystem value. Use \c 2 for the \a unitSystem to get binary units, use
  \c 10 to get decimal units - by default, decimal units will be returned. The
  \a multiplier can be used if the input \a size is not in pure bytes. If \a
  size is for example given in \a KiB, use a multiplier of \a 1024. The returned
  pair will have the converted size as \a first and the unit as \a second.
*/
KTEXTTEMPLATE_EXPORT std::pair<qreal, QString> calcFileSize(qreal size, int unitSystem = 10, qreal multiplier = 1.0);

/*!
  \internal
  Returns the \a list as a formatted string. This is for debugging purposes
  only.
*/
KTEXTTEMPLATE_EXPORT KTextTemplate::SafeString toString(const QVariantList &list);
}

#endif
