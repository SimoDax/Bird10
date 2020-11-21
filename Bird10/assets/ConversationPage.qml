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

Page {
    property alias twitterApi : twitterApi
    property int main_index
    actionBarVisibility: ChromeVisibility.Overlay

    Container {
        preferredWidth: Infinity
        
        TopBlueBarText {
            text: "Tweet"
        }
        
        TweetList {
            id: tweetList

            attachedObjects: [
                ListScrollStateHandler {
                    onAtEndChanged: {
                        if (atEnd && tweetList.dataModel.size() != 0 && tweetList.dataModel.size() < 500)
                            twitterApi.loadMore()
                    }
                }
            ]

            scrollRole: ScrollRole.Main
            function itemType(data, indexPath) {
                //console.debug(indexPath + " " + data)
                //if (indexPath == dataModel.size() - 1)
                if (typeof(data) == 'undefined')
                    return "loading"
                if (data.main_flag){
                    if (data.video_flag)
                        return "main_video"
                    if (data.gif_flag)
                        return "main_gif"
                    if (data.image_flag)
                        return "main_image"
                    if(data.poll_flag)
                        return "main_poll"
                    return "main"
                }
                if(data.image_flag)
                    return "image"
                if (data.video_flag)
                    return "video"
                if (data.gif_flag)
                    return "gif"
                if (data.poll_flag)
                    return "poll"

                return ""
            }
        }
    }

    attachedObjects: [
        ComponentDefinition {
            id: itemPageDefinition
            source: "ItemPage.qml"
        },
        ConversationApi {
            id: twitterApi
            authenticator: o1Twitter
            onError: {
                error_message(error)
            }
            onNetworkError: {
                error_message("Network error")
            }
            onConversationLoaded: {
                main_index = index
                console.debug(index)
                tweetList.scrollToItem([index], ScrollAnimation.None)
            }
        }
    ]
    actionBarAutoHideBehavior: ActionBarAutoHideBehavior.HideOnScroll
}
