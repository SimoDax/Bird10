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
import com.pipacs.o2 1.0
import org.labsquare 1.0

TabbedPane {
    id: tabbedPane
    showTabsOnActionBar: app.showTabsOnActionBar

    function error_message(text) {
        errorToast.body = text
        errorToast.show()
    }
    function message(text){
        error_message(text)
    }

    Tab {
        title: "Home"
        imageSource: "asset:///images/home_80x80.png"
        delegate: Delegate {
            id: tlDelegate
            source: "asset:///tl.qml"
            
        }
        delegateActivationPolicy: TabDelegateActivationPolicy.ActivateImmediately
    } //end tab
    
    Tab {
        title: "Notifications"
        imageSource: "asset:///images/notifications_80x80.png"
        delegate: Delegate {
            id: notificationsDelegate
            source: "asset:///NotificationsPage.qml"
        }
        delegateActivationPolicy: TabDelegateActivationPolicy.ActivateImmediately
        unreadContentCount: {
            if(notificationsDelegate.active)
                return notificationsDelegate.object.unread;
            else return 0
        }
        newContentAvailable: unreadContentCount > 0
        onTriggered: notificationsDelegate.object.resetUnreadCount()
    }

    Tab {
        title: "Messages"
        imageSource: "asset:///images/messages_80x80.png"
        DMInbox {
            id: inbox
        }
        unreadContentCount: inbox.unread
        newContentAvailable: unreadContentCount > 0
        onTriggered: inbox.resetUnreadCount()
        
//        delegate: Delegate {
//            id: dmDelegate
//            source: "asset:///DMInboxPage.qml"
//        }
//        delegateActivationPolicy: TabDelegateActivationPolicy.ActivateImmediately
//        unreadContentCount: {
//            if (dmDelegate.active)
//                return dmDelegate.object.unread;
//            else return 0
//        }
//        newContentAvailable: unreadContentCount > 0
//        onTriggered: dmDelegate.object.resetUnreadCount()

    }

    Tab {
        title: "Favorites"
        imageSource: "asset:///images/twitterheart.png"
        delegate: Delegate {
            id: favDelegate
            source: "asset:///favorites.qml"
        }
        delegateActivationPolicy: TabDelegateActivationPolicy.ActivateWhenSelected
    }

    Tab {
        title: "Lists"
        imageSource: "asset:///images/add_to_list_80x80.png"
        delegate: Delegate {
            id: listsDelegate
            source: "asset:///ListsPage.qml"
        }

        delegateActivationPolicy: TabDelegateActivationPolicy.ActivateWhenSelected
    }

    Tab {
        id: myProfile
        title: "My Profile"
        imageSource: "asset:///images/me_80x80.png"
        delegate: Delegate {
            id: myProfileDelegate
            source: "asset:///user.qml"
        }
        delegateActivationPolicy: TabDelegateActivationPolicy.ActivateWhenSelected
    }

    attachedObjects: [

        LoginSheet {
            id: loginSheet
        },
        SettingsSheet {
            id: settings
        },
        Delegate {
            id: tweetSheet
            active: false
            source: "asset:///TweetSheet.qml"
        },
        Delegate {
            id: browserDelegate
            active: false
            source: "asset:///BrowserSheet.qml"
        },
        ComponentDefinition {
            id: itemPageDefinition
            source: "ItemPage.qml"
        },
        ComponentDefinition {
            id: conversationPage
            source: "asset:///ConversationPage.qml"
        },
        ComponentDefinition {
            id: dmConversationPage
            source: "asset:///DMConversationPage.qml"
        },
        ComponentDefinition {
            id: dmInboxPage
            source: "asset:///DMInboxPage.qml"
        },
        
        OXTwitter {
            id: o1Twitter
            objectName: "o1Twitter"
            

            onOpenBrowser: {
                //browser.url = url
                //browser.open()
                loginSheet.close()
                browserDelegate.active = true
                browserDelegate.object.url = url
                browserDelegate.object.open()
            }

            onCloseBrowser: {
                browserDelegate.object.close()
                browserDelegate.active = false
            }

            onLinkingSucceeded: {
                if (! linked)
                    o1Twitter.in_to_out()
                else
                    o1Twitter.out_to_in()
            }

            onLinkingFailed: {
                tabbedPane.activePane.at(0).actionAt(1).enabled = true
                loginSheet.loginButton.enabled = true
                error_message("Account linking experienced an error. Please check your credentials and that you have an active internet connection")
            }

            function in_to_out() {
                tabbedPane.activePane.at(0).actionAt(1).enabled = true
                loginSheet.loginButton.enabled = true
                tabbedPane.activePane.at(0).clear()
                loginSheet.open()
                inbox.timer.stop()
                app.dm.clear()
            }

            function out_to_in() {
                loginSheet.close()
                tabbedPane.activePane.at(0).actionAt(1).enabled = true
                tabbedPane.activePane.at(0).load()
                notificationsDelegate.object.loadNotifications()
                inbox.loadInbox()
                //webView.storage.clear()
            }

        },
        O1Twitter{
            id: o1Legacy

            onOpenBrowser: {
                loginSheet.close()
                browserDelegate.active = true
                browserDelegate.object.url = url
                browserDelegate.object.open()
            }

            onCloseBrowser: {
                browserDelegate.object.close()
                browserDelegate.active = false
            }

            onLinkingSucceeded: {
                if (linked)
                    error_message("Successfully logged in. You will now send Tweets from 'Bird10 for BlackBerry'")
                else
                    error_message("Successfully logged out. You will now send Tweets from 'Twitter for Android'")
            }

            onLinkingFailed: {
                error_message("Account linking experienced an error. Please check your credentials and that you have an active internet connection")
            }
        },
        //        TwitterApi {
        //            id: twitterApi
        //            authenticator: o1Twitter
        //            onError: {
        //                error_message(error)
        //            }
        //            onNetworkError: {
        //                error_message("Network error")
        //            }
        //        },
        SystemToast {
            id: errorToast
            body: ""
        }

    ]

    Menu.definition: MenuDefinition {
        actions: [
            ActionItem {
                title: "Switch theme"
                onTriggered: {
                    if (Application.themeSupport.theme.colorTheme.style == VisualStyle.Bright) {
                        Application.themeSupport.setVisualStyle(VisualStyle.Dark)
                        app.setTheme(VisualStyle.Dark)
                    } else {
                        Application.themeSupport.setVisualStyle(VisualStyle.Bright)
                        app.setTheme(VisualStyle.Bright)
                    }
                }
            },
            ActionItem {
                title: "Settings"
                onTriggered: {
                    settings.open()
                }
            }
            //            ,ActionItem {
//                title: "Export core"
//                onTriggered: app.exportCore()
//            }
        ]
    }
    
} //end tabbedPane
