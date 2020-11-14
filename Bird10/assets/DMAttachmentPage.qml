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

Page {
    property alias image: imageView.image
    
    Container {
        attachedObjects: LayoutUpdateHandler {
            property alias height: pageSize.layoutFrame.height
            property alias width: pageSize.layoutFrame.width
            id: pageSize
        }
        
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        layout: DockLayout {
        }
        ScrollView {
            id: scrollView
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill

            scrollViewProperties {
                minContentScale: 1
                initialScalingMethod: ScalingMethod.AspectFill
                scrollMode: ScrollMode.Both
                pinchToZoomEnabled: true
                overScrollEffectMode: OverScrollEffectMode.None
            }
            
            Container{
                maxHeight: pageSize.height
                maxWidth: pageSize.width

                ImageView {
                    id: imageView
                    horizontalAlignment: HorizontalAlignment.Center
                    verticalAlignment: VerticalAlignment.Center
                    scalingMethod: ScalingMethod.AspectFit
                    maxHeight: pageSize.height
                    maxWidth: pageSize.width
                    minWidth: pageSize.width
                    minHeight: pageSize.height
                }
            }
        }
    }
}
