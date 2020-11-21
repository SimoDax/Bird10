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