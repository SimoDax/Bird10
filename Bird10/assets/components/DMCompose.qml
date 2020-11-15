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
import bb.system 1.2

Container {
    
    id: dmComposeContainer
    property alias imageSource: imagePreview.imageSoureFile
    property alias inputText: dmInputField.text
    property alias dmSendButton: dmSendButton
    property string selectedImagePath
    property variant pageHandle
    signal scrollDMListToEnd()

    function showPreviewImage(filePath) {
        imagePreview.imageSoureFile = filePath;
        _directMessagesController.setImageView(imagePreview, filePath);
    }

    function requestFocus() {
        dmInputField.requestFocus();
    }

    function isLengthValid() {
        var tweetLength = inputText.trim().length
        return tweetLength < 10000
    }

    function freeze() {
        dmSendButton.enabled = false
        dmInputField.enabled = false
        clearImage.enabled = false
        photoLibraryButton.enabled = false
        emojiBtn.enabled = false
    }

    function unfreeze() {
        dmSendButton.enabled = true
        dmInputField.enabled = true
        clearImage.enabled = true
        photoLibraryButton.enabled = true
        emojiBtn.enabled = true
    }

    Container {
        minHeight: (128)
        layout: StackLayout {
            orientation: LayoutOrientation.LeftToRight
        }
        
        leftPadding: (10)
        rightPadding: (10)
        ImageButton {
            id: photoLibraryButton
            objectName: "Add Photo Button"
            verticalAlignment: VerticalAlignment.Center
            horizontalAlignment: HorizontalAlignment.Center
            focusPolicy: FocusPolicy.KeyAndTouch
            defaultImageSource: (imageSource != "") ? "asset:///images/btn_pressed_photo_gallery.png" : "asset:///images/btn_default_photo_gallery.png"
            pressedImageSource: "asset:///images/btn_pressed_photo_gallery.png"
            disabledImageSource: "asset:///images/btn_default_photo_gallery.png"
            onClicked: {
                filePicker.open()
            }
            
            // This is a work around for a cascades default focus issue.
            // If this ImageButton is focusable, then the default focus is always set to this button no matter that locallyFocused is set at dmInputField.
            // This ImageButton will be set to focusable at dmImputField.
            onCreationCompleted: {
                navigation.focusPolicy = NavigationFocusPolicy.NotFocusable
            }
        }
        ImageButton {
            id: emojiBtn
            defaultImageSource: "asset:///images/btn_emoji_square.png"
            disabledImageSource: "asset:///images/btn_emoji_square.png"
            verticalAlignment: VerticalAlignment.Center
            horizontalAlignment: HorizontalAlignment.Center
            onClicked: {
                if(!emoji.visible){
                    emoji.visible = true
                    emoji.enableEmojiSelections()
                }
                else{
                    emoji.visible = false    
                    emoji.resetEmojiSelections()
                }
            }
        }
        
        Container {
            verticalAlignment: VerticalAlignment.Center
            layoutProperties: StackLayoutProperties {
                spaceQuota: 3
            }
            
            layout: DockLayout {
            }
            
            ImageView {
                id: slicedBg2
                imageSource: "asset:///images/rounded9slice.amd"
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill
            }
            Container {
                id: inputFieldContainer
                layout: StackLayout {
                    orientation: LayoutOrientation.TopToBottom
                }
                leftPadding: (10)
                topPadding: (10)
                rightPadding: (10)
                bottomPadding: (10)
                Container {
                    id: imageContainer
                    layout: DockLayout {
                    }
                    visible: imagePreview.imageSoureFile != ""
                    
                    onVisibleChanged: {
                        //                    dmInputField.updateTextCounter()
                        //                    dmInputField.checkSendButtonVisual()
                    }
                    
                    ImageView {
                        id: imagePreview
                        property string imageSoureFile: ""
                        objectName: "imagePreview"
                        preferredWidth: (200)
                        preferredHeight: (200)
                        minHeight: (200)
                        minWidth: (200)
                        scalingMethod: ScalingMethod.AspectFill
                        horizontalAlignment: HorizontalAlignment.Left
                        imageSource: imageSoureFile
                    }
                    ImageButton {
                        id: clearImage
                        objectName: "Remove Attached Photo"
                        defaultImageSource: "asset:///images/search_cancel.png" //x button
                        onClicked: {
                            imagePreview.imageSoureFile = ""
                            checkSendButtonVisual();
                        }
                        horizontalAlignment: HorizontalAlignment.Right
                        verticalAlignment: VerticalAlignment.Top
                    }
                }
                Container {
                    id: dmInputFieldContainer
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    TextArea {
                        id: dmInputField
                        objectName: "DM text field"
                        verticalAlignment: VerticalAlignment.Center
                        hintText: ("Send a direct message")
                        content.flags: TextContentFlag.Emoticons
                        backgroundVisible: false
                        locallyFocused: true
                        onTextChanging: {
                        
                        }
                        input.submitKeyFocusBehavior: SubmitKeyFocusBehavior.Next
                        onFocusedChanged: {
                            if (focused) {
                                dmComposeContainer.scrollDMListToEnd()
                            }
                        }
                        
                        
                        // This is a work around for a cascades default focus issue.
                        // Please refer to the comments at photoLibraryButton.
                        // Set photoLibraryButton focusable.
                        eventHandlers: [
                            NavigationHandler {
                                onNavigation: {
                                    //                                photoLibraryButton.navigation.focusPolicy = NavigationFocusPolicy.Focusable;
                                }
                            }
                        ]
                        function updateTextField() {
                            if (Application.themeSupport.theme.colorTheme.style == VisualStyle.Bright) {
                                dmInputFieldContainer.background = null
                                inputFieldContainer.background = null
                            } else {
                                dmInputFieldContainer.background = Color.create("#262626")
                                inputFieldContainer.background = Color.create("#262626")
                            }
                        }
                        onCreationCompleted: updateTextField()
                    }
                }
            }
        }
        
        Container {
            id: sendContainer
            leftPadding: 5
            rightPadding: 5
            verticalAlignment: VerticalAlignment.Center
            layoutProperties: StackLayoutProperties {
                spaceQuota: 1
            }
            Button {
                id: dmSendButton
                objectName: "Message Send Button"
                text: qsTr("Send") + Retranslate.onLocaleOrLanguageChanged
                onClicked: {
                    pageHandle.sendButtonClicked();
                dmInputField.locallyFocused = true;
            }
        }
    }

    function showPhotoSelectOptions(path) {
        selectedImagePath = path
        photoSelectDialog.show();
    }
    
    attachedObjects: [
        FilePicker {
            id: filePicker
            type: FileType.Picture
            title: "Select Picture"
            //directories: [ "/accounts/1000/shared/camera", "/accounts/1000/shared/photos" ]
            onFileSelected: {
                imageSource = "file://"+selectedFiles[0];
                selectedImagePath = selectedFiles[0];
            }
        }
    ]
}

DMEmojiPicker {
    id: emoji
    visible: false
    messageField: dmInputField    // the picker needs access to the input field to add emojis as they're tapped
}

}
