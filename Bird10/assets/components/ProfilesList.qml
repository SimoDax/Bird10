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
import org.labsquare 1.0

ListView {

    listItemComponents: [
        ListItemComponent {
            type: ""
            Container {
                id: itemRoot
                leftPadding: 15
                topPadding: 15
                //bottomPadding: 15
                Container {
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    ProfilePic {
                        image_url: "http://" + ListItemData.profile_image_url_https.substring(8)
                    }
                    Label {
                        id: username
                        text: ListItemData.name
                        textStyle.fontWeight: FontWeight.W500
                        rightMargin: 5.0
                        textStyle.fontFamily: 'Slate Pro'
                        textStyle.fontSize: FontSize.Medium
                        verticalAlignment: VerticalAlignment.Center
                    }
                    ImageView {
                        visible: ListItemData.is_protected
                        imageSource: "asset:///images/profile/icn_lock_sm.png"
                        verticalAlignment: VerticalAlignment.Center
                        maxHeight: ui.du(4)
                        maxWidth: ui.du(4)
                    }
                    Label {
                        id: handle
                        text: "@" + ListItemData.screen_name
                        textStyle.color: Color.create("#7A7A7A")
                        leftMargin: 0.0
                        textStyle.fontFamily: 'Slate Pro'
                        textStyle.fontSize: FontSize.Medium
                        verticalAlignment: VerticalAlignment.Center
                    }
                }
                Divider {
                }
            }
        }
    ]
}
