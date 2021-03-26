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
import bb.cascades.pickers 1.0
import org.labsquare 1.0
import com.simodax 1.0
import bb.multimedia 1.4
import "/components"

Sheet {
    id: sheet
    property string in_reply_to: ""
    property string in_reply_to_status_id: ""
    property string attachment_url: ""
    property string image1: ""
    property string image2: ""
    property string image3: ""
    property string image4: ""
    property string video: ""
    property alias text: tweet.text
    property variant referencedTweet
    property bool busy: false

    Page {
        id: composer
        
        function noImageSelected(){
            return sheet.image1 == "" && sheet.image2 == "" && sheet.image3 == "" && sheet.image4 == ""
        }
        onCreationCompleted: {
            tweet.requestFocus()
        }

        titleBar: TitleBar {
            title: in_reply_to_status_id == "" ? "Tweet" : "Reply"
            acceptAction: ActionItem {
                title: "Tweet"
                onTriggered: {
                    if (sheet.image1 != ""){
                        var a = new Array(sheet.image1);

                        if (sheet.image2 != "")
                            a.push(sheet.image2)
                        if (sheet.image3 != "")
                            a.push(sheet.image3)
                        if (sheet.image4 != "")
                            a.push(sheet.image4)
                            
                        tweetApi.imageTweet(tweet.text, a, in_reply_to_status_id, attachment_url);
                    }
                    else if (sheet.video != "")
                        tweetApi.videoTweet(tweet.text, sheet.video, in_reply_to_status_id, attachment_url);
                    else
                        tweetApi.tweet(tweet.text, in_reply_to_status_id, attachment_url)
                    busy = true
                    close()
                }

                enabled: (tweet.realLength <= 280 && tweet.realLength > 0) || !composer.noImageSelected() || sheet.video != ""
            }
            dismissAction: ActionItem {
                title: "Cancel"
                onTriggered: {
                    close()
                    tweetSheet.active = false
                }
            }
        }

        Container {
            layout: StackLayout {
            }
            verticalAlignment: VerticalAlignment.Fill

            Container {
                visible: in_reply_to != ""
                topPadding: 20
                bottomPadding: 15
                leftPadding: 15
                Label {
                    text: "@" + in_reply_to
                    textStyle.color: ui.palette.primary
                    textStyle.fontSize: FontSize.Medium
                    textStyle.fontWeight: FontWeight.W500
                    multiline: true
                }
            }

            TextArea {
                property int realLength
                property bool isMentioning: false
                property bool isTrending: false
                property string mentionHandle
                property string trend
                property int previousLength
                id: tweet
                topMargin: 10
                leftMargin: 10
                rightMargin: 10

                maximumLength: 500
                onTextChanging: {
                    //accounts for t.co url shortening
                    realLength = tweet.text.trim().length
                    var patt = /\S+[\.][^\s,]+/g
                    var res = tweet.text.match(patt)

                    if (Array.isArray(res)) {
                        for (var i = 0; i < res.length; i ++) realLength = realLength - res[i].length + 23 //TODO: check on twitter api for updated value
                        //console.debug(realLength)
                    }
                    
                    // check whether if the user is trying to @ someone
                    
                    var c = text.charAt(text.length - 1);
                    var cc = c.charCodeAt(0);

                    if(text.charAt(text.length-1) == "@" && !isTrending){
                        isMentioning = true;
                        mentionHandle="@";
                        composer.showProfileList()
                        searchApi.searchUser(mentionHandle)
                        previousLength = text.length
                    }
                    else if(isMentioning)
                    {
                        // handle is being removed with backspace
                        if(text.length < previousLength){
                            if(mentionHandle.length > 1){
                                mentionHandle = mentionHandle.substring(0, mentionHandle.length-1)
                                searchApi.searchUser(mentionHandle)
                            }
                            else{
                                isMentioning = false;
                                composer.hideProfileList()
                            }
                        }
                        else if((cc>47 && cc<58) || (cc>64 && cc<91) || (cc>96 && cc<123) || cc == 95){    // valid text is being added to the handle
                            mentionHandle = mentionHandle + c;
                            searchApi.searchUser(mentionHandle)
                        }
                        else{
                            isMentioning = false;
                            composer.hideProfileList()
                        }
                        previousLength = text.length
                    } 
                    else if (text.charAt(text.length - 1) == "#" && ! isMentioning) {
                        isTrending = true;
                        trend = "#";
                        composer.showTrendList()
                        searchApi.searchTopic(trend)
                        previousLength = text.length
                    }
                    else if(isTrending){
                        // trend is being removed with backspace
                        if (text.length < previousLength) {
                            if (trend.length > 1) {
                                trend = trend.substring(0, trend.length - 1)
                                searchApi.searchTopic(trend)
                            } else {
                                isTrending = false;
                                composer.hideTrendList()
                            }
                        } else if ((cc > 47 && cc < 58) || (cc > 64 && cc < 91) || (cc > 96 && cc < 123) || cc == 95) { // valid text is being added to the trend
                            trend = trend + c;
                            searchApi.searchTopic(trend)
                        } else {
                            isTrending = false;
                            composer.hideTrendList()
                        }
                        previousLength = text.length
                    }
                }
                preferredHeight: ui.du(25)
            }

            // Character count
            Container {
                id: count
                horizontalAlignment: HorizontalAlignment.Fill
                rightMargin: 10
                layout: DockLayout {
                }
                Label {
                    horizontalAlignment: HorizontalAlignment.Right
                    text: tweet.realLength + "/280"
                    textStyle.fontSizeValue: 5.0
                    textStyle.color: tweet.realLength <= 230 ? ui.palette.text : Color.Red
                }
            }

            // List of attached images
            Container {
                id: images
                topPadding: 0
                leftPadding: 10
                rightPadding: 10
                bottomPadding: 10
                topMargin: 0
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                AttachedImage {
                    id: image1
                    imageSource: "file://" + sheet.image1
                }
                AttachedImage {
                    id: image2
                    imageSource: "file://" + sheet.image2
                    leftMargin: 10
                }
                AttachedImage {
                    id: image3
                    imageSource: "file://" + sheet.image3
                    leftMargin: 10
                }
                AttachedImage {
                    id: image4
                    imageSource: "file://" + sheet.image4
                    leftMargin: 10
                }
                Label {
                    visible: sheet.video != ""
                    text: { 
                         var a = sheet.video.split("/")
                         return " Attached video: " + a[a.length-1]
                     }
                }
            }

            // Quoted or replied tweet
            Container {
                property bool shouldBeVisible: (referencedTweet) != null && attachment_url != ""
                visible: shouldBeVisible

                id: attachment

                preferredWidth: Infinity
                maxHeight: ui.du(20)

                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }

                topPadding: 15
                leftPadding: 15.0
                rightPadding: 15.0
                bottomPadding: 15.0
                WebImageView {
                    id: profilePic
                    url:  ((referencedTweet) != null ? referencedTweet.user.profile_image_url : "")
                    minWidth: 80.0
                    minHeight: 80.0
                }

                Container {
                    layout: StackLayout {
                        orientation: LayoutOrientation.TopToBottom
                    }

                    verticalAlignment: VerticalAlignment.Fill
                    leftMargin: 15.0

                    Container {
                        layout: StackLayout {
                            orientation: LayoutOrientation.LeftToRight
                        }

                        Label {
                            id: username
                            text: ((referencedTweet) != null ? referencedTweet.user.name: "")
                            textStyle.fontWeight: FontWeight.Bold
                            textStyle.fontSizeValue: 7.0
                            rightMargin: 5.0
                        }
                        Label {
                            id: handle
                            text: "@" + ((referencedTweet) != null ? referencedTweet.user.screen_name : "")
                            textStyle.fontSizeValue: 7.0
                            textStyle.color: Color.create("#969696")
                            leftMargin: 0.0
                        }
                    }

                    Label {
                        text: ((referencedTweet) != null ? referencedTweet.full_text : "")
                        multiline: true
                        textFormat: TextFormat.Html
                        topMargin: 10

                    }
                }
            }

            // List of Emoji in the People category
            Container {
                id: emojiPeople
                //background: composer.invokedAsCard == true ? Color.create("#f0f0f0") : Qt.colorStyles.emojiBkgColor
                topPadding: 10
                visible: false
                ListView {
                    property variant twemojiStyle: twemoji.style
                    snapMode: SnapMode.LeadingEdge
                    layout: GridListLayout {
                        columnCount: 8
                        cellAspectRatio: 1
                        horizontalCellSpacing: 0.0
                        verticalCellSpacing: 0.0
                        orientation: LayoutOrientation.TopToBottom
                    }
                    dataModel: XmlDataModel {
                        source: "asset:///images/emoji_people.xml"
                    }
                    listItemComponents: [
                        EmojiComponent {
                        }
                    ]
                    onTriggered: {
                        var selectedItem = dataModel.data(indexPath);
                        tweet.editor.insertPlainText(selectedItem.unicode);
                        tweet.requestFocus();
                    }
                }
            }
            
            // List of Emoji in the Nature category
            Container {
                id: emojiNature
                //background: composer.invokedAsCard == true ? Color.create("#f0f0f0") : Qt.colorStyles.emojiBkgColor
                topPadding: 10
                visible: false
                ListView {
                    property variant twemojiStyle: twemoji.style
                    snapMode: SnapMode.LeadingEdge
                    layout: GridListLayout {
                        columnCount: 8
                        cellAspectRatio: 1
                        horizontalCellSpacing: 0.0
                        verticalCellSpacing: 0.0
                        orientation: LayoutOrientation.TopToBottom
                    }
                    dataModel: XmlDataModel {
                        source: "asset:///images/emoji_nature.xml"
                    }
                    listItemComponents: [
                        EmojiComponent {
                        }
                    ]
                    onTriggered: {
                        var selectedItem = dataModel.data(indexPath);
                        tweet.editor.insertPlainText(selectedItem.unicode);
                        tweet.requestFocus();
                    }
                }
            }

            // List of Emoji in the Food category
            Container {
                id: emojiFood
                //background: composer.invokedAsCard == true ? Color.create("#f0f0f0") : Qt.colorStyles.emojiBkgColor
                topPadding: 10
                visible: false
                ListView {
                    property variant twemojiStyle: twemoji.style
                    snapMode: SnapMode.LeadingEdge
                    layout: GridListLayout {
                        columnCount: 8
                        cellAspectRatio: 1
                        horizontalCellSpacing: 0.0
                        verticalCellSpacing: 0.0
                        orientation: LayoutOrientation.TopToBottom
                    }
                    dataModel: XmlDataModel {
                        source: "asset:///images/emoji_foods.xml"
                    }
                    listItemComponents: [
                        EmojiComponent {
                        }
                    ]
                    onTriggered: {
                        var selectedItem = dataModel.data(indexPath);
                        tweet.editor.insertPlainText(selectedItem.unicode);
                        tweet.requestFocus();
                    }
                }
            }

            // List of Emoji in the Places category
            Container {
                id: emojiPlaces
                //background: composer.invokedAsCard == true ? Color.create("#f0f0f0") : Qt.colorStyles.emojiBkgColor
                topPadding: 10
                visible: false
                ListView {
                    property variant twemojiStyle: twemoji.style
                    snapMode: SnapMode.LeadingEdge
                    layout: GridListLayout {
                        columnCount: 8
                        cellAspectRatio: 1
                        horizontalCellSpacing: 0.0
                        verticalCellSpacing: 0.0
                        orientation: LayoutOrientation.TopToBottom
                    }
                    dataModel: XmlDataModel {
                        source: "asset:///images/emoji_places.xml"
                    }
                    listItemComponents: [
                        EmojiComponent {
                        }
                    ]
                    onTriggered: {
                        var selectedItem = dataModel.data(indexPath);
                        tweet.editor.insertPlainText(selectedItem.unicode);
                        tweet.requestFocus();
                    }
                }
            }

            // List of Emoji in the Objects category
            Container {
                id: emojiObjects
                //background: composer.invokedAsCard == true ? Color.create("#f0f0f0") : Qt.colorStyles.emojiBkgColor
                topPadding: 10
                visible: false
                ListView {
                    property variant twemojiStyle: twemoji.style
                    snapMode: SnapMode.LeadingEdge
                    layout: GridListLayout {
                        columnCount: 8
                        cellAspectRatio: 1
                        horizontalCellSpacing: 0.0
                        verticalCellSpacing: 0.0
                        orientation: LayoutOrientation.TopToBottom
                    }
                    dataModel: XmlDataModel {
                        source: "asset:///images/emoji_objects.xml"
                    }
                    listItemComponents: [
                        EmojiComponent {
                        }
                    ]
                    onTriggered: {
                        var selectedItem = dataModel.data(indexPath);
                        tweet.editor.insertPlainText(selectedItem.unicode);
                        tweet.requestFocus();
                    }
                }
            }

            // List of Emoji in the Symbols category
            Container {
                id: emojiSymbols
                //background: composer.invokedAsCard == true ? Color.create("#f0f0f0") : Qt.colorStyles.emojiBkgColor
                topPadding: 10
                visible: false
                ListView {
                    property variant twemojiStyle: twemoji.style
                    snapMode: SnapMode.LeadingEdge
                    layout: GridListLayout {
                        columnCount: 8
                        cellAspectRatio: 1
                        horizontalCellSpacing: 0.0
                        verticalCellSpacing: 0.0
                        orientation: LayoutOrientation.TopToBottom
                    }
                    dataModel: XmlDataModel {
                        source: "asset:///images/emoji_symbols.xml"
                    }
                    listItemComponents: [
                        EmojiComponent {
                        }
                    ]
                    onTriggered: {
                        var selectedItem = dataModel.data(indexPath);
                        tweet.editor.insertPlainText(selectedItem.unicode);
                        tweet.requestFocus();
                    }
                }
            }

            // List of Emoji in the Flags category
            Container {
                id: emojiFlags
                //background: composer.invokedAsCard == true ? Color.create("#f0f0f0") : Qt.colorStyles.emojiBkgColor
                topPadding: 10
                visible: false
                ListView {
                    property variant twemojiStyle: twemoji.style
                    snapMode: SnapMode.LeadingEdge
                    layout: GridListLayout {
                        columnCount: 8
                        cellAspectRatio: 1
                        horizontalCellSpacing: 0.0
                        verticalCellSpacing: 0.0
                        orientation: LayoutOrientation.TopToBottom
                    }
                    dataModel: XmlDataModel {
                        source: "asset:///images/emoji_flags.xml"
                    }
                    listItemComponents: [
                        EmojiComponent {
                        }
                    ]
                    onTriggered: {
                        var selectedItem = dataModel.data(indexPath);
                        tweet.editor.insertPlainText(selectedItem.unicode);
                        tweet.requestFocus();
                    }
                }
            }

            //Button container
            Container {
                property bool emojiSelectionActive: false
                id: contacts
                visible: true
                horizontalAlignment: HorizontalAlignment.Fill
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }

                // Photo library button
                Container {
                    id: photoLibraryButtonContainer
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 0.25
                    }
                    layout: DockLayout {
                    }
                    ImageButton {
                        id: photoLibraryButton
                        //enabled: CommonUtilities.checkFileSystemAccessible()
                        //opacity: CommonUtilities.checkFileSystemAccessible() ? 1 : 0.5
                        objectName: "Pictures Button"
                        enabled: sheet.image4 == "" && sheet.video == ""
                        //preferredWidth: SizeSelector.convert(120)
                        //maxWidth: SizeSelector.convert(120)
                        //preferredHeight: SizeSelector.convert(86)
                        verticalAlignment: VerticalAlignment.Center
                        horizontalAlignment: HorizontalAlignment.Center
                        focusPolicy: FocusPolicy.KeyAndTouch
                        defaultImageSource: "asset:///btn_default_gallery.png"
                        disabledImageSource: "asset:///btn_default_gallery.png"
                        onClicked: {
                            filePicker.type = FileType.Picture
                            filePicker.open()
                            /*
                             * if (_tweetComposeController.filePickerImageSet) {
                             * tweetAction.enabled = (tweet.text.trim() != "");
                             * _tweetComposeController.triggerPhotoPicker()
                             * composer.disableCancel();
                             * composer.changesDone = true;
                             * } else {
                             * _tweetComposeController.triggerPhotoPicker()
                             * composer.disableCancel();
                             * }
                             */
                        }

                    }
                }

                // Emoji people button - only shown when emoji selection is active
                Container {
                    id: emojiPeopleButtonContainer
                    visible: false
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 0.25
                    }
                    layout: DockLayout {
                    }
                    ImageButton {
                        id: emojiPeopleButton
                        objectName: "Emoji People button"
                        //preferredWidth: SizeSelector.convert(120)
                        //maxWidth: SizeSelector.convert(120)
                        //preferredHeight: SizeSelector.convert(86)
                        verticalAlignment: VerticalAlignment.Center
                        horizontalAlignment: HorizontalAlignment.Center
                        defaultImageSource: "asset:///images/btn_emoji_people.png"
                        onClicked: {
                            emojiPeople.visible = true;
                            emojiPlaces.visible = false;
                            emojiNature.visible = false;
                            emojiFood.visible = false;
                            emojiObjects.visible = false;
                            emojiSymbols.visible = false;
                            emojiFlags.visible = false;

                            tweet.requestFocus();
                        }
                    }
                }
                // Camera button
                Container {
                    id: cameraButtonContainer
                    visible: true
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 0.25
                    }
                    layout: DockLayout {
                    }
                    ImageButton {
                        id: cameraButton
                        objectName: "Camera Button"
                        visible: true
                        enabled: composer.noImageSelected()
                        onEnabledChanged: {
                            console.debug('enabled: ' +composer.noImageSelected())
                        }
                        //opacity: CommonUtilities.enableCameraButton() ? 1 : 0.5
                        //preferredWidth: SizeSelector.convert(120)
                        //maxWidth: SizeSelector.convert(120)
                        //preferredHeight: SizeSelector.convert(86)
                        verticalAlignment: VerticalAlignment.Center
                        horizontalAlignment: HorizontalAlignment.Center
                        defaultImageSource: "asset:///btn_video.png"
                        disabledImageSource: "asset:///btn_video.png"
                        focusPolicy: FocusPolicy.KeyAndTouch
                        onClicked: {
                            filePicker.type = FileType.Video
                            filePicker.open()
                            /*
                             * if (! CommonUtilities.isCameraAllowed()) {
                             * _tweetComposeController.showCameraPermissionDialog();
                             * } else if (_tweetComposeController.cameraImageSet) {
                             * tweetAction.enabled = (tweet.text.trim() != "");
                             * _tweetComposeController.triggerCameraButton();
                             * composer.disableCancel();
                             * } else {
                             * _tweetComposeController.triggerCameraButton();
                             * composer.disableCancel();
                             * }
                             */
                        }
                    }
                }
                // Emoji places button - only shown when emoji selection is active
                Container {
                    id: emojiPlacesButtonContainer
                    visible: false
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 0.25
                    }
                    layout: DockLayout {
                    }
                    ImageButton {
                        id: emojiPlacesButton
                        objectName: "Emoji Places button"
                        //preferredWidth: SizeSelector.convert(120)
                        //maxWidth: SizeSelector.convert(120)
                        //preferredHeight: SizeSelector.convert(86)
                        verticalAlignment: VerticalAlignment.Center
                        horizontalAlignment: HorizontalAlignment.Center
                        defaultImageSource: "asset:///images/btn_emoji_places.png"
                        onClicked: {
                            emojiPeople.visible = false;
                            emojiPlaces.visible = true;
                            emojiNature.visible = false;
                            emojiFood.visible = false;
                            emojiObjects.visible = false;
                            emojiSymbols.visible = false;
                            emojiFlags.visible = false;

                            tweet.requestFocus();
                        }
                    }
                }

                // Emoji Nature button - only shown when emoji selection is active
                Container {
                    id: emojiNatureButtonContainer
                    visible: false
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 0.25
                    }
                    layout: DockLayout {
                    }
                    ImageButton {
                        id: emojiNatureButton
                        objectName: "Emoji Nature button"
                        //preferredWidth: SizeSelector.convert(120)
                        //maxWidth: SizeSelector.convert(120)
                        //preferredHeight: SizeSelector.convert(86)
                        verticalAlignment: VerticalAlignment.Center
                        horizontalAlignment: HorizontalAlignment.Center
                        defaultImageSource: "asset:///images/btn_emoji_nature.png"
                        onClicked: {
                            emojiPeople.visible = false;
                            emojiPlaces.visible = false;
                            emojiNature.visible = true;
                            emojiFood.visible = false;
                            emojiObjects.visible = false;
                            emojiSymbols.visible = false;
                            emojiFlags.visible = false;

                            tweet.requestFocus();
                        }
                    }
                }

                // Emoji Food button - only shown when emoji selection is active
                Container {
                    id: emojiFoodButtonContainer
                    visible: false
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 0.25
                    }
                    layout: DockLayout {
                    }
                    ImageButton {
                        id: emojiFoodButton
                        objectName: "Emoji Nature button"
                        //preferredWidth: SizeSelector.convert(120)
                        //maxWidth: SizeSelector.convert(120)
                        //preferredHeight: SizeSelector.convert(86)
                        verticalAlignment: VerticalAlignment.Center
                        horizontalAlignment: HorizontalAlignment.Center
                        defaultImageSource: "asset:///images/btn_emoji_food.png"
                        onClicked: {
                            emojiPeople.visible = false;
                            emojiPlaces.visible = false;
                            emojiNature.visible = false;
                            emojiFood.visible = true;
                            emojiObjects.visible = false;
                            emojiSymbols.visible = false;
                            emojiFlags.visible = false;

                            tweet.requestFocus();
                        }
                    }
                }

                // Emoji Objects button - only shown when emoji selection is active
                Container {
                    id: emojiObjectsButtonContainer
                    visible: false
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 0.25
                    }
                    layout: DockLayout {
                    }
                    ImageButton {
                        id: emojiObjectsButton
                        objectName: "Emoji Objects button"
                        //preferredWidth: SizeSelector.convert(120)
                        //maxWidth: SizeSelector.convert(120)
                        //preferredHeight: SizeSelector.convert(86)
                        verticalAlignment: VerticalAlignment.Center
                        horizontalAlignment: HorizontalAlignment.Center
                        defaultImageSource: "asset:///images/btn_emoji_objects.png"
                        onClicked: {
                            emojiPeople.visible = false;
                            emojiPlaces.visible = false;
                            emojiNature.visible = false;
                            emojiFood.visible = false;
                            emojiObjects.visible = true;
                            emojiSymbols.visible = false;
                            emojiFlags.visible = false;

                            tweet.requestFocus();
                        }
                    }
                }

                // Emoji Symbols button - only shown when emoji selection is active
                Container {
                    id: emojiSymbolsButtonContainer
                    visible: false
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 0.25
                    }
                    layout: DockLayout {
                    }
                    ImageButton {
                        id: emojiSymbolsButton
                        objectName: "Emoji Symbols button"
                        //preferredWidth: SizeSelector.convert(120)
                        //maxWidth: SizeSelector.convert(120)
                        //preferredHeight: SizeSelector.convert(86)
                        verticalAlignment: VerticalAlignment.Center
                        horizontalAlignment: HorizontalAlignment.Center
                        defaultImageSource: "asset:///images/btn_emoji_symbols.png"
                        onClicked: {
                            emojiPeople.visible = false;
                            emojiPlaces.visible = false;
                            emojiNature.visible = false;
                            emojiFood.visible = false;
                            emojiObjects.visible = false;
                            emojiSymbols.visible = true;
                            emojiFlags.visible = false;

                            tweet.requestFocus();
                        }
                    }
                }

                // Emoji Flags button - only shown when emoji selection is active
                Container {
                    id: emojiFlagsButtonContainer
                    visible: false
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 0.25
                    }
                    layout: DockLayout {
                    }
                    ImageButton {
                        id: emojiFlagsButton
                        objectName: "Emoji Flags button"
                        //preferredWidth: SizeSelector.convert(120)
                        //maxWidth: SizeSelector.convert(120)
                        //preferredHeight: SizeSelector.convert(86)
                        verticalAlignment: VerticalAlignment.Center
                        horizontalAlignment: HorizontalAlignment.Center
                        defaultImageSource: "asset:///images/btn_emoji_flags.png"
                        onClicked: {
                            emojiPeople.visible = false;
                            emojiPlaces.visible = false;
                            emojiNature.visible = false;
                            emojiFood.visible = false;
                            emojiObjects.visible = false;
                            emojiSymbols.visible = false;
                            emojiFlags.visible = true;

                            tweet.requestFocus();
                        }
                    }
                }
                
                // Emoji selection button
                Container {
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 0.25
                    }
                    layout: DockLayout {
                    }
                    ImageButton {
                        id: emojiButton
                        objectName: "Emoji Button"
                        //preferredWidth: SizeSelector.convert(120)
                        //maxWidth: SizeSelector.convert(120)
                        //preferredHeight: SizeSelector.convert(86)
                        verticalAlignment: VerticalAlignment.Center
                        horizontalAlignment: HorizontalAlignment.Center
                        defaultImageSource: emojiPeopleButtonContainer.visible ? "asset:///btn_compose_tweet.png" : "asset:///btn_emoji.png"
                        onClicked: {
                            contacts.emojiSelectionActive = ! contacts.emojiSelectionActive
                            if (contacts.emojiSelectionActive) {
                                composer.enableEmojiSelections();
                            } else {
                                composer.resetEmojiSelections();
                            }
                        }
                    }
                }
            } // End of Button Container
            ProfilesList{
                id: profiles
                visible: false
                dataModel: searchApi.users
                
                onTriggered: {
                    var selectedItem = dataModel.data(indexPath)
                    tweet.editor.insertPlainText(selectedItem.screen_name.toString().substring(tweet.mentionHandle.length-1))
                    composer.hideProfileList()
                    tweet.isMentioning = false
                }
            }
            ListView {
                id: trends
                visible: false
                dataModel: searchApi.trends

                onTriggered: {
                    var selectedItem = dataModel.data(indexPath)
                    tweet.editor.setSelection(tweet.text.length - tweet.trend.length, tweet.text.length)
                    tweet.editor.insertPlainText(selectedItem.toString())
                    composer.hideTrendList()
                    tweet.isTrending = false
                }
            }
        }

        function enableEmojiSelections() {
            images.visible = false
            count.visible = false
            attachment.visible = false
            // Replace other buttons on this row with emoji category buttons. This
            // allows us to show another line of emojis in the selected categoy
            cameraButtonContainer.visible = false;
            photoLibraryButtonContainer.visible = false;
            emojiPeopleButtonContainer.visible = true;
            emojiPlacesButtonContainer.visible = true;
            emojiNatureButtonContainer.visible = true;
            emojiFoodButtonContainer.visible = true;
            emojiObjectsButtonContainer.visible = true;
            emojiSymbolsButtonContainer.visible = true;
            emojiFlagsButtonContainer.visible = true;
            // Set the people category as active
            emojiPeople.visible = true;

            tweet.requestFocus();
        }

        function resetEmojiSelections() {
            images.visible = true
            count.visible = true
            attachment.visible = attachment.shouldBeVisible
            // Hide all the emoji buttons
            emojiPeopleButtonContainer.visible = false;
            emojiPlacesButtonContainer.visible = false;
            emojiNatureButtonContainer.visible = false;
            emojiFoodButtonContainer.visible = false;
            emojiObjectsButtonContainer.visible = false;
            emojiSymbolsButtonContainer.visible = false;
            emojiFlagsButtonContainer.visible = false;
            // Reset button row to defaults
            cameraButtonContainer.visible = true;
            photoLibraryButtonContainer.visible = true;

            // Set all emoji categories hidden
            emojiPeople.visible = false;
            emojiPlaces.visible = false;
            emojiNature.visible = false;
            emojiFood.visible = false;
            emojiObjects.visible = false;
            emojiSymbols.visible = false;
            emojiFlags.visible = false;
            contacts.emojiSelectionActive = false;

            tweet.requestFocus();
        }
        
        
        function showProfileList(){
            images.visible = false
            count.visible = false
            attachment.visible = false

            cameraButtonContainer.visible = false;
            photoLibraryButtonContainer.visible = false;
            emojiPeopleButtonContainer.visible = false;
            emojiPlacesButtonContainer.visible = false;
            emojiNatureButtonContainer.visible = false;
            emojiObjectsButtonContainer.visible = false;
            emojiSymbolsButtonContainer.visible = false;
            emojiFlagsButtonContainer.visible = false;
            emojiFoodButtonContainer.visible = false;
            // Set all emoji categories hidden
            emojiPeople.visible = false;
            emojiPlaces.visible = false;
            emojiNature.visible = false;
            emojiObjects.visible = false;
            emojiSymbols.visible = false;
            emojiFlags.visible = false;
            emojiFood.visible = false;
            contacts.emojiSelectionActive = false;

            contacts.visible = false
            trends.visible = false
            profiles.visible = true
        }

        function showTrendList() {
            images.visible = false
            count.visible = false
            attachment.visible = false

            cameraButtonContainer.visible = false;
            photoLibraryButtonContainer.visible = false;
            emojiPeopleButtonContainer.visible = false;
            emojiPlacesButtonContainer.visible = false;
            emojiNatureButtonContainer.visible = false;
            emojiObjectsButtonContainer.visible = false;
            emojiSymbolsButtonContainer.visible = false;
            emojiFlagsButtonContainer.visible = false;
            emojiFoodButtonContainer.visible = false;
            // Set all emoji categories hidden
            emojiPeople.visible = false;
            emojiPlaces.visible = false;
            emojiNature.visible = false;
            emojiObjects.visible = false;
            emojiSymbols.visible = false;
            emojiFlags.visible = false;
            emojiFood.visible = false;
            contacts.emojiSelectionActive = false;

            contacts.visible = false
            profiles.visible = false
            trends.visible = true
        }

        function hideProfileList() {
            resetEmojiSelections()

            profiles.visible = false
            trends.visible = false
            contacts.visible = true
            
            searchApi.clearUsers()
        }

        function hideTrendList() {
            resetEmojiSelections()

            profiles.visible = false
            trends.visible = false
            contacts.visible = true

            searchApi.clearTrends()
        }
        
        attachedObjects: [
            FilePicker {
                id: filePicker
                type: FileType.Picture
                title: "Select Picture"
                //directories: [ "/accounts/1000/shared/camera", "/accounts/1000/shared/photos" ]
                onFileSelected: {
                    if(type == FileType.Picture){
                        if (sheet.image1 == "")
                            sheet.image1 = selectedFiles[0];
                        else if (sheet.image2 == "")
                            sheet.image2 = selectedFiles[0];
                        else if (sheet.image3 == "")
                            sheet.image3 = selectedFiles[0];
                        else if (sheet.image4 == "")
                            sheet.image4 = selectedFiles[0]
                    }
                    else{
//                        if (tweetApi.fileSize(selectedFiles[0]) > 15 * 1000 * 1000) {
//                            error_message("File size is too big!")
//                            return;
//                        }
                        sheet.video = selectedFiles[0]
                    }

                    //console.log("FileSelected signal received : " + selectedFiles)
                }
            },
            TweetApi {
                id: tweetApi
                authenticator: o1Legacy.linked ? o1Legacy : o1Twitter
                onTweeted: {
                    message("Tweet published")
                    if(twitterApi.objectName == "homeTimeline")
                        twitterApi.requestLatestTweets()
                    tweetSheet.active = false
                }
                onError: {
                    message(error)
                    busy = false
                }
                onNetworkError: {
                    message(error)
                    busy = false
                }
            },
            SearchApi {
                id: searchApi
                authenticator: o1Twitter
            },
            TextStyleDefinition {
                id: twemoji
                rules: FontFaceRule {
                    id: font1
                    source: "asset:///fonts/Twemoji.ttf"
                    fontFamily: "Twemoji"
                }
            }
        ]
    }
}