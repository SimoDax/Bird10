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

Page {
    property alias twitterApi: twitterApi
    property string screen_name
    property string name
    
    actionBarVisibility: ChromeVisibility.Compact
    
    function setUser(user_screen_name, user_name) {
        screen_name = user_screen_name
        name = user_name
        twitterApi.requestUserTweets(screen_name)
    }
    
    Container {
        //preferredWidth: Infinity
        horizontalAlignment: HorizontalAlignment.Fill
        //verticalAlignment: VerticalAlignment.Fill
        
        attachedObjects: LayoutUpdateHandler {
            property alias width: pageSize.layoutFrame.width
            property alias height: pageSize.layoutFrame.height
            id: pageSize
        }
        
        TopBlueBarText {
            //text: name    //not needed now
        }
        
        ScrollView {
            id: sw
            horizontalAlignment: HorizontalAlignment.Fill
            scrollViewProperties.pinchToZoomEnabled: false
            scrollViewProperties.overScrollEffectMode: OverScrollEffectMode.None
            scrollViewProperties.scrollMode: ScrollMode.Vertical
            
            onViewableAreaChanged: {
                if(viewableArea.y + viewableArea.height < scrollViewContentSize.layoutFrame.height - 50){
                    //console.debug("visible area changed: listview passthrough")
                    tweetList.touchPropagationMode = TouchPropagationMode.PassThrough
                    sw.scrollRole = ScrollRole.Main
                    tweetList.scrollRole = ScrollRole.None
                }
                else{
                    //console.debug("visible area changed: listview full touch")
                    tweetList.touchPropagationMode = TouchPropagationMode.Full
                    sw.scrollRole = ScrollRole.None
                    tweetList.scrollRole = ScrollRole.Main
                }
            }
            
            Container {
                id: scrollViewContent
                attachedObjects: LayoutUpdateHandler {
                    id: scrollViewContentSize
                }
                
                //horizontalAlignment: HorizontalAlignment.Fill
                minWidth: pageSize.width
                verticalAlignment: VerticalAlignment.Top
                
                ProfileHeaderTop {
                    id: pht
                }
                ProfileHeaderBottom {
                    id: phb
                }
                
                SegmentedControl {
                    options: [
                        Option {
                            text: "Tweets"
                        },
                        Option {
                            text: "+ Replies"
                        },
                        Option {
                            text: "Media"
                        }
                    ]
                }
                
                TweetList {
                    id: tweetList
                    
                    maxHeight: pageSize.height - ui.du(8.5) - ui.du(9) //segmentedcontrol remains shown when scrollview reaches bottom
                    
                    //scrollRole: ScrollRole.Main
                    function itemType(data, indexPath) {
                        //console.debug(indexPath + " " + data)
                        //if (indexPath == dataModel.size() - 1)
                        if (typeof (data) == 'undefined')
                            return "loading"
                        if (data.image_flag)
                            return "image"
                        if (data.video_flag)
                            return "video"
                        if (data.gif_flag)
                            return "gif"
                        
                        return ""
                    }
                    
                    attachedObjects: [
                        ListScrollStateHandler {
                            onAtEndChanged: {
                                if (atEnd && twitterApi.tweetModel.size() != 0 && twitterApi.tweetModel.size() < 500)
                                    twitterApi.requestOlderUserTweets(screen_name)
                            }
                        }
                    ]
                }
            }
        }
    }
    
    
    attachedObjects: [
        TwitterApi {
            id: twitterApi
            authenticator: o1Twitter
            onError: {
                error_message(error)
            }
            onNetworkError: {
                error_message("Network error")
            }
            onUserNotFoundError: {
                error_message("User not found")
                nav.pop()
            }
        }
    ]
    
    actionBarAutoHideBehavior: ActionBarAutoHideBehavior.HideOnScroll
}
