/*
 * This file is part of the KTextTemplate library
 *
 * SPDX-FileCopyrightText: 2020 Matthias Fehring <mf@huessenbergnetz.de>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "l10n_filesize.h"

#include "abstractlocalizer.h"
#include "exception.h"
#include "parser.h"
#include "util.h"

#include <limits>

L10nFileSizeNodeFactory::L10nFileSizeNodeFactory() = default;

Node *L10nFileSizeNodeFactory::getNode(const QString &tagContent, Parser *p) const
{
    QStringList parts = smartSplit(tagContent);
    parts.removeFirst(); // not interested in the name of the tag
    if (parts.isEmpty()) {
        throw Exception(TagSyntaxError,
                        QStringLiteral("Error: l10n_filesize requires at least the "
                                       "file size as first parameter"));
    }

    FilterExpression size(parts.at(0), p);

    FilterExpression unitSystem;
    if (parts.size() > 1) {
        unitSystem = FilterExpression(parts.at(1), p);
    }

    FilterExpression precision;
    if (parts.size() > 2) {
        precision = FilterExpression(parts.at(2), p);
    }

    FilterExpression multiplier;
    if (parts.size() > 3) {
        multiplier = FilterExpression(parts.at(3), p);
    }

    return new L10nFileSizeNode(size, unitSystem, precision, multiplier, p);
}

L10nFileSizeVarNodeFactory::L10nFileSizeVarNodeFactory() = default;

Node *L10nFileSizeVarNodeFactory::getNode(const QString &tagContent, Parser *p) const
{
    QStringList parts = smartSplit(tagContent);
    parts.removeFirst(); // not interested in the name of the tag
    if (parts.size() < 2) {
        throw Exception(TagSyntaxError,
                        QStringLiteral("Error: l10n_filesize_var tag takes at least 2 "
                                       "arguments, the file size and the variable name"));
    }

    FilterExpression size(parts.at(0), p);

    FilterExpression unitSystem;
    if (parts.size() > 2) {
        unitSystem = FilterExpression(parts.at(1), p);
    }

    FilterExpression precision;
    if (parts.size() > 3) {
        precision = FilterExpression(parts.at(2), p);
    }

    FilterExpression multiplier;
    if (parts.size() > 4) {
        multiplier = FilterExpression(parts.at(3), p);
    }

    const auto resultName = parts.last();

    return new L10nFileSizeVarNode(size, unitSystem, precision, multiplier, resultName, p);
}

L10nFileSizeNode::L10nFileSizeNode(const FilterExpression &size,
                                   const FilterExpression &unitSystem,
                                   const FilterExpression &precision,
                                   const FilterExpression &multiplier,
                                   QObject *parent)
    : Node(parent)
    , m_size(size)
    , m_unitSystem(unitSystem)
    , m_precision(precision)
    , m_multiplier(multiplier)
{
}

void L10nFileSizeNode::render(OutputStream *stream, Context *c) const
{
    bool convertNumbers = true;

    qreal size = 0.0F;
    if (m_size.resolve(c).canConvert<qreal>()) {
        size = m_size.resolve(c).toReal(&convertNumbers);
    } else {
        size = getSafeString(m_size.resolve(c)).get().toDouble(&convertNumbers);
    }
    if (!convertNumbers) {
        qWarning("%s", "Failed to convert input file size into a floating point number.");
        return;
    }

    int unitSystem = m_unitSystem.isValid() ? m_unitSystem.resolve(c).toInt(&convertNumbers) : 10;
    if (!convertNumbers) {
        qWarning("%s",
                 "Failed to convert unit system for file size into integer "
                 "value. Using default decimal system as default.");
        unitSystem = 10;
    }

    int precision = m_precision.isValid() ? m_precision.resolve(c).toInt(&convertNumbers) : 2;
    if (!convertNumbers) {
        qWarning("%s",
                 "Failed to convert decimal precision for file size into an "
                 "integer value. Using default value 2.");
        precision = 2;
    }

    qreal multiplier = m_multiplier.isValid() ? m_multiplier.resolve(c).toReal(&convertNumbers) : 1.0F;
    if (!convertNumbers) {
        qWarning("%s",
                 "Failed to convert multiplier file size into a floating "
                 "point number. Using default value 1.0.");
        multiplier = 1.0F;
    }

    if (multiplier == 0.0F) {
        qWarning("%s",
                 "It makes no sense to multiply the file size by zero. Using "
                 "default value 1.0.");
        multiplier = 1.0F;
    }

    const qreal sizeMult = size * multiplier;

    if (unitSystem == 10) {
        if ((sizeMult > -1000) && (sizeMult < 1000)) {
            precision = 0;
        }
    } else if (unitSystem == 2) {
        if ((sizeMult > -1024) && (sizeMult < 1024)) {
            precision = 0;
        }
    }

    QString resultString;

    if (sizeMult > static_cast<qreal>(std::numeric_limits<qint64>::min()) && sizeMult < static_cast<qreal>(std::numeric_limits<qint64>::max())) {
        QLocale l(c->localizer()->currentLocale());
        QLocale::DataSizeFormats format = unitSystem == 10 ? QLocale::DataSizeSIFormat : QLocale::DataSizeIecFormat;

        resultString = l.formattedDataSize(static_cast<qint64>(sizeMult), precision, format);

    } else {
        const std::pair<qreal, QString> fspair = calcFileSize(size, unitSystem, multiplier);

        if (precision == 2) {
            resultString = c->localizer()->localizeNumber(fspair.first) + QChar(QChar::Space) + fspair.second;
        } else {
            QLocale l(c->localizer()->currentLocale());
            resultString = l.toString(fspair.first, 'f', precision) + QChar(QChar::Space) + fspair.second;
        }
    }

    streamValueInContext(stream, resultString, c);
}

L10nFileSizeVarNode::L10nFileSizeVarNode(const FilterExpression &size,
                                         const FilterExpression &unitSystem,
                                         const FilterExpression &precision,
                                         const FilterExpression &multiplier,
                                         const QString &resultName,
                                         QObject *parent)
    : Node(parent)
    , m_size(size)
    , m_unitSystem(unitSystem)
    , m_precision(precision)
    , m_multiplier(multiplier)
    , m_resultName(resultName)
{
}

void L10nFileSizeVarNode::render(OutputStream *stream, Context *c) const
{
    Q_UNUSED(stream)
    bool convertNumbers = true;

    qreal size = 0.0F;
    if (m_size.resolve(c).canConvert<qreal>()) {
        size = m_size.resolve(c).toReal(&convertNumbers);
    } else {
        size = getSafeString(m_size.resolve(c)).get().toDouble(&convertNumbers);
    }
    if (!convertNumbers) {
        qWarning("%s", "Failed to convert input file size into a floating point number.");
        return;
    }

    int unitSystem = m_unitSystem.isValid() ? m_unitSystem.resolve(c).toInt(&convertNumbers) : 10;
    if (!convertNumbers) {
        qWarning("%s",
                 "Failed to convert unit system for file size into integer "
                 "value. Using default decimal system.");
        unitSystem = 10;
    }

    int precision = m_precision.isValid() ? m_precision.resolve(c).toInt(&convertNumbers) : 2;
    if (!convertNumbers) {
        qWarning("%s",
                 "Failed to convert decimal precision for file size into an "
                 "integer value. Using default value 2.");
        precision = 2;
    }

    qreal multiplier = m_multiplier.isValid() ? m_multiplier.resolve(c).toReal(&convertNumbers) : 1.0F;
    if (!convertNumbers) {
        qWarning("%s",
                 "Failed to convert multiplier file size into a floating "
                 "point number. Using default value 1.0.");
        multiplier = 1.0F;
    }

    if (multiplier == 0.0F) {
        qWarning("%s",
                 "It makes no sense to mulitply the file size by zero. Using "
                 "default value 1.0.");
        multiplier = 1.0F;
    }

    const double sizeMult = size * multiplier;

    if (unitSystem == 10) {
        if ((sizeMult > -1000) && (sizeMult < 1000)) {
            precision = 0;
        }
    } else if (unitSystem == 2) {
        if ((sizeMult > -1024) && (sizeMult < 1024)) {
            precision = 0;
        }
    }

    QString resultString;

    if (sizeMult > static_cast<qreal>(std::numeric_limits<qint64>::min()) && sizeMult < static_cast<qreal>(std::numeric_limits<qint64>::max())) {
        QLocale l(c->localizer()->currentLocale());
        QLocale::DataSizeFormats format = unitSystem == 10 ? QLocale::DataSizeSIFormat : QLocale::DataSizeIecFormat;

        resultString = l.formattedDataSize(static_cast<qint64>(sizeMult), precision, format);

    } else {
        const std::pair<qreal, QString> fspair = calcFileSize(size, unitSystem, multiplier);

        if (precision == 2) {
            resultString = c->localizer()->localizeNumber(fspair.first) + QChar(QChar::Space) + fspair.second;
        } else {
            QLocale l(c->localizer()->currentLocale());
            resultString = l.toString(fspair.first, 'f', precision) + QChar(QChar::Space) + fspair.second;
        }
    }

    c->insert(m_resultName, resultString);
}

#include "moc_l10n_filesize.cpp"
