/*
  This file is part of the KTextTemplate library

  SPDX-FileCopyrightText: 2009, 2010 Stephen Kelly <steveire@gmail.com>

  SPDX-License-Identifier: LGPL-2.1-or-later

*/

#ifndef KTEXTTEMPLATE_SAFESTRING_H
#define KTEXTTEMPLATE_SAFESTRING_H

#include "ktexttemplate_export.h"

#include <QString>
#include <QVariant>

namespace KTextTemplate
{

/*!
  \class KTextTemplate::SafeString
  \inheaderfile KTextTemplate/SafeString
  \inmodule KTextTemplate

  \brief A QString wrapper class for containing whether a string is safe or
  needs to be escaped.

  This allows lazy escaping of strings. Otherwise a string may be escaped
  multiple times where it should only be escaped once.

  The SafeString class itself provides information about whether a string
  is safe from further escaping through the isSafe method. The actual
  string content held by the SafeString instance is available through the
  get method. The get method returns a QString subclass which should
  be used like any other QString. The difference is that all methods on
  NestedString return a SafeString instead of a QString.

  \code
    SafeString s("this & that", SafeString::IsSafe);
    s.get().replace( "this", "these" ).toUpper();

    qDebug() << s.get() << s.isSafe(); // outputs "these & that" false
  \endcode

  Note that most operations on strings make the string unsafe. For example,
  while "K &amp; R" is safe, using replace("m", "n") will result in
  "K &anp; R", which is unsafe. Likewise using upper() will return
  "K &AMP; R", which is unsafe. Because the SafeString can not
  determine whether a method call with particular arguments will change
  a SafeString from being safe to being unsafe, any operation which can
  possibly make the string unsafe does cause the string to become unsafe. It is
  then up to the caller to restore safe-ness if needed.

  NestedString has overloads for SafeStrings whereever appropriate so that
  strings remain marked as safe where possible.

  For example:

  \code
    SafeString s1("this & that", SafeString::IsSafe);
    s2 = s1;
    s1.append( QString( " & the other" ) );
    // s1 is now "this & that & the other" and is unsafe.

    SafeString s3(" Wobl & Bob", SafeString::IsSafe);
    s2.append(s3);
    // Both s2 and s3 are safe, and append is a safe operation, so s2
    // is still safe
  \endcode

  The SafeString class has appropriate operator overloads to make it
  convenient to use in methods returning a QVariant, such as Filter::doFilter,
  or as a QString. Note that a raw QString is essentially the same
  as a SafeString which is marked as unsafe.
*/
class KTEXTTEMPLATE_EXPORT SafeString
{
public:
    /*!
      Possible safety states of a SafeString

      \value IsSafe The string is safe and requires no further escaping
      \value IsNotSafe The string is not safe. It will be escaped before being added to the output of rendering.
    */
    enum Safety {
        IsSafe,
        IsNotSafe
    };

    /*!
      Constructs an empty SafeString.
    */
    SafeString();

    SafeString(const SafeString &safeString);

    /*!
      Constructs a SafeString with the content \a str whose safety is given
      by \a safe.
    */
    SafeString(const QString &str, bool safe);

    /*!
      Constructs a SafeString with the content \a str whose safety is given
      by \a safety.
    */
    /* implicit */ SafeString(const QString &str,
                              Safety safety = IsNotSafe); // krazy:exclude=explicit

    ~SafeString();

    /*!
      Set whether the string should be escaped.
    */
    void setNeedsEscape(bool needsEscape);

    /*!
      Whether the string needs to be escaped.
    */
    bool needsEscape() const;

    /*!
      Whether the string is safe.
    */
    bool isSafe() const;

    /*!
      Set whether the string is safe.
    */
    void setSafety(Safety safety);

    /*!
      \inmodule KTextTemplate
      The NestedString is a QString whose methods always return a
      SafeString

      This class is largely an implementation detail. See the SafeString
      documentation for details.
    */
    class KTEXTTEMPLATE_EXPORT NestedString : public QString
    {
        friend class SafeString;
        SafeString *m_safeString;

    public:
        explicit NestedString(SafeString *safeString);
        NestedString(const QString &content, SafeString *safeString);

