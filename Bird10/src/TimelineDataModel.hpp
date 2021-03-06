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
    void updateCard(const QString& url, const QVariantMap& newData);


    // Getters and Setters - nothing interesting
    const QString& cursorBottom() const;
    void setCursorBottom(const QString& cursorBottom);
    const QString& cursorTop() const;
    void setCursorTop(const QString& cursorTop);
    bool isTerminateBottom() const;
    void setTerminateBottom(bool terminateBottom);
    bool isTerminateTop() const;
    void setTerminateTop(bool terminateTop);

signals:
    void cardUpdated(int indexPath);

public slots:
    void clear();


protected:
    bool m_terminateBottom, m_terminateTop;
    QString m_cursorTop, m_cursorBottom;

protected:
    QString getTimeString(const QDateTime& dt);
    QString getFullTimeString(const QDateTime& dt);

};

#endif /* TIMELINEDATAMODEL_HPP_ */
