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

ActiveTextHandler {
    onTriggered: {
        console.debug(event.href)
        if (event.href.toString().charAt(0) == "#"){
            event.abort()
            itemRoot.ListItem.view.openHashtag(event.href.toString())
        }
        else if (event.href.toString().charAt(0) == "@"){
            event.abort()
            itemRoot.ListItem.view.openProfileFromScreenName(event.href.toString().substring(1))    //remove leading @
        }
    }
}