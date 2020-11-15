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
#include <bb/system/InvokeRequest>
#include <bb/system/InvokeManager>
#include <bb/device/DisplayInfo>
#include <bb/cascades/AbstractPane>
#include <src/DMApi.hpp>

using namespace bb::system;
using namespace bb::cascades;
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
    Q_INVOKABLE void setBackgroundUpdatesEnabled(bool enabled){ m_backgroundUpdatesEnabled = enabled; emit backgroundUpdatesEnabledChanged();};

    Q_PROPERTY(bool showTabsOnActionBar READ showTabsOnActionBar WRITE setShowTabsOnActionBar NOTIFY showTabsOnActionBarChanged)
    inline bool showTabsOnActionBar(){ return m_showTabsOnActionBar; };
    Q_INVOKABLE void setShowTabsOnActionBar(bool show){ m_showTabsOnActionBar = show; QSettings settings("simodax","bird10"); settings.setValue("showTabsOnActionBar", show); emit showTabsOnActionBarChanged();};

    Q_PROPERTY(DMApi* dm READ dmApi CONSTANT)
    inline DMApi* dmApi(){return m_dmApi;};

    Q_PROPERTY(bb::device::DisplayInfo* display READ display CONSTANT)
    inline bb::device::DisplayInfo* display(){return m_displayInfo;};


    ApplicationUI();
    virtual ~ApplicationUI() {}

    static QThread curlThread;
    static QNetworkDiskCache * mNetworkDiskCache;
    //static OXTwitter * authenticator;
    Q_INVOKABLE void saveImage(const QUrl& url);
    Q_INVOKABLE void shareImage(const QUrl& url);
    Q_INVOKABLE void openVideo(const QUrl& url, const QString& name);

    Q_INVOKABLE inline void setTheme(int theme){QSettings settings("simodax","bird10"); settings.setValue("theme", theme);};

    Q_INVOKABLE void startService();
    Q_INVOKABLE void stopService();

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
    void showTabsOnActionBarChanged();

    void openConversation(const QString& id);   // Emitted when app is invoked through active text (twitter.com url)
    void openProfile(const QString& screen_name);
    void openList(const QString& id);

public slots:
    void shareImageLoaded();
    void saveImageLoaded();

    void onManualExit();

private slots:
    void onInvokeResult();
    void checkVersion();
    void onDialogFinished(bb::system::SystemUiResult::Type result);
    void onQueryResponse();
    void lateInit();
    void handleInvoke(const bb::system::InvokeRequest& invoke);

private:
    static QNetworkAccessManager * mNetManager;
    bb::device::DisplayInfo* m_displayInfo;
    DMApi* m_dmApi;
    AbstractPane* m_root;
    InvokeManager* m_invokeManager;
    InvokeTargetReply * m_reply;
    InvokeQueryTargetsReply* _queryResults;
    bool m_backgroundUpdatesEnabled, m_showTabsOnActionBar;
};

#endif /* ApplicationUI_HPP_ */
