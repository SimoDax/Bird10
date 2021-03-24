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
import com.simodax 1.0
import "/components"
import "/components/actions"

NavigationPane {
    id: nav
    peekEnabled: true
    
    onCreationCompleted: {
        if (o1Twitter.linked){
            twitterApi.requestTweets()
            fleetApi.requestFleets()
            timer.start()
        }
        else
            loginSheet.open()
        //loginAction.triggered()
    }
    
    
    TimelinePage {
        onCreationCompleted: {
            app.openConversation.connect(tweetList.openConversationFromId)
            app.openProfile.connect(tweetList.openProfileFromScreenName)
            app.openTweetSheet.connect(tweet)
        }
        
        function tweet(text, image, video){
            topBar.ts.active = true
            if(text != "")
                topBar.ts.object.text = text
            else if (image != "")
                topBar.ts.object.image1 = image
            else if(video != "")
                topBar.ts.object.video = video
            
            topBar.ts.object.open()
        }
        
        function clear(){
            twitterApi.clearTweetModel()
        }
        function load(){
            twitterApi.requestTweets()
        }
        
        Container {
            verticalAlignment: VerticalAlignment.Fill
            
            TopBlueBarText{
                id: topBar
                horizontalAlignment: HorizontalAlignment.Fill
            }
            
            Container {
                id: fleets
                visible: !fleetApi.allRead
                preferredWidth: Infinity
                preferredHeight: ui.du(12)
                topPadding: ui.du(1)
//                bottomMargin: topPadding
                leftPadding: ui.du(0.5)
                ListView {
                    dataModel: fleetApi.fleetThreads
                    layout: StackListLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    onTriggered: {
                        var p = fleetPage.createObject()
                        p.thread = dataModel.data(indexPath)
                        nav.push(p)
                    }
                    listItemComponents: [
                        ListItemComponent {
                            type: ""
                            Container {
                                preferredHeight: Infinity
                                preferredWidth: ui.du(11)
                                layout: DockLayout {
                                }
                                WebImageView {
                                    id: profilePic
                                    preferredWidth: parent.preferredWidth - 20
                                    preferredHeight: parent.preferredWidth - 20
                                    verticalAlignment: VerticalAlignment.Center
                                    horizontalAlignment: HorizontalAlignment.Center
                                    url: ListItemData.user.profile_image_url_https
                                    scalingMethod: ScalingMethod.AspectFit
                                }
                                ImageView {
                                    verticalAlignment: VerticalAlignment.Fill
                                    horizontalAlignment: HorizontalAlignment.Fill
                                    imageSource: ListItemData.fully_read ? "asset:///fleet_border.png" : "asset:///fleet_border_blue.png"
                                    scalingMethod: ScalingMethod.AspectFit
                                    visible: !profilePic.loading
                                }
                            }
                        }
                    ]
                }
                Divider {
                }
                attachedObjects: ComponentDefinition {
                    id: fleetPage
                    source: "asset:///FleetPage.qml"
                }
            }
            
            TweetList {
                id: tweetList
                
                attachedObjects: [
                    ListScrollStateHandler {
                        onAtEndChanged: {
                            if (atEnd && twitterApi.tweetModel.size() != 0 && twitterApi.tweetModel.size() < 500)
                                twitterApi.requestOlderTweets()
                        }
                    }
                ]
            }
            QmlTimer {
                property int fleetTimerCount: 0
                id: timer
                duration: 60000
                onTriggered: {
                    if (app.backgroundUpdatesEnabled){
                        console.debug('refresh id : ' + ((tweetList.dataModel.data([ 0 ]).id_str)))
                        twitterApi.requestLatestTweets()
                        timer.start()
                        
                        fleetTimerCount++
                        if(fleetTimerCount >= 5 && nav.top.objectName != "fleetPage"){
                            fleetTimerCount = 0
                            fleetApi.requestFleets()
                        }
                    }
                }
            }
        }
        actions: [
            ActionItem {
                id: toggleFleets
                title: fleets.visible ? "Hide Fleets" : "Show Fleets"
                imageSource: fleets.visible ? "asset:///images/ic_collapse.png" : "asset:///images/ic_expand.png"
                onTriggered: {
                    fleets.visible = ! fleets.visible
                }
            },
            ScrollToTop {
                id: scrollTopAction
            },
            RefreshAction {
                id: refreshAction
            },
            LoginAction {
                id: loginAction
            },
            PayAction {
                id: payAction
            }
        ]
        shortcuts: [
            Shortcut {
                key: 't'
                onTriggered: {
                    tweetList.scrollToPosition(ScrollPosition.Beginning, ScrollAnimation.Default)
                }
            },
            Shortcut {
                key: ' '
                onTriggered: {
                    tweetList.scroll(display.pixelSize.height - ui.du(20))
                }
            },
            Shortcut {
                key: 'b'
                onTriggered: {
                    tweetList.scrollToPosition(ScrollPosition.End, ScrollAnimation.Default)
                }
            }
        ]
    }
    attachedObjects: [
        TwitterApi {
            id: twitterApi
            objectName: "homeTimeline"
            authenticator: o1Twitter
            onError: {
                error_message(error)
            }
            onNetworkError: {
                error_message("Network error")
            }
//            onQuote: {
//                tweetSheet.active = true
//                tweetSheet.object.attachment_url = url;
//                tweetSheet.object.open()
//            }
        },
        FleetApi{
            id: fleetApi
            authenticator: o1Twitter
        }
    ]
    onPopTransitionEnded: {
        page.destroy();
    }

}
