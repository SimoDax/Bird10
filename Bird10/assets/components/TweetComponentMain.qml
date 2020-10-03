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
    type: "main"
    
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
            visible: ! ListItemData.is_quote_status
            maxHeight: 10
            minHeight: 10
        }
        
        QuotedTweetComponent {
            visible: delegateActive
            delegateActive: ListItemData.is_quote_status
        }
        
        TweetActionBarMain{
        }
    }
}