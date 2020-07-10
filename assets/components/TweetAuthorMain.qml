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
import com.pipacs.o2 1.0
import bb.system 1.2
import org.labsquare 1.0
import bb.multimedia 1.4

Container {
    layout: StackLayout {
        orientation: LayoutOrientation.LeftToRight
    }

    verticalAlignment: VerticalAlignment.Fill
    leftMargin: 15.0

    ProfilePicMain {
    }

    Container {
        layout: StackLayout {
            orientation: LayoutOrientation.TopToBottom
        }
        leftMargin: 20

        Label {
            id: username
            text: ListItemData.user.name
            //textStyle.fontWeight: FontWeight.Bold
            textStyle.fontSizeValue: 8.0
            bottomMargin: 5.0
            textStyle.fontFamily: 'Slate Pro'
            textStyle.fontSize: FontSize.Large
            textStyle.fontWeight: FontWeight.W500
        }
        Label {
            id: handle
            text: "@" + ListItemData.user.screen_name
            textStyle.fontSizeValue: 8.0
            //textStyle.color: Color.create("#454545")
            textStyle.color: Color.create("#7A7A7A")
            topMargin: 0.0
            textStyle.fontFamily: 'Slate Pro'
            textStyle.fontSize: FontSize.PointValue

            //textStyle.fontSize: FontSize.Large
        }

    }
}
