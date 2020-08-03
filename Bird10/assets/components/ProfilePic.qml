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

WebImageView {
    property alias image_url : profilePic.url
    
    id: profilePic
    url: "http://" + ListItemData.user.profile_image_url_https.substring(8)    // replace https with http
    //url: ListItemData.user.profile_image_url_https.replace("https","http")
    minWidth: 80.0
    minHeight: 80.0

    gestureHandlers: TapHandler {
        onTapped: {
            tapHandled = true
            itemRoot.ListItem.view.openProfile(itemRoot.ListItem.indexPath)
        }
    }
}