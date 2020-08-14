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

#include "applicationui.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/LocaleHandler>
#include <bb/cascades/ThemeSupport>
#include <bb/cascades/VisualStyle>
#include <bb/system/SystemDialog>
#include <bb/system/InvokeManager>
#include <bb/system/InvokeRequest>
#include <bb/system/InvokeQueryTargetsReply>
#include <bb/system/InvokeQueryTargetsRequest>

#include <src/o2/o1twitter.h>
#include <src/twitterapi.hpp>
#include <src/ConversationApi.hpp>
#include <src/NotificationsApi.hpp>
#include <src/WebImageView/WebImageView.h>
#include <src/WebImageView/AuthorizedWebImageView.hpp>
#include <src/RetweetDialog.hpp>
#include <src/TweetApi.hpp>
#include <src/ProfileApi.hpp>
#include <src/DiscoverApi.hpp>
#include <src/SearchApi.hpp>
#include <src/DMApi.hpp>
#include <src/ListApi.hpp>
#include <QString>
#include <Qnetworkaccessmanager>
#include <QNetworkRequest>
#include <QSettings>
#include <openssl/crypto.h>
#include <bb/PpsObject>


using namespace bb::cascades;
using namespace bb::system;

QThread ApplicationUI::curlThread;
//OXTwitter * ApplicationUI::authenticator = new OXTwitter();
QNetworkAccessManager * ApplicationUI::mNetManager = new QNetworkAccessManager();
QNetworkDiskCache * ApplicationUI::mNetworkDiskCache = new QNetworkDiskCache();

ApplicationUI::ApplicationUI() :
        QObject()
        , m_backgroundUpdatesEnabled(true)
{
    qmlRegisterType<OXTwitter>("com.pipacs.o2", 1, 0, "O1Twitter");
    qmlRegisterType<TwitterApi>("com.pipacs.o2", 1, 0, "TwitterApi");
    qmlRegisterType<WebImageView>("org.labsquare", 1, 0, "WebImageView");
    qmlRegisterType<AuthorizedWebImageView>("org.labsquare", 1, 0, "AuthorizedWebImageView");
    qmlRegisterType<ConversationApi>("com.simodax", 1, 0, "ConversationApi");
    qmlRegisterType<NotificationsApi>("com.simodax", 1, 0, "NotificationsApi");
    qmlRegisterType<DiscoverApi>("com.simodax", 1, 0, "DiscoverApi");
    qmlRegisterType<SearchApi>("com.simodax", 1, 0, "SearchApi");
    qmlRegisterType<RetweetDialog>("com.simodax", 1, 0, "RetweetDialog");
    qmlRegisterType<TweetApi>("com.simodax", 1, 0, "TweetApi");
    qmlRegisterType<ProfileApi>("com.simodax", 1, 0, "ProfileApi");
    qmlRegisterType<DMApi>("com.simodax", 1, 0, "DMApi");
    qmlRegisterType<ListApi>("com.simodax", 1, 0, "ListApi");
    qRegisterMetaType<CURLcode>("CURLcode");
    qRegisterMetaType< QMap<QString, GroupDataModel*> >("QMap<QString COMMA GroupDataModel*>");
    qRegisterMetaType< QMap<QString, Conversation*> >("QMap<QString COMMA Conversation*>");
    qRegisterMetaType<Conversation*>("Conversation*");

    // Start web requestor thread
    curlThread.start();

    // Set visual style from settings (if there is any)
    QSettings settings("simodax","bird10");
        if(settings.value("theme", 0).toInt())
            Application::instance()->themeSupport()->setVisualStyle(settings.value("theme").toInt());

    // Create scene document from main.qml asset, the parent is set
    // to ensure the document gets destroyed properly at shut down.
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);
    qml->setContextProperty("app", this);

    // Create root object for the UI
    AbstractPane *root = qml->createRootObject<AbstractPane>();

    // Set created root object as the application scene
    Application::instance()->setScene(root);


    //OpenSSL verify version
    const char * p = SSLeay_version(SSLEAY_VERSION);
    QString osslVersion = QString::fromAscii(p);
    qDebug()<<osslVersion;

    if(!osslVersion.contains("1.0.2")){
        osslVersion.chop(12);
        SystemDialog* m_listdialog;
        m_listdialog = new SystemDialog("Ok");
        m_listdialog->setTitle("Warning");
        m_listdialog->setBody("An old OpenSSL library version has been detected. The app probably won't work.\n" \
                              "Please check that you are running it under OS 10.3.3\n" \
                              "Required version: OpenSSL 1.0.2g\n" \
                              "Your version: " + osslVersion);
        m_listdialog->exec();
    }

    // Initialize network cache
    mNetworkDiskCache->setCacheDirectory("/tmp/bird10");

    // Set cache in manager
