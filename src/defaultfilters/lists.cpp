/*
  This file is part of the KTextTemplate library

  SPDX-FileCopyrightText: 2009, 2010, 2011 Stephen Kelly <steveire@gmail.com>

  SPDX-License-Identifier: LGPL-2.1-or-later

*/

#include "lists.h"

#include "metatype.h"
#include "util.h"
#include "variable.h"

#include <QDateTime>
#include <QRandomGenerator>
#include <QSequentialIterable>
#include <QtTypes>
#include <algorithm>

QVariant JoinFilter::doFilter(const QVariant &input, const QVariant &argument, bool autoescape) const
{
    if (!input.canConvert<QVariantList>())
        return {};

    auto iter = input.value<QSequentialIterable>();

    QString ret;
    for (auto it = iter.begin(); it != iter.end(); ++it) {
        const auto var = *it;
        auto s = getSafeString(var);
        if (autoescape)
            s = conditionalEscape(s);

        ret.append(s);
        if ((it + 1) != iter.end()) {
            auto argString = getSafeString(argument);
            ret.append(conditionalEscape(argString));
        }
    }
    return markSafe(ret);
}

QVariant LengthFilter::doFilter(const QVariant &input, const QVariant &argument, bool autoescape) const
{
    Q_UNUSED(autoescape)
    Q_UNUSED(argument)
    if (input.canConvert<QVariantList>())
        return input.value<QSequentialIterable>().size();

    if (input.userType() == qMetaTypeId<SafeString>() || input.userType() == qMetaTypeId<QString>())
        return getSafeString(input).get().size();

    return {};
}

QVariant LengthIsFilter::doFilter(const QVariant &input, const QVariant &argument, bool autoescape) const
{
    Q_UNUSED(autoescape)
    if (!input.isValid() || (input.userType() == qMetaTypeId<int>()) || (input.userType() == qMetaTypeId<QDateTime>()))
        return {};

    auto size = 0;
    if (input.canConvert<QVariantList>())
        size = input.value<QSequentialIterable>().size();
    else if (input.userType() == qMetaTypeId<SafeString>() || input.userType() == qMetaTypeId<QString>())
        size = getSafeString(input).get().size();

    bool ok;
    auto argInt = getSafeString(argument).get().toInt(&ok);

    if (!ok)
        return {};

    return size == argInt;
}

QVariant FirstFilter::doFilter(const QVariant &input, const QVariant &argument, bool autoescape) const
{
    Q_UNUSED(autoescape)
    Q_UNUSED(argument)

    if (!input.canConvert<QVariantList>())
        return {};

    auto iter = input.value<QSequentialIterable>();

    if (iter.size() == 0)
        return QString();

    return *iter.begin();
}

QVariant LastFilter::doFilter(const QVariant &input, const QVariant &argument, bool autoescape) const
{
    Q_UNUSED(autoescape)
    Q_UNUSED(argument)

    if (!input.canConvert<QVariantList>())
        return {};

    auto iter = input.value<QSequentialIterable>();

    if (iter.size() == 0)
        return QString();

    return *(iter.end() - 1);
}

QVariant RandomFilter::doFilter(const QVariant &input, const QVariant &argument, bool autoescape) const
{
    Q_UNUSED(autoescape)
    Q_UNUSED(argument)

    if (!input.canConvert<QVariantList>())
        return {};

    auto varList = input.value<QVariantList>();

    if (varList.isEmpty())
        return {};

    auto rnd = QRandomGenerator::global()->bounded((int)varList.size());
    return varList.at(rnd);
}

QVariant SliceFilter::doFilter(const QVariant &input, const QVariant &argument, bool autoescape) const
{
    Q_UNUSED(autoescape)
    auto argString = getSafeString(argument);
    auto splitterIndex = argString.get().indexOf(QLatin1Char(':'));
    QString inputString = getSafeString(input);
    if (inputString.isEmpty())
        return {};

    if (splitterIndex >= 0) {
        auto startString = argString.get().left(splitterIndex);
        auto endString = argString.get().mid(splitterIndex + 1);
        auto getIndex = [&inputString](const auto &str, int defaultValue) -> qsizetype {
            if (str.get().isEmpty()) {
                return defaultValue;
            }
            bool ok;
            int value = str.get().toInt(&ok);
            if (!ok) {
                return defaultValue;
            }
            if (value < 0) {
                value = inputString.size() + value;
            }
            value = std::clamp(value, 0, static_cast<int>(inputString.size()));
            return value;
        };
        int start = getIndex(startString, 0);
        int end = getIndex(endString, inputString.size());
        return inputString.mid(start, end > start ? (end - start) : 0);
    }
    return QString(inputString.at(argument.value<int>()));
}

QVariant MakeListFilter::doFilter(const QVariant &_input, const QVariant &argument, bool autoescape) const
{
    Q_UNUSED(autoescape)
    Q_UNUSED(argument)
    if (_input.userType() == qMetaTypeId<QVariantList>())
        return _input;
    if (_input.canConvert<QVariantList>())
        return _input.value<QVariantList>();

    auto input = _input;

    if (input.userType() == qMetaTypeId<int>())
        input.convert(QMetaType::fromType<QString>());

    if (input.userType() == qMetaTypeId<SafeString>() || input.userType() == qMetaTypeId<QString>()) {
        QVariantList list;
        const auto strings = getSafeString(input).get().split(QString(), Qt::SkipEmptyParts);
        list.reserve(strings.size());
        for (const auto &var : strings)
            list << QVariant(var);
        return list;
    }
    return {};
}

