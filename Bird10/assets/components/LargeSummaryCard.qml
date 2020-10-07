import bb.cascades 1.4
import org.labsquare 1.0

ControlDelegate {
    visible: delegateActive
    bottomMargin: ui.du(2)
    delegateActive: (typeof (ListItemData.card) != "undefined") && ListItemData.card.name == "summary_large_image"
    sourceComponent: ComponentDefinition {
        Container {
            topPadding: 1
            leftPadding: 1
            bottomPadding: 1
            rightPadding: 1
            preferredWidth: Infinity
            verticalAlignment: VerticalAlignment.Fill
            gestureHandlers: TapHandler {
                onTapped: {
                    tapHandled = true
                    openLink.trigger("bb.action.OPEN")
                }
            }
            WebImageView {
                id: preview
                property real size: ui.du(40)
                scalingMethod: ScalingMethod.AspectFill
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill
                minHeight: size
                maxHeight: size
                loadEffect: ImageViewLoadEffect.None
                url: ListItemData.card.binding_values.photo_image_full_size_original.image_value.url
            }
            Container {
                verticalAlignment: VerticalAlignment.Center
                topPadding: ui.du(1)
                bottomPadding: ui.du(1)
                leftPadding: ui.du(1)
                maxHeight: preview.size / 2
                Label {
                    text: ListItemData.card.binding_values.title.string_value
                    textStyle.fontWeight: FontWeight.W500
                }
                Label {
                    text: ListItemData.card.binding_values.description.string_value
                    textStyle.color: Color.create("#7A7A7A")
                    multiline: true
                    bottomMargin: 0
                    topMargin: 0
                }
                Container {
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    ImageView {
                        maxHeight: ui.du(3)
                        maxWidth: ui.du(3)
                        scalingMethod: ScalingMethod.AspectFit
                        imageSource: "asset:///images/link.png"
                        verticalAlignment: VerticalAlignment.Center
                    }
                    Label {
                        leftMargin: 0
                        textStyle.color: Color.create("#7A7A7A")
                        text: ListItemData.card.binding_values.domain.string_value
                        verticalAlignment: VerticalAlignment.Center
                    }
                }
            }
            background: back.imagePaint
            attachedObjects: [
                ImagePaintDefinition {
                    id: back
                    imageSource: "asset:///quoted_border.amd"
                },
                Invocation {
                    id: openLink
                    query {
                        uri: ListItemData.card.url
                        //                            invokeTargetId: "sys.browser"
                        onQueryChanged: {
                            openLink.query.updateQuery()
                        }
                    }
                }
            ]
        }
    }
}