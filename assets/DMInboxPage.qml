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
import com.simodax 1.0
import "../components" 1.0

NavigationPane {
    id: nav
    property alias unread: dmApi.unreadCount
    property bool isActiveTab: tabbedPane.activeTab.title == "Messages"    // Used to decide whether to call updateLastSeenId on new messages

    // Called when the tab becomes the current one
    function resetUnreadCount() {
        dmApi.updateLastSeenId()    // this also resets dmApi.unreadCount
        dmHistoryId.scrollToPosition(ScrollPosition.Beginning)
    }

    function loadInbox() { // called  by this page on creation and by main on login
        dmApi.loadInboxInitialState();
        dmApi.requestBadgeCount();
    }

    onPopTransitionEnded: {
        if(page.objectName == "thisPersonDMPage")
            dmApi.resetCurrentConversation();
        page.destroy()
    }
    
    
    Page {
        actionBarVisibility: ChromeVisibility.Overlay
        actionBarAutoHideBehavior: ActionBarAutoHideBehavior.HideOnScroll
        
        onCreationCompleted: {
            loadInbox();
            timer.start()
        }
        
        id: thisDMHistory
        objectName: "Messages Page"
        //property bool needRefreshControl: true
        //    function titleBarButtonClicked() {
        //        var msgSheet = mainPage.getMsgSheet();
        //        msgSheet.setController(dmApi);
        //        msgSheet.openSheet()
        //    }
        
        function markAllAsReadButtonClicked() {
            markAsReadDialog.show()
        }
        
        
        Container {
            TopBlueBarText {
                text: "Messages"
                searchVisible: false
                tweetVisible: false
            }
            
            Container {
                layout: DockLayout {
                }
                
                ListView {
                    
                    id: dmHistoryId
                    scrollRole: ScrollRole.Main
                    property bool itemActive: false
                    property variant authenticator: o1Twitter
                    property variant pageSize: handler
                    
                    dataModel: dmApi.inbox
                    
//                    function itemType(data, indexPath){
//                        return "item"
//                    }
                    
                    
//                    leadingVisual: PullToRefresh {
//                        id: pullToRefresh
//                        control: dmHistoryId
//                        preferredWidth: SizeSelector.ScreenWidth
//                        onRefreshActiveChanged: {
//                            fetchData()
//                        }
//                    }
                    
                    leadingVisualSnapThreshold: 0.99
                    
                    listItemComponents: [
                        ListItemComponent {
                            type: "item"
                            DMConversationComponent {
                                userScreenName: ListItemData.previewData.screen_name != "" ? '@' + ListItemData.previewData.screen_name : ""
                                userFullName: ListItemData.previewData.name
                                profImageUrl: ListItemData.previewData.avatar_image
                                timeStamp: ListItemData.previewData.date
                                messageStatus: ListItemData.previewData.status
                                //                                userScreenName: ListItemData.screen_name != "" ? '@' + ListItemData.screen_name : ""
//                                userFullName: ListItemData.name
//                                profImageUrl: ListItemData.avatar_image
//                                timeStamp: ListItemData.date
//                                messageStatus: ListItemData.status
                            }
                        }
                    ]
                    
                    onTriggered: {
                        var mySelectedItem = dataModel.data(indexPath).previewData;
                        var id = mySelectedItem.id

                        var page = conversationPage.createObject(nav)
                        page.conversationId = id;
                        page.remotePartyName = mySelectedItem.name;
                        dmApi.setCurrentConversation(id)
                        //dmApi.markRead(id);    // already called by the conversation page when setCurrentConversation changes the datamodel
//                        console.debug(page.conversationId)
//                        console.debug(dmApi.getConversationFromId(page.conversationId))
                        nav.push(page);

                    }

                    function showProfile(userName) {
                         var detailsPage;
                         detailsPage = mePageDefinition.createObject()
                         nav.push(detailsPage);
                    }
//                    attachedObjects: [
//                        // This handler is tracking the scroll state of the ListView.
//                        ListScrollStateHandler {
//                            id: scrollStateHandler
//                            onScrollingChanged: {
//                                if (! scrolling) {
//                                    pullToRefresh.scrollingStopped();
//                                }
//                            }
//                        }
//                    ]
                }
            }
            attachedObjects: LayoutUpdateHandler {
                property alias width: handler.layoutFrame.width
                property alias height: handler.layoutFrame.height
                id: handler
            }

            QmlTimer {
                id: timer
                duration: 30000
                onTriggered: {
                    if (app.backgroundUpdatesEnabled){
                        dmApi.pollUpdates()
                        
                        if(!isActiveTab)
                            dmApi.requestBadgeCount()
                    }
                    timer.start()
                }
            }
        }
        
        //    function lateDataBind() {
        //        if (dmApi.initialized)
        //            bindData()
        //    }
        
        function scrollToTop() {
            dmHistoryId.scrollToPosition(ScrollPosition.Beginning, ScrollAnimation.None)
        }
        
        //    function bindData() {
        //        dmApi.bindWholeDMHistoryList(dmHistoryId.objectName);
        //        hideRefresh();
        //    }
        //
        //    function hideRefresh() {
        //        pullToRefresh.refreshDone();
        //        // if we're at the top of the list and have no new results, scroll so that pullToRefresh is off the screen
        //        if (scrollStateHandler.atBeginning) {
        //            scrollToTop();
        //        }
        //    }
        //
        //    function fetchData() {
        //        dmApi.fetchHistory()
        //    }
        //
        //    function dmSuccessMsg(msg) {
        //        if (! isCurrentTopPage())
        //            return;
        //        mainPage.toast(msg)
        //    }
        //
        //    function deleteSuccessMsg(msg) {
        //        mainPage.toast(msg)
        //    }
        
        function onActive() {
            
            //NOT USE at this moment, we are not actually to trigger "controler refresh" at this moment from QML since we have an early fetch for DM in c++.
            // we can use this one if we find a way to easily cache DM history. ( then, we will disable the early fetch in c++)
            /* if(needRefreshControl)
             * {
             * // dmApi.fetchHistory()
             * //but it appears that we still need to call the binddata(), some time, the signal DMAvailable() were missed before this page is created
             * 
             * needRefreshControl = false;
             }*/
        }
        
        //    function dmFailureMsg(msg) {
                //        mainPage.toast(msg)
                //    }
                //    function markAllDMAsRead() {
                //        _controller.logToConsole(" !!!!!!!markAllDMAsRead ")
                //        dmApi.markAllMessagesAsRead(dmHistoryId.objectName);
                //    }

                function scrollToBottom() {
                    
                    dmHistoryId.scrollToPosition(ScrollPosition.End, ScrollAnimation.None)
                }
                function scrollNext() {
                    if (scrollStateHandler.firstVisibleItem[0] != undefined)
                        dmHistoryId.scrollToItem([ scrollStateHandler.firstVisibleItem[0] + 1 ], ScrollAnimation.None)
                }
                function scrollPrev() {
                    if (scrollStateHandler.firstVisibleItem[0] != undefined)
                        dmHistoryId.scrollToItem([ scrollStateHandler.firstVisibleItem[0] - 1 ], ScrollAnimation.None)
                }
                
                attachedObjects: [
                    DMApi{
                        id: dmApi
                        authenticator: o1Twitter
                        
                        onInboxModelChanged: {
                            if(isActiveTab)
                                updateLastSeenId()
                        }
                    },
                    ComponentDefinition {
                        id: conversationPage
                        source: "asset:///DMConversationPage.qml"
                    },
                    Waiter{
                        id: waiter
                    },
                    SystemDialog {
                        id: markAsReadDialog
                        buttonAreaLimit: 1
                        title: qsTr("Mark all as read?") + Retranslate.onLocaleOrLanguageChanged
                        confirmButton.label: qsTr("Mark all as read") + Retranslate.onLocaleOrLanguageChanged
                        
                        onFinished: {
                            if (markAsReadDialog.result == SystemUiResult.ConfirmButtonSelection) {
                                dmApi.emitMarkAllAsRead();
                            }
                        }
                    }
                ]
            }
    }
