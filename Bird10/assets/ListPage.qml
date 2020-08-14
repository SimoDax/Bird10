import bb.cascades 1.4
import com.simodax 1.0

Page {
    property alias listId: twitterApi.listId
    property alias name: topBar.text
    property alias twitterApi: twitterApi
    
    
    Container {
        verticalAlignment: VerticalAlignment.Fill

        TopBlueBarText {
            id: topBar
            horizontalAlignment: HorizontalAlignment.Fill
            searchVisible: false
        }

        TweetList {
            id: tweetList

            attachedObjects: [
                ListScrollStateHandler {
                    onAtEndChanged: {
                        if (atEnd && twitterApi.tweetModel.size() != 0 && twitterApi.tweetModel.size() < 500)
                            twitterApi.requestOlderTweets()
                    }
                }
            ]
        }
    }

    actions: [
        ActionItem {
            id: refreshAction
            enabled: o1Twitter.linked
            title: "Refresh"
            imageSource: "asset:///images/ic_resume.png"
            onTriggered: twitterApi.requestTweets()
            ActionBar.placement: ActionBarPlacement.InOverflow
        },
        ActionItem {
            id: loginAction

            title: o1Twitter.linked ? "Logout" : "Login"
            imageSource: "asset:///images/logout.png"
            onTriggered: {
                enabled = false
                if (o1Twitter.linked) {
                    o1Twitter.unlink()
                } else {
                    o1Twitter.link()
                }
            }
            ActionBar.placement: ActionBarPlacement.InOverflow

            //onCreationCompleted: o1Twitter.
        },
        ActionItem {
            id: payAction
            title: "Donate"
            imageSource: "asset:///images/heart.png"
            onTriggered: {
                _pay.trigger("bb.action.OPEN")
            }
            attachedObjects: Invocation {
                id: _pay
                query {
                    uri: "https://paypal.me/pools/c/8pJlhpRSa8"
                    invokeTargetId: "sys.browser"
                }
            }
        }
    ]
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
        }
    ]
}

