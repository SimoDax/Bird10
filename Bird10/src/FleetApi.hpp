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

#ifndef FLEETAPI_HPP_
#define FLEETAPI_HPP_

#include "TwitterApiBase.hpp"
#include <bb/cascades/ArrayDataModel>

using namespace bb::cascades;

class FleetApi: public TwitterApiBase
{
    Q_OBJECT

    Q_PROPERTY(bb::cascades::ArrayDataModel* fleetThreads READ fleetThreads CONSTANT)
    ArrayDataModel* fleetThreads(){return m_fleetThreads;};

    Q_PROPERTY(bool allRead READ allRead NOTIFY allReadChanged)
    bool allRead(){return m_allRead;};

public:
    FleetApi(QObject* parent = 0);

    Q_INVOKABLE void requestFleets();
    Q_INVOKABLE void markRead(const QString& fleet_thread_id, const QStringList& ids);
    Q_INVOKABLE void markRead(const QString& id);
//    Q_INVOKABLE void preloadFleetThread(int indexPath);

signals:
    void fleetsLoaded();
    void allReadChanged();

protected slots:
    void onFleetsReceived();
    void onUserInfo();

protected:
    void parseFleets(const QVariantList& threads);
    void requestUserInfo(int thread_index);


protected:
    ArrayDataModel* m_fleetThreads;
    QVariantList m_threads;
    int m_fleetsParsed;
    bool m_allRead;
};

#endif /* FLEETAPI_HPP_ */
