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
    property bool tapHandled: false
    gestureHandlers: TapHandler {
        onTapped: {
            if (tapHandled)
                tapHandled = false;
            else {
                itemRoot.ListItem.view.openConversation(itemRoot.ListItem.indexPath)
            }
        }
    }


     property bool initialized: ListItem.initialized
     onInitializedChanged: {
         if(ListItemData.user.screen_name == itemRoot.ListItem.view.authenticator.extraTokens.screen_name){
             actionSet.add(deleteActionItem)
         }
         else if(actionSet.indexOf(deleteActionItem) != -1)
             actionSet.remove(deleteActionItem)
     }

    id: itemRoot

    preferredWidth: Infinity
    background: (itemRoot.ListItem.selected || itemRoot.ListItem.active) ? ui.palette.primary : SystemDefaults.Paints.ContainerBackground

    contextActions: [
            ActionSet {
                id: actionSet
                ActionItem {
                    title: "Open in browser"
                    imageSource: "asset:///images/ic_open.png"
                    onTriggered: {
                        //console.debug(itemRoot.ListItem.indexPath)
                        //console.debug(ListItemData.user.screen_name)
                        open.trigger("bb.action.OPEN")
                    }

                    attachedObjects: Invocation {
                        id: open
                        query {
                            uri: "https://twitter.com/" + ListItemData.user.screen_name + "/statuses/" + ListItemData.id_str
                            invokeTargetId: "sys.browser"
                            onQueryChanged: open.query.updateQuery() //magic line to make it work in listview. Remove this and you're fucked.
                        }
                    }
                }
                ActionItem {
                    title: "Send via DM"
                    imageSource: "asset:///images/share_tweet_white.png"
                    onTriggered: itemRoot.ListItem.view.dmLink("https://twitter.com/" + ListItemData.user.screen_name + "/statuses/" + ListItemData.id_str)
                }
                ActionItem {
                    title: "DM user"
                    imageSource: "asset:///images/mail_80x80.png"
                    onTriggered: itemRoot.ListItem.view.dmUser(ListItemData.user)
                    enabled: ListItemData.user.can_dm
                }
//                                                    ActionItem {
//                                                        title: "Share Tweet"
//                                                        imageSource: "asset:///images/ic_open.png"
//                                                        onTriggered: {
//                                                            //console.debug(itemRoot.ListItem.indexPath)
//                                                            //console.debug(ListItemData.user.screen_name)
//                                                            open.trigger("bb.action.SHARE")
//                                                        }
//                
//                                                        attachedObjects: Invocation {
//                                                            id: share
//                                                            query: InvokeQuery {
//                                                            invokeActionId: "bb.action.SHARE" 
//                                                            invokerIncluded: true
//                        //data: "https://twitter.com/" + ListItemData.user.screen_name + "/statuses/" + ListItemData.id_str
//                                                                data: "aaaaa"
//                                                                mimeType: "text/plain"
//                                                                uri: "data://"
//                                                                onQueryChanged: open.query.updateQuery() //magic line to make it work in listview. Remove this and you're fucked.
//                                                            }
//                                                        }
//                                                    }
            }
        ]
        attachedObjects: [
            DeleteActionItem {
                id: deleteActionItem
                title: "Delete Tweet"
                onTriggered: {
                    itemRoot.ListItem.view.deleteDialog.tweet_id = ListItemData.id_str
                    itemRoot.ListItem.view.deleteDialog.show()
                }
            }
        ]
    }
