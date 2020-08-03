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

Container {
//    property alias imageUrl: profilePic.image_url

    layout: StackLayout {
        orientation: LayoutOrientation.LeftToRight
    }

    topPadding: 15.0
    leftPadding: 15.0
    rightPadding: 15.0
    bottomPadding: 15.0

    ProfilePic {
        id: profilePic
    }

    Container {
        layout: StackLayout {
            orientation: LayoutOrientation.TopToBottom
        }

        verticalAlignment: VerticalAlignment.Fill
        leftMargin: 15.0

        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }

            Label {
                id: username
                text: ListItemData.user.name
                textStyle.fontWeight: FontWeight.Bold
                textStyle.fontSizeValue: 7.0
                rightMargin: 5.0
                textStyle.fontFamily: 'Slate Pro'
            }
            Label {
                id: handle
                text: "@" + ListItemData.user.screen_name
                textStyle.fontSizeValue: 7.0
                textStyle.color: Color.create("#7A7A7A")
                leftMargin: 0.0
                textStyle.fontFamily: 'Slate Pro'
            }

        }

        Label {
            text: ListItemData.full_text
            multiline: true
            textFormat: TextFormat.Html
            //textStyle.fontFamily: 'Slate Pro'
            topMargin: 10
            content.flags: TextContentFlag.ActiveText | TextContentFlag.Emoticons

            activeTextHandler: TweetTextHandler {
            }
        }
    }
}