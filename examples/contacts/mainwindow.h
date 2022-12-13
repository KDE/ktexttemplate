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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>

#include <KTextTemplate/TemplateLoader>
#include <QListWidget>
#include <QWebEngineView>

namespace KTextTemplate
{
class Engine;
}

class QComboBox;

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(const QString &templateDir, QWidget *parent = nullptr);

protected:
    virtual void initLocalizer();

private Q_SLOTS:
    void render() const;
    void delayedInit();

protected:
    QSharedPointer<KTextTemplate::AbstractLocalizer> m_localizer;

private:
    QListWidget *m_list;
    QWebEngineView *m_webView;
    QComboBox *m_combo;
    QString m_templateDir;

    KTextTemplate::Engine *m_engine;
    QSharedPointer<KTextTemplate::FileSystemTemplateLoader> m_templateLoader;
};

template<typename T>
class AppMainWindow : public MainWindow
{
public:
    explicit AppMainWindow(const QString &templateDir, QWidget *parent = nullptr)
        : MainWindow(templateDir, parent)
    {
    }

protected:
    virtual void initLocalizer()
    {
        m_localizer = QSharedPointer<KTextTemplate::AbstractLocalizer>(new T);
    }
};

#endif
