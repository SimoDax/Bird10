import bb.cascades 1.4

ListItemComponent {
    type: "emoji"
    Label {
        text: ListItemData.unicode
        textStyle.textAlign: TextAlign.Center
        textStyle.fontSize: FontSize.PointValue
        textStyle.fontSizeValue: 9
        textStyle.base: ListItem.view.twemojiStyle
        textStyle.fontFamily: "Twemoji"
        textFormat: TextFormat.Html
    }
}
