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

#include <src/RetweetDialog.hpp>


RetweetDialog::RetweetDialog() : SystemDialog("Quote tweet", "Retweet", "Cancel"){
    //SystemDialog("Retweet", "Quote tweet", "Cancel");
    setTitle("Select an action");
    connect(this, SIGNAL(finished(bb::system::SystemUiResult::Type)), this, SLOT(onFinished(bb::system::SystemUiResult::Type)));
}

void RetweetDialog::open(int indexPath){
    m_indexPath = indexPath;

    show();
}

void RetweetDialog::onFinished(bb::system::SystemUiResult::Type value){
    if(value == bb::system::SystemUiResult::ConfirmButtonSelection)
        emit quote(m_indexPath);
    else if(value == bb::system::SystemUiResult::CustomButtonSelection)
        emit retweet(m_indexPath);
//    else if(value == bb::system::SystemUiResult::CancelButtonSelection)
//        this->deleteLater();
}
