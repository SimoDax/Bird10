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


#ifndef RETWEETDIALOG_HPP_
#define RETWEETDIALOG_HPP_

#include <QObject>
#include <bb/system/SystemDialog>

class RetweetDialog: public bb::system::SystemDialog{
    Q_OBJECT

public:
//    Q_PROPERTY(QVariantMap tweet READ tweet)
//    inline QVariantMap tweet() {return m_tweet;}

    RetweetDialog();
    Q_INVOKABLE void open(int indexPath);

Q_SIGNALS:
    void retweet(int indexPath);
    void quote(int indexPath);

private Q_SLOTS:
    void onFinished(bb::system::SystemUiResult::Type value);

private:
    int m_indexPath;
};

#endif /* RETWEETDIALOG_HPP_ */