//    mNetManager->setCache(mNetworkDiskCache);

    CurlEasy * request = new CurlEasy(NULL, &ApplicationUI::curlThread);
    request->set(CURLOPT_URL, QUrl("https://bird10app.netlify.com/version.txt"));
    request->set(CURLOPT_FAILONERROR, 1L);
    request->set(CURLOPT_FOLLOWLOCATION, 1L);   // Follow netlify 3xx redirects
    connect(request, SIGNAL(done(CURLcode)), this, SLOT(checkVersion()));
    //connect(request, SIGNAL(error(CURLcode)), this, SLOT(requestFailed(CURLcode)));

    //Since the object lives on a different thread its function calls need to be queued
    //This will trigger the perform method without making an unnecessary signal-slot connection
    QMetaObject::invokeMethod(request, "perform", Qt::QueuedConnection);

    // Hit
    if(!QFile::exists("./data/versioncheck_" + Application::applicationVersion())){
        CurlEasy * request = new CurlEasy(NULL, &ApplicationUI::curlThread);
        request->set(CURLOPT_URL, QUrl("http://odassi.altervista.org/bird10/version.php?v=" + Application::applicationVersion()));
        request->set(CURLOPT_FAILONERROR, 1L);
        request->set(CURLOPT_FOLLOWLOCATION, 1L);
        connect(request, SIGNAL(done(CURLcode)), request, SLOT(deleteLater()));

        //Since the object lives on a different thread its function calls need to be queued
        //This will trigger the perform method without making an unnecessary signal-slot connection
        QMetaObject::invokeMethod(request, "perform", Qt::QueuedConnection);

        QFile file("./data/versioncheck_" + Application::applicationVersion());
        file.open(QFile::WriteOnly);
        file.close();
    }



//    QtCUrl cUrl;
//    cUrl.setTextCodec("Windows-1251");
//
//    QUrl url("https://api.twitter.com/oauth/request_token");
//    //url.addQueryItem("id", "42");
//
//    QtCUrl::Options opt;
//    opt[CURLOPT_URL] = url;
//    opt[CURLOPT_POST] = true;
//    opt[CURLOPT_FOLLOWLOCATION] = true;
//    opt[CURLOPT_FAILONERROR] = false;
//    QStringList headers;

//    QString result = cUrl.exec(opt);
//
//    if (cUrl.lastError().isOk()) {
//        qDebug() << result;
//    }
//    else {
//        qDebug() << QString("Error: %1 Buffer: %2")
//            .arg(cUrl.lastError().text())
//            .arg(cUrl.errorBuffer());
//    }
}

void ApplicationUI::checkVersion(){
    CurlEasy* reply = qobject_cast<CurlEasy *>(sender());

    if(QString(reply->data()).split("\r\n")[0] != Application::applicationVersion()){
        SystemDialog* m_listdialog;
        m_listdialog = new SystemDialog("Open", "Cancel");
        m_listdialog->setTitle("Update available");
        m_listdialog->setBody("A newer version of Bird10 is available\n" \
                              "Please check the app thread on CrackBerry forums.\n" \
                              "Your version: " + Application::applicationVersion() + "\n" \
                              "New version: " + reply->data());

        connect(m_listdialog, SIGNAL(finished(bb::system::SystemUiResult::Type)), this, SLOT(onDialogFinished(bb::system::SystemUiResult::Type)));
        m_listdialog->exec();
    }
    reply->deleteLater();
}

void ApplicationUI::exportLogs()
{
    if(QFile::exists("./shared/documents/bird10log.txt"))
        QFile::remove("./shared/documents/bird10log.txt");

    QFile::copy("./logs/log", "./shared/documents/bird10log.txt");
}

void ApplicationUI::onDialogFinished(bb::system::SystemUiResult::Type result){
    SystemDialog *m_listdialog = qobject_cast<SystemDialog*>(sender());

    if (result == bb::system::SystemUiResult::ConfirmButtonSelection) {
        InvokeManager manager;
        InvokeRequest request;

        request.setTarget("sys.browser");
        request.setAction("bb.action.OPEN");
        request.setUri("https://forums.crackberry.com/app-announcements-f281/bird10-native-app-twitter-1178312/");
        m_reply = manager.invoke(request);
        m_reply->setParent(this);
        connect(m_reply, SIGNAL(finished()), this,  SLOT(onInvokeResult()));
    }

    m_listdialog->deleteLater();
}

void ApplicationUI::shareImage(const QUrl& url){
    QNetworkRequest request(url);
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
    QNetworkReply * reply = mNetManager->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(shareImageLoaded()));
}

void ApplicationUI::saveImage(const QUrl& url){
    QNetworkRequest request(url);
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
    QNetworkReply * reply = mNetManager->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(saveImageLoaded()));
}

