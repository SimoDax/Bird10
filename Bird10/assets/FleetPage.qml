import bb.cascades 1.4
import org.labsquare 1.0
import "/components"
import bb.multimedia 1.4
import com.simodax 1.0


Page {
    objectName: "fleetPage"
    property variant thread
    property int currentFleet: -1
    actionBarVisibility: ChromeVisibility.Compact
    
    Container {
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        layout: DockLayout {   
        }
        onTouch: {
            if(event.touchType == TouchType.Down)
                fleetTimer.pause()
            else if (event.touchType == TouchType.Up)
                fleetTimer.resume()
        }
        ForeignWindowControl {
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            id: videoFleet
            windowId: "fleetfwc"
            updatedProperties: WindowProperty.Position | WindowProperty.Size | WindowProperty.Visible
            visible: true
        }
        Container{
            id: imageFleet
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            WebImageView {
                property int index
                id: img1
                url: thread.fleets[index].media_entity.media_url_https
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill
                scalingMethod: ScalingMethod.AspectFit
                visible: false
            }
            WebImageView {    // second image to make the switching between fleets feel smoother: only one of these two will be visible at a given time, however the hidden one will still preload the content
                property int index
                id: img2
                url: thread.fleets[index].media_entity.media_url_https
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill
                scalingMethod: ScalingMethod.AspectFit
                visible: false
            }
        }
        Container {
            property int index
            id: textFleet
            visible: false
            layout: DockLayout {
            }
            Container {
                verticalAlignment: VerticalAlignment.Center
                horizontalAlignment: HorizontalAlignment.Center
                background: Color.create("#000000");
                Label {
                    text: thread.fleets[textFleet.index].text
                    textStyle.fontWeight: FontWeight.Bold
                    textStyle.color: Color.create("#F0F0F0");
                }
            }
        }
        Container {
            topPadding: ui.du(2)
            leftPadding: ui.du(2)
            rightPadding: ui.du(2)
            bottomPadding: ui.du(4)
            background: gradient.imagePaint
            Container {
                id: progressContainer
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
            }
            attachedObjects: ImagePaintDefinition {
                id: gradient
                imageSource: "asset:///images/fleet_gradient.png"
                repeatPattern: RepeatPattern.X
            }
        }
        
        QmlTimer {
            id: tmr
            duration: 10000
            onTriggered: {
                if(currentFleet < thread.fleets.length - 1){
                    //                    // switch between the images
                    //                    img1.visible = ! img1.visible // the visible binding will toggle img2 as well
                    //                    
                    //                    if(img1.visible)    // start preloading on the one that has become hidden
                    //                        img2.index = currentFleet + 2     // no problem if it accesses an invalid index, if that's the case it won't be shown anyway
                    //                    else 
                    //                        img1.index = currentFleet + 2
                    
                    currentFleet ++
                    switchFleet()
                }
                else nav.pop()
            }
        }
        attachedObjects: [
            Timer{
                id: fleetTimer
                duration: 10000
                onTriggered: switchFleet()
                onActiveChanged: {
                    console.debug('timer active: '+active)
                }
            },
            ComponentDefinition {
                id: pi
                content: ProgressIndicator {
                    property int index: -1    // set dynamically when the progress bar container on top of the page is populated
                    horizontalAlignment: HorizontalAlignment.Fill
                    verticalAlignment: VerticalAlignment.Top
                    fromValue: 0
                    toValue: 1
                    value: {
                        if(currentFleet == index)    // it's my time to shine
                            return fleetTimer.active ? fleetTimer.progress : (player.position / player.duration);
                        else if (currentFleet > index)
                            return 1
                        else return 0
                    }
                    state: ProgressIndicatorState.Progress
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 1
                    }
                }
            }, 
            MediaPlayer {
                id: player
                windowId: videoFleet.windowId
                repeatMode: RepeatMode.None
                videoOutput: VideoOutput.PrimaryDisplay
                statusInterval: 16    // 60 fps
                onMediaStateChanged: {
                    console.debug(mediaState+ " "+videoFleet.boundToWindow)
                    if(mediaState == MediaState.Stopped){
                        switchFleet()
                        reset()
                    }
                }
                onPositionChanged: {
                    console.debug(fleetTimer.active)
                }
            },
            LayoutUpdateHandler {
                property alias width: videoHandler.layoutFrame.width
                property alias height: videoHandler.layoutFrame.height
                id: videoHandler
                onLayoutFrameChanged: {
                    console.debug('width: ' + videoHandler.layoutFrame.width + ' height: ' + videoHandler.layoutFrame.height)
                }
            }
        ]
    }
    
    function switchFleet(){
        if (currentFleet < thread.fleets.length - 1) {
            currentFleet ++
            
            // Show the current fleet..
            if (typeof (thread.fleets[currentFleet].media_key) == 'undefined'){
                fleetTimer.start()
                img1.visible = img2.visible = false
                textFleet.visible = true
                videoFleet.visible = false
            }
            else if(thread.fleets[currentFleet].media_key.media_category == "TWEET_IMAGE"){
                fleetTimer.start()
                if(img1.index == currentFleet){
                    img1.visible = true
                    img2.visible = false
                }
                else{
                    img2.visible = true
                    img1.visible = false
                }
                textFleet.visible = false
                videoFleet.visible = false
            }
            else if(thread.fleets[currentFleet].media_key.media_category == "TWEET_VIDEO"){
                console.debug('video is still experimental!')
                img1.visible = img2.visible = false
                textFleet.visible = false
                videoFleet.visible = true
                player.play()
                // no need to start the timer, once the video ends the mediaplayer will call this function again
            }
            
            // .. and start preloading the next one
            preloadNextFleet()
        }
        else {
            close()
        }
    }
    
    function preloadNextFleet(){
        if (currentFleet < thread.fleets.length - 1) {
            if (typeof (thread.fleets[currentFleet + 1].media_key) != 'undefined' && thread.fleets[currentFleet + 1].media_key.media_category == "TWEET_IMAGE") {
                if (img1.visible) // start preloading on the one that is hidden
                    img2.index = currentFleet + 1
                else
                    img1.index = currentFleet + 1
            }
            // remember you removed videofleet.visible == false condition for testing
            if (thread.fleets[currentFleet + 1].media_key != 'undefined' && thread.fleets[currentFleet + 1].media_key.media_category == "TWEET_VIDEO") {
                var variants = thread.fleets[currentFleet + 1].media_entity.media_info.video_info.variants
                player.sourceUrl = "http://" + variants[variants.length - 1].url.substring(8) // replace https with http
                console.debug(videoFleet.url)
                
                var aspect = thread.fleets[currentFleet + 1].media_entity.media_info.video_info.aspect_ratio
                if(aspect.numerator < aspect.denominator){    // video is vertical
                    videoFleet.verticalAlignment = VerticalAlignment.Fill
                    videoFleet.horizontalAlignment = HorizontalAlignment.Center
                    videoFleet.preferredWidth = parseFloat(display.pixelSize.height)*parseFloat(aspect.numerator)/parseFloat(aspect.denominator)
                    console.debug('preferredWidth: '+ videoFleet.preferredWidth)
                }
                else{
                    videoFleet.verticalAlignment = VerticalAlignment.Center
                    videoFleet.horizontalAlignment = HorizontalAlignment.Fill
                    videoFleet.preferredHeight = parseFloat(display.pixelSize.width) * aspect.denominator / aspect.numerator
                }
                player.prepare()
            }
        }
    }
    
    function close(){
        var ids = new Array()
        for (var i = 0; i <= currentFleet; i ++)
            ids.push(thread.fleets[i].fleet_id)
        fleetApi.markRead(thread.fleet_thread_id, ids)
        nav.pop();
    }
    
    onThreadChanged: {
        for(var i = 0; i<thread.fleets.length; i++){
            var x = pi.createObject()
            x.index = i
            progressContainer.add(x)
        }
        
        preloadNextFleet()
        switchFleet()
    }
    paneProperties: NavigationPaneProperties {
        backButton: ActionItem {
            onTriggered: close()
            }
        }
    }
