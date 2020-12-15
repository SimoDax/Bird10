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

#ifndef WEBIMAGEVIEW_H_
#define WEBIMAGEVIEW_H_

#include <bb/cascades/ImageView>
#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include <QUrl>
#include <QByteArray>
using namespace bb::cascades;

namespace bb {
namespace cascades {
class ImageView;
}
}

class WebImageView: public bb::cascades::ImageView {
	Q_OBJECT
	Q_PROPERTY (QUrl url READ url WRITE setUrl NOTIFY urlChanged)

	Q_PROPERTY (bool loading READ loading NOTIFY loadingChanged)
	bool loading(){ return mUrl != mDesiredUrl;};

public:
	WebImageView();
	const QUrl& url() const;

	public Q_SLOTS:
	void setUrl(const QUrl& url);
    void clearCache();
    Q_INVOKABLE QByteArray data();

protected Q_SLOTS:
	void imageLoaded();
	void sslError(const QList<QSslError>& errors);

signals:
	void urlChanged();
	void loadingChanged();
    void loadingFinished();

protected:
	static QNetworkAccessManager * mNetManager;
	QUrl mUrl, mDesiredUrl;
	QByteArray imageData;

	//bool isARedirectedUrl(QNetworkReply *reply);
	//void setURLToRedirectedUrl(QNetworkReply *reply);
};

#endif /* WEBIMAGEVIEW_H_ */
