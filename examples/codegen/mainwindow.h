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

#include <QMainWindow>

#include <KTextTemplate/TemplateLoader>

class DesignWidget;

namespace KTextTemplate
{
class Engine;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow();

private Q_SLOTS:
    void generateOutput();

private:
    void createOutputTab(const QString &label, const QString &content);

    void generateCpp();

private:
    DesignWidget *m_designWidget;
    KTextTemplate::Engine *m_engine;
    QSharedPointer<KTextTemplate::FileSystemTemplateLoader> m_loader;
    QTabWidget *m_tabWidget;
};

#endif
