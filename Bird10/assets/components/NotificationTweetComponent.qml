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
import bb.cascades 1.4
import org.labsquare 1.0

ListItemComponent {
    id: lic
    //type: "item"
    //type: ""
    property bool actionsVisible: true
    property bool dividerVisible: true

    Container {
        property bool tapHandled: false
        gestureHandlers: TapHandler {
            onTapped: {
                if (tapHandled)
                    tapHandled = false;
                else {
                    itemRoot.ListItem.view.openConversation(itemRoot.ListItem.indexPath)
                }
            }
        }

        id: itemRoot

        preferredWidth: Infinity
        background: (itemRoot.ListItem.selected ) ? ui.palette.primary : SystemDefaults.Paints.ContainerBackground

        Container {

            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }

            topPadding: 15.0
            leftPadding: 15.0
            rightPadding: 15.0
            bottomPadding: 15.0

            ProfilePic {
            }

            Container {
                layout: StackLayout {
                    orientation: LayoutOrientation.TopToBottom
                }

                verticalAlignment: VerticalAlignment.Fill
                leftMargin: 15.0

                Container {
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }

                    Label {
                        id: username
                        text: ListItemData.user.name
                        textStyle.fontWeight: FontWeight.Bold
                        textStyle.fontSizeValue: 7.0
                        rightMargin: 5.0
                        textStyle.fontFamily: 'Slate Pro'
                    }
                    Label {
                        id: handle
                        text: "@" + ListItemData.user.screen_name
                        textStyle.fontSizeValue: 7.0
                        textStyle.color: Color.create("#969696")
                        leftMargin: 0.0
                        textStyle.fontFamily: 'Slate Pro'
                    }

                }

                Label {
                    text: ListItemData.full_text
                    multiline: true
                    textFormat: TextFormat.Html
                    //textStyle.fontFamily: 'Slate Pro'
                    topMargin: 10
                    content.flags: TextContentFlag.ActiveText | TextContentFlag.Emoticons
                    textStyle.base: itemRoot.ListItem.view.twemojiStyle
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

            contextActions: [
                ActionSet {
                    ActionItem {
                        title: "Open in browser"
                        imageSource: "asset:///images/ic_open.png"
                        onTriggered: {
                            //console.debug(itemRoot.ListItem.indexPath)
                            //console.debug(ListItemData.user.screen_name)
                            open.trigger("bb.action.OPEN")
                        }

                        attachedObjects: Invocation {
                            id: open
                            query {
                                uri: "https://twitter.com/" + ListItemData.user.screen_name + "/statuses/" + ListItemData.id_str
                                invokeTargetId: "sys.browser"
                                onQueryChanged: open.query.updateQuery() //magic line to make it work in listview. Remove this and you're fucked.
                            }
                        }
                    }
                    //                                    ActionItem {
                    //                                        title: "Share Tweet"
                    //                                        imageSource: "asset:///images/ic_open.png"
                    //                                        onTriggered: {
                    //                                            //console.debug(itemRoot.ListItem.indexPath)
                    //                                            //console.debug(ListItemData.user.screen_name)
                    //                                            open.trigger("bb.action.SHARE")
                    //                                        }
                    //
                    //                                        attachedObjects: Invocation {
                    //                                            id: share
                    //                                            query {
                    //                                                uri: "https://twitter.com/" + ListItemData.user.screen_name + "/statuses/" + ListItemData.id_str
                    //                                                mimeType: "text/plain"
                    //                                                onQueryChanged: open.query.updateQuery() //magic line to make it work in listview. Remove this and you're fucked.
                    //                                            }
                    //                                        }
                    //                                    }
                }
            ]
            attachedObjects: [
                ComponentDefinition {
                    id: imageDefinition
                    property url url

                    WebImageView {
                        id: imageView_
                        url: imageDefinition.url
                        //                                      url: {
                        //                                          var url_string = ListItemData.extended_entities.media[0].media_url.toString()
                        //                                          url_string.slice(0,-4)
                        //                                          url_string = url_string.concat("?format=jpg&name=medium")
                        //                                          return url_string
                        //                                      }
                        scalingMethod: ScalingMethod.AspectFill
                        horizontalAlignment: HorizontalAlignment.Fill
                        //loadEffect: ImageViewLoadEffect.FadeZoom
                        maxHeight: 300
                    }
                },
                ComponentDefinition {
                    id: gridDefinition
                    Container {
                        layout: GridLayout {
                            columnCount: 2
                        }
                        horizontalAlignment: HorizontalAlignment.Fill
                        background: Color.DarkGreen
                    }
                }
            ]
        }

        Divider {
            visible: itemRoot.ListItem.component.dividerVisible
        }
    }
}
