import bb.cascades 1.4

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