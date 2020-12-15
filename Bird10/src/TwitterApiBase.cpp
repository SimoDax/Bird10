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

void TwitterApiBase::parseEmojiInText(std::wstring& text)
{

    // this function returns an html string forcing the os to use the Twemoji font only on emoji characters by placing a <span> element around them
    try{
        std::wstring spanStart(L"<span style=\"font-family:Twemoji\">");
        std::wstring spanEnd(L"</span>");
        bool inEmojiSection = false;
        for(unsigned int i = 0; i<text.size(); i++){
            switch(text[i]){
                case 0x1F100 ... 0x1F64F:
                case 0xFE00 ... 0xFE0F:
                case 0x200B ... 0x200F:
                case 0x2190 ... 0x23FF:
                case 0x2460 ... 0x25FF:
                case 0x2600 ... 0x27EF:
                case 0x2900 ... 0x29FF:
                case 0x2B00 ... 0x2BFF:
                case 0x2C60 ... 0x2C7F:
                case 0x2E00 ... 0x2E7F:
                case 0x3000 ... 0x303F:
                case 0xA490 ... 0xA4CF:
                case 0xE000 ... 0xF8FF:
                case 0xFE30 ... 0xFE4F:
                case 0x1F000 ... 0x1F02F:
                case 0x1F0A0 ... 0x1F0FF:
                case 0x1F680 ... 0x1F6FF:
                case 0x1F90D ... 0x1F9FF:
                    if(!inEmojiSection){
                        text.insert(i,spanStart);
                        i += spanStart.size();
                        inEmojiSection = true;
                    }
                    break;

                default:
                    if(inEmojiSection){
                        text.insert(i, spanEnd);
                        i += spanEnd.size();
                        inEmojiSection = false;
                    }
                    break;
            }
        }
        if(inEmojiSection)
            text.append(spanEnd);

    }catch (const std::exception& e){
        qDebug()<<e.what();
    }
}
