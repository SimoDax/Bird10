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
    type: "main_video"
    
    BaseTweetContainer {
        gestureHandlers: [] // we don't want to open another conversation identical to the current one when the user taps the main tweet

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
            gestureHandlers: TapHandler {
                onTapped: {
                    itemRoot.ListItem.view.openMedia(itemRoot.ListItem.indexPath, 0)
                    tapHandled = true
                }
            }
            WebImageView {
                id: imageView
                //url: ListItemData.image_flag ? ListItemData.extended_entities.media[0].media_url : ""
                url: {
                    var url_string = ListItemData.extended_entities.media[0].media_url.toString()
                    url_string.slice(0, -4)
                    url_string = url_string.concat("?format=jpg&name=medium")
                    return url_string
                }
                scalingMethod: ScalingMethod.AspectFill
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill
                maxHeight: 300
                loadEffect: ImageViewLoadEffect.Subtle
            }
            ImageView {
                maxHeight: 300
                //opacity: .9
                scalingMethod: ScalingMethod.AspectFit
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill
                imageSource: "asset:///images/play.png"
            }
            topPadding: 5
            leftPadding: 5
            rightPadding: 5
            bottomPadding: 5
            layout: DockLayout {

            }
            horizontalAlignment: HorizontalAlignment.Fill
        }
        CardComponent {
        }
        QuotedTweetComponent {
            visible: delegateActive
            delegateActive: ListItemData.is_quote_status
        }
        TweetActionBarMain{
        }
        
    }
}