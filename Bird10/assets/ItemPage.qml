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
import "/components"

Page {
    property alias itemPageTitle: titlebar.title
    property alias dataModel: carouselDataModel
    
    function navigateToIndex(index){
        lv.scrollToItem(index)
    }
    
    titleBar: TitleBar {
        id: titlebar
    }
    Container {
        attachedObjects: LayoutUpdateHandler {
            property alias height: pageSize.layoutFrame.height
            property alias width: pageSize.layoutFrame.width
            id: pageSize
        }
        ListView {
            id: lv
            property int n : carouselDataModel.size()
            property variant pageSize: pageSize
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

                    Container {
                        id: itemRoot
                        property variant pageSize: ListItem.view.pageSize
                        maxWidth: pageSize.width
                        minWidth: pageSize.width
                        //horizontalAlignment: HorizontalAlignment.Fill
                        //verticalAlignment: VerticalAlignment.Fill
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
                        BigPic {
                            id: containerMediaItem
                            imagePath: ListItemData.media_url

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
