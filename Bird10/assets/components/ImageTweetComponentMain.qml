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
import com.pipacs.o2 1.0
import bb.system 1.2
import org.labsquare 1.0
import bb.multimedia 1.4

ListItemComponent {
    id: lic
    type: "main_image"
    
    BaseTweetContainer {
        
        id: itemRoot

        layout: StackLayout {
            orientation: LayoutOrientation.TopToBottom
        }
        
        topPadding: 10.0
        leftPadding: 20
        rightPadding: 20.0
        bottomPadding: 15
        
        TweetAuthorMain {
        }
        
        Label {
            text: ListItemData.full_text
            multiline: true
            textFormat: TextFormat.Html
            textStyle.fontFamily: 'Slate Pro'
            //topMargin: 10
            horizontalAlignment: HorizontalAlignment.Fill
            textStyle.fontSize: FontSize.PointValue
            textStyle.fontSizeValue: 7.5
            content.flags: TextContentFlag.ActiveText | TextContentFlag.Emoticons
            activeTextHandler: TweetTextHandler {
            }
        }

        Container {
            layout: GridLayout {
                columnCount: 2
            }
            bottomMargin: 5
            horizontalAlignment: HorizontalAlignment.Fill
            gestureHandlers: TapHandler {
                onTapped: {
                    //TODO check which image was tapped
                    itemRoot.ListItem.view.openMedia(itemRoot.ListItem.indexPath, 0)
                    tapHandled = true
                }
            }

            Container {
                WebImageView {
                    id: imageView
                    //url: ListItemData.image_flag ? ListItemData.extended_entities.media[0].media_url : ""
                    url: {
                        if (ListItemData.extended_entities.media.length > 0) {
                            var url_string = ListItemData.extended_entities.media[0].media_url.toString()
                            url_string.slice(0, -4)
                            url_string = url_string.concat("?format=jpg&name=medium")
                            return url_string
                        } else return ""
                    }

                    function update() {
                        if (ListItemData.image_flag) {
                            var url_string = ListItemData.extended_entities.media[0].media_url.toString()
                            url_string.slice(0, -4)
                            url_string = url_string.concat("?format=jpg&name=medium")
                            url = url_string
                            console.debug(url_string)
                        }
                    }

                    scalingMethod: ScalingMethod.AspectFill
                    horizontalAlignment: HorizontalAlignment.Fill
                    verticalAlignment: VerticalAlignment.Fill
                    //loadEffect: ImageViewLoadEffect.FadeZoom
                    minHeight: 400
                    maxHeight: 400
                    loadEffect: ImageViewLoadEffect.None

                }
                topPadding: 5
                leftPadding: 5
                rightPadding: 5
                bottomPadding: 5
                horizontalAlignment: HorizontalAlignment.Fill
            }

            Container {
                visible: ListItemData.extended_entities.media.length > 1
                WebImageView {
                    id: imageView2
                    //url: ListItemData.image_flag ? ListItemData.extended_entities.media[0].media_url : ""
                    url: {
                        if (ListItemData.extended_entities.media.length > 1) {
                            var url_string = ListItemData.extended_entities.media[1].media_url.toString()
                            url_string.slice(0, -4)
                            url_string = url_string.concat("?format=jpg&name=medium")
                            return url_string
                        } else return ""
                    }
                    scalingMethod: ScalingMethod.AspectFill
                    horizontalAlignment: HorizontalAlignment.Fill
                    verticalAlignment: VerticalAlignment.Fill
                    minHeight: 400
                    maxHeight: 400
                    loadEffect: ImageViewLoadEffect.None
                }
                topPadding: 5
                leftPadding: 5
                rightPadding: 5
                bottomPadding: 5
                horizontalAlignment: HorizontalAlignment.Fill
            }

            Container {
                visible: ListItemData.extended_entities.media.length > 2
                WebImageView {
                    id: imageView3
                    //url: ListItemData.image_flag ? ListItemData.extended_entities.media[0].media_url : ""
                    url: {
                        if (ListItemData.extended_entities.media.length > 2) {
                            var url_string = ListItemData.extended_entities.media[2].media_url.toString()
                            url_string.slice(0, -4)
                            url_string = url_string.concat("?format=jpg&name=medium")
                            return url_string
                        } else return ""
                    }
                    scalingMethod: ScalingMethod.AspectFill
                    horizontalAlignment: HorizontalAlignment.Fill
                    verticalAlignment: VerticalAlignment.Fill
                    minHeight: 400
                    maxHeight: 400
                    loadEffect: ImageViewLoadEffect.None
                }
                topPadding: 5
                leftPadding: 5
                rightPadding: 5
                bottomPadding: 5
                horizontalAlignment: HorizontalAlignment.Fill
            }

            Container {
                visible: ListItemData.extended_entities.media.length > 3
                WebImageView {
                    id: imageView4
                    //url: ListItemData.image_flag ? ListItemData.extended_entities.media[0].media_url : ""
                    url: {
                        if (ListItemData.extended_entities.media.length > 3) {
                            var url_string = ListItemData.extended_entities.media[3].media_url.toString()
                            url_string.slice(0, -4)
                            url_string = url_string.concat("?format=jpg&name=medium")
                            return url_string
                        } else return ""
                    }
                    scalingMethod: ScalingMethod.AspectFill
                    horizontalAlignment: HorizontalAlignment.Fill
                    verticalAlignment: VerticalAlignment.Fill
                    minHeight: 400
                    maxHeight: 400
                    loadEffect: ImageViewLoadEffect.None
                }
                topPadding: 5
                leftPadding: 5
                rightPadding: 5
                bottomPadding: 5
                horizontalAlignment: HorizontalAlignment.Fill
            }
        }

        QuotedTweetComponent {
            visible: delegateActive
            delegateActive: ListItemData.is_quote_status
        }
        
        TweetActionBarMain{
        }
    }
}