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

#ifndef TWITTERAPIBASE_HPP_
#define TWITTERAPIBASE_HPP_

#include <QObject>
#include <src/o2/oXtwitter.h>
#include <src/o2/o1requestor.h>

class TwitterApiBase : public QObject
{
    Q_OBJECT

public:
   /// OAuth authenticator
   Q_PROPERTY(OXTwitter *authenticator READ authenticator WRITE setAuthenticator)
   inline OXTwitter *authenticator() const { return authenticator_; };
   inline void setAuthenticator(OXTwitter *v){ authenticator_ = v; requestor = new O1Requestor(authenticator_, this); };

   TwitterApiBase(QObject *parent = 0);
   virtual ~TwitterApiBase();

signals:
    void networkError();
    void error(const QString& error);

protected:
    QList<O0RequestParameter> basicGetParameters();
    void saveReply(CurlEasy * reply, const QString& filename);

protected slots:
    void onRequestFailed(CURLcode error);

protected:
   OXTwitter* authenticator_;
   O1Requestor* requestor;
};

#endif /* TWITTERAPIBASE_HPP_ */
