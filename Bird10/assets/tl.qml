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
        if (o1Twitter.linked){
            twitterApi.requestTweets()
            timer.start()
        }
        else
            loginSheet.open()
        //loginAction.triggered()
    }
    
    
    TimelinePage {
        onCreationCompleted: {
            refreshAction.triggered.connect(twitterApi.requestTweets)
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
                horizontalAlignment: HorizontalAlignment.Fill
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
                id: timer
                duration: 60000
                onTriggered: {
                    if (app.backgroundUpdatesEnabled){
                        console.debug('refresh id : ' + ((tweetList.dataModel.data([ 0 ]).id_str)))
                        twitterApi.requestLatestTweets()
                        timer.start()
                    }
                }
            }
        }
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
        }
    ]
    onPopTransitionEnded: {
        page.destroy();
    }

}
