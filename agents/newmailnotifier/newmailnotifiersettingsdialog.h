/*
    Copyright (c) 2013-2018 Laurent Montel <montel@kde.org>

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

#ifndef NEWMAILNOTIFIERSETTINGSDIALOG_H
#define NEWMAILNOTIFIERSETTINGSDIALOG_H

#include <QDialog>
#include <Collection>

class KNotifyConfigWidget;
class QCheckBox;
class QLineEdit;
class NewMailNotifierSelectCollectionWidget;
class NewMailNotifierSettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit NewMailNotifierSettingsDialog(QWidget *parent = nullptr);
    ~NewMailNotifierSettingsDialog();

private:
    void slotOkClicked();
    void slotHelpLinkClicked(const QString &);
    void writeConfig();
    void readConfig();
    QCheckBox *mShowPhoto = nullptr;
    QCheckBox *mShowFrom = nullptr;
    QCheckBox *mShowSubject = nullptr;
    QCheckBox *mShowFolders = nullptr;
    QCheckBox *mExcludeMySelf = nullptr;
    QCheckBox *mAllowToShowMail = nullptr;
    QCheckBox *mKeepPersistentNotification = nullptr;
    KNotifyConfigWidget *mNotify = nullptr;
    QCheckBox *mTextToSpeak = nullptr;
    QLineEdit *mTextToSpeakSetting = nullptr;
    NewMailNotifierSelectCollectionWidget *mSelectCollection = nullptr;
};

#endif // NEWMAILNOTIFIERSETTINGSDIALOG_H