        SafeString &append(const SafeString &str);
        SafeString &append(const QString &str);
        SafeString &append(const QLatin1String &str);
#ifndef QT_NO_CAST_FROM_ASCII
        SafeString &append(const QByteArray &ba)
        {
            QString::append(ba);
            m_safeString->m_safety = IsNotSafe;
            return *m_safeString;
        }

        SafeString &append(const char *str)
        {
            QString::append(str);
            m_safeString->m_safety = IsNotSafe;
            return *m_safeString;
        }
#endif
        SafeString &append(const QChar ch);

        SafeString &fill(QChar ch, int size = -1);

        SafeString &insert(int position, const SafeString &str);
        SafeString &insert(int position, const QString &str);
        SafeString &insert(int position, const QLatin1String &str);
        SafeString &insert(int position, const QChar *unicode, int size);
        SafeString &insert(int position, QChar ch);

        SafeString left(int n) const;
        SafeString leftJustified(int width, QChar fill = QLatin1Char(' '), bool truncate = {}) const;
        SafeString mid(int position, int n = -1) const;

        SafeString normalized(NormalizationForm mode) const;
        SafeString normalized(NormalizationForm mode, QChar::UnicodeVersion version) const;

        SafeString &prepend(const SafeString &str);
        SafeString &prepend(const QString &str);
        SafeString &prepend(const QLatin1String &str);
#ifndef QT_NO_CAST_FROM_ASCII
        SafeString &prepend(const QByteArray &ba)
        {
            QString::prepend(ba);
            m_safeString->m_safety = IsNotSafe;
            return *m_safeString;
        }

        SafeString &prepend(const char *str)
        {
            QString::prepend(str);
            m_safeString->m_safety = IsNotSafe;
            return *m_safeString;
        }
#endif
        SafeString &prepend(QChar ch);

        void push_back(const SafeString &other);
        void push_front(const SafeString &other);

        SafeString &remove(int position, int n);
        SafeString &remove(QChar ch, Qt::CaseSensitivity cs = Qt::CaseSensitive);
        SafeString &remove(const SafeString &str, Qt::CaseSensitivity cs = Qt::CaseSensitive);
        SafeString &remove(const QString &str, Qt::CaseSensitivity cs = Qt::CaseSensitive);
        SafeString &remove(const QRegularExpression &rx);
        SafeString repeated(int times) const;
        SafeString &replace(int position, int n, const SafeString &after);
        SafeString &replace(int position, int n, const QString &after);
        SafeString &replace(int position, int n, const QChar *unicode, int size);
        SafeString &replace(int position, int n, QChar after);
        SafeString &replace(const SafeString &before, const SafeString &after, Qt::CaseSensitivity cs = Qt::CaseSensitive);
        SafeString &replace(const QString &before, const SafeString &after, Qt::CaseSensitivity cs = Qt::CaseSensitive);
        SafeString &replace(const SafeString &before, const QString &after, Qt::CaseSensitivity cs = Qt::CaseSensitive);
        SafeString &replace(const QString &before, const QString &after, Qt::CaseSensitivity cs = Qt::CaseSensitive);
        SafeString &replace(const QChar *before, int blen, const QChar *after, int alen, Qt::CaseSensitivity cs = Qt::CaseSensitive);
        SafeString &replace(QChar ch, const SafeString &after, Qt::CaseSensitivity cs = Qt::CaseSensitive);
        SafeString &replace(QChar ch, const QString &after, Qt::CaseSensitivity cs = Qt::CaseSensitive);
        SafeString &replace(QChar before, QChar after, Qt::CaseSensitivity cs = Qt::CaseSensitive);
        SafeString &replace(const QLatin1String &before, const QLatin1String &after, Qt::CaseSensitivity cs = Qt::CaseSensitive);
        SafeString &replace(const QLatin1String &before, const SafeString &after, Qt::CaseSensitivity cs = Qt::CaseSensitive);
        SafeString &replace(const QLatin1String &before, const QString &after, Qt::CaseSensitivity cs = Qt::CaseSensitive);
        SafeString &replace(const SafeString &before, const QLatin1String &after, Qt::CaseSensitivity cs = Qt::CaseSensitive);
        SafeString &replace(const QString &before, const QLatin1String &after, Qt::CaseSensitivity cs = Qt::CaseSensitive);
        SafeString &replace(QChar c, const QLatin1String &after, Qt::CaseSensitivity cs = Qt::CaseSensitive);
        SafeString &replace(const QRegularExpression &rx, const SafeString &after);
        SafeString &replace(const QRegularExpression &rx, const QString &after);

