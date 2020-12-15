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
    id: lic
    type: "main_poll"
    
    BaseTweetContainer {
        gestureHandlers: []    // we don't want to open another conversation identical to the current one when the user taps the main tweet
        
        id: itemRoot

        property variant i: ListItem.initialized
        property string previousTweetId
        property bool previousUserHasVoted

        onCreationCompleted: {
            itemRoot.ListItem.view.dataModel.cardUpdated.connect(updatePollContainer)    // emitted when the server acknowledges the vote and returns the (temoprary) results
        }

        onIChanged: {
            // Filter out all updates to the ListItemComponent not having to do with the poll:
            // Only update if the displayed tweet has changed (due to ListView's component recycling) or once the user has voted
            if (ListItemData.id_str != previousTweetId || userHasVoted() != previousUserHasVoted){
                updatePollContainer()
                
                /*
                 * Conversation tl needs to be scrolled to the right position after all poll containers appear, 
                 * since their height misalignes the timeline vertically if other tweets are present on top of the main one
                 */
                itemRoot.ListItem.view.scrollToItem([ itemRoot.ListItem.view.index ], ScrollAnimation.None)
            }
        }

        function userHasVoted() {
            return typeof (ListItemData.card.binding_values.selected_choice) != 'undefined'
        }
        
        function pollExpired() {
            return typeof (ListItemData.card.binding_values.counts_are_final.boolean_value) != 'undefined' && ListItemData.card.binding_values.counts_are_final.boolean_value == true
        }

        function updatePollContainer() {
            if (pollExpired() || userHasVoted()) {
                //Either the poll has expired or the user has already voted, hide the buttons and show the result
                pollButtons.visible = false
                results.visible = true

                // Show as many cointainers as there are options in the poll
                for (var i = 1; i <= 4; i ++) {
                    if (i <= ListItemData.poll_choice_count) {
                        results.controls[i - 1].text = ListItemData.card.binding_values['choice' + i + '_label'].string_value
                        results.controls[i - 1].barPercentage = parseInt(ListItemData.card.binding_values['choice' + i + '_count'].string_value) / ListItemData.poll_vote_count
                        results.controls[i - 1].isWinner = (ListItemData.poll_winner == i)
                        results.controls[i - 1].visible = true
                        if (userHasVoted() && parseInt(ListItemData.card.binding_values.selected_choice.string_value) == i)
                            results.controls[i - 1].isVotedByUser = true
                        else
                            results.controls[i - 1].isVotedByUser = false
                    } else {
                        results.controls[i - 1].visible = false
                    }
                }
            } else {
                for (var i = 1; i <= 4; i ++) {
                    if (i <= ListItemData.poll_choice_count) {
                        pollButtons.controls[i - 1].enabled = true
                        pollButtons.controls[i - 1].visible = true
                        pollButtons.controls[i - 1].text = ListItemData.card.binding_values['choice' + i + '_label'].string_value
                    } else {
                        pollButtons.controls[i - 1].visible = false
                    }
                }

                pollButtons.visible = true
                results.visible = false
            }
            previousTweetId = ListItemData.id_str
            previousUserHasVoted = userHasVoted()
        }

        function disablePollButtons() {
            button1.enabled = false
            button2.enabled = false
            button3.enabled = false
            button4.enabled = false
        }

        layout: StackLayout {
            orientation: LayoutOrientation.TopToBottom
        }
        attachedObjects: LayoutUpdateHandler {
            property int width: layoutFrame.width
            id: resultsContainerSize
        }

        topPadding: 10.0
        leftPadding: 20
        rightPadding: 20.0
        bottomPadding: 15
        
        TweetAuthorMain {
        }
        
        Label {
            text: ListItemData.full_text
            multiline: true
            textFormat: TextFormat.Html
            textStyle.fontFamily: 'Slate Pro'
            //topMargin: 10
            horizontalAlignment: HorizontalAlignment.Fill
            textStyle.fontSize: FontSize.PointValue
            textStyle.fontSizeValue: 7.5
            content.flags: TextContentFlag.ActiveText | TextContentFlag.Emoticons
            textStyle.base: itemRoot.ListItem.view.twemojiStyle
            activeTextHandler: TweetTextHandler {
            }
        }
        Container {
            id: pollContainer
            horizontalAlignment: HorizontalAlignment.Fill
            minHeight: ui.du(20)
            Container {
                id: pollButtons
                horizontalAlignment: HorizontalAlignment.Fill
                bottomPadding: ui.du(1)
                
                PollButtonEntry {
                    id: button1
                    index: 1
                }
                PollButtonEntry {
                    id: button2
                    index: 2
                }
                PollButtonEntry {
                    id: button3
                    index: 3
                }
                PollButtonEntry {
                    id: button4
                    index: 4
                }
            }
            Container {
                id: results
                horizontalAlignment: HorizontalAlignment.Fill
                bottomMargin: 0
                bottomPadding: 0
                //                        bottomPadding: ui.du(1)
                PollResultEntry {
                    id: result1
                }
                PollResultEntry {
                    id: result2
                }
                PollResultEntry {
                    id: result3
                }
                PollResultEntry {
                    id: result4
                }
            }
            Container {
                topMargin: 0
                topPadding: 0
                bottomPadding: ui.du(1)
                Label {
                    visible: true
                    text: ListItemData.poll_vote_count == 1 ? "1 vote" : ListItemData.poll_vote_count + " votes"
                    textStyle.color: Color.create("#7A7A7A")
                    bottomMargin: ui.du(10)
                    topMargin: 0
                }
            }
        }

        CardComponent {
        }
        Container {
            visible: ! ListItemData.is_quote_status
            maxHeight: 10
            minHeight: 10
        }
        
        QuotedTweetComponent {
            visible: delegateActive
            delegateActive: ListItemData.is_quote_status
        }
        
        TweetActionBarMain{
        }
    }
}