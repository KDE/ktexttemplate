/*
  This file is part of the KTextTemplate library

  Copyright (c) 2021 Stephen Kelly <steveire@gmail.com>

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

#ifndef _TEST_COVERAGE_OBJECT_H
#define _TEST_COVERAGE_OBJECT_H
#include <QObject>
class CoverageObject : public QObject
{
  Q_OBJECT
public:
  CoverageObject(QObject *p = {}) : QObject(p) {}
  virtual void initTest() {}
  virtual void cleanupTest() {}
protected Q_SLOTS:
  void init();
  void cleanup();

private:
  void saveCoverageData();
  QString generateTestName() const;
};
#endif
