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


Container {
    layout: StackLayout {
        orientation: LayoutOrientation.LeftToRight
    }
    Container {
        layoutProperties: StackLayoutProperties {
            spaceQuota: 1.0
        }
        ImageButton {
            defaultImageSource: "asset:///images/reply_hvr_80x80.png"
            pressedImageSource: "asset:///images/reply_on_hvr_80x80.png"
            horizontalAlignment: HorizontalAlignment.Left
            preferredWidth: ui.du(5.5)
            preferredHeight: ui.du(5.5)
            onClicked: {
                if(ListItemData.limited_replies){
                    itemRoot.ListItem.view.pane.error_message("The original Tweet author restricted who can reply to this Tweet")
                    return
                }
                
                var user_mentions = ""
                if(typeof(ListItemData.entities.user_mentions) != 'undefined')
                    for(var i = 0; i < ListItemData.entities.user_mentions.length; i++)
                        user_mentions = user_mentions + " @" + ListItemData.entities.user_mentions[i].screen_name
                        
                if(ListItemData.rt_flag)
                    itemRoot.ListItem.view.reply(ListItemData.user.screen_name + " @" + ListItemData.rt_screen_name + user_mentions, ListItemData.rt_id, itemRoot.ListItem.indexPath)
                else
                    itemRoot.ListItem.view.reply(ListItemData.user.screen_name + user_mentions, ListItemData.id_str, itemRoot.ListItem.indexPath)
            }
        }
    }
    Container {
        layoutProperties: StackLayoutProperties {
            spaceQuota: 1.0
        }
        ImageButton {
            defaultImageSource: ListItemData.retweeted ? "asset:///images/retweet_on_80x80.png" : "asset:///images/retweet_80x80.png"
            horizontalAlignment: HorizontalAlignment.Right
            preferredWidth: ui.du(5.5)
            preferredHeight: ui.du(5.5)
            onClicked: {
                    itemRoot.ListItem.view.rtDialog.open(itemRoot.ListItem.indexPath)
            }
        }
    }
    Container {
        layoutProperties: StackLayoutProperties {
            spaceQuota: 1.0
        }
        verticalAlignment: VerticalAlignment.Center
        leftPadding: 10.0
        Label {
            text: {
                if (ListItemData.retweet_count != 0)
                    return ListItemData.retweet_count < 10000 ? ListItemData.retweet_count : (Math.round(ListItemData.retweet_count / 1000) + "k");
                else
                    return ""
            }
            textStyle.color: ListItemData.retweeted ? Color.create("#669832") : Color.create("#7A7A7A")
            horizontalAlignment: HorizontalAlignment.Left
            verticalAlignment: VerticalAlignment.Center
            textStyle.fontSize: FontSize.XSmall
            preferredHeight: ui.du(5)
        }
    }

    Container {
        layoutProperties: StackLayoutProperties {
            spaceQuota: .7
        }
        verticalAlignment: VerticalAlignment.Center
        ImageButton {
            defaultImageSource: ListItemData.favorited ? "asset:///images/heart_red.png" : "asset:///images/heart_grey.png"
            horizontalAlignment: HorizontalAlignment.Right
            preferredWidth: ui.du(4.2)
            preferredHeight: ui.du(4.2)
            onClicked: {
                if(ListItemData.rt_flag)
                    itemRoot.ListItem.view.api.favorite(ListItemData.rt_id, ListItemData.favorited)
                else
                    itemRoot.ListItem.view.api.favorite(ListItemData.id_str, ListItemData.favorited)
            }
        }
    }
    Container {
        layoutProperties: StackLayoutProperties {
            spaceQuota: 1.0
        }
        verticalAlignment: VerticalAlignment.Center
        leftPadding: 10.0
        Label {
            text: {
                if (ListItemData.retweet_count != 0)
                    return ListItemData.retweet_count < 10000 ? ListItemData.favorite_count : (Math.round(ListItemData.favorite_count / 1000) + "k");
                else
                    return ""
            }
            textStyle.color: ListItemData.favorited ? Color.create("#e0245e") : Color.create("#7A7A7A")
            horizontalAlignment: HorizontalAlignment.Left
            verticalAlignment: VerticalAlignment.Center
            textStyle.fontSize: FontSize.XSmall
        }
    }
    Container {
        layoutProperties: StackLayoutProperties {
            spaceQuota: 1.0
        }
        ImageButton {
            defaultImageSource: "asset:///images/share_tweet.png"
            pressedImageSource: "asset:///images/share_tweet_hvr.png"
            horizontalAlignment: HorizontalAlignment.Center
            preferredWidth: ui.du(5)
            preferredHeight: ui.du(5)
            onClicked: {
                share.trigger("bb.action.SHARE")
            }
            attachedObjects: Invocation {
                id: share
                query {
                    data: "https://twitter.com/" + ListItemData.user.screen_name + "/statuses/" + ListItemData.id_str
                    mimeType: "text/plain"
                    onQueryChanged: share.query.updateQuery() //magic line to make it work in listview. Remove this and you're fucked.
                }
            }
        }
    }
}
