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
import bb.multimedia 1.4
import com.simodax 1.0

import "/components"

ListView {
    property variant api : twitterApi    //for accessing api inside list item
    property variant rtDialog : rtDialog
    property variant deleteDialog: deleteDialog
    property variant pane: tabbedPane    //for accessing global definitions (main.qml) inside list item
    property variant authenticator: o1Twitter

    function itemType(data, indexPath) {
        //console.debug(indexPath + " " + data)
        //if (indexPath == dataModel.size() - 1)
        if(typeof(data) == 'undefined')
            return "loading"
        if (data.video_flag)
            return "video"
        if (data.image_flag)
            return "image"
        if (data.gif_flag)
            return "gif"
        if (data.poll_flag)
            return "poll"

        return ""
    }

    dataModel: twitterApi.tweetModel
    bufferedScrollingEnabled: false
    
    function openMedia(indexPath, media_index) {
        if(dataModel.data(indexPath).video_flag == true){
//            player.sourceUrl = dataModel.data(indexPath).extended_entities.media[0].video_info.variants[0].url
//            var videopage = videoPageDefinition.createObject()
//            nav.push(videopage)
//            player.play()
            var array = dataModel.data(indexPath).extended_entities.media[0].video_info.variants
            array.sort(function(a,b){
                if(a.bitrate < b.bitrate)
                    return 1
                if(a.bitrate > b.bitrate)
                    return -1
                return 0
            })
            app.openVideo(array[0].url, dataModel.data(indexPath).user.name)    //highest quality
            return;
        }
        
        if (dataModel.data(indexPath).extended_entities != null) {
            var chosenItem = dataModel.data(indexPath);
            var contentpage = itemPageDefinition.createObject();

            //contentpage.itemPageTitle = chosenItem.name
            for (var i = 0; i < chosenItem.extended_entities.media.length; i ++) contentpage.dataModel.append({
                    media_url: chosenItem.extended_entities.media[i].media_url
                })
            nav.push(contentpage)
            contentpage.navigateToIndex(media_index)
        }
    }

    function openConversation(indexPath, openQuoted){
        var page = conversationPage.createObject()
        if(openQuoted)
            page.twitterApi.loadConversation(dataModel.data(indexPath).quoted_status_id_str)
        else
            page.twitterApi.loadConversation(dataModel.data(indexPath).id_str)
        nav.push(page)
    }
    
    // Needed when app is invoked through active text (twitter.com url)
    function openConversationFromId(id){
        var page = conversationPage.createObject()
        page.twitterApi.loadConversation(id)
        nav.push(page)
    }
    
    function openProfile(indexPath){
        var comp = Qt.createComponent("asset:///UserProfile.qml")
        var page = comp.createObject(nav)
        page.setUser(dataModel.data(indexPath).user.screen_name, dataModel.data(indexPath).user.name)
        nav.push(page)
    }
    
    function openProfileFromScreenName(screen_name){
        var comp = Qt.createComponent("asset:///UserProfile.qml")
        var page = comp.createObject(nav)
        page.setUser(screen_name, screen_name)
        nav.push(page)
    }
    
    function openHashtag(hashtag){
        var comp = Qt.createComponent("asset:///DiscoverPage.qml")
        var page = comp.createObject(nav)
        page.search(hashtag)
        nav.push(page)
    }
    
    function reply(screen_name, id_str, indexPath){
        tweetSheet.active = true
        tweetSheet.object.in_reply_to = screen_name
        tweetSheet.object.in_reply_to_status_id = id_str
        console.debug(dataModel.data(indexPath))
        tweetSheet.object.referencedTweet = dataModel.data([0])
        tweetSheet.object.open()
    }
    
    function dmUser(user){
        var id = app.dm.getConversationIdFromUser(user.id_str)
        var page = dmConversationPage.createObject(nav)
        page.conversationId = id;
        page.remotePartyName = user.name;
        app.dm.setCurrentConversation(id)
        nav.push(page);
    }

    function dmLink(shared_url) {
        var comp = Qt.createComponent("asset:///DMInboxPage.qml")
        var page = comp.createObject(nav)
        page.shared_url = shared_url
        nav.push(page);
    }
    

    listItemComponents: [
        LoadingComponent {
            id: lc
        },
        VideoTweetComponent {
            id: vtc
        },
        TweetComponentMain {
            id: tcm
        },
        VideoTweetComponentMain {
            id: vtcm
        },
        ImageTweetComponent{
            id: itc
        },
        ImageTweetComponentMain {
            id: itcm
        },
        GifTweetComponent {
            id: gtc
        },
        GifTweetComponentMain {
            id: gtcm
        },
        PollTweetComponent {
            id: ptc
        },
        TweetComponent {
            id: lic
            //type: ""
        }
    
    ]
    scrollRole: ScrollRole.Main
    attachedObjects: [
        ComponentDefinition {
            id: conversationPage
            source: "ConversationPage.qml"
        },
        RetweetDialog {
            id: rtDialog
            onQuote: {
                tweetSheet.active = true 
                tweetSheet.object.attachment_url = "https://twitter.com/"+dataModel.value(indexPath).user.screen_name+"/status/"+dataModel.value(indexPath).id_str
                tweetSheet.object.referencedTweet = dataModel.data([indexPath])
                tweetSheet.object.open()
            }
            onRetweet: twitterApi.retweet(dataModel.value(indexPath).rt_flag ? dataModel.value(indexPath).rt_id : dataModel.value(indexPath).id_str, dataModel.value(indexPath).retweeted)
        },
        SystemDialog {
            property string tweet_id: ""
            
            id: deleteDialog
            title: "Delete Tweet"
            body: "Are you sure you want to delete this tweet?"
            customButton.enabled: false
            confirmButton.label: "Delete"
            cancelButton.label: "Cancel"
            onFinished: {
                if(value == SystemUiResult.ConfirmButtonSelection)
                    api.destroyTweet(tweet_id)
            }
         },
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
        SystemToast {
            id: errorToast
            body: ""
        }
        
    ]
    
    function error_message(text) {
        errorToast.body = text
        errorToast.show()
    }
}
