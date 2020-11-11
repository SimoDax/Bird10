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
import com.pipacs.o2 1.0
import bb.system 1.2
import org.labsquare 1.0
import com.simodax 1.0
import "/components" 

Sheet {
    id: sheet
    Page {
        id: searchPage
        property variant appScene: Application.scene
        property bool showTrends: true
        
        onAppSceneChanged: searchfield.requestFocus()
        onCreationCompleted: {
            searchApi.requestTrends()
        }
        
        function searchQuery(query){
            var page = discoverPage.createObject(nav)
            page.search(query)
            nav.push(page)
            close()
            searchSheet.active = false
        }
        
        Container {
            Container {
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                topPadding: 10
                leftPadding: 10
                rightPadding: 10
                
                TextField {
                    id: searchfield
                    hintText: "Press Enter to search"
                    input.submitKey: SubmitKey.EnterKey
                    input.onSubmitted: {
                        searchPage.searchQuery(text.trim())
                    }
                    onTextChanging: {
                        if (text.charAt(0) == "@"){
                            open.visible = true
                            openlbl.text = "Open "+text
                            searchApi.searchUser(text.trim())
                            searchPage.showTrends = false
                        }
                        else{
                            open.visible = false
                            searchPage.showTrends = true
                            if(text.trim().length > 0){
                                searchApi.searchTopic(text.trim())
                            }
                            else searchApi.requestTrends()
                        }
                    }
                }
                Button {
                    text: "Cancel"
                    maxWidth: ui.du(30)
                    onClicked: {
                        close()
                        searchSheet.active = false
                    }
                }
            }
            Container {
                id: open
                visible: false
                minHeight: 100
                leftPadding: 15
                layout: DockLayout {
                }
                gestureHandlers: TapHandler {
                    onTapped: {
                        var page = userProfileDefinition.createObject(nav)
                        page.setUser(searchfield.text.trim().substring(1, searchfield.text.trim().length), "@" + searchfield.text.trim().substring(1, searchfield.text.trim().length))
                        close()
                        nav.push(page)
                        searchSheet.active = false
                    }
                }
                Label {
                    id: openlbl
                    text: "Open " + searchfield.text
                    //textStyle.color: Color.create("#969696")
                    textStyle.fontSize: FontSize.Medium
                    verticalAlignment: VerticalAlignment.Center
                }
                Divider {
                    verticalAlignment: VerticalAlignment.Bottom
                }
            }
            ProfilesList {
                id: results
                visible: !searchPage.showTrends
                dataModel: searchApi.users
                
                onTriggered: {
                    var page = userProfileDefinition.createObject(nav)
                    page.setUser(dataModel.data(indexPath).screen_name, dataModel.data(indexPath).name)
                    close()
                    nav.push(page)
                    searchSheet.active = false
                }
                
            }
            ListView {
                dataModel: searchApi.trends
                visible: searchPage.showTrends
                
                onTriggered: {
                    searchPage.searchQuery(dataModel.data(indexPath))
                }
            }
        }
    }
    attachedObjects: [
        SearchApi {
            id: searchApi
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