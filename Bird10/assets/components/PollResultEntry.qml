import bb.cascades 1.4

Container {
    property real barPercentage: 0
    property bool isWinner: false
    property bool isVotedByUser: false
    property alias barColor: bg.background
    property alias text: choiceLabel.text
    //                                        minHeight: ui.du(6)
    //                                        maxHeight: ui.du(6)
    topPadding: ui.du(.5)
    bottomPadding: ui.du(.5)
    preferredWidth: Infinity
    layout: DockLayout {
    }
    Container {
        id: bg
        verticalAlignment: VerticalAlignment.Fill
        horizontalAlignment: HorizontalAlignment.Left
        minWidth: barPercentage * resultsContainerSize.width
        maxWidth: barPercentage * resultsContainerSize.width
        background: {
            if(Application.themeSupport.theme.colorTheme.style == VisualStyle.Bright)
                return isWinner ? Color.create("#71C9F8") : Color.create("#CCD6DD")
            else 
                return isWinner ? Color.create("#66b2fe") : Color.create("#3D5466")
        }
    }
    Container {
        verticalAlignment: VerticalAlignment.Center
        topPadding: ui.du(1)
        bottomPadding: ui.du(1)
        leftPadding: ui.du(1)
        Label {
            id: choiceLabel
            verticalAlignment: VerticalAlignment.Center
            horizontalAlignment: HorizontalAlignment.Left
            textStyle.fontWeight: isWinner ? FontWeight.W500 : FontWeight.Normal
        }
    }
    ImageView {
        id: checkmark
        visible: isVotedByUser
        imageSource: Application.themeSupport.theme.colorTheme.style == VisualStyle.Bright ? "asset:///images/ic_done_black.png" : "asset:///images/ic_done.png"
        verticalAlignment: VerticalAlignment.Center
        horizontalAlignment: HorizontalAlignment.Right
        scalingMethod: ScalingMethod.AspectFit
        maxHeight: ui.du(4)
        rightMargin: ui.du(1)
    }
}