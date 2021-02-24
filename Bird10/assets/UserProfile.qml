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

Page {
    id: profilePage
    //property alias twitterApi: twitterApi
//    property string screen_name
//    property string name

    actionBarVisibility: app.showTabsOnActionBar ? ChromeVisibility.Visible : ChromeVisibility.Compact

    function setUser(user_screen_name, user_name) { // Called from TweetList
        //        screen_name = user_screen_name
        //        name = user_name
        waiter.open()
        profileApi.screenName = user_screen_name
        profileApi.requestUserTweets()
        profileApi.requestUserData()
    }

    Container {
        //preferredWidth: Infinity
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill

        attachedObjects: LayoutUpdateHandler {
            property alias width: pageSize.layoutFrame.width
            property alias height: pageSize.layoutFrame.height
            id: pageSize
        }

        TopBlueBarText {
            text: "Profile"
        }

        TweetList {
            id: lv
            visible: ! waiter.opened
            horizontalAlignment: HorizontalAlignment.Fill
            //verticalAlignment: VerticalAlignment.Fill
            scrollRole: ScrollRole.Main

            property variant twitterApi: twitterApi
            property variant profileApi: profileApi
            property variant pageSize: pageSize

            function itemType(data, indexPath) {
                if (typeof data == "string")
                    return data
                if (data.video_flag)
                    return "video"
                if (data.image_flag)
                    return "image"
                if (data.gif_flag)
                    return "gif"
                return ""
            }

            function openUserTweetsPage() {
                var x = userTweetsPage.createObject(this)
                x.dataModel = profileApi.tweetModel
                x.loadOlder.connect(profileApi.requestOlderUserTweets)
                nav.push(x)
            }

            function openFullUserTweetsPage() {
                profileApi.requestFullUserTweets()
                var x = userTweetsPage.createObject(this)
                x.dataModel = profileApi.fullTweetModel
                x.loadOlder.connect(profileApi.requestOlderFullUserTweets)
                nav.push(x)
            }
            
            function openMediaTweetsPage() {
                var x = userTweetsPage.createObject(this)
                x.dataModel = profileApi.mediaTweetModel
                x.loadOlder.connect(profileApi.requestOlderMediaUserTweets)
                nav.push(x)
            }

            function openMediaGallery(indexPath){
                var x = profileMediaPage.createObject()
                x.dataModel = profileApi.mediaTweetModel
                x.itemIndex = indexPath
                nav.push(x)
            }

            onTriggered: {
                var data = dataModel.data(indexPath)
                if (itemType(data, indexPath) == "showTweetsArrowOption")
                    openUserTweetsPage();
                else if (itemType(data, indexPath) == "showFullTweetsArrowOption")
                    openFullUserTweetsPage()
                else if (itemType(data, indexPath) == "showMediaTweetsArrowOption")
                    openMediaTweetsPage()
            }

            listItemComponents: [
                ListItemComponent {
                    type: "pht"
                    ProfileHeaderTop {
                        id: pht
                        //user: ListItem.view.twitterApi.userModel.value(0)
                        pageSize: ListItem.view.pageSize
                        twitterApi: ListItem.view.profileApi
                    }
                },
                ListItemComponent {
                    type: "phb"
                    ProfileHeaderBottom {
                        id: phb
                        //user: ListItem.view.twitterApi.userModel.value(0)
                        pageSize: ListItem.view.pageSize
                        twitterApi: ListItem.view.profileApi
                    }
                },
                ArrowItemComponent {
                    type: "showTweetsArrowOption"
                    text: "Show more Tweets"
                },
                ArrowItemComponent {
                    type: "showFullTweetsArrowOption"
                    text: "Show more Tweets & Replies"
                },
                ListItemComponent {
                    id: mediaComponent
                    type: "mediaComponent"
                    Container {
                        id: mediaContainer

                        property int indexOfGalleryData

                        Container {
                            verticalAlignment: VerticalAlignment.Fill
                            horizontalAlignment: HorizontalAlignment.Fill
                            preferredHeight: (290)
                            maxHeight: (290)
                            topPadding: (20)
                            bottomPadding: (20)
                            maxWidth: mediaContainer.ListItem.view.pageSize.width
                            minWidth: mediaContainer.ListItem.view.pageSize.width

                            ListView {
                                id: mediaList
                                dataModel: mediaContainer.ListItem.view.profileApi.mediaTweetModel
                                
                                layout: StackListLayout {
                                    orientation: LayoutOrientation.LeftToRight
                                }
                                
                                attachedObjects: ListScrollStateHandler {
                                    onAtEndChanged: {
                                        if (atEnd && !atBeginning && mediaContainer.ListItem.view.profileApi.mediaTweetModel.size() > 0)
                                            mediaContainer.ListItem.view.profileApi.requestOlderMediaUserTweets()
                                    }
                                }

                                leftPadding: (20)
                                preferredHeight: (290)
                                preferredWidth: mediaContainer.ListItem.view.pageSize.width
                                horizontalAlignment: HorizontalAlignment.Center
                                scrollIndicatorMode: ScrollIndicatorMode.None
                                function itemType(data, indexPath) {
                                    if (typeof (data) == 'undefined')
                                        return "loading"
                                    if (data.video_flag)
                                        return "video"
                                    if (data.image_flag)
                                        return "image"
                                    if (data.gif_flag)
                                        return "image"

                                    return ""
                                }

                                listItemComponents: [
                                    ListItemComponent {
                                        id: liMedia
                                        type: "image"
                                        Container {
                                            id: containerMediaItem
                                            preferredWidth: (250)
                                            preferredHeight: (250)
                                            rightMargin: (20)
                                            verticalAlignment: VerticalAlignment.Fill
                                            horizontalAlignment: HorizontalAlignment.Fill
                                            layout: DockLayout {
                                            }
                                            WebImageView {
                                                id: netImage
                                                objectName: "Photo Rail Thumbnail"
                                                preferredWidth: (250)
                                                preferredHeight: (250)
                                                minWidth: (250)
                                                minHeight: (250)
                                                maxWidth: (250)
                                                maxHeight: (250)
                                                verticalAlignment: VerticalAlignment.Fill
                                                horizontalAlignment: HorizontalAlignment.Fill
                                                scalingMethod: ScalingMethod.AspectFill
                                                url: ListItemData.extended_entities.media[0].media_url + ":small"
                                            }
                                        }
                                    },
                                    ListItemComponent {
                                        id: liVideo
                                        type: "video"
                                        Container {
                                            id: containerMediaItem
                                            preferredWidth: (250)
                                            preferredHeight: (250)
                                            rightMargin: (20)
                                            verticalAlignment: VerticalAlignment.Fill
                                            horizontalAlignment: HorizontalAlignment.Fill
                                            layout: DockLayout {
                                            }
                                            WebImageView {
                                                id: netImage
                                                objectName: "Photo Rail Thumbnail"
                                                preferredWidth: (250)
                                                preferredHeight: (250)
                                                minWidth: (250)
                                                minHeight: (250)
                                                maxWidth: (250)
                                                maxHeight: (250)
                                                verticalAlignment: VerticalAlignment.Fill
                                                horizontalAlignment: HorizontalAlignment.Fill
                                                scalingMethod: ScalingMethod.AspectFill
                                                url: ListItemData.extended_entities.media[0].media_url + ":small"
                                            }
                                            ImageView {
                                                maxHeight: 200
                                                //opacity: .9
                                                scalingMethod: ScalingMethod.AspectFit
                                                horizontalAlignment: HorizontalAlignment.Center
                                                verticalAlignment: VerticalAlignment.Center
                                                imageSource: "asset:///images/play.png"
                                            }
                                        }
                                    },
                                    ListItemComponent {
                                        type: "loading"
                                        Container {
                                            preferredWidth: (250)
                                            preferredHeight: (250)
                                            layout: DockLayout {
                                                
                                            }
                                            ActivityIndicator {
                                                verticalAlignment: VerticalAlignment.Fill
                                                horizontalAlignment: HorizontalAlignment.Fill
                                                running: true
                                            }
                                        }
                                    }
                                ]
                                onTriggered: {
                                    var chosenItem = mediaList.dataModel.data(indexPath);
                                    //Open Media Gallery from Profile page
                                    mediaContainer.ListItem.view.openMediaGallery(indexPath);
                                }
                            }
                        }
                        Divider {
                            topMargin: 0
                        }
                    }
                },
                ArrowItemComponent {
                    type: "showMediaTweetsArrowOption"
                    text: "Show media Tweets"
                },
                ListItemComponent {
                    type: "spacer"
                    Container {
                        minHeight: ui.du(10)
                    }
                }
            ]
        }
    }

//    actions: [
//        ActionItem {
//            title: profileApi.following ? "Unfollow" : "Follow"
//            imageSource: "asset:///images/profile/follow.png"
//            onTriggered: profileApi.followUser(profileApi.screenName, profileApi.following)
//            ActionBar.placement: ActionBarPlacement.InOverflow
//            
//        },
//        ActionItem {
//            id: refreshAction
//            enabled: o1Twitter.linked
//            title: "Refresh"
//            imageSource: "asset:///images/ic_resume.png"
//            onTriggered: profileApi.requestUserTweets()
//            ActionBar.placement: ActionBarPlacement.InOverflow
//        },
//        ActionItem {
//            id: payAction
//            title: "Donate"
//            imageSource: "asset:///images/heart.png"
//            onTriggered: {
//                _pay.trigger("bb.action.OPEN")
//            }
//            attachedObjects: Invocation {
//                id: _pay
//                query {
//                    uri: "https://www.paypal.me/simodax/2.99"
//                    invokeTargetId: "sys.browser"
//                }
//            }
//        }
//    ]

    attachedObjects: [
        ProfileApi {
            id: profileApi
            authenticator: o1Twitter
            onError: {
                error_message(error)
                waiter.close()
                nav.pop()
            }
            onNetworkError: {
                error_message("Network error")
                waiter.close()
                nav.pop()
            }
            onUserNotFoundError: {
                error_message("User not found")
                waiter.close()
                nav.pop()
            }
            onPreviewTweetModelChanged: {
                waiter.close()
                var m = profileApi.previewTweetModel
                m.insert(0, [ "pht", "phb" ])
                m.append([ "showTweetsArrowOption", "showFullTweetsArrowOption", "mediaComponent",
                        "showMediaTweetsArrowOption", "spacer" ])
                lv.dataModel = m
                
                if(actions.length == 0){
                    var actionArray = new Array();
                    if(profileApi.screenName == o1Twitter.extraTokens.screen_name)
                        actionArray.push(refreshAction, payAction);
                    else
                        actionArray.push(followAction, dmAction, refreshAction, payAction);
                        
                    actions=actionArray
                }

                requestMediaUserTweets()
            }
            onFollowed: {
                error_message("You are now following @" + profileApi.screenName)
            }
            onUnfollowed: {
                error_message("You stopped following @" + profileApi.screenName)
            }
        },
        TwitterApi {
            id: twitterApi
            authenticator: o1Twitter
            tweetModel: profileApi.previewTweetModel
        },
        Waiter {
            id: waiter
        },
        ComponentDefinition {
            id: userTweetsPage
            source: "asset:///UserTweets.qml"
        },
        ComponentDefinition {
            id: profileMediaPage
            source: "asset:///ProfileMediaPage.qml"
        },
        ActionItem {
            id: dmAction
            title: "DM user"
            imageSource: "asset:///images/mail_80x80.png"
            onTriggered: lv.dmUser(profileApi.userModel.value(0))
            enabled: profileApi.userModel.value(0).can_dm
        },
         ActionItem {
             id: followAction
             title: profileApi.following ? "Unfollow" : "Follow"
             imageSource: profileApi.following ? "asset:///images/profile/unfollow.png" : "asset:///images/profile/follow.png"
             onTriggered: profileApi.followUser(profileApi.screenName, profileApi.following)
             ActionBar.placement: ActionBarPlacement.InOverflow
         
         },
         ActionItem {
             id: refreshAction
             enabled: o1Twitter.linked
             title: "Refresh"
             imageSource: "asset:///images/ic_resume.png"
             onTriggered: profileApi.requestUserTweets()
             ActionBar.placement: ActionBarPlacement.InOverflow
         },
         PayAction {
             id: payAction
         }
    ]

    actionBarAutoHideBehavior: ActionBarAutoHideBehavior.HideOnScroll
}
