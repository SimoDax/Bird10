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

ListItemComponent {
    id: lic
    type: "showThread"
    Container {
        id: itemRoot
        gestureHandlers: TapHandler {
            onTapped: {
                 itemRoot.ListItem.view.openConversation([itemRoot.ListItem.indexPath[0] - 1])    // this component is always placed below the first tweet in the thread
            }
        }

        layout: DockLayout {
        }
        preferredWidth: Infinity
        leftPadding: 15
        topPadding: 15
        bottomPadding: 5
        
        background: bg.imagePaint
        
        Container {
            leftPadding: 110-15
            topPadding: 0
            bottomPadding: 0
            rightPadding: 0
            verticalAlignment: VerticalAlignment.Center
            
            Label {
                text: "Show this conversation"
                textStyle.color: Application.themeSupport.theme.colorTheme.primary
                verticalAlignment: VerticalAlignment.Center
            }
        }
        
        attachedObjects: ImagePaintDefinition {
            id: bg
            imageSource: "asset:///three_dots.amd"
        }
    }
}