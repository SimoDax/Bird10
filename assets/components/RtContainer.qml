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
    leftPadding: 110
    topPadding: 20
    horizontalAlignment: HorizontalAlignment.Fill
    layout: StackLayout {
        orientation: LayoutOrientation.LeftToRight
    }
    ImageView {
        imageSource: "asset:///images/retweet_80x80.png"
        scalingMethod: ScalingMethod.AspectFit
        minWidth: ui.du(4)
        maxWidth: ui.du(4)
        verticalAlignment: VerticalAlignment.Center
    }
    Label {
        text: ListItemData.rt_name + " retweeted"
        textStyle.color: Color.create("#7A7A7A")
    }
}