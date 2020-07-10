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
    id: discoverPage
    property string query
    
    function search(text){
        twitterApi.requestSearch(text)
        query = text
    }
    
    actionBarAutoHideBehavior: ActionBarAutoHideBehavior.HideOnScroll
    actionBarVisibility: ChromeVisibility.Compact
    
    Container {
        verticalAlignment: VerticalAlignment.Fill
        
        TopBlueBarText{
            text: query
            searchVisible: false
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
            leadingVisual: Container{
                topPadding: (15)
                leftPadding: (15)
                rightPadding: 15
                bottomPadding: 10
                SegmentedControl {
                    id: seg
                    options: [
                        Option {
                            id: popular
                            text: "Popular"
                        },
                        Option {
                            id: recent
                            text: "Recent"
                        },
                        Option {
                            id: image
                            text: "Image"
                        },
                        Option {
                            id: video
                            text: "Video"
                        }
                    ]
                    onSelectedOptionChanged: {
                        if(selectedOption.text != "Popular")
                            twitterApi.requestSearch(query, selectedOption.text.toLowerCase())
                        else
                            twitterApi.requestSearch(query)
                    }
                }}
            leadingVisualSnapThreshold: 0.1
        }
    }
    
    actions: [
        ActionItem {
            id: refreshAction
            enabled: o1Twitter.linked
            title: "Refresh"
            imageSource: "asset:///images/ic_resume.png"
            onTriggered: twitterApi.requestSearch(discoverPage.query, seg.selectedOption.text.toLowerCase())
            ActionBar.placement: ActionBarPlacement.InOverflow
        },
        ActionItem {
            id: payAction
            title: "Donate"
            imageSource: "asset:///images/heart.png"
            onTriggered: {
                _pay.trigger("bb.action.OPEN")
                }
                attachedObjects: Invocation {
                    id: _pay
                    query {
                        uri: "https://paypal.me/pools/c/8pJlhpRSa8"
                        invokeTargetId: "sys.browser"
                    }
                }
            }
        ]

    attachedObjects: [
        DiscoverApi {
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

}
