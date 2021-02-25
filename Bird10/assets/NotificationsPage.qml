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
import org.labsquare 1.0
import com.simodax 1.0
import "/components"
import "/components/actions"

NavigationPane {
    id: nav
    property alias unread: notificationsApi.unreadCount
    
    function resetUnreadCount(){
        notificationsApi.updateUnreadIndex()
    }
    
    function loadNotifications(){            // called  by this page on creation and by main on login
        notificationsApi.requestNotifications()
    }
    
    Page {
        actionBarVisibility: app.showTabsOnActionBar ? ChromeVisibility.Visible : ChromeVisibility.Compact
        onCreationCompleted: {
            loadNotifications()
            timer.start()
        }
        
        function clear(){
            notificationsApi.clearNotifications()
        }


        Container {
            TopBlueBarText {
                text: "Notifications"
                tweetVisible: false
                searchVisible: false
            }
            ListView {
                id: notificationList
                property variant twemojiStyle: twemoji.style
                dataModel: notificationsApi.notifications

                function reply(screen_name, id_str) {
                    tweetSheet.active = true
                    tweetSheet.object.in_reply_to = screen_name
                    tweetSheet.object.in_reply_to_status_id = id_str
                    tweetSheet.object.open()
                }

                function itemType(data, indexPath) {
                    return data.type
                }
                
                onTriggered: {
                    var page = conversationPage.createObject()
                    if(dataModel.data(indexPath).type == "tweet")
                        page.twitterApi.loadConversation(dataModel.data(indexPath).id_str)
                    else if (dataModel.data(indexPath).type == "notification")
                        page.twitterApi.loadConversation(dataModel.data(indexPath).tweet.id_str)
                    nav.push(page)
                }
                
                listItemComponents: [
                    ListItemComponent {
                        type: "notification"
                        
                        Container {
                            id: itemRoot
                            

                            Container{
                                layout: StackLayout {
                                    orientation: LayoutOrientation.LeftToRight
                                }
                                topPadding: 15.0
                                leftPadding: 15.0
                                rightPadding: 15.0
                                bottomPadding: 15.0

                                ProfilePic {
                                    id: profilePic
                                    image_url: ListItemData.user_flag ? ListItemData.profile_image_url_https : ""
                                    visible: ListItemData.user_flag
                                }
                                ImageView {
                                    minWidth: 80.0
                                    minHeight: 80.0
                                    imageSource: Application.themeSupport.theme.colorTheme.style == VisualStyle.Bright ? "asset:///images/notifications_active_80x80.png" : "asset:///images/notifications_80x80.png"
                                    visible: ! ListItemData.user_flag
                                }
                                
                                Container {
                                    layout: StackLayout {
                                        orientation: LayoutOrientation.TopToBottom
                                    }
                                    
                                    horizontalAlignment: HorizontalAlignment.Fill
                                    verticalAlignment: VerticalAlignment.Fill
                                    leftMargin: 15.0
                                    
                                    Container {
                                        layout: StackLayout {
                                            orientation: LayoutOrientation.LeftToRight
                                        }
                                        
                                        Label {
                                            text: ListItemData.text
                                            multiline: true
                                            textStyle.fontSizeValue: 7.0
                                            textStyle.fontFamily: 'Slate Pro'
                                        }
                                    
                                    }
                                    
                                    Label {
                                        text: typeof(ListItemData.tweet) != 'undefined' ? ListItemData.tweet.full_text : ""
                                        multiline: true
                                        //                                    textFormat: TextFormat.Html
                                        textStyle.color: Application.themeSupport.theme.colorTheme.style == VisualStyle.Bright ? Color.create("#767676") : Color.create("#969696")
                                        topMargin: 10
                                        textStyle.base: itemRoot.ListItem.view.twemojiStyle
                                        textFormat: TextFormat.Html
                                    }
                                }
                            }
                            Divider {
                            }
                        }
                    },
                    NotificationTweetComponent {
                        type: "tweet"
                        actionsVisible: false
                        dividerVisible: true
                    }
                ]
            }
            QmlTimer {
                id: timer
                duration: 60000
                onTriggered: {
                    if(tabbedPane.activeTab.title != "Notifications")
                        notificationsApi.requestNotifications()
                    if (app.backgroundUpdatesEnabled)
                        timer.start()
                }
            }
        }

        attachedObjects: [
            NotificationsApi {
                id: notificationsApi
                authenticator: o1Twitter
            },
            ComponentDefinition {
                id: conversationPage
                source: "asset:///ConversationPage.qml"
            }
        ]
        
        actions: [
            ScrollToTop {
                id: scrollToTopAction
                onTriggered: {
                    notificationList.scrollToItem([0])
                }
            },
            ActionItem {
                id: refreshAction
                enabled: o1Twitter.linked
                title: "Refresh"
                imageSource: "asset:///images/ic_resume.png"
                onTriggered: notificationsApi.requestNotifications()
                ActionBar.placement: ActionBarPlacement.InOverflow
            },
            LoginAction {
                id: loginAction
            },
            PayAction {
                id: payAction
            }
        ]
    }
    onPopTransitionEnded: {
        page.destroy()
    }
}
