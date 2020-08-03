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

#ifndef AUTHORIZEDWEBIMAGEVIEW_HPP_
#define AUTHORIZEDWEBIMAGEVIEW_HPP_

#include "WebImageView.h"
#include <src/o2/oxtwitter.h>

class AuthorizedWebImageView: public WebImageView
{
    Q_OBJECT
    Q_PROPERTY (QUrl url READ url WRITE setUrl NOTIFY urlChanged)

public:

    /// OAuth authenticator
    Q_PROPERTY(OXTwitter *authenticator READ authenticator WRITE setAuthenticator)
    inline OXTwitter *authenticator() const { return authenticator_; };
    void setAuthenticator(OXTwitter *v);

    AuthorizedWebImageView();
    virtual ~AuthorizedWebImageView();

    void setUrl(const QUrl& url);
    const QUrl& url() const;

signals:
    void urlChanged();

private slots:
    void onError();

private:
    OXTwitter* authenticator_;
};

#endif /* AUTHORIZEDWEBIMAGEVIEW_HPP_ */
