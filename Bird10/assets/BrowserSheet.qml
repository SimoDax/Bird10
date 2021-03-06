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

import bb.cascades 1.4

Sheet {
    id: browser
    property url url

    content: Page {
        ScrollView {
            verticalAlignment: VerticalAlignment.Fill
            WebView {
                id: webView
                url: browser.url
                verticalAlignment: VerticalAlignment.Fill
                // I'm an iPhone i swear
                settings.userAgent: "Mozilla/5.0 (iPhone; CPU iPhone OS 13_1_2 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/13.0.1 Mobile/15E148 Safari/604.1"
                // besides, they copied keyless navigation from bb10 devices..
                
                onUrlChanged: {
                    console.debug("Browser url: " + url)
                    if(url == "https://twitter.com/account/login_challenge/success" || url == "https://twitter.com/account/login_verification/success")
                        o1Twitter.onChallengeSuccess();
                }
            }
        }
    }
}