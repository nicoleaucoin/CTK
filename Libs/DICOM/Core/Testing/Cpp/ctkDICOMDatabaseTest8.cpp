/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes
#include <QCoreApplication>
#include <QDir>
#include <QTimer>

// CTK includes
#include "ctkCoreTestingMacros.h"

// ctkDICOMCore includes
#include "ctkDICOMDatabase.h"

// STD includes
#include <iostream>
#include <cstdlib>


int ctkDICOMDatabaseTest8( int argc, char * argv [] )
{
  QCoreApplication app(argc, argv);

  ctkDICOMDatabase database;
  QDir databaseDirectory = QDir::temp();
  QFileInfo databaseFile(databaseDirectory, QString("database.test"));

  // Testing setting a password on a database with no user name
  const QString emptyUserName = "";
  const QString connectionName = "testConnection";
  
  // test with null password
  database.openDatabase(databaseFile.absoluteFilePath(), connectionName, emptyUserName, NULL);
  CHECK_QSTRING(database.lastError(), QString());

  // test with empty password
  database.openDatabase(databaseFile.absoluteFilePath(), connectionName, emptyUserName, "");
  CHECK_QSTRING(database.lastError(), QString());

  // test with valid password, this fails without the user name set
  const QString testPassword = "testingPassword";
  database.openDatabase(databaseFile.absoluteFilePath(), connectionName, emptyUserName, testPassword);
  CHECK_QSTRING_DIFFERENT(database.lastError(), QString());

  // test with valid password and user name
  const QString userName = "testUser";
  database.openDatabase(databaseFile.absoluteFilePath(), connectionName, userName, testPassword);
  CHECK_QSTRING(database.lastError(), QString());
  CHECK_BOOL(database.database().isValid(), true);
  CHECK_BOOL(database.isInMemory(), false);
  CHECK_QSTRING(database.databaseFilename(), databaseFile.absoluteFilePath());

  if (QDir(database.databaseDirectory()) != databaseDirectory)
    {
    std::cerr << "ctkDICOMDatabase::databaseDirectory() with password failed"
              << qPrintable(database.databaseDirectory()) << std::endl;
    return EXIT_FAILURE;
    }

  bool res = database.initializeDatabase();
  CHECK_BOOL(res, true);

  // check if it doesn't crash
  database.insert(0, true, true);
  database.insert(0, true, false);
  database.insert(0, false, true);
  database.insert(0, false, false);

  // close, must not be used afterward
  database.closeDatabase();
  std::cout << "\tDatabase closed." << std::endl;

  // try to reopen it with a different password, same user
  //const QString newPassword = "somethingNew";
  //database.openDatabase(databaseFile.absoluteFilePath(), connectionName, userName, newPassword);
  //CHECK_QSTRING_DIFFERENT(database.lastError(), QString());

  // this deletes all data
  bool initFlag = database.initializeDatabase();
  CHECK_BOOL(initFlag, true);

  return EXIT_SUCCESS;
}
