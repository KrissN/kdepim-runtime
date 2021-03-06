/*
 * Copyright (C) 2012  Christian Mollekopf <mollekopf@kolabsys.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef KOLABMIMEUTILS_H
#define KOLABMIMEUTILS_H

#include "kolab_export.h"

#include <kmime/kmime_message.h>

namespace Kolab {
class Attachment;
namespace Mime {
KMime::Content *findContentByName(const KMime::Message::Ptr &data, const QString &name, QByteArray &type);
KMime::Content *findContentByType(const KMime::Message::Ptr &data, const QByteArray &type);
QList<QByteArray> getContentMimeTypeList(const KMime::Message::Ptr &data);

/**
* Get Attachments from a Mime message
*
* Set the attachments listed in @param attachments on @param incidence from @param mimeData
*/
Kolab::Attachment getAttachment(const std::string &id, const KMime::Message::Ptr &mimeData);
Kolab::Attachment getAttachmentByName(const QString &name, const KMime::Message::Ptr &mimeData);

KMime::Message::Ptr createMessage(const QByteArray &mimetype, const QByteArray &xKolabType, const QByteArray &xml, bool v3, const QByteArray &productId, const QByteArray &fromEmail,
                                  const QString &fromName, const QString &subject);
KMime::Message::Ptr createMessage(const std::string &mimetype, const std::string &xKolabType, const std::string &xml, bool v3, const std::string &productId, const std::string &fromEmail,
                                  const std::string &fromName, const std::string &subject);

///Generic serializing functions
KMime::Message::Ptr createMessage(const QString &subject, const QString &mimetype, const QString &xKolabType, const QByteArray &xml, bool v3, const QString &prodid);
KMime::Message::Ptr createMessage(const QByteArray &mimeType, bool v3, const QByteArray &prodid);

KMime::Content *createExplanationPart();
KMime::Content *createMainPart(const QByteArray &mimeType, const QByteArray &decodedContent);
KMime::Content *createAttachmentPart(const QByteArray &cid, const QByteArray &mimeType, const QString &fileName, const QByteArray &decodedContent);
QString fromCid(const QString &cid);
}
} //Namespace

#endif
