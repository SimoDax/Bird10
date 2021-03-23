import bb.cascades 1.4

Sheet {
    id: blackSheet
    
    onCreationCompleted: {
        app.showBlackBg.connect(blackSheet.open)
        app.hideBlackBg.connect(blackSheet.close)
    }
    
    Page {
        Container {
            layout: DockLayout {
            }
            verticalAlignment: VerticalAlignment.Fill
            horizontalAlignment: HorizontalAlignment.Fill
            background: Color.Black
        }
    }
}