/*
 * Notificator.hpp
 *
 *  Created on: 08/ago/2020
 *      Author: Simone
 */

#ifndef NOTIFICATOR_HPP_
#define NOTIFICATOR_HPP_

#include <QObject>
#include <QSettings>
#include "o2/oxtwitter.h"

#ifdef QT_DEBUG
#define NOTIFICATION_TITLE "Bird10 (debug)"
#else
#define NOTIFICATION_TITLE "Bird10"
#endif


class Notificator : public QObject
{
    Q_OBJECT
public:
    Notificator(OXTwitter* authenticator, QSettings* settings, QObject* parent);

    void checkTwitterNotifications();

signals:
    void done();

protected slots:
    void onNotifications();
    void onError();

protected:
    void sendNotification(const QString& body, const QString& sortIndex);
    void deleteNotification(const QString& key);

    QString buildBody(const QVariantMap& notification);

protected:

    OXTwitter* m_authenticator;
    QSettings* m_settings;
};

#endif /* NOTIFICATOR_HPP_ */
