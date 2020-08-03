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

import bb.cascades 1.4

Sheet {
    id: loginSheet
    property alias loginButton : loginButton

    content: Page {
        Container {
            verticalAlignment: VerticalAlignment.Fill
            horizontalAlignment: HorizontalAlignment.Fill
            layout: DockLayout {
            }
            Container {
                verticalAlignment: VerticalAlignment.Center
                //minHeight: 500
                horizontalAlignment: HorizontalAlignment.Center
                leftPadding: ui.du(10.0)
                rightPadding: ui.du(10.0)
                topPadding: ui.du(10.0)
                bottomPadding: ui.du(10.0)
                Label {
                    horizontalAlignment: HorizontalAlignment.Fill
                    text: "Welcome"
                    //textStyle.fontSizeValue: 20.0
                    textStyle.textAlign: TextAlign.Center
                    //textFit.minFontSizeValue: 20.0
                    //textFit.maxFontSizeValue: 20.0
                    textStyle.fontSize: FontSize.XXLarge
                }
                TextField {
                    topMargin: ui.ddu(5)
                    id: username
                    hintText: "Username or email"
                }
                TextField {
                    id: password
                    hintText: "Password"
                    inputMode: TextFieldInputMode.Password
                }
                Button {
                    id: loginButton
                    text: "Login"
                    topMargin: ui.ddu(5)
                    horizontalAlignment: HorizontalAlignment.Center
                    onClicked: {
                        enabled = false
                        o1Twitter.username = username.text.trim()
                        o1Twitter.password = password.text.trim()
                        o1Twitter.link()
                        //loginSheet.close()
                    }
                }
//                Button {
//                    text: "Export logs"
//                    horizontalAlignment: HorizontalAlignment.Center
//                    onClicked: {
//                        app.exportLogs()
//                    }
//                }
            }
        }
    }
}