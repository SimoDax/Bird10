import bb.cascades 1.4

ActionItem {
    id: loginAction

    title: o1Twitter.linked ? "Logout" : "Login"
    imageSource: "asset:///images/logout.png"
    onTriggered: {
        enabled = false
        if (o1Twitter.linked) {
            o1Twitter.unlink()
            o1Legacy.unlink()
        } else {
            o1Twitter.link()
        }
    }
    ActionBar.placement: ActionBarPlacement.InOverflow
}