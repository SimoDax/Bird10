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

NavigationPane {
    id: nav
    peekEnabled: true
    
    onCreationCompleted: {
        if (o1Twitter.linked)
            twitterApi.requestProfileTweets()
        else
            loginSheet.open()
            //loginAction.triggered()
    }
    
    function error_message(text){
        errorToast.body = text
        errorToast.show()
    }

    UserProfile {
        onCreationCompleted: {
            setUser(o1Twitter.extraTokens.screen_name)    //Self profile name
        }
    }

    attachedObjects: [
        ComponentDefinition {
            id: tweetSheetDefinition
            content: TweetSheet {
            }
        },
        Delegate {
            id: tweetSheet
            active: false
            sourceComponent: tweetSheetDefinition

        }
    ]
    onPopTransitionEnded: {
        page.destroy();
    }

}
