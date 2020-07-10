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

#ifndef ApplicationUI_HPP_
#define ApplicationUI_HPP_

#include <QObject>
#include <QThread>
#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include <QSettings>
#include <bb/system/SystemDialog>
#include <bb/system/InvokeTargetReply>
#include <bb/system/InvokeQueryTargetsReply>

using namespace bb::system;
/*!
 * @brief Application UI object
 *
 * Use this object to create and init app UI, to create context objects, to register the new meta types etc.
 */
class ApplicationUI : public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY(bool backgroundUpdatesEnabled READ backgroundUpdatesEnabled WRITE setBackgroundUpdatesEnabled NOTIFY backgroundUpdatesEnabledChanged)
    inline bool backgroundUpdatesEnabled(){ return m_backgroundUpdatesEnabled; };
    inline void setBackgroundUpdatesEnabled(bool enabled){ m_backgroundUpdatesEnabled = enabled; emit backgroundUpdatesEnabledChanged();};

    ApplicationUI();
    virtual ~ApplicationUI() {}

    static QThread curlThread;
    static QNetworkDiskCache * mNetworkDiskCache;
    //static OXTwitter * authenticator;
    Q_INVOKABLE void saveImage(const QUrl& url);
    Q_INVOKABLE void shareImage(const QUrl& url);
    Q_INVOKABLE void openVideo(const QUrl& url, const QString& name);

    Q_INVOKABLE inline void setTheme(int theme){QSettings settings("simodax","bird10"); settings.setValue("theme", theme);};

    Q_INVOKABLE void exportLogs();

    // experimental
    Q_INVOKABLE void exportCore(){
        QFile core("./logs/cascades.core");
        if(core.exists()){
            QFile out("./shared/documents/cascades.core");
            out.open(QFile::WriteOnly);
            if(out.isOpen())
                out.write(core.readAll());      // make a new identical file to avoid copying file permissions. If you directly copy it, you won't have permissions to delete it afterwise
            else
            {
                SystemDialog* m_listdialog;
                m_listdialog = new SystemDialog("Ok");
                m_listdialog->setTitle("Permission denied");
                m_listdialog->exec();
            }
        }
        else
        {
            SystemDialog* m_listdialog;
            m_listdialog = new SystemDialog("Ok");
            m_listdialog->setTitle("Core not found");
            QString namestring;
            QStringList names = QDir("./logs/").entryList();
            foreach(const QString& name, names)
                namestring += name + "\n";
            m_listdialog->setBody(namestring);
            m_listdialog->exec();
        }
    }

signals:
    void backgroundUpdatesEnabledChanged();

public slots:
    void shareImageLoaded();
    void saveImageLoaded();

    void onManualExit();

private slots:
    void onInvokeResult();
    void checkVersion();
    void onDialogFinished(bb::system::SystemUiResult::Type result);
    void onQueryResponse();

private:
    static QNetworkAccessManager * mNetManager;
    InvokeTargetReply * m_reply;
    InvokeQueryTargetsReply* _queryResults;
    bool m_backgroundUpdatesEnabled;
};

#endif /* ApplicationUI_HPP_ */
