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

ActionItem {
    id: payAction
    title: "Donate"
    imageSource: "asset:///images/heart.png"
    onTriggered: {
        _pay.trigger("bb.action.OPEN")
    }
    attachedObjects: Invocation {
        id: _pay
        query {
            uri: "https://paypal.me/pools/c/8pJlhpRSa8"
            invokeTargetId: "sys.browser"
        }
    }
}