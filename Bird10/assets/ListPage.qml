import bb.cascades 1.4
import com.simodax 1.0
import "/components"
import "/components/actions"

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
        LoginAction {
            id: loginAction
        },
        PayAction {
            id: payAction
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