        SafeString right(int n) const;
        SafeString rightJustified(int width, QChar fill = QLatin1Char(' '), bool truncate = {}) const;

        SafeString section(QChar sep, int start, int end = -1, SectionFlags flags = SectionDefault) const;
        SafeString section(const SafeString &sep, int start, int end = -1, SectionFlags flags = SectionDefault) const;
        SafeString section(const QString &sep, int start, int end = -1, SectionFlags flags = SectionDefault) const;
        SafeString section(const QRegularExpression &reg, int start, int end = -1, SectionFlags flags = SectionDefault) const;
        SafeString &setNum(int n, int base = 10);
        SafeString &setNum(uint n, int base = 10);
        SafeString &setNum(long n, int base = 10);
        SafeString &setNum(ulong n, int base = 10);
        SafeString &setNum(qlonglong n, int base = 10);
        SafeString &setNum(qulonglong n, int base = 10);
        SafeString &setNum(short n, int base = 10);
        SafeString &setNum(ushort n, int base = 10);
        SafeString &setNum(double n, char format = 'g', int precision = 6);
        SafeString &setNum(float n, char format = 'g', int precision = 6);
        SafeString &setUnicode(const QChar *unicode, int size);
        SafeString &setUtf16(const ushort *unicode, int size);
        SafeString simplified() const;

        QStringList split(const SafeString &sep, Qt::SplitBehavior behavior = Qt::KeepEmptyParts, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
        QStringList split(const QString &sep, Qt::SplitBehavior behavior = Qt::KeepEmptyParts, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
        QStringList split(const QChar &sep, Qt::SplitBehavior behavior = Qt::KeepEmptyParts, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
        QStringList split(const QRegularExpression &rx, Qt::SplitBehavior behavior = Qt::KeepEmptyParts) const;

        SafeString toLower() const;
        SafeString toUpper() const;
        SafeString trimmed() const;

        void chop(int n);
    };

    /*!
      Returns the String held by this SafeString
    */
    const NestedString &get() const
    {
        return m_nestedString;
    }

    /*!
      Returns the String held by this SafeString
    */
    NestedString &get()
    {
        return m_nestedString;
    }

    /*!
      Convenience operator for treating a SafeString like a QString.
    */
    operator QString() const
    {
        return m_nestedString;
    }

    /*!
      Assignment operator.
    */
    SafeString &operator=(const SafeString &str);

    /*!
      Returns a concatenation of this with \a str.

      The result is not safe because str is not safe.
    */
    SafeString operator+(const QString &str);

    /*!
      Returns a concatenation of this with \a str.

      The result is safe if both this and str are safe.
    */
    SafeString operator+(const SafeString &str);

    /*!
      Appends the content of \a str to this.

      The result is not safe because \a str is not safe.
    */
    SafeString &operator+=(const QString &str);

    /*!
      Appends the content of \a str to this.

      The result is safe if both this and \a str are safe.
    */
    SafeString &operator+=(const SafeString &str);

    /*!
      Returns true if the content of \a other matches the content of this.

      Safeness and needing escaping are not accounted for in the comparison.
    */
    bool operator==(const SafeString &other) const;

    /*!
      Returns true if the content of \a other matches the content of this.

      Safeness and needing escaping are not accounted for in the comparison.
    */
    bool operator==(const QString &other) const;

    /*!
      Convenience operator for storing a SafeString in a QVariant.
    */
    operator QVariant() const
    {
        return QVariant::fromValue(*this);
    }

private:
    NestedString m_nestedString;
    Safety m_safety;
    bool m_needsescape;
};
}

Q_DECLARE_METATYPE(KTextTemplate::SafeString)

#endif
