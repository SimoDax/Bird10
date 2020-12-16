/*
 * FleetApi.hpp
 *
 *  Created on: 09/dic/2020
 *      Author: Simone
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
