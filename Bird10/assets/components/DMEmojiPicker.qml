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
import "../"

Container{
    id: emojiPicker
    property variant messageField
    minHeight: ui.du(40)
    
    function enableEmojiSelections() {
        emojiPeopleButtonContainer.visible = true;
        emojiPlacesButtonContainer.visible = true;
        emojiNatureButtonContainer.visible = true;
        emojiObjectsButtonContainer.visible = true;
        emojiSymbolsButtonContainer.visible = true;
        emojiFlagsButtonContainer.visible = true;
        emojiFoodButtonContainer.visible = true;
        emojiButtonContainer.visible = true;
        // Set the people category as active
        emojiPeople.visible = true;

        messageField.requestFocus();
    }

    function resetEmojiSelections() {
        // Hide all the emoji buttons
        emojiPeopleButtonContainer.visible = false;
        emojiPlacesButtonContainer.visible = false;
        emojiNatureButtonContainer.visible = false;
        emojiObjectsButtonContainer.visible = false;
        emojiSymbolsButtonContainer.visible = false;
        emojiFlagsButtonContainer.visible = false;
        emojiFoodButtonContainer.visible = false;
        emojiButtonContainer.visible = false;

        // Set all emoji categories hidden
        emojiPeople.visible = false;
        emojiPlaces.visible = false;
        emojiNature.visible = false;
        emojiObjects.visible = false;
        emojiSymbols.visible = false;
        emojiFlags.visible = false;
        emojiFood.visible = false;

        messageField.requestFocus();
    }
    // List of Emoji in the People category
    Container {
        id: emojiPeople
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
                messageField.editor.insertPlainText(selectedItem.unicode);
                messageField.requestFocus();
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
                messageField.editor.insertPlainText(selectedItem.unicode);
                messageField.requestFocus();
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
                messageField.editor.insertPlainText(selectedItem.unicode);
                messageField.requestFocus();
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
                messageField.editor.insertPlainText(selectedItem.unicode);
                messageField.requestFocus();
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
                messageField.editor.insertPlainText(selectedItem.unicode);
                messageField.requestFocus();
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
        id: bottomTabs
        visible: true
        horizontalAlignment: HorizontalAlignment.Fill
        layout: StackLayout {
            orientation: LayoutOrientation.LeftToRight
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
                    emojiObjects.visible = false;
                    emojiSymbols.visible = false;
                    
                    messageField.requestFocus();
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
                    emojiObjects.visible = false;
                    emojiSymbols.visible = false;
                    emojiFlags.visible = false;
                    emojiFood.visible = false;
                    
                    messageField.requestFocus();
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
                    emojiObjects.visible = false;
                    emojiSymbols.visible = false;
                    emojiFlags.visible = false;
                    emojiFood.visible = false;

                    messageField.requestFocus();
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
                    emojiObjects.visible = true;
                    emojiSymbols.visible = false;
                    emojiFlags.visible = false;
                    emojiFood.visible = false;

                    messageField.requestFocus();
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
                    emojiObjects.visible = false;
                    emojiSymbols.visible = true;
                    emojiFlags.visible = false;
                    emojiFood.visible = false;

                    messageField.requestFocus();
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

                    messageField.requestFocus();
                }
            }
        }
        
        // Emoji selection button
        Container {
            id: emojiButtonContainer
            visible: false
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
                     resetEmojiSelections();
                     emojiPicker.visible = false
                }
            }
        }
    }
}