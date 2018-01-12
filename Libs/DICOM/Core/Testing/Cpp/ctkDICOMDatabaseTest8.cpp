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
  const QString userName = "";
  const QString connectionName = "testConnection";
  
  // test with null password
  database.openDatabase(databaseFile.absoluteFilePath(), connectionName, userName, NULL);
  if (!database.lastError.isEmpty())
    {
    std::cerr << "ctkDICOMDatabase::openDatabase() with NULL password failed: "
	      << qPrintable(database.lastError()) << std::endl;
    return EXIT_FAILURE;
    }

  // test with empty password
  database.openDatabase(databaseFile.absoluteFilePath(), connectionName, userName, "");
  if (!database.lastError.isEmpty())
    {
    std::cerr << "ctkDICOMDatabase::openDatabase() with empty password failed: "
	      << qPrintable(database.lastError()) << std::endl;
    return EXIT_FAILURE;
    }

  // test with valid password
  const QString testPassword = 'testingPassword';
  database.openDatabase(databaseFile.absoluteFilePath(), connectionName, userName, testPassword);

  if (!database.lastError().isEmpty())
    {
    std::cerr << "ctkDICOMDatabase::openDatabase() with password failed: "
              << qPrintable(database.lastError()) << std::endl;
    return EXIT_FAILURE;
    }

  if (!database.database().isValid())
    {
    std::cerr << "ctkDICOMDatabase::openDatabase() with password failed: "
              << "invalid sql database" << std::endl;
    return EXIT_FAILURE;
    }

  if (database.isInMemory())
    {
    std::cerr << "ctkDICOMDatabase::openDatabase() with password failed: "
              << "database should not be in memory" << std::endl;
    return EXIT_FAILURE;    
    }

  if (database.databaseFilename() != databaseFile.absoluteFilePath())
    {
    std::cerr << "ctkDICOMDatabase::databaseFilename() with password failed: "
              << qPrintable( database.databaseFilename()) << std::endl;
    return EXIT_FAILURE;
    }

  if (QDir(database.databaseDirectory()) != databaseDirectory)
    {
    std::cerr << "ctkDICOMDatabase::databaseDirectory() with password failed"
              << qPrintable(database.databaseDirectory()) << std::endl;
    return EXIT_FAILURE;
    }

  bool res = database.initializeDatabase();
  
  if (!res)
    {
    std::cerr << "ctkDICOMDatabase::initializeDatabase() with password failed." << std::endl;
    return EXIT_FAILURE;
    }

  // check if it doesn't crash
  database.insert(0, true, true);
  database.insert(0, true, false);
  database.insert(0, false, true);
  database.insert(0, false, false);

  database.closeDatabase();
  database.initializeDatabase();

  return EXIT_SUCCESS;
}
