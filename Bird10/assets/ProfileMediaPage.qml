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

import bb.cascades 1.2
import bb.multimedia 1.4
import org.labsquare 1.0
import "/components" 1.0

Page {
    id: mediaPage
    property variant itemIndex
    property bool tweetVisible: true
    property alias dataModel: mediaGallery.dataModel    // set by userProfile on creation

    Container{
        id: imageContainer        
        layout: DockLayout {        
        }
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill

        attachedObjects: LayoutUpdateHandler {
            property alias height: pageSize.layoutFrame.height
            property alias width: pageSize.layoutFrame.width
            id: pageSize
        }

        Container {
            id: gallery
            property alias mediaGalleryList: mediaGallery

            background: Color.Black

            ListView {
                id: mediaGallery
                property variant myIndex: itemIndex
                property variant pageSize: pageSize
                property variant scrolling: scrollStateHandler.scrolling
                property variant mediaPage: mediaPage
                property variant a: app
                property variant twemojiStyle: twemoji.style

                snapMode: SnapMode.LeadingEdge
                flickMode: FlickMode.SingleItem
                scrollRole: ScrollRole.Main

                layout: StackListLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                preferredHeight: pageSize.height
                preferredWidth: pageSize.width
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Fill
                scrollIndicatorMode: ScrollIndicatorMode.None

                onMyIndexChanged: {
                    if (myIndex != undefined) {
                        scrollToItem(myIndex, ScrollAnimation.None);
                    }
                }

                function openConversation(indexPath) {
                    var comp = Qt.createComponent("asset:///ConversationPage.qml")
                    var page = comp.createObject(nav)
                    page.twitterApi.loadConversation(dataModel.data(indexPath).id_str)
                    nav.push(page)
                }
                
                function openMedia(indexPath) {
                     var array = dataModel.data(indexPath).extended_entities.media[0].video_info.variants
                     array.sort(function(a,b){
                             if(a.bitrate < b.bitrate)
                                 return 1
                             if(a.bitrate > b.bitrate)
                                 return -1
                             return 0
                     })
                     app.openVideo(array[0].url, dataModel.data(indexPath).user.name)    //highest quality
                }

                function itemType(data, indexPath) {
                    if (typeof (data) == 'undefined')
                        return "loading"
                    if (data.video_flag)
                        return "video"
                    if (data.image_flag)
                        return "image"
                    if (data.gif_flag)
                        return "gif"

                    return ""
                }

                listItemComponents: [
                    ListItemComponent {
                        id: liPicture
                        type: "image"
                        Container {
                            id: itemRoot
                            property variant scrolling: ListItem.view.scrolling
                            property variant mediaPage: ListItem.view.mediaPage
                            property variant pageSize: ListItem.view.pageSize
                            layout: DockLayout {
                            }
                            maxWidth: pageSize.width
                            minWidth: pageSize.width

                            function toggleShowingTweet() {
                                if(! mediaPage.tweetVisible){
                                    mediaTweet.opacity = 0.8
                                    mediaPage.tweetVisible = true
                                } else {
                                    mediaTweet.opacity = 0
                                    mediaPage.tweetVisible = false
                                }
                            }
                            function hideTweet() {
                                mediaTweet.opacity = 0
                                mediaPage.tweetVisible = false
                            }
                            onScrollingChanged: {
                                if (scrolling) {
                                    mediaTweet.opacity = 0
                                } else {
                                     if (mediaPage.tweetVisible)
                                         fade.play()
                                }
                            }
                            
                            BigPic {
                                id: containerMediaItem
                                imagePath: ListItemData.extended_entities.media[0].media_url
                                onHideTweet: itemRoot.hideTweet()
                                gestureHandlers: TapHandler {
                                    onTapped: {
                                        toggleShowingTweet()
                                    }
                                }
                            }
                            TweetItem {
                                verticalAlignment: VerticalAlignment.Bottom
                                id: mediaTweet
                                background: Color.Black
                                opacity: 0
                                //preferredHeight: 200
                                horizontalAlignment: HorizontalAlignment.Fill
                                animations: [
                                    FadeTransition {
                                        id: fade
                                        duration: 1000
                                        easingCurve: StockCurve.CubicOut
                                        fromOpacity: 0.0
                                        toOpacity: 0.8
                                    }
                                ]
                                gestureHandlers: [
                                    TapHandler {
                                        onTapped: itemRoot.ListItem.view.openConversation(itemRoot.ListItem.indexPath)
                                    }
                                ]
                            }
                            contextActions: [
                                ActionSet {
                                    ActionItem {
                                        title: "Save"
                                        imageSource: "asset:///images/ic_save.png"
                                        onTriggered: {
                                            itemRoot.ListItem.view.a.saveImage(ListItemData.extended_entities.media[0].media_url)
                                        }
                                    }
                                    ActionItem {
                                        title: "Share / Edit"
                                        imageSource: "asset:///images/ic_open.png"
                                        onTriggered: {
                                            itemRoot.ListItem.view.a.shareImage(ListItemData.extended_entities.media[0].media_url)
                                        }
                                    }
                                }
                            ]
                        }
                        
                    },
                    ListItemComponent {
                        id: liGif
                        type: "gif"
                        Container {
                            id: itemRoot
                            property variant scrolling: ListItem.view.scrolling
                            property variant mediaPage: ListItem.view.mediaPage
                            property variant pageSize: ListItem.view.pageSize
                            layout: DockLayout {
                            }
                            maxWidth: pageSize.width
                            minWidth: pageSize.width
                            preferredHeight: Infinity     //stocazzo, stocazzo, stocazzo

                            onScrollingChanged: {
                                if (scrolling) {
                                    mediaTweet.opacity = 0;
                                } else {
                                    if (mediaPage.tweetVisible)
                                        fade.play();
                                }
                            }
                            
                            ListItem.onInitializedChanged: {
                                if (initialized == false)
                                    player.reset();
                                else {
                                    player.setSourceUrl(ListItemData.extended_entities.media[0].video_info.variants[0].url.toString())
                                    player.prepare()
                                }
                            }
                            
                            onCreationCompleted: {
                                //player.prepare()
                            }

                            Container {
                                horizontalAlignment: HorizontalAlignment.Center
                                verticalAlignment: VerticalAlignment.Center
                                ActivityIndicator {
                                    horizontalAlignment: HorizontalAlignment.Center
                                    running: true
                                }
                                Label {
                                    horizontalAlignment: HorizontalAlignment.Center
                                    text: "Loading GIF.."
                                }
                            }

                            ForeignWindowControl {
                                id: fwcVideoSurface
                                windowId: "myWinId" + itemRoot.ListItem.indexPath
                                visible: boundToWindow && (player.mediaState == MediaState.Started || player.mediaState == MediaState.Paused)
                                updatedProperties: WindowProperty.Position | WindowProperty.Visible | WindowProperty.Size
                                horizontalAlignment: HorizontalAlignment.Fill
                                verticalAlignment: VerticalAlignment.Fill
                                //preferredHeight: player.videoDimensions.width != 0 ? player.videoDimensions.height * handler.layoutFrame.width / player.videoDimensions.width : 300

                                attachedObjects: [
                                    // layout handler to track control dimensions
                                    LayoutUpdateHandler {
                                        id: handler
                                    }
                                ]
                                gestureHandlers: TapHandler {
                                    onTapped: {
                                        if (player.speed == 0)
                                            player.play()
                                        else
                                            player.pause()
                                    }
                                }
                            }

                            TweetItem {
                                verticalAlignment: VerticalAlignment.Bottom
                                id: mediaTweet
                                background: Color.Black
                                //preferredHeight: 200
                                horizontalAlignment: HorizontalAlignment.Fill
                                animations: [
                                    FadeTransition {
                                        id: fade
                                        duration: 1000
                                        easingCurve: StockCurve.CubicOut
                                        fromOpacity: 0.0
                                        toOpacity: 0.8
                                    }
                                ]
                                gestureHandlers: [
                                    TapHandler {
                                        onTapped: itemRoot.ListItem.view.openConversation(itemRoot.ListItem.indexPath)
                                    }
                                ]
                            }
                            
                            attachedObjects: MediaPlayer {
                                id: player

                                onMediaStateChanged: {
                                    //console.debug("media state: " + mediaState)
                                    if (mediaState == MediaState.Prepared)
                                        play()
                                }

                                // Set these properties for video
                                videoOutput: VideoOutput.PrimaryDisplay
                                windowId: fwcVideoSurface.windowId
                                repeatMode: RepeatMode.Track

                            }
                        }
                    },
                    ListItemComponent {
                        id: liVideo
                        type: "video"
                        Container {
                            id: itemRoot
                            property variant scrolling: ListItem.view.scrolling
                            property variant mediaPage: ListItem.view.mediaPage
                            property variant pageSize: ListItem.view.pageSize
                            layout: DockLayout {
                            }
                            maxWidth: pageSize.width
                            minWidth: pageSize.width
                            preferredHeight: Infinity        //stocazzo, stocazzo, stocazzo

                            onScrollingChanged: {
                                if (scrolling) {
                                    mediaTweet.opacity = 0
                                } else {
                                    if (mediaPage.tweetVisible)
                                        fade.play()
                                }
                            }

                            Container {
                                id: previewContainer
                                verticalAlignment: VerticalAlignment.Fill
                                horizontalAlignment: HorizontalAlignment.Fill
                                gestureHandlers: TapHandler {
                                    onTapped: {
                                        itemRoot.ListItem.view.openMedia(itemRoot.ListItem.indexPath)
                                    }
                                }
                                WebImageView {
                                    id: imageView
                                    url: ListItemData.extended_entities.media[0].media_url
                                    scalingMethod: ScalingMethod.AspectFit
                                    horizontalAlignment: HorizontalAlignment.Fill
                                    verticalAlignment: VerticalAlignment.Fill
                                }
                                ImageView {
                                    maxHeight: 300
                                    //opacity: .9
                                    scalingMethod: ScalingMethod.AspectFit
                                    horizontalAlignment: HorizontalAlignment.Center
                                    verticalAlignment: VerticalAlignment.Center
                                    imageSource: "asset:///images/play.png"
                                }
                                topPadding: 5
                                leftPadding: 5
                                rightPadding: 5
                                bottomPadding: 5
                                layout: DockLayout {

                                }
                            }

                            TweetItem {
                                verticalAlignment: VerticalAlignment.Bottom
                                id: mediaTweet
                                background: Color.Black
                                opacity: 0
                                //preferredHeight: 200
                                horizontalAlignment: HorizontalAlignment.Fill
                                animations: [
                                    FadeTransition {
                                        id: fade
                                        duration: 1000
                                        easingCurve: StockCurve.CubicOut
                                        fromOpacity: 0.0
                                        toOpacity: 0.8
                                    }
                                ]
                                gestureHandlers: [
                                    TapHandler {
                                        onTapped: itemRoot.ListItem.view.openConversation(itemRoot.ListItem.indexPath)
                                    }
                                ]
                            }
                        }
                    },
                    ListItemComponent {
                        type: "loading"
                        Container {
                            id: itemRoot
                            property variant scrolling: ListItem.view.scrolling
                            property variant mediaPage: ListItem.view.mediaPage
                            property variant pageSize: ListItem.view.pageSize
                            layout: DockLayout {
                            }
                            maxWidth: pageSize.width
                            minWidth: pageSize.width
                            preferredHeight: Infinity //stocazzo, stocazzo, stocazzo
                            
                            ActivityIndicator {
                                verticalAlignment: VerticalAlignment.Fill
                                horizontalAlignment: HorizontalAlignment.Fill
                                running: true
                            }
                         }
                     }
                ]
                onCreationCompleted: {
                    listFade.play();
                }


                animations: [
                    FadeTransition {
                        id: listFade
                        duration: 2000
                        easingCurve: StockCurve.CubicOut
                        fromOpacity: 0.0
                        toOpacity: 1.0
                    }
                ]
                attachedObjects: [
                    // This handler is tracking the scroll state of the ListView.
                    ListScrollStateHandler {
                        id: scrollStateHandler
                        onAtEndChanged: {
                            if (atEnd && ! atBeginning && profileApi.mediaTweetModel.size() > 10)
                                profileApi.requestOlderMediaUserTweets()
                        }
                    }
                ]
            }
        }
    }


//    function bindMediaData(imageUrl, statusId, isMyProfile, index){        
//        mediaPage.mediaTweetId = statusId;
//        mediaPage.isMyProfile = isMyProfile;
//        if(isMyProfile) {
//            _myProfilePageController.bindMediaPageTweetData(mediaTweetList.listViewName, statusId);
//            _myProfilePageController.bindMediaListData(gallery.mediaGalleryList.objectName);
//        }
//        else {
//            _profilePageController.bindMediaPageTweetData(mediaTweetList.listViewName, statusId);
//            _profilePageController.bindMediaListData(gallery.mediaGalleryList.objectName);
//        }
//        itemIndex = index
//
//   
//    }
//    function bindMediaTweet(tweetIndex) {
//
//        if(isMyProfile) 
//         _myProfilePageController.updateMediaPageTweetData(mediaTweetList.listViewName, tweetIndex);
//        else
//        _profilePageController.updateMediaPageTweetData(mediaTweetList.listViewName, tweetIndex);
//        
//        itemIndex = tweetIndex 
//    }
//
//
//    function disconnectSlot(){
//        gallery.mediaGalleryList.backgroundImageTapped.disconnect(toggleShowingTweet);
//        gallery.mediaGalleryList.hideTweet.disconnect(hideTweet);
//    
//    }
}
