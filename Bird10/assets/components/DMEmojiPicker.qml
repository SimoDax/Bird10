import bb.cascades 1.4

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
        emojiButtonContainer.visible = false;

        // Set all emoji categories hidden
        emojiPeople.visible = false;
        emojiPlaces.visible = false;
        emojiNature.visible = false;
        emojiObjects.visible = false;
        emojiSymbols.visible = false;

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
                ListItemComponent {
                    type: "emoji"
                    Label {
                        text: ListItemData.unicode
                        textStyle.textAlign: TextAlign.Center
                        textStyle.fontSize: FontSize.PointValue
                        textStyle.fontSizeValue: 8
                        textStyle.base: ListItem.view.twemojiStyle
                        textStyle.fontFamily: "Twemoji"
                        textFormat: TextFormat.Html
                    }
                }
            ]
            onTriggered: {
                var selectedItem = dataModel.data(indexPath);
                messageField.editor.insertPlainText(selectedItem.unicode);
                messageField.requestFocus();
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
                ListItemComponent {
                    type: "emoji"
                    Label {
                        text: ListItemData.unicode
                        textStyle.textAlign: TextAlign.Center
                        textStyle.fontSize: FontSize.PointValue
                        textStyle.fontSizeValue: 8
                        textStyle.base: ListItem.view.twemojiStyle
                        textStyle.fontFamily: "Twemoji"
                        textFormat: TextFormat.Html
                    }
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
                ListItemComponent {
                    type: "emoji"
                    Label {
                        text: ListItemData.unicode
                        textStyle.textAlign: TextAlign.Center
                        textStyle.fontSize: FontSize.PointValue
                        textStyle.fontSizeValue: 8
                        textStyle.base: ListItem.view.twemojiStyle
                        textStyle.fontFamily: "Twemoji"
                        textFormat: TextFormat.Html
                    }
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
                ListItemComponent {
                    type: "emoji"
                    Label {
                        text: ListItemData.unicode
                        textStyle.textAlign: TextAlign.Center
                        textStyle.fontSize: FontSize.PointValue
                        textStyle.fontSizeValue: 8
                        textStyle.base: ListItem.view.twemojiStyle
                        textStyle.fontFamily: "Twemoji"
                        textFormat: TextFormat.Html
                    }
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
                ListItemComponent {
                    type: "emoji"
                    Label {
                        text: ListItemData.unicode
                        textStyle.textAlign: TextAlign.Center
                        textStyle.fontSize: FontSize.PointValue
                        textStyle.fontSizeValue: 8
                        textStyle.base: ListItem.view.twemojiStyle
                        textStyle.fontFamily: "Twemoji"
                        textFormat: TextFormat.Html
                    }
                }
            ]
            onTriggered: {
                var selectedItem = dataModel.data(indexPath);
                messageField.editor.insertPlainText(selectedItem.unicode);
                messageField.requestFocus();
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
                    
                    messageField.requestFocus();
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