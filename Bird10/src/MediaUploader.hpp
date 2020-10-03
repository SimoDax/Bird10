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

#ifndef MEDIAUPLOADER_HPP_
#define MEDIAUPLOADER_HPP_

#include <QObject>
#include <QStringList>
#include <QFile>
#include "TwitterApiBase.hpp"

class MediaUploader : public TwitterApiBase
{
    Q_OBJECT

    enum MediaType{
        IMAGE,
        VIDEO,
        GIF
    };

public:
    MediaUploader(O1Twitter* authenticator, QObject* parent = 0);

    void uploadVideo(const QString& path);
    void uploadPictures(const QList<QString> paths);

signals:
    void uploadComplete(QString media_ids);

protected slots:
    void append();
    void finalize();
    void onFinalized();

protected:
    MediaType m_mediaType;
    QString m_id;
    QFile m_currentFile;
    int m_index;
    QStringList m_mediaIds;
};

#endif /* MEDIAUPLOADER_HPP_ */
