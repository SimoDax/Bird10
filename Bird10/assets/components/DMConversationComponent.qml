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
import com.simodax 1.0
import org.labsquare 1.0


//    ListItem.onInitializedChanged: {
//
//        if (ListItem.initialized) {
//            //at this moment, only observed content is subject to item reuse
//            // we can add more if necessary.
//            updatePhoto()
//            dmContentLabel.text = ListItemData.previewData.dmContent
//
//        }
//    }
//
//    function updatePhoto() {
//        photoImage.visible = ListItemData.previewData.photoUrlThumb != undefined && ListItemData.previewData.photoUrlThumb != ""
//        if (photoImage.visible)
//            photoImage.imageUrl = ListItemData.previewData.photoUrlThumb
//    }

    Container {

        id: dmListItem
        property string userScreenName
        property string userFullName
        property string profImageUrl //the sender  profile image Url
        property string timeStamp
        property int messageStatus: 1

        property int imageSize: 80
        property int sidePadding: 10
        property int dmPhotoSize: 80
        property variant pageSize: ListItem.view.pageSize
        property variant authenticator: ListItem.view.authenticator

    Container{
        objectName: "DM conversation"
        layout: StackLayout {
            orientation: LayoutOrientation.LeftToRight

        }
        topPadding: (10)
        leftPadding: (10)
        rightPadding: (10)
        bottomPadding: (10)
        

        Container {
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Center
            preferredWidth: (35)
            preferredHeight: (35)
            leftPadding: 0
            rightPadding: (15)
            ImageView {
                id: messageStatusId
                preferredWidth: (30)
                preferredHeight: (30)
                minWidth: (30)
                minHeight: (30)
                //property int messageStatus: 0 // 0: Received and already read, 1: new incoming, 2: sent by this user
                imageSource: ListItemData.previewData.status == 1 ? "asset:///images/new_msg.png" : "asset:///images/card_null.png"
//                function getImageSource() {
//
//                    if (messageStatus == 2) return "asset:///images/actions/reply_hvr_80x80.png";
//                    else if (messageStatus == 1) return "asset:///images/lists/new_msg.png";
//                    else return "asset:///images/tweet/card_null.png";
//                }

            }

        }
        Container {
            id: profileImageContainer
            layout: DockLayout {
            }
            

            onTouch: {
                /*
                 * When clicking on the list item avatar, we want to first disable the 'tappable' state
                 * for the parent container so the entire list items doesn't get highlighted,
                 * enable the avatar overlay.
                 * 
                 * As soon as the touch is released, we want to reverse the property states.
                 */
                if (event.isDown() || event.isMove()) {
                    dmListItem.tappable = false; //Disable parent container active state
                    avatarImage.tapped = true; //Enable avatar overlay
                } else {
                    dmListItem.tappable = true; //Enable parent container active state
                    avatarImage.tapped = false; //Hide avatar overlay
                }
            }

            gestureHandlers: [
                TapHandler {
                    onTapped: {
                        profileImageContainer.clicked()
                    }
                }
            ]

            function clicked() {
                dmListItem.ListItem.view.showProfile(userScreenName, ListItemData.previewData.isMyMessage)
            }

            minWidth: imageSize
            maxWidth: imageSize
            WebImageView {
                id: avatarImage
                objectName: "ListItemDM User Avatar"
                preferredWidth: imageSize
                preferredHeight: imageSize
                minWidth: imageSize
                maxWidth: imageSize
                minHeight: imageSize
                maxHeight: imageSize
                url: profImageUrl
                horizontalAlignment: HorizontalAlignment.Center

            }
        }
        Container {

            Container {
                leftPadding: (20)
                topPadding: (10)
                rightPadding: (10)
                preferredWidth: pageSize.width - (10) - imageSize - sidePadding

                Container {
                    id: nameAndTimeStamp
                    rightPadding: (10)
                    preferredWidth: pageSize.width - (30) - imageSize - sidePadding
                    horizontalAlignment: HorizontalAlignment.Fill
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }

                    Container {
                        horizontalAlignment: HorizontalAlignment.Left
                        verticalAlignment: VerticalAlignment.Center
                        preferredWidth: pageSize.width - (50) - imageSize - sidePadding - (170)

                        layout: StackLayout {
                            orientation: LayoutOrientation.LeftToRight
                        }

                        layoutProperties: StackLayoutProperties {
                            spaceQuota: 1
                        }

                        Label {
                            layoutProperties: StackLayoutProperties {
                                spaceQuota: -1
                            }
                            multiline: false
                            rightMargin: 0
                            verticalAlignment: VerticalAlignment.Center
                            textStyle.fontSize: FontSize.Small
                            //textStyle.color: itemDarkTextColor
                            textStyle.fontWeight: FontWeight.W500
                            text: userFullName
                            textStyle.textAlign: TextAlign.Left
                        }

                        Label {
                            layoutProperties: StackLayoutProperties {
                                spaceQuota: 1
                            }
                            leftMargin: 10
                            multiline: false
                            horizontalAlignment: HorizontalAlignment.Left
                            verticalAlignment: VerticalAlignment.Center
                            textStyle.fontSize: FontSize.Small
                            textStyle.fontWeight: FontWeight.W400
                            textStyle.color: Color.create("#7A7A7A")
                            text: userScreenName
                        }

                    }

                    Container {
                        horizontalAlignment: HorizontalAlignment.Right
                        verticalAlignment: VerticalAlignment.Center
                        rightPadding: (10)
                        topPadding: (5)
                        layoutProperties: StackLayoutProperties {
                            spaceQuota: -1
                        }
                        Label {
                            text: timeStamp
                            textStyle.fontSize: FontSize.Small
                            textStyle.fontWeight: FontWeight.W400
                            textStyle.textAlign: TextAlign.Right

                        }
                    }

                }

                Container {
                    preferredWidth: pageSize.width - (30) - imageSize - sidePadding
                    rightPadding: (10)
                    Container {
                        horizontalAlignment: HorizontalAlignment.Fill
                        layout: DockLayout {
                        }
                        AuthorizedWebImageView {
                            id: photoImage
                            objectName: "Photo Thumbnail"
                            horizontalAlignment: HorizontalAlignment.Right
                            minWidth: dmPhotoSize
                            maxWidth: dmPhotoSize
                            minHeight: dmPhotoSize
                            maxHeight: dmPhotoSize
                            scalingMethod: ScalingMethod.AspectFill
                            url: ListItemData.previewData.photoUrlThumb
                            visible: ListItemData.previewData.photoUrlThumb != ""
                            authenticator: authenticator

                        }
                        Label {
                            id: dmContentLabel
                            horizontalAlignment: HorizontalAlignment.Left
                            maxWidth: pageSize.width - (90) - messageStatusId.preferredWidth - imageSize - sidePadding - dmPhotoSize

                            multiline: true
                            textStyle.fontSize: FontSize.Small
                            textStyle.color: ui.palette.text
                            textStyle.fontWeight: FontWeight.W400
                            text: ListItemData.previewData.message
                            textFormat: TextFormat.Html
                            content.flags: TextContentFlag.Emoticons
                            activeTextHandler: ActiveTextHandler {
                                onTriggered: {
                                    event.abort()
                                }
                            }
                        }
                    }
                }
            }
        }
    }
 Divider {
 }
}

