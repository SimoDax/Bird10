/*
* Bird10
* Copyright (C) 2020  Simone Dassi
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
*/

// QmlTimer.qml
import bb.cascades 1.0

Container {
    id: root
    visible: false
    opacity: 0.0

    property int duration: 0
    property bool timerActive: false

    signal triggered

    function start() {
        if (duration == 0) {
            console.log("timer can't start when duration is 0")
            return
        }

        if (timerActive == true) {
            console.log("timer is already active, not starting")
            return
        }

        opacity = 0;
        timerActive = true;
        timerAnimation.play()
    }

    function stop() {
        timerActive = false
        timerAnimation.stop()
    }

    animations: [
        FadeTransition {
            id: timerAnimation
            fromOpacity: 0
            toOpacity: 1
            duration: root.duration
            
            onEnded: {
                timerActive = false
                root.triggered()
            }
        }
    ]
}