/*
  This file is part of the KTextTemplate library

  SPDX-FileCopyrightText: 2010 Stephen Kelly <steveire@gmail.com>

  SPDX-License-Identifier: LGPL-2.1-or-later

*/

#ifndef KTEXTTEMPLATE_OUTPUTSTREAM_H
#define KTEXTTEMPLATE_OUTPUTSTREAM_H

#include "ktexttemplate_export.h"

#include <QSharedPointer>
#include <QTextStream>

namespace KTextTemplate
{

class SafeString;

/*!
  \class KTextTemplate::OutputStream
  \inheaderfile KTextTemplate/OutputStream
  \inmodule KTextTemplate

  \brief The OutputStream class is used to render templates to a
  QTextStream.

  A OutputStream instance may be passed to the render method of a Template
  to render the template to a stream.

  \code
    QFile outputFile("./output");
    outputFile.open(QFile::WriteOnly);
    QTextStream tstream( &outputFile );

    OutputStream os(&tstream);
    t->render( &os, &context );
  \endcode

  The OutputStream is used to escape the content streamed to it. By
  default, the escaping is html escaping, converting "&" to "&amp;" for example.
  If generating non-html output, the escape method may be overriden to
  perform a different escaping, or non at all.

  If overriding the escape method, the clone method must also be
  overriden to return an OutputStream with the same escaping behaviour.

  \code
    class NoEscapeStream : public KTextTemplate::OutputStream
    {
    public:
      // ...

      QString escape( const QString &input ) const
      {
        return input;
      }

      QSharedPointer<OutputStream> clone( QTextStream *stream ) const
      {
        return QSharedPointer<NoEscapeStream>::create( stream );
      }
    };
  \endcode
*/
class KTEXTTEMPLATE_EXPORT OutputStream
{
public:
    /*!
      Creates a null OutputStream. Content streamed to
      this OutputStream is sent to /dev/null
    */
    OutputStream();

    /*!
      Creates an OutputStream which will stream content to \a stream
      with appropriate escaping.
    */
    explicit OutputStream(QTextStream *stream);

    /*!
      Destructor
    */
    virtual ~OutputStream();

    /*!
      Returns an escaped version of \a input. Does not write anything to the
      stream.
    */
    virtual QString escape(const QString &input) const;

    /*!
      Returns an escaped version of \a input. Does not write anything to the
      stream.
    */
    QString escape(const SafeString &input) const;

    /*!
      Returns a cloned OutputStream with the same filtering behaviour.
    */
    virtual QSharedPointer<OutputStream> clone(QTextStream *stream) const;

    /*!
      Returns \a after escaping it, unless \a input is "safe", in which case,
      \a input is returned unmodified.
    */
    QString conditionalEscape(const KTextTemplate::SafeString &input) const;

    /*!
      Writes \a input to the stream after escaping it.
    */
    OutputStream &operator<<(const QString &input);

    /*!
      Writes \a input to the stream after escaping it if necessary.
    */
    OutputStream &operator<<(const SafeString &input);

    /*!
      Reads the content of \a stream and writes it unmodified to the result
      stream.
    */
    OutputStream &operator<<(QTextStream *stream);

private:
    QTextStream *m_stream;
    Q_DISABLE_COPY(OutputStream)
};
}

#endif
