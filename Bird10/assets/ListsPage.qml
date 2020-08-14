import bb.cascades 1.4
import com.pipacs.o2 1.0
import com.simodax 1.0
import org.labsquare 1.0
import "/components"

NavigationPane {
    id: nav
    peekEnabled: false

    
    Page {
        actionBarVisibility: ChromeVisibility.Compact
        onCreationCompleted: {
            twitterApi.requestLists()
        }
        
        Container {
            TopBlueBarText {
                text: "Lists"
                searchVisible: false
                tweetVisible: false
            }
            ListView {
                id: listsView
                dataModel: twitterApi.listsModel
                
                onTriggered: {
                    var page = listPage.createObject()
                    page.listId = dataModel.data(indexPath).id_str
                    page.name = dataModel.data(indexPath).name
                    page.twitterApi.requestTweets()
                    nav.push(page)
                }
                
                listItemComponents: [
                    ListItemComponent {
                        type: ""
                        Container {
                            leftPadding: 15
                            topPadding: 15
                            bottomPadding: 15
                            rightPadding: 15
                            Container {
                                id: topRow
                                bottomMargin: ui.du(3)
                                layout: StackLayout {
                                    orientation: LayoutOrientation.LeftToRight
                                }
                                Label {
                                    text: ListItemData.name
                                    textStyle.fontWeight: FontWeight.Bold
                                    textStyle.fontSize: FontSize.Large
                                }
                                ImageView {
                                    visible: ListItemData.mode == "private"
                                    imageSource: "asset:///images/profile/icn_lock_sm.png"
                                    horizontalAlignment: HorizontalAlignment.Center
                                    verticalAlignment: VerticalAlignment.Bottom
                                    maxHeight: ui.du(4)
                                    maxWidth: ui.du(4)
                                    scalingMethod: ScalingMethod.AspectFit
                                }
                            }
                            Container {
                                id: bottomRow
                                layout: StackLayout {
                                    orientation: LayoutOrientation.LeftToRight
                                }
                                WebImageView{
                                    id: profilePic
                                    url: "http://" + ListItemData.user.profile_image_url_https.substring(8) // replace https with http
                                    //url: ListItemData.user.profile_image_url_https.replace("https","http")
                                    minWidth: 50.0
                                    minHeight: 50.0
                                }
                                Label {
                                    id: username
                                    text: ListItemData.user.name
                                    verticalAlignment: VerticalAlignment.Center
                                    textStyle.fontWeight: FontWeight.Bold
                                    textStyle.fontSizeValue: 7.0
                                    rightMargin: 5.0
                                    textStyle.fontFamily: 'Slate Pro'
                                    layoutProperties: StackLayoutProperties {
                                        spaceQuota: 1
                                    }
                                }
                                Label {
                                    id: handle
                                    verticalAlignment: VerticalAlignment.Center
                                    text: "@" + ListItemData.user.screen_name
                                    textStyle.fontSizeValue: 7.0
                                    textStyle.color: Color.create("#7A7A7A")
                                    leftMargin: 0.0
                                    textStyle.fontFamily: 'Slate Pro'
                                }
                            }
                            Divider {
                            }
                        }
                    }
                ]
            }
        }
        
        attachedObjects: [
            ListApi {
                id: twitterApi
                authenticator: o1Twitter
                onError: {
                    error_message(error)
                }
                onNetworkError: {
                    error_message("Network error")
                }
            },
            ComponentDefinition {
                id: listPage
                source: "ListPage.qml"
            }
        ]
    }
    
    onPopTransitionEnded: {
        page.destroy()
    }
}
