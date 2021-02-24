import bb.cascades 1.4

ListItemComponent {
    id: lic
    type: "showThread"
    Container {
        id: itemRoot
        gestureHandlers: TapHandler {
            onTapped: {
                 itemRoot.ListItem.view.openConversation([itemRoot.ListItem.indexPath[0] - 1])    // this component is always placed below the first tweet in the thread
            }
        }

        layout: DockLayout {
        }
        preferredWidth: Infinity
        leftPadding: 15
        topPadding: 15
        bottomPadding: 5
        
        background: bg.imagePaint
        
        Container {
            leftPadding: 110-15
            topPadding: 0
            bottomPadding: 0
            rightPadding: 0
            verticalAlignment: VerticalAlignment.Center
            
            Label {
                text: "Show this conversation"
                textStyle.color: Application.themeSupport.theme.colorTheme.primary
                verticalAlignment: VerticalAlignment.Center
            }
        }
        
        attachedObjects: ImagePaintDefinition {
            id: bg
            imageSource: "asset:///three_dots.amd"
        }
    }
}