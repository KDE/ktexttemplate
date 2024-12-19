/*
  This file is part of the KTextTemplate library

  SPDX-FileCopyrightText: 2010 Stephen Kelly <steveire@gmail.com>

  SPDX-License-Identifier: LGPL-2.1-or-later

*/

#ifndef KTEXTTEMPLATE_ABSTRACTLOCALIZER_H
#define KTEXTTEMPLATE_ABSTRACTLOCALIZER_H

#include "ktexttemplate_export.h"

#include <QLocale>
#include <QSharedPointer>
#include <QVariantList>

class QDateTime;

namespace KTextTemplate
{

/*!
  \class KTextTemplate::AbstractLocalizer
  \inheaderfile KTextTemplate/AbstractLocalizer
  \inmodule KTextTemplate

  \brief Interface for implementing an internationalization system.

  This class defines an interface for accessing an internationalization, such as
  QLocale/QTranslator or gettext/KLocale.
*/
class KTEXTTEMPLATE_EXPORT AbstractLocalizer
{
public:
    /*!
     Constructor
    */
    AbstractLocalizer();

    virtual ~AbstractLocalizer();

    /*!
      Processes the data in \a variant into a localized string.

      The base implementation can localize numbers, dates and times, and
      strings.
    */
    virtual QString localize(const QVariant &variant) const;

    /*!
      Returns the current locale as a string.
    */
    virtual QString currentLocale() const = 0;

    /*!
      Makes this localizer use the locale specified by \a localeName for output.
    */
    virtual void pushLocale(const QString &localeName) = 0;

    /*!
      Makes this localizer use the previous locale for output.
     */
    virtual void popLocale() = 0;

    /*!
      Loads the \a catalog from \a path.
    */
    virtual void loadCatalog(const QString &path, const QString &catalog) = 0;

    /*!
      Unloads the \a catalog
    */
    virtual void unloadCatalog(const QString &catalog) = 0;

    /*!
      Localizes \a number
    */
    virtual QString localizeNumber(int number) const = 0;

    /*!
      Localizes \a number
    */
    virtual QString localizeNumber(qreal number) const = 0;

    /*!
      Localizes \a value as a monetary value in the currency specified by \a
      currencyCode.
    */
    virtual QString localizeMonetaryValue(qreal value, const QString &currencyCode = {}) const = 0;

    /*!
      Localizes \a date with the specified \a formatType
    */
    virtual QString localizeDate(const QDate &date, QLocale::FormatType formatType = QLocale::ShortFormat) const = 0;

    /*!
      Localizes \a time with the specified \a formatType
    */
    virtual QString localizeTime(const QTime &time, QLocale::FormatType formatType = QLocale::ShortFormat) const = 0;

    /*!
      Localizes \a dateTime with the specified \a formatType
    */
    virtual QString localizeDateTime(const QDateTime &dateTime, QLocale::FormatType formatType = QLocale::ShortFormat) const = 0;

    /*!
      Localizes \a string with the specified \a arguments for substitution
    */
    virtual QString localizeString(const QString &string, const QVariantList &arguments = {}) const = 0;

    /*!
      Localizes \a string, disambiguated by \a context with the specified \a
      arguments for substitution
    */
    virtual QString localizeContextString(const QString &string, const QString &context, const QVariantList &arguments = {}) const = 0;

    /*!
      Localizes \a string or its \a pluralForm with the specified \a arguments
      for substitution
    */
    virtual QString localizePluralString(const QString &string, const QString &pluralForm, const QVariantList &arguments = {}) const = 0;

    /*!
      Localizes \a string or its \a pluralForm, disambiguated by \a context with
      the specified \a arguments for substitution
    */
    virtual QString
    localizePluralContextString(const QString &string, const QString &pluralForm, const QString &context, const QVariantList &arguments = {}) const = 0;

private:
    Q_DISABLE_COPY(AbstractLocalizer)
};
}

#endif
