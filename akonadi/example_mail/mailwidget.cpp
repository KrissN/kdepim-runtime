/*
    This file is part of the Akonadi Mail example.

    Copyright (c) 2009 Stephen Kelly <steveire@gmail.com>

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

#include "mailwidget.h"

#include <QHBoxLayout>
#include <QSplitter>
#include <QTextBrowser>

#include <akonadi/control.h>
#include <akonadi/entitydisplayattribute.h>
#include <akonadi/entitytreemodel.h>
#include <akonadi/entityfilterproxymodel.h>
#include "entitytreeview.h"
#include <akonadi/item.h>
#include <akonadi/itemfetchscope.h>
#include <akonadi/monitor.h>
#include <akonadi/session.h>

#include "mailmodel.h"

#include "contactsmodel.h"

#include <kdebug.h>
#include <KStandardDirs>
#include "modeltest.h"
#include <QTimer>

#include <grantlee/template.h>
#include <grantlee/context.h>
#include "akonaditemplateloader.h"

#include <kmime/kmime_message.h>

#include <boost/shared_ptr.hpp>

#include <KDebug>
#include "akonaditextdocument.h"
#include <kselectionproxymodel.h>

typedef boost::shared_ptr<KMime::Message> MessagePtr;


using namespace Akonadi;

MailWidget::MailWidget( QWidget * parent, Qt::WindowFlags f )
    : QWidget( parent, f )
{

  QSplitter *splitter = new QSplitter( this );
  QHBoxLayout *layout = new QHBoxLayout( this );


  ItemFetchScope templateScope;
  templateScope.fetchFullPayload( true );

  Monitor *templateMonitor = new Monitor( this );
  templateMonitor->fetchCollection( true );
  templateMonitor->setItemFetchScope( templateScope );
  templateMonitor->setCollectionMonitored( Collection::root() );
  templateMonitor->setMimeTypeMonitored( "text/x-vnd.grantlee-template" );

  Session *templateSession = new Session( QByteArray( "AkonadiTemplateLoader-" ) + QByteArray::number( qrand() ), this );

  EntityTreeModel *m_templateModel = new EntityTreeModel( templateSession, templateMonitor, this );

  AkonadiTemplateLoader *akoLoader = new AkonadiTemplateLoader(templateMonitor, this);

  connect( templateMonitor, SIGNAL( itemChanged(const Akonadi::Item &, const QSet< QByteArray > &) ),
           SLOT(someSlot( const Akonadi::Item &, const QSet< QByteArray > & ) ) );

  connect( templateMonitor, SIGNAL( itemChanged() ), SLOT( templateChanged() ) );

  Grantlee::Engine *engine = Grantlee::Engine::instance();
  KStandardDirs KStd;
  engine->setPluginDirs(KStd.findDirs("lib", "grantlee"));

  engine->addTemplateLoader(akoLoader);

  treeview = new EntityTreeView( splitter );

  if ( !Akonadi::Control::start() ) {
    kFatal() << "Unable to start Akonadi server, exit application";
    return;
  }

  // TODO: Tell akonadi to load a local mail resource if it's not already loaded.
  // This can't be done synchronously, but the monitor will notify the model that it has new collections
  // when the job is done.

  // Check  Akonadi::AgentManager::self()->instances() if it contains one, if it doesn't create one and report errors
  // if necessary.

  Collection rootCollection = Collection::root();

  ItemFetchScope scope;
  scope.fetchFullPayload( true ); // Need to have full item when adding it to the internal data structure
  scope.fetchAttribute< EntityDisplayAttribute >();

  Monitor *monitor = new Monitor( this );
  monitor->fetchCollection( true );
  monitor->setItemFetchScope( scope );
  // Only monitoring the root collection works.
  monitor->setCollectionMonitored( Collection::root() );
  monitor->setMimeTypeMonitored( "message/rfc822" );
//   monitor->setCollectionMonitored( rootCollection );
//   monitor->fetchCollectionStatistics( false );

  Session *session = new Session( QByteArray( "MailApplication-" ) + QByteArray::number( qrand() ), this );

  etm = new MailModel( session, monitor, this);

  // TODO: This stuff should probably be in the mailmodel constructor.
  etm->setItemPopulationStrategy(EntityTreeModel::LazyPopulation);

  collectionTree = new EntityFilterProxyModel(this);

  collectionTree->setSourceModel(etm);

  // Include only collections in this proxy model.
  collectionTree->addMimeTypeInclusionFilter( Collection::mimeType() );
  collectionTree->setHeaderSet(EntityTreeModel::CollectionTreeHeaders);

  treeview->setModel(collectionTree);
  treeview->setColumnHidden(2, true);
  QSplitter *hSplitter = new QSplitter(Qt::Vertical, splitter);

  KSelectionProxyModel *selectionProxy = new KSelectionProxyModel(treeview->selectionModel(), this);
  selectionProxy->setSourceModel( etm );
  selectionProxy->setFilterBehavior( KSelectionProxyModel::OnlySelectedChildren );

  itemList = new EntityFilterProxyModel(this);
  itemList->setSourceModel(selectionProxy);

  // Exclude collections from the list view.
  itemList->addMimeTypeExclusionFilter( Collection::mimeType() );
  itemList->setHeaderSet(EntityTreeModel::ItemListHeaders);

  listView = new EntityTreeView(splitter);
  listView->setModel(itemList);
  hSplitter->addWidget(listView);

  layout->addWidget( splitter );

  browser = new QTextBrowser( splitter );
  Akonadi::TextDocument *td = new Akonadi::TextDocument(akoLoader, this);
  browser->setDocument(td);
  hSplitter->addWidget(browser);

  connect( treeview->selectionModel(),
      SIGNAL( selectionChanged( const QItemSelection &, const QItemSelection & ) ),
      SLOT( treeSelectionChanged ( const QItemSelection &, const QItemSelection & ) ) );

  connect( listView->selectionModel(),
      SIGNAL( selectionChanged( const QItemSelection &, const QItemSelection & ) ),
      SLOT( listSelectionChanged ( const QItemSelection &, const QItemSelection & ) ) );

  connect( etm, SIGNAL( dataChanged( const QModelIndex &, const QModelIndex & ) ),
      SLOT( modelDataChanged( const QModelIndex &, const QModelIndex & ) ) );

}

MailWidget::~MailWidget()
{

}

void MailWidget::listSelectionChanged( const QItemSelection & selected, const QItemSelection &deselected )
{
//   if ( selected.indexes().size() == 1 )
//   {
    QModelIndex idx = selected.indexes().at( 0 );
    renderMail(idx);
//   }
}

void MailWidget::someSlot(const Akonadi::Item &item, const QSet< QByteArray > &partIdentifiers)
{
  kDebug() << item.remoteId();

  QModelIndex idx = listView->selectionModel()->selection().indexes().at(0);

  if (!idx.isValid())
    return;
  renderMail(idx);
}

void MailWidget::templateChanged()
{
  qDebug() << "t c";
  QModelIndex idx = listView->selectionModel()->selection().indexes().at(0);
  renderMail(idx);
}

void MailWidget::renderMail(const QModelIndex &idx)
{
  Item i = itemList->data( idx, EntityTreeModel::ItemRole ).value< Item >();
  if ( i.isValid() )
  {
    if (!i.hasPayload<MessagePtr>())
    {
      kWarning() << "not a MessagePtr" << i.id() << i.remoteId() << i.mimeType();
      return;
    }
    const MessagePtr mail = i.payload<MessagePtr>();
    KMime::Content *content = mail->mainBodyPart();
    if (!content)
      return;
    QByteArray ba = i.payloadData();

    Grantlee::Engine *engine = Grantlee::Engine::instance();
    Grantlee::Template *t = engine->loadByName("template.html", this);
    if (!t)
      return;
    QVariantHash h;
    h.insert( "subject", mail->subject()->asUnicodeString() );
    h.insert( "date", mail->date()->dateTime().dateTime() );
    h.insert( "to", mail->to()->asUnicodeString() );
    h.insert( "from", mail->from()->asUnicodeString() );

    QString messageHtml = content->decodedContent();
    messageHtml.replace("\n", "<br />");
    h.insert( "messageContent", messageHtml );

    Grantlee::Context c(h);
    QString renderedContent = t->render(&c);
    browser->setHtml( renderedContent );
  }
}

void MailWidget::treeSelectionChanged ( const QItemSelection & selected, const QItemSelection & deselected )
{
//   if ( selected.indexes().size() == 1 )
//   {
    QModelIndex idx = selected.indexes().at(0);

    QModelIndex etmIndex = collectionTree->mapToSource( idx );

    itemList->setRootIndex(etmIndex);
    QModelIndex filteredListIndex = itemList->mapFromSource(etmIndex);

    listView->setRootIndex(filteredListIndex);
//   }
}

void MailWidget::modelDataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight)
{
}

#include "mailwidget.moc"
