/*
  This file is part of the KTextTemplate library

  SPDX-FileCopyrightText: 2009, 2010 Stephen Kelly <steveire@gmail.com>

  SPDX-License-Identifier: LGPL-2.1-or-later

*/

#include "scriptablefilter.h"
#include "scriptablesafestring.h"

#include "util.h"

#include <QJSEngine>

ScriptableFilter::ScriptableFilter(const QJSValue &filterObject, QJSEngine *engine)
    : m_filterObject(filterObject)
    , m_scriptEngine(engine)
{
}

ScriptableFilter::~ScriptableFilter() = default;

bool ScriptableFilter::isSafe() const
{
    auto safety = m_filterObject.property(QStringLiteral("isSafe"));
    if (safety.isBool()) {
        return safety.toBool();
    }
    return false;
}

QVariant ScriptableFilter::doFilter(const QVariant &input, const QVariant &argument, bool autoescape) const
{
    Q_UNUSED(autoescape)
    QJSValueList args;
    if (input.userType() == qMetaTypeId<QVariantList>()) {
        auto inputList = input.value<QVariantList>();
        auto array = m_scriptEngine->newArray(inputList.size());
        for (auto i = 0; i < inputList.size(); ++i) {
            if (inputList.at(i).canConvert<QObject *>()) {
                array.setProperty(i, m_scriptEngine->newQObject(inputList.at(i).value<QObject *>()));
            } else {
                array.setProperty(i, m_scriptEngine->toScriptValue(inputList.at(i)));
            }
        }
        args << array;
    } else {
        if (isSafeString(input)) {
            auto ssObj = new ScriptableSafeString(m_scriptEngine);
            ssObj->setContent(getSafeString(input));
            args << m_scriptEngine->newQObject(ssObj);
        } else if (input.canConvert<QObject *>()) {
            args << m_scriptEngine->newQObject(input.value<QObject *>());
        } else {
            args << m_scriptEngine->toScriptValue(input);
        }
    }

    if (argument.userType() == qMetaTypeId<SafeString>()) {
        auto ssObj = new ScriptableSafeString(m_scriptEngine);
        ssObj->setContent(getSafeString(argument));
        args << m_scriptEngine->newQObject(ssObj);
    } else {
        args << m_scriptEngine->toScriptValue(argument);
    }
    auto filterObject = m_filterObject;
    auto returnValue = filterObject.call(args);

    if (returnValue.isString()) {
        return getSafeString(returnValue.toString());
    }
    if (returnValue.isQObject()) {
        auto returnedObject = qjsvalue_cast<QObject *>(returnValue);
        auto returnedStringObject = qobject_cast<ScriptableSafeString *>(returnedObject);
        if (!returnedStringObject)
            return {};
        auto returnedString = returnedStringObject->wrappedString();
        return returnedString;
    } else if (returnValue.isArray()) {
        return qjsvalue_cast<QVariantList>(returnValue);
    } else if (const auto v = returnValue.toVariant(); !v.isNull()) {
        return v;
    }
    return {};
}
