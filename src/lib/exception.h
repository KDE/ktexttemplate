/*
  This file is part of the KTextTemplate library

  SPDX-FileCopyrightText: 2009, 2010 Stephen Kelly <steveire@gmail.com>

  SPDX-License-Identifier: LGPL-2.1-or-later

*/

#ifndef KTEXTTEMPLATE_EXCEPTION_H
#define KTEXTTEMPLATE_EXCEPTION_H

#include "ktexttemplate_export.h"

#include <QString>

#include <exception>

namespace KTextTemplate
{

/*!
  Types of errors that can occur while using KTextTemplate

  \value NoError
  \value EmptyVariableError
  \value EmptyBlockTagError
  \value InvalidBlockTagError
  \value UnclosedBlockTagError
  \value UnknownFilterError
  \value TagSyntaxError
  \value VariableNotInContext
  \value ObjectReturnTypeInvalid
  \value CompileFunctionError
*/
enum Error {
    NoError,
    EmptyVariableError,
    EmptyBlockTagError,
    InvalidBlockTagError,
    UnclosedBlockTagError,
    UnknownFilterError,
    TagSyntaxError,
    //   VariableSyntaxError,

    VariableNotInContext,
    ObjectReturnTypeInvalid,
    CompileFunctionError
};

/*!
  \class KTextTemplate::Exception
  \inheaderfile KTextTemplate/Exception
  \inmodule KTextTemplate

  \brief An exception for use when implementing template tags.

  The Exception class can be used when implementing
  AbstractNodeFactory::getNode. An exception can be thrown to indicate that
  the syntax of a particular tag is invalid.

  For example, the following template markup should throw an error because the
  include tag should have exactly one argument:

  \code
    <div>
      {% include %}
    </div>
  \endcode

  The corresponding implementation of IncludeNodeFactory::getNode is

  \code
    QStringList tagContents = smartSplit( tagContent );

    if ( tagContents.size() != 2 )
      throw KTextTemplate::Exception( TagSyntaxError,
        "Error: Include tag takes exactly one argument" );

    // The item at index 0 in the list is the tag name, "include"
    QString includeName = tagContents.at( 1 );
  \endcode
*/
class KTEXTTEMPLATE_EXPORT Exception
{
public:
    /*!
      Creates an exception for the error \a errorCode and the verbose
      message \a what
    */
    Exception(Error errorCode, const QString &what)
        : m_errorCode(errorCode)
        , m_what(what)
    {
    }

    virtual ~Exception() throw()
    {
    }

    /*!
      \internal

      Returns the verbose message for the exception.
    */
    const QString what() const throw()
    {
        return m_what;
    }

    /*!
      \internal

      Returns the error code for the exception.
    */
    Error errorCode() const
    {
        return m_errorCode;
    }

private:
    Error m_errorCode;
    QString m_what;
};
}

#endif
