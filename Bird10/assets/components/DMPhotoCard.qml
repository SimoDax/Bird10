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

Container {
    id: photoContainer
    property variant listViewHandle
    
    
    preferredWidth: HorizontalAlignment.Fill
    AuthorizedWebImageView {
        authenticator: {
            console.debug(dmListItem.authenticator);
            return dmListItem.authenticator;
        }
        id: photoImage
//        defaultUrl: "asset:///images/lists/default_placeholder.png"
        url: ListItemData.photoUrlSmall
        maxHeight: (300)
        minHeight: (200)
        preferredHeight: 300
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        scalingMethod: ScalingMethod.AspectFit
        //        persistent: true
        gestureHandlers: [
            TapHandler {
                onTapped: {
                    photoImage.clicked()
                }
            }
        ]
        
        

        function clicked() {
            listViewHandle.openFullScreenImage(photoImage);
        }

        onCreationCompleted: {
//            compatibilityManager.setFocusable(photoImage, true);

            //sometimes, the card is created then being recyled ( due to user fast scrolling or gap item)
            //in that case, the defaul url is not set. then, later on, if network image error happens, we will show some wrong image ( due to recycle)
            //make sure the default url is set
//            defaultUrl = "asset:///images/lists/default_placeholder.png"
        }
    }
}
