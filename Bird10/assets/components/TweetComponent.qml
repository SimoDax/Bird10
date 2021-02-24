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

ListItemComponent {
    id: lic
    //type: "item"
    type: ""
    property bool actionsVisible : true
    property bool dividerVisible: true
    
    property variant a: app
    
    BaseTweetContainer {
        
        id: itemRoot   

        RtContainer {
            visible: ListItemData.rt_flag
        }
        
        Container{
            attachedObjects: ImagePaintDefinition {
                id: bg
                imageSource: "asset:///thread_bar.amd"
            }
            background: typeof(ListItemData.divider_visible_flag) == 'undefined' || ListItemData.divider_visible_flag ? SystemDefaults.Paints.ContainerBackground : bg.imagePaint

            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
                }
                
                topPadding: 15.0
                leftPadding: 15.0
                rightPadding: 15.0
                bottomPadding: 15.0

                Container {
                topPadding: 0
                leftPadding: 0
                rightPadding: 0
                bottomPadding: 0
                layout: StackLayout {
                        orientation: LayoutOrientation.TopToBottom
                    }
    
                    ProfilePic {
                    }
                    
                    
                }
                
                Container {
                    layout: StackLayout {
                        orientation: LayoutOrientation.TopToBottom
                    }
                    
                    verticalAlignment: VerticalAlignment.Fill
                    leftMargin: 15.0
                    
                    TweetHeader {
                        
                    }
                    
                    Label {
                        text: ListItemData.full_text
                        multiline: true
                        textFormat: TextFormat.Html
                        //textStyle.fontFamily: 'Slate Pro'
                        topMargin: 10
                        content.flags: TextContentFlag.ActiveText | TextContentFlag.Emoticons
                        textStyle.base: itemRoot.ListItem.view.twemojiStyle
                        activeTextHandler: TweetTextHandler {
                        }
                    }
                    
                    QuotedTweetComponent {
                        visible: delegateActive
                        delegateActive: ListItemData.is_quote_status
                    }
                    
                    TweetActionBar {
                        id: actionBar
                        visible: itemRoot.ListItem.component.actionsVisible
                    }
                    
                    //                                ControlDelegate {
                    //                                    id:imageGrid
                    //                                    delegateActive: ListItemData.image_flag
                    //                                    sourceComponent: gridDefinition
                    //
                    //                                    onCreationCompleted: {
                    //                                        if(delegateActive)
                    //                                            for(var i = 0; i<ListItemData.extended_entities.media.length; i++){
                    //                                                var x = imageDefinition.createObject()
                    //                                                var url_string = ListItemData.extended_entities.media[i].media_url.toString()
                    //                                                url_string.slice(0, -4)
                    //                                                url_string = url_string.concat("?format=jpg&name=medium")
                    //                                                x.url = url_string
                    //                                                imageGrid.control.add(x)
                    //                                                console.debug(ListItemData.user.screen_name+" "+url_string)
                    //                                            }
                    //                                    }
                    //                                }
                }
            }

        Divider {
            visible: ListItemData.divider_visible_flag
        }
    }
}