QVariant UnorderedListFilter::doFilter(const QVariant &input, const QVariant &argument, bool autoescape) const
{
    Q_UNUSED(argument)

    if (!input.canConvert<QVariantList>())
        return {};

    return markSafe(processList(input.value<QVariantList>(), 1, autoescape));
}

SafeString UnorderedListFilter::processList(const QVariantList &list, int tabs, bool autoescape) const
{
    QString indent;
    for (auto i = 0; i < tabs; ++i)
        indent.append(QLatin1Char('\t'));
    QStringList output;

    auto i = 0;
    auto listSize = list.size();
    while (i < listSize) {
        auto titleObject = list.at(i);
        auto title = getSafeString(titleObject);
        QString sublist;
        QVariant sublistItem;

        if (titleObject.userType() == qMetaTypeId<QVariantList>()) {
            sublistItem = titleObject;
            title.get().clear();
        } else if (i < listSize - 1) {
            auto nextItem = list.at(i + 1);
            if (nextItem.userType() == qMetaTypeId<QVariantList>()) {
                sublistItem = nextItem;
            }
            ++i;
        }
        if (sublistItem.isValid()) {
            sublist = processList(sublistItem.value<QVariantList>(), tabs + 1, autoescape);
            sublist = QStringLiteral("\n%1<ul>\n%2\n%3</ul>\n%4").arg(indent, sublist, indent, indent);
        }
        output.append(QStringLiteral("%1<li>%2%3</li>").arg(indent, autoescape ? conditionalEscape(title) : title, sublist));
        ++i;
    }

    // Should be QLatin1Char() ?
    return output.join(QChar::fromLatin1('\n'));
}

struct DictSortLessThan {
    bool operator()(const std::pair<QVariant, QVariant> &lp, const std::pair<QVariant, QVariant> &rp) const
    {
        const auto l = lp.first;
        const auto r = rp.first;
        switch (l.userType()) {
        case QMetaType::UnknownType:
            return (r.isValid());
        case QMetaType::Int:
            return l.value<int>() < r.value<int>();
        case QMetaType::UInt:
            return l.value<uint>() < r.value<uint>();
        case QMetaType::LongLong:
            return l.value<long long>() < r.value<long long>();
        case QMetaType::ULongLong:
            return l.value<unsigned long long>() < r.value<unsigned long long>();
        case QMetaType::Float:
            return l.value<float>() < r.value<float>();
        case QMetaType::Double:
            return l.value<double>() < r.value<double>();
        case QMetaType::QChar:
            return l.value<QChar>() < r.value<QChar>();
        case QMetaType::QDate:
            return l.value<QDate>() < r.value<QDate>();
        case QMetaType::QTime:
            return l.value<QTime>() < r.value<QTime>();
        case QMetaType::QDateTime:
            return l.value<QDateTime>() < r.value<QDateTime>();
        case QMetaType::QObjectStar:
            return l.value<QObject *>() < r.value<QObject *>();
        }
        if (l.userType() == qMetaTypeId<KTextTemplate::SafeString>()) {
            if (r.userType() == qMetaTypeId<KTextTemplate::SafeString>()) {
                return l.value<KTextTemplate::SafeString>().get() < r.value<KTextTemplate::SafeString>().get();
            }
            if (r.userType() == qMetaTypeId<QString>()) {
                return l.value<KTextTemplate::SafeString>().get() < r.value<QString>();
            }
        } else if (r.userType() == qMetaTypeId<KTextTemplate::SafeString>()) {
            if (l.userType() == qMetaTypeId<QString>()) {
                return l.value<QString>() < r.value<KTextTemplate::SafeString>().get();
            }
        } else if (l.userType() == qMetaTypeId<QString>()) {
            if (r.userType() == qMetaTypeId<QString>()) {
                return l.value<QString>() < r.value<QString>();
            }
        }
        return false;
    }
};

QVariant DictSortFilter::doFilter(const QVariant &input, const QVariant &argument, bool autoescape) const
{
    Q_UNUSED(autoescape)

    if (!input.canConvert<QVariantList>())
        return {};

    QList<std::pair<QVariant, QVariant>> keyList;
    const auto inList = input.value<QSequentialIterable>();
    for (const QVariant &item : inList) {
        auto var = item;

        const Variable v(getSafeString(argument));

        if (v.literal().isValid()) {
            var = MetaType::lookup(var, v.literal().value<QString>());
        } else {
            const auto lookups = v.lookups();
            for (const QString &lookup : lookups) {
                var = MetaType::lookup(var, lookup);
            }
        }
        keyList << std::make_pair(var, item);
    }

    DictSortLessThan lt;
    std::stable_sort(keyList.begin(), keyList.end(), lt);

    QVariantList outList;
    auto it = keyList.constBegin();
    const auto end = keyList.constEnd();
    for (; it != end; ++it) {
        outList << it->second;
    }
    return outList;
}