void ApplicationUI::shareImageLoaded(){
    QNetworkReply * reply = qobject_cast<QNetworkReply*>(sender());

    if (reply->error() == QNetworkReply::NoError) {
            QByteArray imageData = reply->readAll();
            QString ext = reply->url().toString().contains("jpg") ? "jpg" : "png";
            QString fileName = reply->url().toString().split("/").last();

            QFile imageFile(QDir::tempPath()+"/" + fileName);
            if(imageFile.open(QIODevice::WriteOnly)){
                imageFile.write(imageData);
                imageFile.close();

                InvokeManager manager;
                InvokeRequest request;

                //request.setAction("bb.action.SHARE");
                //request.setTarget("sys.invokeTargetSelection");
                request.setMimeType("image/*");
                request.setUri("file:///"+QDir::tempPath()+"/"+fileName);
                //request.setFileTransferMode(FileTransferMode::Link);
                qDebug()<<"Uri: "<<"file:///"<<QDir::tempPath()<<fileName;

                m_reply = manager.invoke(request);
                m_reply->setParent(this);
                connect(m_reply, SIGNAL(finished()), this,  SLOT(onInvokeResult()));
            }
    }

    // Memory management
    reply->deleteLater();
}

void ApplicationUI::saveImageLoaded(){
    QNetworkReply * reply = qobject_cast<QNetworkReply*>(sender());

    if (reply->error() == QNetworkReply::NoError) {
            QByteArray imageData = reply->readAll();
            QString fileName = reply->url().toString().split("/").last();

            QFile imageFile("./shared/downloads/" + fileName);
            if(imageFile.open(QIODevice::WriteOnly)){
                imageFile.write(imageData);
                imageFile.close();
            }
    }

    // Memory management
    reply->deleteLater();
}

void ApplicationUI::openVideo(const QUrl& url, const QString& title){
    InvokeManager manager;
    InvokeRequest request;

    request.setTarget("sys.mediaplayer.previewer");
    request.setAction("bb.action.VIEW");
    request.setMimeType("video/mp4");
    request.setUri(url);
    qDebug()<<"Uri: "<<url;
    QVariantMap map;
    map.insert("contentTitle", title);
    QByteArray requestData = bb::PpsObject::encode(map, NULL);
    request.setData(requestData);

    m_reply = manager.invoke(request);
    m_reply->setParent(this);
    connect(m_reply, SIGNAL(finished()), this,  SLOT(onInvokeResult()));
}

void ApplicationUI::onInvokeResult(){   //helper function pasted stright from docs, saves a lot of headaches
    // Check for errors
    switch(m_reply->error()) {
        // Invocation could not find the target;
        // did we use the right target ID?
        case InvokeReplyError::NoTarget: {
            qDebug() << "invokeFinished(): Error: no target" << endl;
            break;
        }

        // There was a problem with the invocation request;
        // did we set all of the values correctly?
        case InvokeReplyError::BadRequest: {
            qDebug() << "invokeFinished(): Error: bad request" << endl;
            break;
        }

        // Something went completely
        // wrong inside the invocation request,
        // so find an alternate route
        case InvokeReplyError::Internal: {
            qDebug() << "invokeFinished(): Error: internal" << endl;
            break;
        }

        // Message received if the invocation request is successful
        default:
            qDebug() << "invokeFinished(): " << m_reply->error() << endl;
            break;
    }
}

void ApplicationUI::onManualExit()
{
    mNetworkDiskCache->clear();
    qDebug()<<"leftover cache size: "<<mNetworkDiskCache->cacheSize();

    Application* app = qobject_cast<Application*>(sender());
    if(app)
        app->quit();
    else
        qDebug()<<"What the fuck";

}

void ApplicationUI::startService()
{
    InvokeManager manager;
    InvokeRequest request;
    request.setTarget("com.simodax.Bird10HeadlessService");
    request.setAction("com.simodax.Bird10HeadlessService.RESET");
    m_reply = manager.invoke(request);
    connect(m_reply, SIGNAL(finished()), this,  SLOT(onInvokeResult()));
}

void ApplicationUI::stopService()
{
    InvokeManager manager;
    InvokeRequest request;
    request.setTarget("com.simodax.Bird10HeadlessService");
    request.setAction("com.simodax.Bird10HeadlessService.STOP");
    m_reply = manager.invoke(request);
    connect(m_reply, SIGNAL(finished()), this,  SLOT(onInvokeResult()));
}

void ApplicationUI::onQueryResponse()
{
    switch(_queryResults->error()) {
        case InvokeReplyError::BadRequest:
            // Resolve the error
            break;
        case InvokeReplyError::Internal:
            // Resolve the error
            break;
        default:
            // If the error is resolved, proceed
            QList<InvokeAction> targetsByAction = _queryResults->actions();
            for (int i = 0; i < targetsByAction.size(); i++) {
                // See the API documentation for the full set of action attributes
                QString actionName = targetsByAction[i].name();
                QList<InvokeTarget> targets = targetsByAction[i].targets();
                for (int j = 0; j < targets.size(); j++) {
                    // See the API documentation for the full set of target attributes
                    QString targetId = targets[j].name();
                    qDebug()<<targetId;
                }
            }
            break;
    }

    // Free the resources that were allocated for the query
    delete _queryResults;
}


