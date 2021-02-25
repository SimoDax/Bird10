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
    property real barPercentage: 0
    property bool isWinner: false
    property bool isVotedByUser: false
    property alias barColor: bg.background
    property alias text: choiceLabel.text
    //                                        minHeight: ui.du(6)
    //                                        maxHeight: ui.du(6)
    topPadding: ui.du(.5)
    bottomPadding: ui.du(.5)
    preferredWidth: Infinity
    layout: DockLayout {
    }
    Container {
        id: bg
        verticalAlignment: VerticalAlignment.Fill
        horizontalAlignment: HorizontalAlignment.Left
        minWidth: barPercentage * resultsContainerSize.width
        maxWidth: barPercentage * resultsContainerSize.width
        background: {
            if(Application.themeSupport.theme.colorTheme.style == VisualStyle.Bright)
                return isWinner ? Color.create("#71C9F8") : Color.create("#CCD6DD")
            else 
                return isWinner ? Color.create("#66b2fe") : Color.create("#3D5466")
        }
    }
    Container {
        verticalAlignment: VerticalAlignment.Center
        topPadding: ui.du(1)
        bottomPadding: ui.du(1)
        leftPadding: ui.du(1)
        Label {
            id: choiceLabel
            verticalAlignment: VerticalAlignment.Center
            horizontalAlignment: HorizontalAlignment.Left
            textStyle.fontWeight: isWinner ? FontWeight.W500 : FontWeight.Normal
        }
    }
    ImageView {
        id: checkmark
        visible: isVotedByUser
        imageSource: Application.themeSupport.theme.colorTheme.style == VisualStyle.Bright ? "asset:///images/ic_done_black.png" : "asset:///images/ic_done.png"
        verticalAlignment: VerticalAlignment.Center
        horizontalAlignment: HorizontalAlignment.Right
        scalingMethod: ScalingMethod.AspectFit
        maxHeight: ui.du(4)
//        rightMargin: ui.du(1)
    }
}