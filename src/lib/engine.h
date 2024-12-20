/*
  This file is part of the KTextTemplate library

  SPDX-FileCopyrightText: 2009, 2010 Stephen Kelly <steveire@gmail.com>

  SPDX-License-Identifier: LGPL-2.1-or-later

*/

#ifndef KTEXTTEMPLATE_ENGINE_H
#define KTEXTTEMPLATE_ENGINE_H

#include "template.h"
#include "templateloader.h"

namespace KTextTemplate
{
class TagLibraryInterface;

class EnginePrivate;

/*!
  \class KTextTemplate::Engine
  \inheaderfile KTextTemplate/Engine
  \inmodule KTextTemplate

  \brief KTextTemplate::Engine is the main entry point for creating KTextTemplate
  Templates.

  The KTextTemplate::Engine is responsible for configuring and creating Template
  objects. In typical use, one or more AbstractTemplateLoader objects will be
  added to the Engine to load template objects, and plugin directories will be
  set to enable finding template tags and filters.

  \code
    auto engine = new Engine();

    auto loader = QSharedPointer<FileSystemTemplateLoader>::create();
    loader->setTemplateDirs( {"/usr/share/myapp/templates"} );
    engine->addTemplateLoader( loader );

    engine->addPluginPath( "/usr/lib/myapp" );

    auto template1 = engine->newTemplate( "Template content", "template name" );

    auto template2 = engine->loadByName( "templatefile.html" );
  \endcode

  Once it is configured, the Engine can be used to create new templates by
  name by loading the templates with the loadByName method, or by defining
  the content in the newTemplate method.

  By default the builtin tags and filters distributed with KTextTemplate are
  available in all templates without using the \c {{% load %}} tag in the
  template. These pre-loaded libraries may be configured if appropriate to the
  application. For example, an application which defines its own tags and
  filters may want them to be always available, or it may be desirable to
  restrict the features available to template authors by removing built in
  libraries.

  Different Engine objects can be used to create templates with differing
  configurations.

  \section1 Insignificant whitespace

  The output of rendering a template depends on the content of the template. In
  some cases when generating content in which whitespace is significant, this
  can have undesired effects. For example, given a template to generate C++ code
  like:

  \badcode
    class MyClass {
    {# This loop creates the  #}
    {# methods in the class   #}
    {% for method in methods %}
      {% if method.hasDox %}
        {{ method.dox }}
      {% endif %}
        {{ method.signature }}
    {% endfor %}
    };
  \endcode

  The output would have a lot of whitespace which is not necessarily wanted.

  \code
    class MyClass {




      void foo() const;

    };
  \endcode

  It is possible to strip insignificant whitespace by enabling the smartTrim
  feature with setSmartTrimEnabled. When enabled the output will not
  contain a newline for any line in the template which has only one token of
  template syntax, such as a comment, tag or variable.

  \code
    class MyClass {

      void foo() const;
    };
  \endcode
*/
class KTEXTTEMPLATE_EXPORT Engine : public QObject
{
    Q_OBJECT
public:
    /*!
      Constructor
    */
    Engine(QObject *parent = {});

    ~Engine() override;

    /*!
      Returns the TemplateLoaders currently configured on the Engine.
    */
    QList<QSharedPointer<AbstractTemplateLoader>> templateLoaders();

    /*!
      Adds \a loader to the TemplateLoaders currently configured on
      the Engine.
    */
    void addTemplateLoader(QSharedPointer<AbstractTemplateLoader> loader);

    /*!
      Sets the plugin dirs currently configured on the Engine to \a dirs.

      \warning This overwrites the default paths. You normally want
      addPluginPath.
    */
    void setPluginPaths(const QStringList &dirs);

    /*!
      Prepend \a dir to the list of plugin dirs.
    */
    void addPluginPath(const QString &dir);

    /*!
      Removes all instances of \a dir from the list of plugin dirs.
    */
    void removePluginPath(const QString &dir);

    /*!
      Returns the currently configured plugin dirs
    */
    QStringList pluginPaths() const;

    /*!
      Returns a URI for a media item with the name \a name.

      Typically this will be used for images. For example the media URI for the
      image "header_logo.png" may be
      "/home/user/common/header_logo.png" or
      "/home/user/some_theme/header_logo.png"
      depending on the templateLoaders configured.

      This method will not usually be called by application code.  To load media
      in a template, use the \c {{% media_finder %}} template tag.
    */
    std::pair<QString, QString> mediaUri(const QString &fileName) const;

    /*!
      Load the Template identified by \a name.

      The Templates and plugins loaded will be determined by
      the Engine configuration.
    */
    Template loadByName(const QString &name) const;

    /*!
      Create a new Template with the content \a content identified by \a name.

      The secondary Templates and plugins loaded will be determined by
      the Engine configuration.
    */
    Template newTemplate(const QString &content, const QString &name) const;

    /*!
      Returns the libraries available by default to new Templates.
    */
    QStringList defaultLibraries() const;

    /*!
      Adds the library named \a libName to the libraries available by default to
      new Templates.
    */
    void addDefaultLibrary(const QString &libName);

    /*!
      Removes the library named \a libName from the libraries available by
      default to new Templates.
    */
    void removeDefaultLibrary(const QString &libName);

    /*!
      Returns whether the smart trim feature is enabled for newly loaded
      templates.

      This is false by default.
     */
    bool smartTrimEnabled() const;

    /*!
      Sets whether the smart trim feature is enabled for newly loaded templates.
     */
    void setSmartTrimEnabled(bool enabled);

    /*!
      \internal

      Loads and returns the libraries specified in defaultLibraries or \a state.
    */
    void loadDefaultLibraries();

    /*!
      \internal

      Loads and returns the library specified by \a name in the
      current **%Engine** configuration or \a state.

      Templates wishing to load a library should use the \c {{% load %}} tag.
    */
    TagLibraryInterface *loadLibrary(const QString &name);

private:
    Q_DECLARE_PRIVATE(Engine)
    EnginePrivate *const d_ptr;
};
}

#endif
