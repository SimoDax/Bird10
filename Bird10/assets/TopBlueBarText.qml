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

Container {
    property alias text : titleText.text
    property alias searchVisible : searchBtn.visible
    property alias tweetVisible: tweetBtn.visible
    property alias ts: tweetSheet
    
    layout: DockLayout {
    }
    preferredHeight: ui.du(8.5)
    background: Application.themeSupport.theme.colorTheme.style == VisualStyle.Bright ? Color.create("#66b2fe") : Color.create("#262626")
    leftPadding: 20
    horizontalAlignment: HorizontalAlignment.Fill

    Container {
        horizontalAlignment: HorizontalAlignment.Left
        verticalAlignment: VerticalAlignment.Center
        layout: StackLayout {
            orientation: LayoutOrientation.LeftToRight
        }
        ImageView {
            preferredHeight: ui.du(5)
            preferredWidth: ui.du(5)
            imageSource: "asset:///images/bird_logo_white.png"
            scalingMethod: ScalingMethod.AspectFit
        }
    }
        Label {
            horizontalAlignment: HorizontalAlignment.Center
            id: titleText
            text: "Home"
            textStyle.color: Color.White
            textStyle.fontWeight: FontWeight.W500
            textStyle.fontSize: FontSize.PointValue
            textStyle.fontSizeValue: 8.5
            verticalAlignment: VerticalAlignment.Center
        }
        
    Container{
        horizontalAlignment: HorizontalAlignment.Right
        verticalAlignment: VerticalAlignment.Center
        layout: StackLayout {
            orientation: LayoutOrientation.LeftToRight
        }
        ImageButton {
            id: searchBtn
            //horizontalAlignment: HorizontalAlignment.Right
            verticalAlignment: VerticalAlignment.Center
            rightMargin: 0
            //minHeight: ui.du(2)
            //preferredWidth: ui.du(5)
            maxWidth: ui.du(9)
            maxHeight: ui.du(10)
    
            defaultImageSource: "asset:///images/icn_search_white_100x100.png"
            pressedImageSource: "asset:///images/icn_search_focused_100x100.png"
    
            onClicked: {
                searchSheet.active = true
                searchSheet.object.open()
            }
        }
    
        ImageButton {
            id: tweetBtn
            //horizontalAlignment: HorizontalAlignment.Right
            verticalAlignment: VerticalAlignment.Center
            leftMargin: 0
            //minHeight: ui.du(2)
            //preferredWidth: ui.du(5)
            maxWidth: ui.du(9.5)
            maxHeight: ui.du(10)
            
            defaultImageSource: "asset:///images/icn_compose_white_100x100.png"
            pressedImageSource: "asset:///images/icn_compose_focused_100x100.png"
            
            onClicked: {
                tweetSheet.active = true
                tweetSheet.object.open()
            }
            visible: ! (tweetSheet.active && tweetSheet.object.busy)
        }
        Container {
            verticalAlignment: VerticalAlignment.Center
            rightPadding: 20
            leftPadding: 20
            visible: tweetSheet.active && tweetSheet.object.busy
            ActivityIndicator {
                id: tweeting
                maxHeight: 10    // just to set the smallest preset available
                maxWidth: 10
                leftMargin: 0
                rightMargin: 0
                verticalAlignment: VerticalAlignment.Center
                running: tweetSheet.active && tweetSheet.object.busy
            }
        }
    }
        
    attachedObjects: [
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
        ComponentDefinition {
            id: searchSheetDefinition
            content: Search {
            }
        },
        Delegate {
            id: searchSheet
            active: false
            sourceComponent: searchSheetDefinition
        },
        ComponentDefinition {
            id: userProfileDefinition
            source: "asset:///UserProfile.qml"
        },
        ComponentDefinition {
            id: discoverPage
            source: "asset:///DiscoverPage.qml"
        }
    ]
}
