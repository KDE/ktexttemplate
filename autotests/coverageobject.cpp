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
#include "coverageobject.h"
#include <QDebug>
#include <QDir>
#include <QLibrary>
#include <QMetaObject>
#include <QString>
#include <QTest>
#include <QDebug>

#include "ktexttemplate_paths.h"

void CoverageObject::init() { initTest(); }

QString CoverageObject::generateTestName() const
{
  QString test_name;
  test_name += QString::fromLatin1(metaObject()->className());
  test_name += QLatin1String("/");
  test_name += QString::fromLatin1(QTest::currentTestFunction());
  if (QTest::currentDataTag()) {
    test_name += QLatin1String("/");
    test_name += QString::fromLatin1(QTest::currentDataTag());
  }
  return test_name;
}

void CoverageObject::saveCoverageData()
{
#ifdef __COVERAGESCANNER__
  QString test_name;
  test_name += generateTestName();

  __coveragescanner_testname(test_name.toStdString().c_str());
  if (QTest::currentTestFailed())
    __coveragescanner_teststate("FAILED");
  else
    __coveragescanner_teststate("PASSED");
  __coveragescanner_save();
  __coveragescanner_testname("");
  __coveragescanner_clear();
#endif
}

void CoverageObject::cleanup()
{
  cleanupTest();
  saveCoverageData();
}
