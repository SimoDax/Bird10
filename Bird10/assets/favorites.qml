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
import bb.system 1.2
import org.labsquare 1.0
import "/components"
import "/components/actions"

NavigationPane {
    id: nav
    peekEnabled: true
    
    onCreationCompleted: {
        if (o1Twitter.linked)
            twitterApi.requestFavoriteTweets()
        else
            loginSheet.open()
            //loginAction.triggered()
    }
    
    function error_message(text){
        errorToast.body = text
        errorToast.show()
    }

    TimelinePage {
        
        function clear() {
            twitterApi.clearTweetModel()
        }
        function load() {
            twitterApi.requestTweets()
        }

        actionBarAutoHideBehavior: ActionBarAutoHideBehavior.HideOnScroll
        actionBarVisibility: ChromeVisibility.Compact

        Container {
            verticalAlignment: VerticalAlignment.Fill
            
            TopBlueBarText{
                text: "Favorites"
                horizontalAlignment: HorizontalAlignment.Fill
            }

            TweetList {
                id: tweetList

                attachedObjects: [
                    ListScrollStateHandler {
                        onAtEndChanged: {
                            if(atEnd && twitterApi.tweetModel.size()!=0 && twitterApi.tweetModel.size() < 500)
                                twitterApi.requestOlderFavoriteTweets()
                        }
                    }
                ]
            }
        }
    
    actions: [
         ActionItem {
             id: refreshAction
             enabled: o1Twitter.linked
             title: "Refresh"
             imageSource: "asset:///images/ic_resume.png"
             onTriggered: twitterApi.requestFavoriteTweets()
             ActionBar.placement: ActionBarPlacement.InOverflow
         }
        ]
    }
    attachedObjects: [
        ComponentDefinition {
            id: tweetSheetDefinition
            content: TweetSheet {
            }
        },
        Delegate {
            id: tweetSheet
            active: false
            sourceComponent: tweetSheetDefinition

        },
        TwitterApi {
            id: twitterApi
            authenticator: o1Twitter
            onError: {
                error_message(error)
            }
            onNetworkError: {
                error_message("Network error")
            }
        }
    ]
    onPopTransitionEnded: {
        page.destroy();
    }

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
            }
        ]
    }
}