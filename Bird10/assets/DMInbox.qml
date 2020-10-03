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
import bb.system 1.2
import com.simodax 1.0
import "../components" 1.0

NavigationPane {
    id: nav

    property variant dmApi: app.dm
    property variant inbox: app.dm.inbox
    onInboxChanged: {
        if (isActiveTab)
            dmApi.updateLastSeenId()
    }

    property variant unread: dmApi.unreadCount
    property bool isActiveTab: tabbedPane.activeTab.title == "Messages" // Used to decide whether to call updateLastSeenId on new messages

    // Called when the tab becomes the current one
    function resetUnreadCount() {
        dmApi.updateLastSeenId() // this also resets dmApi.unreadCount
        dmHistoryId.scrollToItem(0)
    }

    function loadInbox() { // called by main on login
        dmApi.loadInboxInitialState();
        dmApi.requestBadgeCount();
    }
    
    DMInboxPage {
        id: thisDMHistory
    }

    onPopTransitionEnded: {
        if (page.objectName == "thisPersonDMPage")
            dmApi.resetCurrentConversation();
        page.destroy()
    }
}
