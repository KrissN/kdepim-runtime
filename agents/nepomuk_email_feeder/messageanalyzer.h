/*
    Copyright (c) 2006, 2009 Volker Krause <vkrause@kde.org>
    Copyright (c) 2008 Sebastian Trueg <trueg@kde.org>

    This library is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This library is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to the
    Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301, USA.
*/

#ifndef MESSAGEANALYZER_H
#define MESSAGEANALYZER_H

#include <contact.h>
#include <email.h>

#include <akonadi/item.h>

#include <kmime/kmime_headers.h>
#include <kmime/kmime_message.h>

#include <KDE/KUrl>
#include <QtCore/QObject>

class NepomukFeederAgentBase;

/**
  Does the actual analysis of the email, split out from the feeder agent due to possibly asynchronous
  operations in the OTP, so we need to isolate state in case multiple items are processed at the same time.
  Also gives us the possibility to parallelizer this later on.
*/
class MessageAnalyzer : public QObject
{
  Q_OBJECT
  public:
    MessageAnalyzer( const Akonadi::Item &item, const QUrl &graphUri, NepomukFeederAgentBase* parent = 0 );

    inline QUrl graphUri() const { return m_graphUri; }

  private:
    QList<NepomukFast::Contact> extractContactsFromMailboxes( const KMime::Types::Mailbox::List& mbs, const QUrl&graphUri );
    void processHeaders( const KMime::Message::Ptr &msg );
    void processPart( KMime::Content *content );

  private:
    NepomukFeederAgentBase *m_parent;
    Akonadi::Item m_item;
    NepomukFast::Email m_email;
    QUrl m_graphUri;
};

#endif
