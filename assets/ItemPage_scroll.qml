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

Page {
    property alias itemPageTitle: titlebar.title
    property alias dataModel: carouselDataModel
    
    titleBar: TitleBar {
        id: titlebar
    }
    Container {
        ListView {
            property int n : carouselDataModel.size()
            //horizontalAlignment: HorizontalAlignment.Fill
            //verticalAlignment: VerticalAlignment.Fill
            
            dataModel: ArrayDataModel {
                id: carouselDataModel
            }
            //scrollRole: ScrollRole.None
            
            property variant a : app
            
            function save(u){
                app.saveImage(u)
            }
            
            function share(u){
                app.shareImage(u)
            }
            
            listItemComponents: [
                ListItemComponent {
                    type: ""

                    ScrollView {
                        id: itemRoot
                        scrollViewProperties {
                            minContentScale: 1
                            initialScalingMethod: ScalingMethod.AspectFit
                            scrollMode: ScrollMode.Both
                            pinchToZoomEnabled: true
                            overScrollEffectMode: OverScrollEffectMode.None
                        }
                        //horizontalAlignment: HorizontalAlignment.Fill
                        verticalAlignment: VerticalAlignment.Fill
                        //maxWidth: 720
                        //minWidth: 720
//                        scrollViewProperties.pinchToZoomEnabled: true
//                        scrollViewProperties.initialScalingMethod: ScalingMethod.Fill
//                        scrollViewProperties.minContentScale: 1.0
//                        scrollViewProperties.maxContentScale: 8.0
//                        scrollViewProperties.overScrollEffectMode: OverScrollEffectMode.OnPinchAndScroll
//                        scrollRole: ScrollRole.Main

                            //overlapTouchPolicy: OverlapTouchPolicy.Allow
                            //touchPropagationMode: TouchPropagationMode.PassThrough
                            ScreenContainer{
                                verticalAlignment: VerticalAlignment.Fill
                            WebImageView {
                            url: ListItemData.media_url
                            scalingMethod: ScalingMethod.AspectFit
                            visible: true
                            //maxWidth: 720
                            //minWidth: 720
                            horizontalAlignment: HorizontalAlignment.Fill
                            verticalAlignment: VerticalAlignment.Fill
                            loadEffect: ImageViewLoadEffect.None
                            
//                            onLoadingFinished: {
//                                itemRoot.ListItem.view.n -= 1
//                                if(itemRoot.ListItem.view.n == 0){
//                                    //forces re-rendering
//                                    itemRoot.ListItem.view.visible = false
//                                    itemRoot.ListItem.view.visible = true
//                                }
//                            }
                        }
                        }
                            
                    
                        contextActions: [
                            ActionSet {
                                ActionItem {
                                    title: "Save"
                                    imageSource: "asset:///images/ic_save.png"
                                    onTriggered: {
                                        itemRoot.ListItem.view.a.saveImage(ListItemData.media_url)
                                    }
                                }
                                ActionItem {
                                    title: "Share / Edit"
                                    imageSource: "asset:///images/ic_open.png"
                                    onTriggered: {
                                        itemRoot.ListItem.view.share(ListItemData.media_url)
                                    }
                                }
                            }
                        ]

                        attachedObjects: [
                            LayoutUpdateHandler {
                                id: layoutHandler
                                onLayoutFrameChanged: {
                                    console.log(" Layout Frame: [" + layoutFrame.x + ", " + layoutFrame.y + "]");
                                    /*
                                     * Check if the layout frame x or y values have changed (we've zoomed in).
                                     * If they have, then prevent scroll on the listview.
                                     */
//                                    if (layoutFrame.x != 0 || layoutFrame.y != 0) {
//                                        itemRoot.ListItem.view.touchPropagationMode = TouchPropagationMode.PassThrough;
//                                        itemRoot.ListItem.view.scrollRole = ScrollRole.None;
//                                    } else {
//                                        itemRoot.ListItem.view.touchPropagationMode = TouchPropagationMode.Full;
//                                        itemRoot.ListItem.view.scrollRole = ScrollRole.Main;
//                                    }
                                }
                            }
                        ]
                    }
                }
            ]
            flickMode: FlickMode.SingleItem
            snapMode: SnapMode.LeadingEdge
            layout: StackListLayout {
                orientation: LayoutOrientation.LeftToRight

            }
            //touchPropagationMode: TouchPropagationMode.PassThrough
            //overlapTouchPolicy: OverlapTouchPolicy.Allow
        }
    }
}
