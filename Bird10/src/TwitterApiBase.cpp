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

#include <src/TwitterApiBase.hpp>
#include <QDebug>
#include <QFile>
#include <src/QJson4/QJsonArray.h>
#include <src/QJson4/QJsonDocument.h>
#include <src/QJson4/QJsonValue.h>
#include <src/QJson4/QJsonObject.h>


TwitterApiBase::TwitterApiBase(QObject *parent) : QObject(parent){
    authenticator_ = NULL;
}


TwitterApiBase::~TwitterApiBase()
{
    requestor->deleteLater();
}

void TwitterApiBase::onRequestFailed(CURLcode error) {
    CurlEasy* reply = qobject_cast<CurlEasy *>(sender());
    long code = reply->get<long>(CURLINFO_RESPONSE_CODE);

    if(error == CURLE_OK){

        QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->data());
        QVariantMap content = jsonResponse.object().toVariantMap();

        QString errorString;
        if(content.keys().contains("error"))
            errorString = content["error"].toString();
        else
            if (content.keys().contains("errors"))
                errorString = content["errors"].toList()[0].toMap()["message"].toString();

        if(code == 429)
            emit this->error("Too many requests in a 15-minutes window. Please try again later");
        else
            emit this->error("Twitter error: " + !errorString.isEmpty() ? errorString : QString::number(code));
    }
    else
        emit networkError();
    qWarning() << "TwitterApiBase::requestFailed:" << (int)error << code << reply->data();

    reply->deleteLater();
}

QList<O0RequestParameter> TwitterApiBase::basicGetParameters()
{
    /// This function sets all parameters except:
    ///
    /// - include_user_entities
    /// - count

    QList<O0RequestParameter> par;
    par.append(O0RequestParameter("tweet_mode", "extended"));
    par.append(O0RequestParameter("include_profile_interstitial_type", "1"));
    par.append(O0RequestParameter("include_blocking", "1"));
    par.append(O0RequestParameter("include_blocked_by", "1"));
    par.append(O0RequestParameter("include_followed_by", "1"));
    par.append(O0RequestParameter("include_want_retweets", "1"));
    par.append(O0RequestParameter("include_mute_edge", "1"));
    par.append(O0RequestParameter("include_can_dm", "1"));
    par.append(O0RequestParameter("include_can_media_tag", "1"));
    par.append(O0RequestParameter("skip_status", "1"));
    par.append(O0RequestParameter("cards_platform", "Web-12"));
    par.append(O0RequestParameter("include_cards", "1"));
    par.append(O0RequestParameter("include_composer_source", "true"));
    par.append(O0RequestParameter("include_ext_alt_text", "true"));
    par.append(O0RequestParameter("include_reply_count", "1"));
    par.append(O0RequestParameter("include_entities", "true"));
    par.append(O0RequestParameter("include_ext_media_color", "false"));
    par.append(O0RequestParameter("include_ext_media_availability", "false"));
    par.append(O0RequestParameter("send_error_codes", "true"));
    par.append(O0RequestParameter("simple_quoted_tweets", "true"));
    par.append(O0RequestParameter("ext", "mediaStats,highlightedLabel,cameraMoment"));

    return par;
}

void TwitterApiBase::saveReply(CurlEasy * reply, const QString& filename){
    QFile file("./data/" + filename + ".txt");
    file.open(QIODevice::WriteOnly);
    if(file.isOpen()){
        QTextStream out(&file);
        out << reply->data();
    }
    file.close();
}
