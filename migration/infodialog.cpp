/*
    Copyright (c) 2008 Volker Krause <vkrause@kde.org>

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

#include "infodialog.h"

#include <QDebug>
#include <QIcon>

#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QProgressBar>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>

enum {
    // The max value of the scrollbar. Don't change this without making the kmail
    // migrator use this. It still uses hardcoded "100".
    MAX_PROGRESS = 100
};

bool InfoDialog::mError = false;

InfoDialog::InfoDialog(bool closeWhenDone)
    : mMigratorCount(0)
    , mChange(false)
    , mCloseWhenDone(closeWhenDone)
    , mAutoScrollList(true)
{
    setAttribute(Qt::WA_DeleteOnClose);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    mList = new QListWidget(this);
    mList->setMinimumWidth(640);
    mainLayout->addWidget(mList);

    QHBoxLayout *statusLayout = new QHBoxLayout;
    mainLayout->addLayout(statusLayout);

    mStatusLabel = new QLabel(this);
    mStatusLabel->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
    statusLayout->addWidget(mStatusLabel);

    mProgressBar = new QProgressBar(this);
    mProgressBar->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    mProgressBar->setMinimumWidth(200);
    statusLayout->addWidget(mProgressBar);

    mButtonBox = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(mButtonBox, &QDialogButtonBox::accepted, this, &InfoDialog::accept);
    connect(mButtonBox, &QDialogButtonBox::rejected, this, &InfoDialog::reject);
    mButtonBox->button(QDialogButtonBox::Close)->setEnabled(false);
    mainLayout->addWidget(mButtonBox);
}

InfoDialog::~InfoDialog()
{
}

static KMigratorBase::MessageType convertType(MigratorBase::MessageType type)
{
    switch (type) {
    case MigratorBase::Success:
        return KMigratorBase::Success;
    case MigratorBase::Error:
        return KMigratorBase::Error;
    case MigratorBase::Skip:
        return KMigratorBase::Skip;
    case MigratorBase::Warning:
        return KMigratorBase::Warning;
    case MigratorBase::Info:
        return KMigratorBase::Info;
    }
    return KMigratorBase::Info;
}

void InfoDialog::message(MigratorBase::MessageType type, const QString &msg)
{
    message(convertType(type), msg);
}

void InfoDialog::message(KMigratorBase::MessageType type, const QString &msg)
{
    bool autoScroll = mAutoScrollList;

    QListWidgetItem *item = new QListWidgetItem(msg, mList);
    switch (type) {
    case KMigratorBase::Success:
        item->setIcon(QIcon::fromTheme(QStringLiteral("dialog-ok-apply")));
        mChange = true;
        qDebug() << msg;
        break;
    case KMigratorBase::Skip:
        item->setIcon(QIcon::fromTheme(QStringLiteral("dialog-ok")));
        qDebug() << msg;
        break;
    case KMigratorBase::Info:
        item->setIcon(QIcon::fromTheme(QStringLiteral("dialog-information")));
        qDebug() << msg;
        break;
    case KMigratorBase::Warning:
        item->setIcon(QIcon::fromTheme(QStringLiteral("dialog-warning")));
        qDebug() << msg;
        break;
    case KMigratorBase::Error:
    {
        item->setIcon(QIcon::fromTheme(QStringLiteral("dialog-error")));
        QFont currentFont = font();
        currentFont.setBold(true);
        item->setFont(currentFont);
        mError = true;
        qCritical() << msg;
        break;
    }
    default:
        qCritical() << "WTF?";
    }

    mAutoScrollList = autoScroll;

    if (autoScroll) {
        mList->scrollToItem(item);
    }
}

void InfoDialog::migratorAdded()
{
    ++mMigratorCount;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
}

void InfoDialog::migratorDone()
{
    QApplication::restoreOverrideCursor();

    --mMigratorCount;
    if (mMigratorCount == 0) {
        mButtonBox->button(QDialogButtonBox::Close)->setEnabled(true);
        status(QString());
        if (mCloseWhenDone && !hasError() && !hasChange()) {
            accept();
        }
    }
}

void InfoDialog::status(const QString &msg)
{
    mStatusLabel->setText(msg);
    if (msg.isEmpty()) {
        progress(0, MAX_PROGRESS, MAX_PROGRESS);
        mProgressBar->setFormat(QString());
    }
}

void InfoDialog::progress(int value)
{
    mProgressBar->setFormat(QStringLiteral("%p%"));
    mProgressBar->setValue(value);
}

void InfoDialog::progress(int min, int max, int value)
{
    mProgressBar->setFormat(QStringLiteral("%p%"));
    mProgressBar->setRange(min, max);
    mProgressBar->setValue(value);
}

void InfoDialog::scrollBarMoved(int value)
{
    mAutoScrollList = (value == mList->verticalScrollBar()->maximum());
}
