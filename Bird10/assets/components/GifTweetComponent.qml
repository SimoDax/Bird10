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
import bb.multimedia 1.4

ListItemComponent {
    type: "gif"
    BaseTweetContainer {
        id: itemRoot

        property variant i: ListItem.initialized

        onIChanged: {
            //console.debug("indexPath: " + ListItem.indexPath + " initialized: " + i)
            if (i == false)
                player.reset();
            else {
                player.setSourceUrl(ListItemData.extended_entities.media[0].video_info.variants[0].url.toString())
                player.prepare()
            }
        }

        preferredWidth: Infinity
        background: (itemRoot.ListItem.selected ) ? ui.palette.primary : SystemDefaults.Paints.ContainerBackground

        RtContainer {
            visible: ListItemData.rt_flag
        }

        Container {

            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }

            topPadding: 15.0
            leftPadding: 15.0
            rightPadding: 15.0
            bottomPadding: 15.0
            ProfilePic {
            }

            Container {
                layout: StackLayout {
                    orientation: LayoutOrientation.TopToBottom
                }

                verticalAlignment: VerticalAlignment.Fill
                leftMargin: 15.0

                TweetHeader {

                }

                Label {
                    text: ListItemData.full_text
                    multiline: true
                    textFormat: TextFormat.Html
                    textStyle.fontFamily: 'Slate Pro'
                    topMargin: 10
                    content.flags: TextContentFlag.ActiveText | TextContentFlag.Emoticons
                    activeTextHandler: TweetTextHandler {
                    }
                }

                Container {
                    gestureHandlers: TapHandler {
                        onTapped: {
                            if (player.speed == 0)
                                player.play();
                            else
                                player.pause()
                            tapHandled = true
                        }
                    }
                    ForeignWindowControl {
                        id: fwcVideoSurface
                        windowId: "myWinId" + itemRoot.ListItem.indexPath
                        visible: boundToWindow
                        updatedProperties: WindowProperty.Position | WindowProperty.Visible | WindowProperty.Size
                        horizontalAlignment: HorizontalAlignment.Fill
                        verticalAlignment: VerticalAlignment.Fill
                        preferredHeight: player.videoDimensions.width != 0 ? player.videoDimensions.height * handler.layoutFrame.width / player.videoDimensions.width : 300

                        attachedObjects: [
                            // layout handler to track control dimensions
                            LayoutUpdateHandler {
                                id: handler
                            }
                        ]
                    }

                    topPadding: 5
                    leftPadding: 5
                    rightPadding: 5
                    bottomPadding: 5

                }
                QuotedTweetComponent {
                    visible: delegateActive
                    delegateActive: ListItemData.is_quote_status
                }
                TweetActionBar {
                }

            }
            attachedObjects: MediaPlayer {
                id: player

                //sourceUrl: ListItemData.extended_entities.media[0].video_info.variants[0].url.toString()

                onMediaStateChanged: {
                    //console.debug("media state: " + mediaState)
                    if (mediaState == MediaState.Prepared)
                        play()
                }

                //            onError: {
                //                setSourceUrl(ListItemData.extended_entities.media[0].video_info.variants[0].url.toString())
                //            }

                //            onSourceUrlChanged: {
                //                console.debug("source url: "+sourceUrl)
                //                play()
                //            }

                //            onWindowIdChanged: {
                //                  setSourceUrl("")
                //                  setSourceUrl(ListItemData.extended_entities.media[0].video_info.variants[0].url.toString())
                //                play()
                //            }

                // Set these properties for video
                videoOutput: VideoOutput.PrimaryDisplay
                windowId: fwcVideoSurface.windowId
                repeatMode: RepeatMode.Track

            }
            onCreationCompleted: {
                player.prepare()
            }
        }

        Divider {
        }
    }
}