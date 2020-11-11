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

#ifndef TIMELINEDATAMODEL_HPP_
#define TIMELINEDATAMODEL_HPP_

#include <bb/cascades/ArrayDataModel>

class TimelineDataModel: public bb::cascades::ArrayDataModel
{
    Q_OBJECT

public:
    TimelineDataModel(QObject* parent = 0);

    void refreshElapsedTime();
    void replaceById(const QString& id, const QVariantMap& newData);
    void removeById(const QString& id);

    bool isTerminateBottom() const
    {
        return m_terminateBottom;
    }

    void setTerminateBottom(bool terminateBottom)
    {
        m_terminateBottom = terminateBottom;
    }

    bool isTerminateTop() const
    {
        return m_terminateTop;
    }

    void setTerminateTop(bool terminateTop)
    {
        m_terminateTop = terminateTop;
    }

public slots:
    void clear(){
        bb::cascades::ArrayDataModel::clear(); m_terminateBottom = false; m_terminateTop = false;
    }


private:
    bool m_terminateBottom, m_terminateTop;

protected:
    QString getTimeString(const QDateTime& dt);
    QString getFullTimeString(const QDateTime& dt);

};

#endif /* TIMELINEDATAMODEL_HPP_ */
