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
    property int index
    property alias text: lbl.text
    horizontalAlignment: HorizontalAlignment.Fill
    verticalAlignment: VerticalAlignment.Fill
    topPadding: ui.du(.5)
    bottomPadding: ui.du(.5)
    layout: DockLayout {

    }
    Container {
        verticalAlignment: VerticalAlignment.Center
        horizontalAlignment: HorizontalAlignment.Center
        topPadding: ui.du(1.5)
        bottomPadding: ui.du(1.5)
        Label {
            id: lbl
            verticalAlignment: VerticalAlignment.Center
            horizontalAlignment: HorizontalAlignment.Center
        }
    }
    ImageButton {
        defaultImageSource: "asset:///images/poll_button.amd"
        disabledImageSource: "asset:///poll_button_disabled.amd"
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        onClicked: {
            itemRoot.disablePollButtons()
            lbl.textStyle.color = Application.themeSupport.theme.colorTheme.primary
            itemRoot.ListItem.view.api.votePoll(ListItemData.card.url, ListItemData.id_str, index, ListItemData.poll_choice_count)
        }
    }
}