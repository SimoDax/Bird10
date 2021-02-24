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

ControlDelegate {
    topMargin: ui.du(4)
    bottomMargin: ui.du(3)
    sourceComponent: ComponentDefinition {

        Container {
            id: quotedContainer

            preferredWidth: Infinity
            //maxHeight: ui.du(20)

            layout: StackLayout {
                orientation: LayoutOrientation.TopToBottom
            }


            topPadding: 15
            leftPadding: 15.0
            rightPadding: 15.0
            bottomPadding: 15.0

            gestureHandlers: TapHandler {
                onTapped: {
                    tapHandled = true
                    itemRoot.ListItem.view.openConversation(itemRoot.ListItem.indexPath, true)
                }
            }

            background: back.imagePaint
            attachedObjects: [
                ImagePaintDefinition {
                    id: back
                    imageSource: "asset:///quoted_border.amd"
                }
            ]

            Container {
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                leftMargin: 15.0

                ProfilePic {
                    id: profilePic
                    //                    url: rtDialog.tweet.user.profile_image_url_https
                    url: ListItemData.quoted_status.user.profile_image_url_https
                    minWidth: 40.0
                    minHeight: 40.0
                }

                Container {
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    leftMargin: 15.0
                    verticalAlignment: VerticalAlignment.Center

                    Label {
                        id: username
                        text: ListItemData.quoted_status.user.name
                        textStyle.fontWeight: FontWeight.Bold
                        textStyle.fontSizeValue: 7.0
                        rightMargin: 5.0
                    }
                    Label {
                        id: handle
                        text: "@" + ListItemData.quoted_status.user.screen_name
                        textStyle.fontSizeValue: 7.0
                        textStyle.color: Color.create("#969696")
                        leftMargin: 0.0
                    }

                }
            }

            Label {
                text: ListItemData.quoted_status.full_text
                multiline: true
                textFormat: TextFormat.Html
                topMargin: 10
                content.flags: TextContentFlag.ActiveText | TextContentFlag.Emoticons
                textStyle.base: itemRoot.ListItem.view.twemojiStyle
                activeTextHandler: ActiveTextHandler {
                    onTriggered: {
                        event.abort()
                    }
             }
            }
        }
    }
}
