/*
   Copyright (c) 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
   Author: Kevin Ottens <kevin@kdab.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or ( at your option ) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#include "imaptestbase.h"

#include "retrievecollectionmetadatatask.h"

#include <akonadi/collectionquotaattribute.h>
#include <akonadi/attributefactory.h>
#include "collectionannotationsattribute.h"
#include "imapaclattribute.h"
#include "imapquotaattribute.h"
#include "noselectattribute.h"
#include "timestampattribute.h"
#include <noinferiorsattribute.h>

typedef QMap<QByteArray, QByteArray> QBYTEARRAYMAP;

Q_DECLARE_METATYPE( Akonadi::Collection::Rights )
Q_DECLARE_METATYPE( QBYTEARRAYMAP )

class TestRetrieveCollectionMetadataTask : public ImapTestBase
{
  Q_OBJECT

private slots:

  void initTestCase()
  {
    Akonadi::AttributeFactory::registerAttribute<Akonadi::ImapAclAttribute>();
    Akonadi::AttributeFactory::registerAttribute<NoSelectAttribute>();
  }

  void shouldCollectionRetrieveMetadata_data()
  {
    QTest::addColumn<Akonadi::Collection>( "collection" );
    QTest::addColumn<QStringList>( "capabilities" );
    QTest::addColumn< QList<QByteArray> >( "scenario" );
    QTest::addColumn<QStringList>( "callNames" );
    QTest::addColumn<Akonadi::Collection::Rights>( "expectedRights" );
    QTest::addColumn<QBYTEARRAYMAP>( "expectedAnnotations" );

    Akonadi::Collection collection;
    QStringList capabilities;
    QList<QByteArray> scenario;
    QStringList callNames;
    QMap<QByteArray, QByteArray> expectedAnnotations;

    collection = createCollectionChain( QLatin1String("/INBOX/Foo") );
    collection.setRights( 0 );
    collection.addAttribute( new TimestampAttribute( QDateTime::currentDateTime().toTime_t() ) );

    capabilities.clear();
    capabilities << "ANNOTATEMORE" << "ACL" << "QUOTA";

    scenario.clear();
    scenario << defaultPoolConnectionScenario()
             << "C: A000003 GETANNOTATION \"INBOX/Foo\" \"*\" \"value.shared\""
             << "S: * ANNOTATION INBOX/Foo /vendor/kolab/folder-test ( value.shared true )"
             << "S: A000003 OK annotations retrieved"
             << "C: A000004 GETACL \"INBOX/Foo\""
             << "S: * ACL INBOX/Foo foo@kde.org lrswipcda"
             << "S: A000004 OK acl retrieved"
             << "C: A000005 MYRIGHTS \"INBOX/Foo\""
             << "S: * MYRIGHTS \"INBOX/Foo\" lrswipkxtecda"
             << "S: A000005 OK rights retrieved"
             << "C: A000006 GETQUOTAROOT \"INBOX/Foo\""
             << "S: * QUOTAROOT INBOX/Foo user/foo"
             << "S: * QUOTA user/foo ( )"
             << "S: A000006 OK quota retrieved";

    callNames.clear();
    callNames << "applyCollectionChanges" << "taskDone";

    expectedAnnotations.clear();
    expectedAnnotations.insert( "/shared/vendor/kolab/folder-test", "true" );

    Akonadi::Collection::Rights rights = Akonadi::Collection::AllRights;
    QTest::newRow( "first listing, connected IMAP" ) << collection << capabilities << scenario
                                                     << callNames << rights << expectedAnnotations;

    //
    // Test that if the parent collection doesn't allow renaming in its ACL, the child mailbox
    // can't be renamed, i.e. doesn't have the CanChangeCollection flag.
    //
    Akonadi::Collection parentCollection = createCollectionChain( QLatin1String("/INBOX") );
    QMap<QByteArray, KIMAP::Acl::Rights> rightsMap;
    rightsMap.insert( "Hans", KIMAP::Acl::Lookup | KIMAP::Acl::Read | KIMAP::Acl::KeepSeen |
                              KIMAP::Acl::Write | KIMAP::Acl::Insert | KIMAP::Acl::Post |
                              KIMAP::Acl::Delete );
    Akonadi::ImapAclAttribute *aclAttribute = new Akonadi::ImapAclAttribute();
    aclAttribute->setRights( rightsMap );
    parentCollection.addAttribute( aclAttribute );
    collection.setParentCollection( parentCollection );
    collection.removeAttribute<TimestampAttribute>();
    rights = Akonadi::Collection::AllRights;
    rights &= ~Akonadi::Collection::CanChangeCollection;
    QTest::newRow( "parent without create rights" ) << collection << capabilities << scenario
                                                    << callNames << rights << expectedAnnotations;

    //
    // Test that if the parent collection is a noselect folder, the child mailbox will not have
    // rename (CanChangeCollection) permission.
    //
    parentCollection = createCollectionChain( QLatin1String("/INBOX") );
    NoSelectAttribute *noSelectAttribute = new NoSelectAttribute();
    parentCollection.addAttribute( noSelectAttribute );
    collection.setParentCollection( parentCollection );
    QTest::newRow( "parent wit noselect" ) << collection << capabilities << scenario
                                           << callNames << rights << expectedAnnotations;
    parentCollection.removeAttribute<NoSelectAttribute>();

    //
    // Test that the rights are properly set on the resulting collection if the mailbox doesn't
    // have full rights.
    //
    collection.setParentCollection( createCollectionChain( QLatin1String("/INBOX") ) );
    scenario.clear();
    scenario << defaultPoolConnectionScenario()
             << "C: A000003 GETANNOTATION \"INBOX/Foo\" \"*\" \"value.shared\""
             << "S: * ANNOTATION INBOX/Foo /vendor/kolab/folder-test ( value.shared true )"
             << "S: A000003 OK annotations retrieved"
             << "C: A000004 GETACL \"INBOX/Foo\""
             << "S: * ACL INBOX/Foo foo@kde.org wi"
             << "S: A000004 OK acl retrieved"
             << "C: A000005 MYRIGHTS \"INBOX/Foo\""
             << "S: * MYRIGHTS \"INBOX/Foo\" wi"
             << "S: A000005 OK rights retrieved"
             << "C: A000006 GETQUOTAROOT \"INBOX/Foo\""
             << "S: * QUOTAROOT INBOX/Foo user/foo"
             << "S: * QUOTA user/foo ( )"
             << "S: A000006 OK quota retrieved";
    rights = Akonadi::Collection::CanCreateItem | Akonadi::Collection::CanChangeItem |
             Akonadi::Collection::CanChangeCollection;
    QTest::newRow( "only some rights" ) << collection << capabilities << scenario
                                        << callNames << rights << expectedAnnotations;

    //
    // Test that a warning is issued if the insert rights of a folder have been revoked on the server.
    //
    collection = createCollectionChain( QLatin1String("/INBOX/Foo") );
    collection.addAttribute( new TimestampAttribute( QDateTime::currentDateTime().toTime_t() ) );
    collection.setParentCollection( parentCollection );
    collection.setRights( Akonadi::Collection::CanCreateItem );

    capabilities.clear();
    capabilities << "ANNOTATEMORE" << "ACL" << "QUOTA";

    scenario.clear();
    scenario << defaultPoolConnectionScenario()
             << "C: A000003 GETANNOTATION \"INBOX/Foo\" \"*\" \"value.shared\""
             << "S: * ANNOTATION INBOX/Foo /vendor/kolab/folder-test ( value.shared true )"
             << "S: A000003 OK annotations retrieved"
             << "C: A000004 GETACL \"INBOX/Foo\""
             << "S: * ACL INBOX/Foo foo@kde.org wi"
             << "S: A000004 OK acl retrieved"
             << "C: A000005 MYRIGHTS \"INBOX/Foo\""
             << "S: * MYRIGHTS \"INBOX/Foo\" w"
             << "S: A000005 OK rights retrieved"
             << "C: A000006 GETQUOTAROOT \"INBOX/Foo\""
             << "S: * QUOTAROOT INBOX/Foo user/foo"
             << "S: * QUOTA user/foo ( )"
             << "S: A000006 OK quota retrieved";

    callNames.clear();
    callNames << "showInformationDialog";
    callNames << "applyCollectionChanges";
    callNames << "taskDone";

    rights = Akonadi::Collection::CanChangeItem | Akonadi::Collection::CanChangeCollection;
    QTest::newRow( "revoked rights" ) << collection << capabilities << scenario
                                      << callNames << rights << expectedAnnotations;

    //
    // Test that NoInferiors overrides acl rigths and disallows creating new mailboxes
    //
    collection.setParentCollection( createCollectionChain( QString() ) );
    collection.setRemoteId( "/INBOX" );
    collection.setRights( Akonadi::Collection::AllRights );
    collection.addAttribute( new NoInferiorsAttribute( true ) );
    collection.removeAttribute<TimestampAttribute>();
    scenario.clear();
    scenario << defaultPoolConnectionScenario()
             << "C: A000003 GETANNOTATION \"INBOX\" \"*\" \"value.shared\""
             << "S: * ANNOTATION INBOX /vendor/kolab/folder-test ( value.shared true )"
             << "S: A000003 OK annotations retrieved"
             << "C: A000004 GETACL \"INBOX\""
             << "S: * ACL INBOX foo@kde.org wik"
             << "S: A000004 OK acl retrieved"
             << "C: A000005 MYRIGHTS \"INBOX\""
             << "S: * MYRIGHTS \"INBOX\" wk"
             << "S: A000005 OK rights retrieved"
             << "C: A000006 GETQUOTAROOT \"INBOX\""
             << "S: * QUOTAROOT INBOX user"
             << "S: * QUOTA user ( )"
             << "S: A000006 OK quota retrieved";

    callNames.clear();
    callNames << "applyCollectionChanges";
    callNames << "taskDone";

    rights = Akonadi::Collection::CanChangeItem | Akonadi::Collection::CanChangeCollection;

    QTest::newRow( "noinferiors" ) << collection << capabilities << scenario
                                      << callNames << rights << expectedAnnotations;

    collection = createCollectionChain( QLatin1String("/INBOX/Foo") );
    collection.setRights( 0 );
    collection.removeAttribute<TimestampAttribute>();

    capabilities.clear();
    capabilities << "METADATA" << "ACL" << "QUOTA";

    expectedAnnotations.clear();
    expectedAnnotations.insert( "/shared/vendor/kolab/folder-test", "true" );
    expectedAnnotations.insert( "/shared/vendor/kolab/folder-test2", "" );

    scenario.clear();
    scenario << defaultPoolConnectionScenario()
             << "C: A000003 GETMETADATA \"INBOX/Foo\" (DEPTH infinity) (/shared)"
             << "S: * METADATA \"INBOX/Foo\" (/shared/vendor/kolab/folder-test \"true\")"
             << "S: * METADATA \"INBOX/Foo\" (/shared/vendor/kolab/folder-test2 \"NIL\")"
             << "S: * METADATA \"INBOX/Foo\" (/shared/vendor/cmu/cyrus-imapd/lastupdate \"true\")"
             << "S: A000003 OK GETMETADATA complete"
             << "C: A000004 GETACL \"INBOX/Foo\""
             << "S: * ACL INBOX/Foo foo@kde.org lrswipcda"
             << "S: A000004 OK acl retrieved"
             << "C: A000005 MYRIGHTS \"INBOX/Foo\""
             << "S: * MYRIGHTS \"INBOX/Foo\" lrswipkxtecda"
             << "S: A000005 OK rights retrieved"
             << "C: A000006 GETQUOTAROOT \"INBOX/Foo\""
             << "S: * QUOTAROOT INBOX/Foo user/Foo"
             << "S: * QUOTA user/Foo ( )"
             << "S: A000006 OK quota retrieved";

    callNames.clear();
    callNames << "applyCollectionChanges" << "taskDone";

    rights = Akonadi::Collection::AllRights;
    QTest::newRow( "METADATA" ) << collection << capabilities << scenario
                                    << callNames << rights << expectedAnnotations;
  }

  void shouldCollectionRetrieveMetadata()
  {
    QFETCH( Akonadi::Collection, collection );
    QFETCH( QStringList, capabilities );
    QFETCH( QList<QByteArray>, scenario );
    QFETCH( QStringList, callNames );
    QFETCH( Akonadi::Collection::Rights, expectedRights );
    QFETCH( QBYTEARRAYMAP, expectedAnnotations );

    FakeServer server;
    server.setScenario( scenario );
    server.startAndWait();

    SessionPool pool( 1 );

    pool.setPasswordRequester( createDefaultRequester() );
    QVERIFY( pool.connect( createDefaultAccount() ) );
    QVERIFY( waitForSignal( &pool, SIGNAL(connectDone(int,QString)) ) );

    DummyResourceState::Ptr state = DummyResourceState::Ptr( new DummyResourceState );
    state->setCollection( collection );
    state->setServerCapabilities( capabilities );
    state->setUserName( "Hans" );
    RetrieveCollectionMetadataTask *task = new RetrieveCollectionMetadataTask( state );

    task->start( &pool );
    QTest::qWait( 100 );

    QCOMPARE( state->calls().count(), callNames.size() );
    for ( int i = 0; i < callNames.size(); i++ ) {
      QString command = QString::fromUtf8(state->calls().at( i ).first);
      QVariant parameter = state->calls().at( i ).second;

      if ( command == "cancelTask" && callNames[i] != "cancelTask" ) {
        kDebug() << "Got a cancel:" << parameter.toString();
      }

      QCOMPARE( command, callNames[i] );

      if ( command == "cancelTask" ) {
        QVERIFY( !parameter.toString().isEmpty() );
      }

      if ( command == "applyCollectionChanges" ) {
        Akonadi::Collection collection = parameter.value<Akonadi::Collection>();
        QCOMPARE( collection.rights(), expectedRights );
        QVERIFY( collection.hasAttribute<TimestampAttribute>() );

        const qint64 timestamp = collection.attribute<TimestampAttribute>()->timestamp();
        const qint64 currentTimestamp = QDateTime::currentDateTime().toTime_t();
        QVERIFY( qAbs( currentTimestamp - timestamp ) < 5 );

        if ( !expectedAnnotations.isEmpty() ) {
          QVERIFY( collection.hasAttribute<Akonadi::CollectionAnnotationsAttribute>() );
          QCOMPARE( collection.attribute<Akonadi::CollectionAnnotationsAttribute>()->annotations(), expectedAnnotations );
        }
      }
    }

    QVERIFY( server.isAllScenarioDone() );

    server.quit();
  }
};

QTEST_KDEMAIN_CORE( TestRetrieveCollectionMetadataTask )

#include "testretrievecollectionmetadatatask.moc"
