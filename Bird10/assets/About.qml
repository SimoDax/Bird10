import bb.cascades 1.4

Sheet {
    id: aboutSheet
    Page {
        titleBar: TitleBar {
            title: "Bird10 for BlackBerry"
            dismissAction: ActionItem {
                title: "Close"
                onTriggered: {
                    aboutSheet.close()
                }
            }
        }
        ScrollView {
            horizontalAlignment: HorizontalAlignment.Fill
            Container {
                horizontalAlignment: HorizontalAlignment.Fill
                topPadding: ui.du(5)
                bottomPadding: topPadding
                leftPadding: topPadding
                rightPadding: topPadding
                ImageView {
                    imageSource: "asset:///images/logo.png"
                    horizontalAlignment: HorizontalAlignment.Center
                    bottomPadding: ui.du(5)
                    scalingMethod: ScalingMethod.AspectFit
                    maxHeight: ui.du(35)
                }
                Label {
                    text: "<html><span style='font-size:x-large;'>A Native Twitter Client for BlackBerry 10 </span></html>"
                    horizontalAlignment: HorizontalAlignment.Center
                    textStyle.textAlign: TextAlign.Center
                    multiline: true
                }
                Label {
                    text: "\nYou can find the source code for this app at \nhttps://github.com/SimoDax/Bird10"
                    horizontalAlignment: HorizontalAlignment.Center
                    textStyle.textAlign: TextAlign.Center
                    multiline: true
                    content.flags: TextContentFlag.ActiveText
                }
            }
        }
    }
}