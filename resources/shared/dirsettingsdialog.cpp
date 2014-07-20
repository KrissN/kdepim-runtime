/*
    Copyright (c) 2009 Tobias Koenig <tokoe@kde.org>

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

#include "dirsettingsdialog.h"

#include "settings.h"

#include <KConfigDialogManager>
#include <KWindowSystem>
#include <KLocalizedString>
#include <KUrl>

#include <QTimer>
#include <QDialogButtonBox>
#include <KConfigGroup>
#include <QPushButton>
#include <QVBoxLayout>

using namespace Akonadi;

SettingsDialog::SettingsDialog( WId windowId )
  : QDialog()
{
  QWidget *mainWidget = new QWidget(this);
  QVBoxLayout *mainLayout = new QVBoxLayout;
  setLayout(mainLayout);
  mainLayout->addWidget(mainWidget);
  ui.setupUi(mainWidget);
  ui.kcfg_Path->setMode( KFile::LocalOnly | KFile::Directory );
  QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
  mOkButton = buttonBox->button(QDialogButtonBox::Ok);
  mOkButton->setDefault(true);
  mOkButton->setShortcut(Qt::CTRL | Qt::Key_Return);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  mainLayout->addWidget(buttonBox);

  if ( windowId )
    KWindowSystem::setMainWindow( this, windowId );

  connect(mOkButton, SIGNAL(clicked()), SLOT(save()) );

  connect( ui.kcfg_Path, SIGNAL(textChanged(QString)), SLOT(validate()) );
  connect( ui.kcfg_ReadOnly, SIGNAL(toggled(bool)), SLOT(validate()) );

  QTimer::singleShot( 0, this, SLOT(validate()) );

  ui.kcfg_Path->setUrl( KUrl( Settings::self()->path() ) );
  ui.kcfg_AutosaveInterval->setSuffix(ki18np(" minute", " minutes"));
  mManager = new KConfigDialogManager( this, Settings::self() );
  mManager->updateWidgets();
}

void SettingsDialog::save()
{
  mManager->updateSettings();
  Settings::self()->setPath( ui.kcfg_Path->url().toLocalFile() );
  Settings::self()->save();
}

void SettingsDialog::validate()
{
  const KUrl currentUrl = ui.kcfg_Path->url();
  if ( currentUrl.isEmpty() ) {
    mOkButton->setEnabled(false);
    return;
  }

  const QFileInfo file( currentUrl.toLocalFile() );
  if ( file.exists() && !file.isWritable() ) {
    ui.kcfg_ReadOnly->setEnabled( false );
    ui.kcfg_ReadOnly->setChecked( true );
  } else {
    ui.kcfg_ReadOnly->setEnabled( true );
  }
  mOkButton->setEnabled(true);
}
