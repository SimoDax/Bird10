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

import bb.cascades 1.3

Sheet {
    id: sheet
    Page {
        titleBar: TitleBar {
            title: "Settings"
        }
        id: settings
        
        ScrollView {
            id:scroll
            Container {
                //-----------------------------------------------------------------------
                Divider {
                }
                Container {
                    layout: StackLayout {
                    }
                    leftPadding: ui.du(1.5)
                    rightPadding: ui.du(1.5)
                    topPadding: ui.du(1.5)

                    Label {
                        text: "Legacy Tweeting Method"
                        textStyle.fontSize: FontSize.Large
                    }
                    Container {
                        layout: StackLayout {
                            orientation: LayoutOrientation.LeftToRight
                        }
                        TextArea {
                            text: "If enabled, your tweets will appear as sent from 'Bird10 for BlackBerry'. You'll have to login into your account one extra time when enabling this"
                            editable: false

                        }
                        ToggleButton {
                            property bool linked: o1Legacy.linked
                            checked: linked
                            onCheckedChanged: {
                                if(checked && ! linked)
                                    o1Legacy.link()
                                else if (!checked && linked)
                                    o1Legacy.unlink()
                            }
                            onLinkedChanged: {
                                checked = linked
                            }
                            verticalAlignment: VerticalAlignment.Center
                        }
                    }
                }  

                //-----------------------------------------------------------------------
                Divider {
                }
                Container {
                    layout: StackLayout {
                    }
                    leftPadding: ui.du(1.5)
                    rightPadding: ui.du(1.5)
                    topPadding: ui.du(1.5)

                    Label {
                        text: "Background Service"
                        textStyle.fontSize: FontSize.Large
                    }
                    Container {
                        layout: StackLayout {
                            orientation: LayoutOrientation.LeftToRight
                        }
                        TextArea {
                            text: "You can start/restart and stop the background service checking for notification and messages. Note that the 'Sync Updates' setting has no effect on the service"
                            editable: false

                        }
                        Container {
                            verticalAlignment: VerticalAlignment.Center
                            layout: StackLayout {
                                orientation: LayoutOrientation.TopToBottom
                            }
                            Button {
                                text: "(Re)Start Service"
                                onClicked: {
                                    app.startService()
                                }
                            }
                            Button {
                                text: "Stop Service"
                                onClicked: {
                                    app.stopService()
                                }
                            }
                        }
                    }
                }
                
                //-----------------------------------------------------------------------
                Divider {
                }
                Container {
                    layout: StackLayout {
                    }
                    leftPadding: ui.du(1.5)
                    rightPadding: ui.du(1.5)
                    topPadding: ui.du(1.5)

                    Label {
                        text: "Sync Updates"
                        textStyle.fontSize: FontSize.Large
                    }
                    Container {
                        layout: StackLayout {
                            orientation: LayoutOrientation.LeftToRight
                        }
                        TextArea {
                            text: "If disabled, the app won't periodically check for notifications or messages when it's open. Useful if you are low on battery"
                            editable: false

                        }
                        ToggleButton {
                            onCreationCompleted: {
                                checked = app.backgroundUpdatesEnabled
                            }
                            onCheckedChanged: {
                                app.setBackgroundUpdatesEnabled(checked)
                            }
                            verticalAlignment: VerticalAlignment.Center
                        }
                    }
                }

                //-----------------------------------------------------------------------
                Divider {
                }
                Container {
                    layout: StackLayout {
                    }
                    leftPadding: ui.du(1.5)
                    rightPadding: ui.du(1.5)
                    topPadding: ui.du(1.5)

                    Label {
                        text: "Show Tabs On Action Bar"
                        textStyle.fontSize: FontSize.Large
                    }
                    Container {
                        layout: StackLayout {
                            orientation: LayoutOrientation.LeftToRight
                        }
                        TextArea {
                            text: "If enabled the Home, Notifications, Messages and User Profile tabs will be shown on the bottom bar instead of the left overflow menu"
                            editable: false

                        }
                        ToggleButton {
                            onCreationCompleted: {
                                checked = app.showTabsOnActionBar
                            }
                            onCheckedChanged: {
                                app.setShowTabsOnActionBar(checked)
                            }
                            verticalAlignment: VerticalAlignment.Center
                        }
                    }
                }
            }//end root container
        }//end scroll
        
        actions: [
            ActionItem {
                title: "Save"
                ActionBar.placement: ActionBarPlacement.Signature
                imageSource: "asset:///images/ic_done.png"
                onTriggered: {
                    sheet.close()
                }
            }
        ]
    }
}