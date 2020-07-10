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

ListItemComponent {
    property variant text: "Show more Tweets"

    Container {
        id: showTweets
        property real height: ui.du(8)
        preferredHeight: height
        preferredWidth: Infinity

        Container {
            preferredWidth: Infinity
            topPadding: 20
            bottomPadding: 20
            leftPadding: 20
            rightPadding: 20

            layout: DockLayout {
            }

            Label {
                id: liText
                horizontalAlignment: HorizontalAlignment.Left
                verticalAlignment: VerticalAlignment.Center
                text: showTweets.ListItem.component.text
                //textStyle.color: itemDarkTextColor
                textStyle.fontSize: FontSize.Small
                textStyle.fontWeight: FontWeight.W500

            }
            ImageView {
                horizontalAlignment: HorizontalAlignment.Right
                verticalAlignment: VerticalAlignment.Fill
                imageSource: "asset:///images/profile/list_item_chevron.png"
                scalingMethod: ScalingMethod.AspectFill
                //                        minWidth: parent.height
                //                        minHeight: parent.height
                //                        preferredWidth: parent.height
                //                        preferredHeight: parent.height
            }
        }
        Divider {
            topMargin: 0
            bottomMargin: 0
            verticalAlignment: VerticalAlignment.Bottom
        }
        gestureHandlers: TapHandler {
            onTapped: {
                console.debug("Show more Tweets")
            }
        }
    }
}