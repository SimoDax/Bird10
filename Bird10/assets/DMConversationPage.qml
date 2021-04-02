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
import bb.system 1.0
import "/components" 1.0


Page {
    id: thisPersonDMPage
    objectName: "thisPersonDMPage"
    property string remotePartyName
    property string dmListViewName
    property string conversationId
    property variant dmApi: app.dm
    property alias inputText: dmComposer.inputText
    
    onCreationCompleted: {
        dmApi.scrollToLast.connect(scrollListToEnd)
        dmApi.mediaUploaded.connect(onMediaUploaded)
        dmApi.pollUpdates(conversationId)
        timer.start()
    }
    
    
    function titleBarButtonClicked() {
    }
    
    function onMediaUploaded(media_id){
        if(media_id != ""){
            dmApi.sendDM(dmComposer.inputText, conversationId, media_id)
            dmList.requestFocus()
            dmComposer.inputText = ""; //clear the text area
            dmComposer.imageSource = "" //clear the image
        }
        else{
            onMediaUploadError("Media upload failed. Image size may be > 5 MB")
        }
    }
    
    function onMediaUploadError(error){
        dmComposer.unfreeze()
        message(error)
    }

    function sendButtonClicked() {
        if (dmComposer.inputText.trim() == "" && dmComposer.imageSource == "")
            return;

        dmComposer.freeze()
        
        if(dmComposer.imageSource != "")
            dmApi.uploadMedia(dmComposer.selectedImagePath)
        else{
            dmApi.sendDM(dmComposer.inputText, conversationId)
            dmList.requestFocus()
            dmComposer.inputText = ""; //clear the text area
            dmComposer.imageSource = "" //clear the image
            dmComposer.unfreeze()
        }

    }

    function isCurrentTopPage() {
        return mainPage.activeTab.content.top == thisPersonDMPage;
    }

    function checkSendButtonVisual() {
        if (dmComposer.isLengthValid() && dmComposer.charCounter.text != "10000") {
            dmComposer.dmSendButton.enabled = true;
        } else {
            dmComposer.dmSendButton.enabled = false;
        }

    }
    Container {
        layout: AbsoluteLayout {

        }

        Container {
            TopBlueBarText {
                text: remotePartyName
                tweetVisible: false
                searchVisible: false
            }
//            DirectMsgTitleBar {
//                id: dmTitleBar
//                currentTitle: remotePartyName
//                pageHandle: thisPersonDMPage
//            }

            ListView {

                id: dmList
                scrollRole: ScrollRole.Main
                objectName: dmListViewName
                property bool itemActive: false //indiciates when a list item is selected
//                property variant pageSize: handler
                property variant pageSize: display.pixelSize
                property variant authenticator: o1Twitter
                property alias attachmentPage: fullViewPicturePageDefinition
                property variant twemojiStyle: twemoji.style
                

                dataModel: dmApi.currentConversation
                
                onDataModelChanged: dmApi.markRead(conversationId)

                layout: StackListLayout {
                    orientation: LayoutOrientation.BottomToTop
                }

                function itemType(data, indexPath) {
                    if(data.time == 0)
                        return "loading";
                    else
                        return "dm";
                }
                
                function openProfileFromScreenName(screen_name) {
                    var comp = Qt.createComponent("asset:///UserProfile.qml")
                    var page = comp.createObject(nav)
                    page.setUser(screen_name, screen_name)
                    nav.push(page)
                }

                attachedObjects: [
                    ListScrollStateHandler {
                        id: scrollHandler
                        onAtEndChanged: {
                            if (atEnd && !atBeginning && dmList.dataModel.size() != 0)
                                dmApi.requestOlderDM(conversationId)
                        }
                    },
                    ComponentDefinition {
                        id: fullViewPicturePageDefinition
                        source: "asset:///DMAttachmentPage.qml"
                    }
                ]

                listItemComponents: [

                    ListItemComponent {
                        id: dm
                        type: "dm"
                        DMMessageComponent {
                            text: ListItemData.text
                            isSentDM: ListItemData.sender_id == ListItem.view.authenticator.extraTokens.user_id
                        }
                    },
                    LoadingComponent {
                        id: lc
                    }
                ]

                function showProfile(userName, isMyMessage) {
                    var detailsPage;
                    if (isMyMessage) {
                        detailsPage = mePageDefinition.createObject()
                        var activeTab = mainPage.activeTab;
                        activeTab.content.push(detailsPage);
                        detailsPage.bindDataBlocks();
                    } else {
                        detailsPage = profilePageDefinition.createObject();
                        var activeTab = mainPage.activeTab;
                        activeTab.content.push(detailsPage);
                        detailsPage.bindData("0", userName);
                    }
                }

                function openFullScreenImage(imageView) {
                    var page = fullViewPicturePageDefinition.createObject();
                    page.image = imageView.image
                    nav.push(page);
                }
            }

            DMCompose {
                id: dmComposer
                inputText: ""
//                imageSource: ""
                locallyFocused: true
                pageHandle: thisPersonDMPage
            }
            QmlTimer {
                id: timer
                duration: 5000
                onTriggered: {
                    dmApi.pollUpdates(conversationId)
                    timer.start()
                }
            }
        }
    }

    function setInputFocus() {
        dmComposer.requestFocus();
    }

    function showPhoto(imageUrl) {
        if (isCurrentTopPage()) {
            dmComposer.showPreviewImage(imageUrl);
            checkSendButtonVisual()
            setInputFocus();
        }
    }
    function showPhotoSelectOptions(path) {
        if (isCurrentTopPage()) {
            dmComposer.showPhotoSelectOptions(path)
        }
    }

    function scrollListToEnd(forceScroll) {
        if(forceScroll || scrollHandler.atBeginning)    // only scroll if we're aligned to the last message (or if forced by c++)
            dmList.scrollToPosition(ScrollPosition.Beginning, ScrollAnimation.None);
    }

}
