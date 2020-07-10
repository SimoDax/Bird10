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


Container {

    property bool profileProtected: false
    property bool myProfile: false
    property real profilePageCountsContainerHeight: ui.du(10)
    property variant user: twitterApi.userModel.value(0)
    property variant pageSize
    property variant twitterApi

    bottomPadding: (10)    //was 20

    maxWidth: pageSize.width
    minWidth: pageSize.width

    id: profileHeaderBottomListItem
    objectName: "profileHeaderBottom"

    background: back.imagePaint

    layout: StackLayout {
        orientation: LayoutOrientation.TopToBottom
    }
    navigation {
        focusPolicy: NavigationFocusPolicy.NotFocusable
    }

    Container {
        layout: StackLayout {
            orientation: LayoutOrientation.LeftToRight
        }

        //Tweet container
        Container {

            id: tweetLabelContainer
            objectName: "Tweet Number"
            layoutProperties: StackLayoutProperties {
                spaceQuota: 1
            }
            topPadding: (10)
            maxHeight: profilePageCountsContainerHeight
            minHeight: profilePageCountsContainerHeight
            leftPadding: (20)
            rightPadding: (20)
            //background: Color.create(1.0, 0.0, 0.0, 1.0)

            layout: StackLayout {
                orientation: LayoutOrientation.TopToBottom
            }
            Container {
                Label {
                    text: user.statuses_count
                    textStyle.fontSize: FontSize.Small
                    textStyle.color: ui.palette.text
                    textStyle.fontWeight: FontWeight.W500
                }
            }
            Container {
                Label {
                    text: "TWEETS"
                    textStyle.color: Color.create("#7A7A7A")
                    textStyle.fontSize: FontSize.XSmall
                    textStyle.fontWeight: FontWeight.Normal
                }
            }

            navigation {
                focusPolicy: NavigationFocusPolicy.Focusable
            }

            gestureHandlers: [
                TapHandler {
                    onTapped: {
                        tweetLabelContainer.clicked()
                    }
                }
            ]

            function clicked() {
                profileHeaderBottomListItem.ListItem.view.showTweetWhenTrigger = true
                profileHeaderBottomListItem.ListItem.view.showFollowersWhenTrigger = false
                profileHeaderBottomListItem.ListItem.view.showFollowingWhenTrigger = false
            }
        }

        Container {
            maxHeight: profilePageCountsContainerHeight
            minHeight: profilePageCountsContainerHeight
            maxWidth: (1)
            minWidth: (1)
            background: Color.create(0.7, 0.7, 0.7, 1.0)
        }
        //Following container
        Container {

            id: followingContainer
            objectName: "Following Number"
            layoutProperties: StackLayoutProperties {
                spaceQuota: 1
            }
            topPadding: (10)
            maxHeight: profilePageCountsContainerHeight
            minHeight: profilePageCountsContainerHeight
            leftPadding: (20)
            rightPadding: (20)
            Container {
                Label {
                    text: user.friends_count
                    textStyle.fontSize: FontSize.Small
                    textStyle.color: ui.palette.text
                    textStyle.fontWeight: FontWeight.W500
                }
            }
            Container {
                Label {
                    text: "FOLLOWING"
                    textStyle.color: Color.create("#7A7A7A")
                    textStyle.fontSize: FontSize.XSmall
                    textStyle.fontWeight: FontWeight.Normal
                }
            }

            navigation {
                focusPolicy: NavigationFocusPolicy.Focusable
            }

            gestureHandlers: [
                TapHandler {
                    onTapped: {
                        followingContainer.clicked()
                    }
                }
            ]

            function clicked() {
                profileHeaderBottomListItem.ListItem.view.showFollowingWhenTrigger = true
                profileHeaderBottomListItem.ListItem.view.showFollowersWhenTrigger = false
                profileHeaderBottomListItem.ListItem.view.showTweetWhenTrigger = false
            }
        }
        Container {

            topPadding: (10)

            maxHeight: profilePageCountsContainerHeight
            minHeight: profilePageCountsContainerHeight
            maxWidth: (1)
            minWidth: (1)
            background: Color.create(0.7, 0.7, 0.7, 1.0)
        }
        //Follower container
        Container {

            id: followerContainer
            objectName: "Follower Number"

            layoutProperties: StackLayoutProperties {
                spaceQuota: 1
            }
            topPadding: (10)
            maxHeight: profilePageCountsContainerHeight
            minHeight: profilePageCountsContainerHeight
            leftPadding: (20)
            Container {
                Label {
                    text: user.followers_count
                    textStyle.fontSize: FontSize.Small
                    textStyle.color: ui.palette.text
                    textStyle.fontWeight: FontWeight.W500
                }
            }
            Container {
                Label {
                    text: "FOLLOWERS"
                    textStyle.color: Color.create("#7A7A7A")
                    textStyle.fontSize: FontSize.XSmall
                    textStyle.fontWeight: FontWeight.Normal
                }
            }
            gestureHandlers: [
                TapHandler {
                    onTapped: {
                        followerContainer.clicked()
                    }
                }
            ]

            function clicked() {
                profileHeaderBottomListItem.ListItem.view.showFollowersWhenTrigger = true
                profileHeaderBottomListItem.ListItem.view.showFollowingWhenTrigger = false
                profileHeaderBottomListItem.ListItem.view.showTweetWhenTrigger = false
            }
        }

    }
    Container {
        maxHeight: (1)
        minHeight: (1)
        horizontalAlignment: HorizontalAlignment.Fill
        background: Color.create(0.7, 0.7, 0.7, 1.0)
    }

    //Button row (removed)
    
    attachedObjects: [
        ImagePaintDefinition {
            id: back
            repeatPattern: RepeatPattern.Y
            imageSource: Application.themeSupport.theme.colorTheme.style == VisualStyle.Bright ? "asset:///images/profile/tweet_tile_bkg.png" : "asset:///images/profile/tweet_tile_bkg_invers.png"
        }
    ]

}
