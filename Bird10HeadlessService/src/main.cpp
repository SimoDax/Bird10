/*
* Bird10
* Copyright (C) 2020  Simone Dassi
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
*/

#include "service.hpp"

#include <bb/Application>

#include <QLocale>
#include <QTranslator>

#include "logger/logger.h"


using namespace bb;


int main(int argc, char **argv)
{

    LOGGER_START(Logger::INFO, "./shared/documents/bird10service.txt")

    Application app(argc, argv);

    // Create the Application UI object, this is where the main.qml file
    // is loaded and the application scene is set.
    Service srv;

    // Enter the application main event loop.
    return Application::exec();
}
