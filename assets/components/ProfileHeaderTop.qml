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
import org.labsquare 1.0


Container {
    id: profileHeaderTop
    property real profilePageTopHeaderHeight: ui.du(40)
    property variant user: twitterApi.userModel.value(0)
    property variant pageSize
    property variant twitterApi

    minHeight: profilePageTopHeaderHeight
    maxHeight: profilePageTopHeaderHeight
//    maxWidth: pageSize.width
//    minWidth: pageSize.width


    layout: DockLayout {
    }

    Container {
        
        minHeight: profilePageTopHeaderHeight
        maxHeight: profilePageTopHeaderHeight
        layout: DockLayout {
        }

        WebImageView {
            id: backgroundImageTracker
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            url: "http://" + user.profile_banner_url.substring(8)

            minHeight: profilePageTopHeaderHeight
            maxHeight: profilePageTopHeaderHeight
            scalingMethod: ScalingMethod.AspectFill
        }

        Container {
            id: darkShade
            property int downX: 0
            property double maxOpacity: 0.8; //this is const property
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Fill
            maxWidth: pageSize.width
            minWidth: pageSize.width
            minHeight: profilePageTopHeaderHeight
            maxHeight: profilePageTopHeaderHeight
            background: darkShadeImage.imagePaint
            opacity: 0

        }
        onTouch: {
            if (event.isDown()) {
                darkShade.downX = event.windowX
            }
            if (event.isMove()) {

                if (darkShade.opacity < darkShade.maxOpacity) { //on first item or moving second item
                    var temp = darkShade.opacity + (darkShade.downX - event.windowX) / (pageSize.width);

                    if (temp >= 0 && temp <= darkShade.maxOpacity) { //check to remain always in range [0:maxOpacity]
                        darkShade.opacity = temp;
                    }
                } else {
                    //start moving second item
                    if (event.windowX > darkShade.downX) {
                        darkShade.opacity = darkShade.opacity - (event.windowX - darkShade.downX) / (pageSize.width)
                    }
                }
                darkShade.downX = event.windowX
            }
        }

        Container {
            id: gradientShade
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Fill
            maxWidth: pageSize.width
            minWidth: pageSize.width
            minHeight: profilePageTopHeaderHeight
            maxHeight: profilePageTopHeaderHeight
            background: gradient.imagePaint
            navigation {
                focusPolicy: NavigationFocusPolicy.NotFocusable
            }
        }
        Container {
            //topPadding: SizeSelector.ProfilePageTopHeaderPadding()
            //bottomPadding: SizeSelector.ProfilePageTopHeaderPadding()
            
            topPadding: ui.du(5)
            bottomPadding: ui.du(1)

            ListView {
                id: profileHeaderTopListView

                property variant pageSize_: pageSize
                layout: StackListLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                
                dataModel: twitterApi.userModel

                /*
                 * Updates opacitiy of darkShade according
                 */
                function setDarkShadeOpactiy(shadeOpacity) {
                    darkShade.opacity = darkShade.maxOpacity * shadeOpacity;

                    listPosLeftShade.opacity = shadeOpacity
                    // Only select the second item when the shadeOpacity is 1 (maximum)
                    listPosRightShade.opacity = 1 - shadeOpacity
                }
                
                function itemType(data, indexPath){
                    console.debug(indexPath)
                    if(indexPath == 0)
                        return "0"
                    else return "1"
                    
                }

                scrollIndicatorMode: ScrollIndicatorMode.None
                snapMode: SnapMode.LeadingEdge
                flickMode: FlickMode.SingleItem

                listItemComponents: [
                    ListItemComponent {
                        type: "0"
                        Container {
                            id: listItemProfileScreenImageContainer
                            layout: StackLayout {
                                orientation: LayoutOrientation.TopToBottom
                            }
                            horizontalAlignment: HorizontalAlignment.Fill
                            verticalAlignment: VerticalAlignment.Center
                            maxWidth: listItemProfileScreenImageContainer.ListItem.view.pageSize_.width
                            minWidth: listItemProfileScreenImageContainer.ListItem.view.pageSize_.width
                            minHeight: listItemProfileScreenImageContainer.ListItem.view.profilePageTopHeaderHeight
                            maxHeight: listItemProfileScreenImageContainer.ListItem.view.profilePageTopHeaderHeight

                            Container {
                                layout: DockLayout {
                                }
                                horizontalAlignment: HorizontalAlignment.Center
                                verticalAlignment: VerticalAlignment.Center

                                ImageView {
                                    horizontalAlignment: HorizontalAlignment.Center
                                    verticalAlignment: VerticalAlignment.Center
                                    imageSource: "asset:///images/profile/avatar_border.amd"
                                    minHeight: ui.du(15.5)
                                    minWidth: ui.du(15.5)
                                }

                                WebImageView {
                                    id: avatarControl
                                    objectName: "Profile Avatar"
                                    horizontalAlignment: HorizontalAlignment.Center
                                    verticalAlignment: VerticalAlignment.Center
                                    url: ListItemData.profile_image_url_https.replace("https", "http").replace("normal", "bigger")
                                    minHeight: ui.du(14)
                                    minWidth: ui.du(14)

//                                    borderWidthLeft: (8)
//                                    borderWidthTop: (8)
//                                    borderWidthRight: (8)
//                                    borderWidthBottom: (8)

                                    gestureHandlers: [
                                        TapHandler {
                                            onTapped: {
                                                avatarControl.clicked()
                                            }
                                        }
                                    ]

                                    function clicked() {
                                        avatarControl.openFullScreenImage(ListItemData.profileImage)
                                    }

                                    function openFullScreenImage(imageUrl) {
                                        var page = picPageDefinition.createObject();
                                        page.imagePath = getFullUrl(imageUrl);
                                        page.setTitleBar(picTitleBar);
                                        profilePicSheet.setContent(page);
                                        profilePicSheet.open();
                                    }
                                    function getFullUrl(imageUrl) {
                                        var fullUrl = imageUrl;
                                        var lastSlash = imageUrl.lastIndexOf("/");
                                        if (lastSlash > 0) {
                                            var lastUnderscore = imageUrl.lastIndexOf("_");
                                            var lastDot = imageUrl.lastIndexOf(".");
                                            if (lastSlash < lastUnderscore && lastUnderscore < lastDot && lastDot < imageUrl.length) {
                                                fullUrl = imageUrl.slice(0, lastUnderscore) + imageUrl.slice(lastDot);
                                            }
                                        }
                                        return fullUrl;
                                    }
                                    attachedObjects: [
                                        Sheet {
                                            id: profilePicSheet
                                            peekEnabled: false
                                        },
                                        ComponentDefinition {
                                            id: picPageDefinition
                                            //source: "asset:///pages/PicPage.qml"
                                        },
                                        TitleBar {
                                            id: picTitleBar
                                            dismissAction: ActionItem {
                                                title: qsTr("Close") + Retranslate.onLocaleOrLanguageChanged
                                                onTriggered: {
                                                    profilePicSheet.close()
                                                    profilePicSheet.resetContent()
                                                }
                                            }
                                        }
                                    ]
                                }

                                gestureHandlers: [
                                    TapHandler {
                                        onTapped: {
                                            avatarControl.clicked()
                                        }
                                    }
                                ]

                                function clicked() {
                                    avatarControl.openFullScreenImage(ListItemData.profileImage)
                                }

                                function openFullScreenImage(imageUrl) {
                                    var page = picPageDefinition.createObject();
                                    page.imagePath = getFullUrl(imageUrl);
                                    page.setTitleBar(picTitleBar);
                                    profilePicSheet.setContent(page);
                                    profilePicSheet.open();
                                }
                                function getFullUrl(imageUrl) {
                                    var fullUrl = imageUrl;
                                    var lastSlash = imageUrl.lastIndexOf("/");
                                    if (lastSlash > 0) {
                                        var lastUnderscore = imageUrl.lastIndexOf("_");
                                        var lastDot = imageUrl.lastIndexOf(".");
                                        if (lastSlash < lastUnderscore && lastUnderscore < lastDot && lastDot < imageUrl.length) {
                                            fullUrl = imageUrl.slice(0, lastUnderscore) + imageUrl.slice(lastDot);
                                        }
                                    }
                                    return fullUrl;
                                }
                            }
                            Container {
                                topPadding: ui.du(5)
                                horizontalAlignment: HorizontalAlignment.Center
                                layout: StackLayout {
                                    orientation: LayoutOrientation.LeftToRight
                                }
                                Label {
                                    id: labelFullName
                                    multiline: true
                                    textStyle.fontSize: FontSize.Large
                                    textStyle.color: Color.White
                                    textStyle.fontWeight: FontWeight.W500
                                    text: ListItemData.name
                                    verticalAlignment: VerticalAlignment.Center
                                }
                                ImageView {
                                    id: imageVerifiedProofile
                                    visible: ListItemData.verified
                                    maxHeight: ui.du(4)
                                    maxWidth: ui.du(4)
                                    imageSource: "asset:///images/profile/verified_profile.png"
                                    horizontalAlignment: HorizontalAlignment.Right
                                    verticalAlignment: VerticalAlignment.Center
                                }
                            }
                            Container {
                                horizontalAlignment: HorizontalAlignment.Center
                                layout: StackLayout {
                                    orientation: LayoutOrientation.LeftToRight
                                }
                                Container {
                                    Label {
                                        id: labelUsername
                                        horizontalAlignment: HorizontalAlignment.Center
                                        multiline: true
                                        textStyle.fontSize: FontSize.Small
                                        textStyle.color: Color.White
                                        //textStyle.fontWeight: Qt.generalStyles.normalFontWeight
                                        text: '@' + ListItemData.screen_name
                                    }
                                }

                                ImageView {
                                    id: imageVerifiedProofile1
                                    horizontalAlignment: HorizontalAlignment.Center
                                    visible: ListItemData.protected
                                    imageSource: "asset:///images/profile/icn_lock_sm.png"
                                }
                            }
                            attachedObjects: [
                                LayoutUpdateHandler {
                                    onLayoutFrameChanged: {
                                        // 1 for maximum opacity
                                        var targetOpacityRatio = 1;

                                        // First item is still visible
                                        var firstItemVisibleWidth = layoutFrame.x + layoutFrame.width;

                                        // First item visible should set darkShade to opacity 0
                                        if (firstItemVisibleWidth > 0) {
                                            var ratio = Math.min(1, firstItemVisibleWidth / layoutFrame.width);
                                            targetOpacityRatio = 1 - ratio;
                                        }

                                        listItemProfileScreenImageContainer.ListItem.view.setDarkShadeOpactiy(targetOpacityRatio)
                                    }
                                }
                            ]
                        }
                    },
                    ListItemComponent {
                        type: "1"
                        Container {
                            id: listItemProfileScreenTextContainer
                            layout: StackLayout {
                                orientation: LayoutOrientation.TopToBottom
                            }
                            topMargin: (10.0)
                            bottomMargin: (10.0)
                            horizontalAlignment: HorizontalAlignment.Fill
                            minHeight: listItemProfileScreenTextContainer.ListItem.view.profilePageTopHeaderHeight
                            maxHeight: listItemProfileScreenTextContainer.ListItem.view.profilePageTopHeaderHeight
                            maxWidth: {console.debug("maxWidth: "+listItemProfileScreenTextContainer.ListItem.view.pageSize_.width); return listItemProfileScreenTextContainer.ListItem.view.pageSize_.width}
                            minWidth: listItemProfileScreenTextContainer.ListItem.view.pageSize_.width
                    
                            Container {
                                id: profileTextContainer
                                horizontalAlignment: HorizontalAlignment.Center
                                
                                Label {
                                    id: labelCaption
                                    horizontalAlignment: HorizontalAlignment.Center
                                    multiline: true
                                    textStyle.fontSize: FontSize.Small
                                    textStyle.color: Color.White
                                    //textStyle.fontWeight: Qt.generalStyles.normalFontWeight
                                    text: ListItemData.description
                                    textStyle.textAlign: TextAlign.Center
                                }
                                Label {
                                    id: labelLocation
                                    horizontalAlignment: HorizontalAlignment.Center
                                    multiline: true
                                    textStyle.fontSize: FontSize.Small
                                    textStyle.color: Color.White
                                    //textStyle.fontWeight: Qt.generalStyles.normalFontWeight
                                    text: ListItemData.location
                                    topMargin: 0
                                    bottomMargin: 0
                                }
                                Label {
                                    id: labelWebsite
                                    horizontalAlignment: HorizontalAlignment.Center
                                    multiline: true
                                    //textStyle.base: SizeSelector.getTextStyleColorWeblink().style
                                    text: ListItemData.entities.url.urls[0].expanded_url
                                    content.flags: TextContentFlag.ActiveText
                                    activeTextHandler: ActiveTextHandler {
                                        onTriggered: {
                                            console.debug("labelWebsite::onTriggered")
                                        }
                                    }
                                }
                            }

                        }
                    }
                ]

            }
            Container {
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                horizontalAlignment: HorizontalAlignment.Center
                Container {
                    layout: DockLayout {

                    }
                    ImageView {
                        id: listPosLeft
                        opacity: 1 - listPosLeftShade
                        imageSource: "asset:///images/profile/profile_position-white.png"
                        horizontalAlignment: HorizontalAlignment.Center

                    }
                    ImageView {
                        id: listPosLeftShade
                        opacity: 0
                        imageSource: "asset:///images/profile/profile_position-gray.png"
                        horizontalAlignment: HorizontalAlignment.Center

                    }
                }
                Container {
                    layout: DockLayout {

                    }
                    ImageView {
                        id: listPosRight
                        opacity: 1 - listPosRightShade
                        imageSource: "asset:///images/profile/profile_position-white.png"
                        horizontalAlignment: HorizontalAlignment.Center
                    }
                    ImageView {
                        id: listPosRightShade
                        opacity: 1
                        imageSource: "asset:///images/profile/profile_position-gray.png"
                        horizontalAlignment: HorizontalAlignment.Center

                    }
                }
            }
        }
    }

    attachedObjects: [
        ImagePaintDefinition {
            id: gradient
            repeatPattern: RepeatPattern.X
            imageSource: "asset:///images/profile/profile_pic_gradient.png"
        },
        ImagePaintDefinition {
            id: darkShadeImage
            imageSource: "asset:///images/profile/dark_background.png"
        }
    ]
}
