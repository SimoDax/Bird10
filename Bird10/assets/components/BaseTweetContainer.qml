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
                    title: "DM user"
                    imageSource: "asset:///images/mail_80x80.png"
                    onTriggered: itemRoot.ListItem.view.dmUser(ListItemData.user)
                    enabled: ListItemData.user.followed_by
                }
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
                    onTriggered: itemRoot.ListItem.view.dmLink("https://twitter.com/" + ListItemData.user.screen_name + "/statuses/" + ListItemData.id_str)
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
                    itemRoot.ListItem.view.api.destroyTweet(ListItemData.id_str)
                }
            }
        ]
    }
