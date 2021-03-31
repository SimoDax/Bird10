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
    
    onCreationCompleted: {
        app.dm.closeCurrentConversation.connect(closeConversationPages)
    }

    function error_message(text) {
        errorToast.body = text
        errorToast.show()
    }
    function message(text){
        error_message(text)
    }

    function closeConversationPages() {
        // Every time a dm conversation opens, all the other conversation pages in the background need to be closed.
        // For example, if a user is chatting, then he taps on a tweet link in the chat and then tries to share that tweet (via context action) to another (or the same) chat, the conversation
        // where he tapped the link needs to be closed before opening a new one. DMApi *could* handle multiple conversations at once but using it in such way opens up to a lot of potential bugs..

        // delete conversation pages in the inbox
        closeConversationPagesOnNavigationStack(inbox)
        
        // now, delete conversation pages from the other tabs (they could be present if the user accessed dms via 'share via dm' or 'dm user' action items)

        closeConversationPagesOnNavigationStack(tlDelegate.object)
        closeConversationPagesOnNavigationStack(notificationsDelegate.object)
        if(favDelegate.active)
            closeConversationPagesOnNavigationStack(favDelegate.object)
        if(listsDelegate.active)
            closeConversationPagesOnNavigationStack(listsDelegate.object)
        if(myProfileDelegate.active)
            closeConversationPagesOnNavigationStack(myProfileDelegate.object)
    }

    function closeConversationPagesOnNavigationStack(stack){
        for (var i = 1; i < stack.count(); i ++) { // we can safely iterate over all the navigation stack (except the bottom page, i starts from 1) since the new conversation page has not been pushed yet
            if (stack.at(i).objectName == "thisPersonDMPage" || (stack.at(i).objectName == "inboxPage" && i != stack.count()-1)) {
                var page = stack.at(i)
                stack.remove(page) // remove does not trigger onPopTransitionEnded -> the current conversation in the api will not be reset
                page.destroy()
                i=i-1;             // since the page is removed, the index of the following ones decreases by 1
            }
        }
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
        Delegate{
            id: aboutSheet
            source: "asset:///About.qml"
        },
        BlackSheet {
            
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
        SystemToast {
            id: errorToast
            body: ""
        },
        TextStyleDefinition {
            id: twemoji
            rules: FontFaceRule {
                id: font1
                source: "asset:///fonts/Twemoji.ttf"
                fontFamily: "Twemoji"
            }
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
        helpAction: HelpActionItem {
            title: "About"
            onTriggered: {
                aboutSheet.active = true
                aboutSheet.object.open()
            }
        }
    }
    
} //end tabbedPane
