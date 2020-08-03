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

#include "applicationui.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/DevelopmentSupport>

#include <QLocale>
#include <QTranslator>

#include <Qt/qdeclarativedebug.h>

#include <curl/curl.h>

using namespace bb::cascades;

Q_DECL_EXPORT int main(int argc, char **argv)
{
    Application app(argc, argv);

    DevelopmentSupport::install();

    // Create the Application UI object, this is where the main.qml file
    // is loaded and the application scene is set.
    ApplicationUI appui;

    QObject::connect(&app, SIGNAL(manualExit()), &appui, SLOT(onManualExit()));
    app.setAutoExit(false);

    // Enter the application main event loop.
    return Application::exec();
}
