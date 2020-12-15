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
import bb.system 1.2
import org.labsquare 1.0

Container {
    id: dmListItem

    property string timeStamp // this seemed not used.
    property bool isSentDM

    property string text // it seemed a cascades bug, dmContentLabel.text binding is broken,
    // it could be because that "dmContentLabel" was in the embeded container
    
    property int listItemDMImageSize: 80
    property int sidePadding: 10
    
    property variant authenticator: ListItem.view.authenticator

    //    ccmAvailable: ListItemData.messageStatus == 2
//    tappable: ListItemData.messageStatus == 2

//    onTextChanged: {
//        dmContentLabel.text = ListItemData.text
//        updateDMPhoto()
//    }

    ListItem.onInitializedChanged: {

        if (ListItem.initialized) {
//            //at this moment, only observed content is subject to item reuse
//            // we can add more if necessary.
//            dmContentLabel.text = ListItemData.dmContent

            updateDMPhoto()
        }
    }

    function updateDMPhoto() {

        if (ListItemData.photoUrlSmall != undefined && ListItemData.photoUrlSmall != "") {
            dmPhotoDelegate.source = "asset:///components/DMPhotoCard.qml";
            dmPhotoDelegate.enabled = true

        } else {
            dmPhotoDelegate.enabled = false
            dmPhotoDelegate.source = "";

        }

    }

    Container {

        layout: StackLayout {
            orientation: LayoutOrientation.TopToBottom
        }
        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight

            }

            topPadding: (10)
            bottomPadding: (10)
            leftPadding: (15)
            rightPadding: (10)
            Container {
                id: profileImageContainer
                layout: DockLayout {
                }
                verticalAlignment: VerticalAlignment.Bottom
//                onTouch: {
//                    /*
//                     * When clicking on the list item avatar, we want to first disable the 'tappable' state
//                     * for the parent container so the entire list items doesn't get highlighted,
//                     * enable the avatar overlay.
//                     * 
//                     * As soon as the touch is released, we want to reverse the property states.
//                     */
//                    if (event.isDown() || event.isMove()) {
//                        dmListItem.tappable = false; //Disable parent container active state
//                        avatarImage.tapped = true; //Enable avatar overlay
//                    } else {
//                        dmListItem.tappable = true; //Enable parent container active state
//                        avatarImage.tapped = false; //Hide avatar overlay
//                    }
//                }
//
//                gestureHandlers: [
//                    TapHandler {
//                        onTapped: {
//                            profileImageContainer.clicked()
//                        }
//                    }
//                ]

                function clicked() {
                    dmListItem.ListItem.view.showProfile(dmListItem.ListItem.data.userScreenName, dmListItem.ListItem.component.type == "DMSentItem")
                }

                minWidth: listItemDMImageSize
                maxWidth: listItemDMImageSize
                rightMargin: 10
                WebImageView {
                    id: avatarImage
                    visible: ! isSentDM
                    preferredWidth: listItemDMImageSize
                    preferredHeight: listItemDMImageSize
                    minWidth: listItemDMImageSize
                    maxWidth: listItemDMImageSize
                    minHeight: listItemDMImageSize
                    maxHeight: listItemDMImageSize
                    url: ListItemData.avatar_image
                    //                    defaultUrl: "asset:///images/profile/default_profile_large.png"
                    horizontalAlignment: HorizontalAlignment.Center
                    verticalAlignment: VerticalAlignment.Bottom
                }
            }

            Container {
                preferredWidth: dmListItem.ListItem.view.pageSize.width - listItemDMImageSize - sidePadding
                minHeight: isSentDM ? 60 : listItemDMImageSize
                layout: StackLayout {
                    orientation: isSentDM ? LayoutOrientation.RightToLeft : LayoutOrientation.LeftToRight
                }
                Container {
                    verticalAlignment: VerticalAlignment.Bottom
                    bottomPadding: listItemDMImageSize / 2 - (20)
                    ImageView {
                        visible: ! isSentDM
                        imageSource: "asset:///images/bubble_arrow_blue.png"
                    }
                    ImageView {
                        visible: isSentDM
                        imageSource: "asset:///images/bubble_arrow_gray.png"

                    }
                }
                Container {
                    id: photoMask
                    layout: DockLayout {

                    }
                    verticalAlignment: VerticalAlignment.Bottom
                    Container {
                        id: photoContainer
                        background: isSentDM ? greyBack.imagePaint : blueBack.imagePaint
                        horizontalAlignment: isSentDM ? HorizontalAlignment.Right : HorizontalAlignment.Left
                        verticalAlignment: VerticalAlignment.Bottom
                        ControlDelegate {
                            id: dmPhotoDelegate
                            visible: ListItemData.photoUrlSmall != undefined && ListItemData.photoUrlSmall != ""
                            horizontalAlignment: HorizontalAlignment.Fill
                            preferredWidth: dmListItem.ListItem.view.pageSize.width - listItemDMImageSize - (160-25)- sidePadding
                            delegateActive: ListItemData.photoUrlSmall != undefined && ListItemData.photoUrlSmall != ""
                            onControlChanged: {
                                control.listViewHandle = dmListItem.ListItem.view;
                            }
                            leftMargin: 0
                        }
                        Container {
                            visible: dmContentLabel.text != "" && dmContentLabel.text != " "
                            leftPadding: (10)
                            topPadding: (10)
                            rightPadding: (10)
                            bottomPadding: (10)
                            Label {
                                id: dmContentLabel
                                multiline: true
                                textStyle.fontSize: FontSize.Small
                                textStyle.color: isSentDM ? Color.create("#000000") : Color.White
                                textStyle.fontWeight: FontWeight.W400
                                textStyle.base: dmListItem.ListItem.view.twemojiStyle
                                text: ListItemData.text
                                textFormat: TextFormat.Html
                                content.flags: TextContentFlag.EmoticonsOff | TextContentFlag.ActiveText
//                                activeTextHandler: ActiveTextHandler {
//                                    onTriggered: {
//                                        if (dmListItem.ListItem.view.getController !== undefined && dmListItem.ListItem.view.getController().invokeActiveText !== undefined) {
//                                            dmContentLabel.text = dmListItem.ListItem.view.getController().highlightActiveTextBackground(dmContentLabel.text, event.text);
//                                            dmListItem.ListItem.view.getController().invokeActiveText(event);
//                                        } else {
//                                            console.debug("Error invokeActiveText or controller are undefined.");
//                                        }
//                                    }
//                                }
                            }
                        }
                        attachedObjects: [
                            LayoutUpdateHandler {
                                id: layoutHandler
                                onLayoutFrameChanged: {
                                    maskContainer.preferredHeight = layoutFrame.height
                                    maskContainer.preferredWidth = layoutFrame.width
                                }
                            }
                        ]
                    }

                    Container {
                        id: maskContainer
                        touchPropagationMode: TouchPropagationMode.None
                        horizontalAlignment: isSentDM ? HorizontalAlignment.Right : HorizontalAlignment.Left
                        verticalAlignment: VerticalAlignment.Bottom
                        background: mask.imagePaint
                        visible: ListItemData.photoUrlSmall != undefined && ListItemData.photoUrlSmall != ""
                        implicitLayoutAnimationsEnabled: false
                    }

                }
            }
        }
        Container {
            horizontalAlignment: isSentDM ? HorizontalAlignment.Right : HorizontalAlignment.Left
            leftPadding: isSentDM ? 0 : (50) + listItemDMImageSize
            rightPadding: isSentDM ? (50) : 0
            Label {
                text: ListItemData.date
                minWidth: (150)
                verticalAlignment: VerticalAlignment.Center
                textStyle.fontSize: FontSize.Small
                textStyle.fontWeight: FontWeight.W400
                textStyle.color: Color.create("#7A7A7A")
                textStyle.textAlign: isSentDM ? TextAlign.Right : TextAlign.Left

            }
        }
        Divider {
        }

    }

    attachedObjects: [
        SystemDialog {
            id: myDialog
            title: ("Delete Message")
            body: ("Are you sure you want to delete this message?")
            onFinished: {
                if (myDialog.result == SystemUiResult.ConfirmButtonSelection)
                    dmListItem.ListItem.view.getDMController().deleteDM(ListItemData.id);
            }
        },
        ActionSet {
            id: actions
            title: ("Direct Message")
            subtitle: ListItemData.dmContentPlainText
            ActionItem {
                id: deleteMsgActionItem
                title: ("Delete Message")
                imageSource: "asset:///images/delete_80x80.png"
                onTriggered: {
                    myDialog.show()

                }
            }
        },
        ImagePaintDefinition {
            id: blueBack
            imageSource: "asset:///images/chat_bubble_blue.amd"
        },
        ImagePaintDefinition {
            id: greyBack
            imageSource: "asset:///images/chat_bubble_gray.amd"
        },
        ImagePaintDefinition {
            id: mask
            imageSource: (Application.themeSupport.theme.colorTheme.style == VisualStyle.Bright ) ? "asset:///images/image_mask.amd" : "asset:///images/image_mask_dark.amd"
        }
    ]

    contextMenuHandler: ContextMenuHandler {
        id: contextHandler
        onPopulating: {
            adjustContextActions()
            //Sets first-stage active state when CCM is about to open
        }
    }

    function adjustContextActions() {
        //  2: sent by this user
        // at this moment, only "My" DM has context menu
        // we can only delete the message sent by this user
        if (ListItemData.messageStatus == 2) {
            if (contextActions.length == 0) {
                var actionArray = new Array();
                actionArray.push(actions);
                contextActions = actionArray;
            }
        } else if (contextActions.length != 0) {
            var actionArray = new Array();
            contextActions = actionArray;
        }
    }
//    function isSentDM() {
//        return ListItemData.messageStatus == 2;
//    }


    function clearActiveTextBackground() {
        if (dmContentLabel.text.length > 0) {
            dmContentLabel.text = dmContentLabel.text.split("<a style='font-weight:normal;color:#FFFFFF;background: #CCE9FF").join("<a style='font-weight:normal;color:#00acee")
        }
    }

    onCreationCompleted: {
        adjustContextActions();
    }

}
