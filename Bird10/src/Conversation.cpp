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

#include <src/Conversation.hpp>
#include <bb/cascades/GroupDataModel>

using namespace bb::cascades;

Conversation::Conversation(QObject *parent) : QObject(parent)
{
    m_messageModel = new GroupDataModel(QStringList() << "time", this);
    m_messageModel->setSortedAscending(false);
    m_messageModel->setGrouping(ItemGrouping::None);

    QVariantMap spinner;
    spinner["time"] = 0;  // placed as oldest entry
    m_messageModel->insert(spinner);
}

Conversation::~Conversation()
{
    delete m_messageModel;
}

void Conversation::insertMessage(const QVariantMap& message)
{
    m_messageModel->insert(message);

    emit messageModelChanged();
}
