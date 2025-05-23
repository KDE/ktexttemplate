/*
  This file is part of the KTextTemplate library

  SPDX-FileCopyrightText: 2009, 2010 Stephen Kelly <steveire@gmail.com>

  SPDX-License-Identifier: LGPL-2.1-or-later

*/

#ifndef KTEXTTEMPLATE_TEMPLATELOADER_H
#define KTEXTTEMPLATE_TEMPLATELOADER_H

#include "ktexttemplate_export.h"
#include "template.h"

#include <QSharedPointer>

namespace KTextTemplate
{

class AbstractLocalizer;

/*!
  \class KTextTemplate::AbstractTemplateLoader
  \inheaderfile KTextTemplate/TemplateLoader
  \inmodule KTextTemplate

  \brief An retrieval interface to a storage location for Template objects.

  This interface can be implemented to define new ways of retrieving the content
  of Templates.

  The interface of this class should not be called directly from applications.
  TemplateLoaders will typically be created, configured and added to the
  KTextTemplate::Engine which will call the appropriate API.
*/
class KTEXTTEMPLATE_EXPORT AbstractTemplateLoader
{
public:
    virtual ~AbstractTemplateLoader();

    /*!
      Load a Template called \a name. Return an invalid Template if no content
      by that name exists.
    */
    virtual Template loadByName(const QString &name, Engine const *engine) const = 0;

    /*!
      Return a complete URI for media identified by fileName.
    */
    virtual std::pair<QString, QString> getMediaUri(const QString &fileName) const = 0;

    /*!
      Return true if a Template identified by \a name exists and can be loaded.
    */
    virtual bool canLoadTemplate(const QString &name) const = 0;
};

class FileSystemTemplateLoaderPrivate;

/*!
  \class KTextTemplate::FileSystemTemplateLoader
  \inheaderfile KTextTemplate/TemplateLoader
  \inmodule KTextTemplate

  \brief The FileSystemTemplateLoader loads Templates from the file system.

  This template loader works by traversing a list of directories to find
  templates. Directories are checked in order, and the first match hit is parsed
  and returned.

  \code
    loader->setTemplateDirs({
        "/home/user/app/templates",
        "/usr/local/share/app/templates"
    });
    engine->setTemplateLoader( loader );

    // This will try /home/user/app/templates/mytemplate.html
    // followed by /usr/local/share/app/templates/mytemplate.html
    engine->loadByName( "mytemplate.html" );
  \endcode

  Additionally, a themeName may be set on the template loader, which will be
  appended to search paths before the template name.

  \code
    loader->setTemplateDirs({
      "/home/user/app/templates" <<
      "/usr/local/share/app/templates"
    });
    loader->setTheme( "simple_theme" );
    engine->setTemplateLoader( loader );

    // This will try /home/user/app/templates/simple_theme/mytemplate.html
    // followed by /usr/local/share/app/templates/simple_theme/mytemplate.html
    engine->loadByName( "mytemplate.html" );
  \endcode

  Media URIs may be retrieved for media relative to the directories searched
  queried for templates.

  \code
    loader->setTemplateDirs({
      "/home/user/app/templates",
      "/usr/local/share/app/templates"
    });
    loader->setTheme( "simple_theme" );
    engine->setTemplateLoader( loader );

    // This will try /home/user/app/templates/simple_theme/logo.png
    // followed by /usr/local/share/app/templates/simple_theme/logo.png
    // and return the first one that exists.
    engine->mediaUri( "logo.png" );
  \endcode

  The template files loaded by a FileSystemTemplateLoader must be UTF-8
  encoded.
*/
class KTEXTTEMPLATE_EXPORT FileSystemTemplateLoader : public AbstractTemplateLoader
{
public:
    /*!
      Constructor
    */
    FileSystemTemplateLoader(const QSharedPointer<AbstractLocalizer> localizer = {});

    ~FileSystemTemplateLoader() override;

    /*!
     *
     */
    Template loadByName(const QString &name, Engine const *engine) const override;

    /*!
     *
     */
    bool canLoadTemplate(const QString &name) const override;

    /*!
     *
     */
    std::pair<QString, QString> getMediaUri(const QString &fileName) const override;

    /*!
      Sets the theme of this loader to \a themeName
    */
    void setTheme(const QString &themeName);

    /*!
      The themeName of this TemplateLoader
    */
    QString themeName() const;

    /*!
      Sets the directories to look for template files to \a dirs.
    */
    void setTemplateDirs(const QStringList &dirs);

    /*!
      The directories this TemplateLoader looks in for template files.
     */
    QStringList templateDirs() const;

private:
    Q_DECLARE_PRIVATE(FileSystemTemplateLoader)
    FileSystemTemplateLoaderPrivate *const d_ptr;
};

/*!
  \class KTextTemplate::InMemoryTemplateLoader
  \inheaderfile KTextTemplate/TemplateLoader
  \inmodule KTextTemplate

  \brief The InMemoryTemplateLoader loads Templates set dynamically in
  memory.

  This class is mostly used for testing purposes, but can also be used for
  simple uses of %KTextTemplate.

  Templates can be made available using the setTemplate method, and will
  then be retrieved by the KTextTemplate::Engine as appropriate.
*/
class KTEXTTEMPLATE_EXPORT InMemoryTemplateLoader : public AbstractTemplateLoader
{
public:
    /*!
     *
     */
    InMemoryTemplateLoader();
    ~InMemoryTemplateLoader() override;

    Template loadByName(const QString &name, Engine const *engine) const override;

    bool canLoadTemplate(const QString &name) const override;

    std::pair<QString, QString> getMediaUri(const QString &fileName) const override;

    /*!
      Add a template content to this Loader.

      Example:

      \code
        auto loader = QSharedPointer<InMemoryTemplateLoader::create();
        loader->setTemplate( "name_template", "My name is {{ name }}" );
        loader->setTemplate( "age_template", "My age is {{ age }}" );
        engine->addTemplateLoader( loader );

        // Both templates may now be retrieved by calling Engine::loadByName.
      \endcode
    */
    void setTemplate(const QString &name, const QString &content);

private:
    QHash<QString, QString> m_namedTemplates;
};
}

#endif
