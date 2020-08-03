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
import bb.system 1.0
import org.labsquare 1.0

Container {
    id: bigPic
    property string imagePath
    property variant mediaController
    signal hideTweet()
    //property variant pageSize: itemRoot.ListItem.view.pageSize
        
    signal cachedFileNameAvailable(string fileName);
    
    background: Color.Black
    minHeight: pageSize.height
    
    ScrollView {
        id: scrollView
        horizontalAlignment: HorizontalAlignment.Center
        verticalAlignment: VerticalAlignment.Center

        scrollViewProperties {
            minContentScale: 1
            initialScalingMethod: ScalingMethod.AspectFill
            scrollMode: ScrollMode.Both
            pinchToZoomEnabled: true
            overScrollEffectMode: OverScrollEffectMode.None                        
        }        

        Container {
            id: imageContainer
            layout: DockLayout {
            }
            layoutProperties: StackLayoutProperties {
                spaceQuota: 1
            }
            maxHeight: pageSize.height //allow for action bar
            maxWidth: pageSize.width
            minWidth: pageSize.width
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Center
            WebImageView {
                id: imageView                
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center
                url: bigPic.imagePath
                scalingMethod: ScalingMethod.AspectFit
                minWidth: pageSize.width
                minHeight: pageSize.height
                maxHeight: pageSize.height //allow for action bar
                maxWidth: pageSize.width
            }    
//            contextActions: [
//                ActionSet {
//                    id: tweetActions                    
//                    title: qsTr("Image") + Retranslate.onLocaleOrLanguageChanged
//                   
//                    SaveImageItem {
//                        onTriggered: {
//                            imageView.saveImageToShare();   
//                        }
//                    }
//                   
//					SetAsActionItem {
//					    id: setAsActionItem	
//					    onTriggered: {
//                            setAsActionItem.invoke(bigPic.mediaController.getCachedImageFilePath(bigPic.getNetImageView().imageUrl));                            
//                        }
//					}
//					
//                    ShareImageActionItem {
//                        id: shareImageActionItem
//                        onTriggered: {
//                            shareImageActionItem.invoke(bigPic.mediaController.getCachedImageFilePath(bigPic.getNetImageView().imageUrl)); 
//                        }
//                    }
//                }
//            ]
            
            attachedObjects: [
                LayoutUpdateHandler {
                    id: layoutHandler
                    onLayoutFrameChanged: {
                        //console.log(" Layout Frame: [" + layoutFrame.x + ", " + layoutFrame.y + "]");
                        /*
                         * Check if the layout frame x or y values have changed (we've zoomed in).
                         * If they have, then prevent scroll on the listview.
                         */
                        if ( layoutFrame.x != 0 || layoutFrame.y != 0) {
                            hideTweet()
                            itemRoot.ListItem.view.touchPropagationMode = TouchPropagationMode.PassThrough;
                        } else {
                            itemRoot.ListItem.view.touchPropagationMode = TouchPropagationMode.Full;
                        }
                    }
                },
   
                OrientationHandler {
                    onOrientationAboutToChange: {
                        if (orientation == UIOrientation.Landscape) {
                            imageView.horizontalAlignment = HorizontalAlignment.Fill
                        } else {
                            imageView.horizontalAlignment = HorizontalAlignment.Center
                            imageView.verticalAlignment = VerticalAlignment.Center
                        }
                    }
                }
            ]   
        }                        
    }
//    onTouch:
//    {
//        if (event.isUp()) {
//            parent.parent.setBackButtonsVisible(! parent.parent.backButtonsVisible);
//        }
//    }      
    

//    onCreationCompleted: {        
//        imageView.imageSaved.connect(toastMessage);
//    }
    
    
    function getNetImageView(){
        return imageView;        
    }
}
