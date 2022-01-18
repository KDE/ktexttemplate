/*
  This file is part of the KTextTemplate library

  Copyright (c) 2010 Stephen Kelly <steveire@gmail.com>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either version
  2.1 of the Licence, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "mainwindow.h"

#include <QApplication>
#include <QLibraryInfo>
#include <QTranslator>

#include <grantlee_templates.h>

#include "grantlee_paths.h"

#define TEMPLATE_DIR KTEXTTEMPLATE_TEMPLATE_PATH "/linguist/"

static QSharedPointer<KTextTemplate::AbstractLocalizer> getLocalizer()
{
  QSharedPointer<KTextTemplate::QtLocalizer> localizer
      = QSharedPointer<KTextTemplate::QtLocalizer>(new KTextTemplate::QtLocalizer);
  localizer->setAppTranslatorPrefix("contacts_");
  localizer->setAppTranslatorPath(qApp->applicationDirPath());

  QStringList locales = QStringList() << "en_US"
                                      << "en_GB"
                                      << "de_DE"
                                      << "fr_FR";
  if (!locales.contains(QLocale::system().name()))
    locales.append(QLocale::system().name());
  for (const QString &localeName : locales) {
    QTranslator *qtTranslator = new QTranslator;
    qtTranslator->load("qt_" + localeName,
                       QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    qtTranslator->setObjectName("qt_" + localeName);
    localizer->installTranslator(qtTranslator, localeName);

    QTranslator *myappTranslator = new QTranslator;
    myappTranslator->load("contacts_" + localeName + ".qm",
                          qApp->applicationDirPath());
    myappTranslator->setObjectName("contacts_" + localeName);
    localizer->installTranslator(myappTranslator, localeName);
  }

  return localizer.staticCast<KTextTemplate::AbstractLocalizer>();
}

template <> void AppMainWindow<KTextTemplate::QtLocalizer>::initLocalizer()
{
  m_localizer = getLocalizer();
}

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  AppMainWindow<KTextTemplate::QtLocalizer> win(TEMPLATE_DIR);
  win.show();

  return app.exec();
}
