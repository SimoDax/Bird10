import bb.cascades 1.4
import "/components/actions"

Page {

    actionBarAutoHideBehavior: ActionBarAutoHideBehavior.HideOnScroll
    actionBarVisibility: ChromeVisibility.Compact

    actions: [
        ActionItem {
            id: scrollTop
            title: "Scroll to top"
            imageSource: "asset:///images/ic_to_top.png"
            onTriggered: {
                tweetList.scrollToItem([ 0 ])
            }
            ActionBar.placement: ActionBarPlacement.InOverflow
        },
        LoginAction {
            id: loginAction
        },
        PayAction {
            id: payAction
        }
    ]
}