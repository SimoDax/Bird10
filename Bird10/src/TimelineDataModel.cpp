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

#include <src/TimelineDataModel.hpp>

TimelineDataModel::TimelineDataModel(QObject* parent) : bb::cascades::ArrayDataModel(parent)
{
}

void TimelineDataModel::refreshElapsedTime()
{
    int n = this->size();
    for(int i = 0; i<n; i++){
        QVariantMap tweet = this->value(i).toMap();
        qulonglong timestamp = tweet["timestamp"].toULongLong();
        QDateTime dt = QDateTime::fromMSecsSinceEpoch(timestamp);

        int deltaTime = dt.secsTo(QDateTime::currentDateTime());

        if(tweet["rt_flag"].toBool() == false && deltaTime > 60*60*24)
            break;

        tweet["time"] = getTimeString(dt.toLocalTime());
        tweet["full_time"] = getFullTimeString(dt.toLocalTime());

        this->ArrayDataModel::replace(i, tweet);
    }
}

void TimelineDataModel::replaceById(const QString& id, const QVariantMap& newData)
{
    int n = this->size();
    for(int i = 0; i<n; i++){
        QVariantMap data = this->value(i).toMap();
        if(data["id_str"].toString() == id){
            this->ArrayDataModel::replace(i, newData);
            return;
        }
    }
}

void TimelineDataModel::removeById(const QString& id)
{
    int n = this->size();
    for(int i = 0; i<n; i++){
        QVariantMap data = this->value(i).toMap();
        if(data["id_str"].toString() == id){
            this->removeAt(i);
            return;
        }
    }
}

QString TimelineDataModel::getTimeString(const QDateTime& dt)
{
    QDateTime current = QDateTime::currentDateTime();

    int deltaDays = dt.daysTo(current);
    if(deltaDays == 0 || deltaDays == 1){
        int deltaHours = dt.secsTo(current)/3600;

        if(deltaHours == 0){
            int deltaMinutes = dt.secsTo(current)/60;
            if(deltaMinutes == 0)
                return QString::number(dt.secsTo(current)) + "s";
            else
                return QString::number(deltaMinutes) + "min";
        }
        else
            return QString::number(deltaHours) + "h";
    }
    else if (deltaDays <= 7)
        return QString::number(deltaDays) + "d";
    else if(dt.date().year() == current.date().year())
        return dt.toString("d MMM").toLower();
    else
        return dt.toString("d MMM yyyy").toLower();
}

QString TimelineDataModel::getFullTimeString(const QDateTime& dt)
{
    return dt.toString("h:mm AP " + QString::fromUtf8("\u00B7") +" d MMM yyyy").toLower();
}

