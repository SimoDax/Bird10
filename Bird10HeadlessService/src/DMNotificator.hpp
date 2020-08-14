/*
 * DMNotificator.hpp
 *
 *  Created on: 08/ago/2020
 *      Author: Simone
 */

#ifndef DMNOTIFICATOR_HPP_
#define DMNOTIFICATOR_HPP_

#include "Notificator.hpp"

class DMNotificator: public Notificator
{
    Q_OBJECT
public:
    DMNotificator(OXTwitter* authenticator, QSettings* settings, QObject* parent);

    void checkTwitterNotifications();

protected slots:
    void onNotifications();

protected:
    void sendNotification(const QString& body, const QString& conversation_id);
    void deleteNotification(const QString& conversation_id);
    void deleteAll();

    QString buildBody(const QVariantMap& conversation);
};

#endif /* DMNOTIFICATOR_HPP_ */
