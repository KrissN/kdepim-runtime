/*
    Copyright (c) 2008 Bertjan Broeksema <b.broeksema@kdemail.org>
    Copyright (c) 2008 Volker Krause <vkrause@kde.org>
    Copyright (c) 2010,2011 David Jarvie <djarvie@kde.org>

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

#ifndef AKONADI_SINGLEFILERESOURCECONFIGDIALOGBASE_H
#define AKONADI_SINGLEFILERESOURCECONFIGDIALOGBASE_H

#include "akonadi-singlefileresource_export.h"

#include "ui_singlefileresourceconfigdialog_desktop.h"

#include <QDialog>
#include <QUrl>
class KConfigDialogManager;
class KJob;
class QPushButton;

namespace KIO {
class StatJob;
}

namespace Akonadi {
class SingleFileValidatingWidget;

/**
 * Base class for the configuration dialog for single file based resources.
 * @see SingleFileResourceConfigDialog
 */
class AKONADI_SINGLEFILERESOURCE_EXPORT SingleFileResourceConfigDialogBase : public QDialog
{
    Q_OBJECT
public:
    explicit SingleFileResourceConfigDialogBase(WId windowId);
    ~SingleFileResourceConfigDialogBase();

    /**
     * Adds @param page to the tabwidget. This can be used to add custom
     * settings for a specific single file resource.
     */
    void addPage(const QString &title, QWidget *page);

    /**
     * Set file extension filter.
     */
    void setFilter(const QString &filter);

    /**
     * Enable and show, or disable and hide, the monitor option.
     * If the option is disabled, its value will not be saved.
     * By default, the monitor option is enabled.
     */
    void setMonitorEnabled(bool enable);

    /**
     * Return the file URL.
     */
    QUrl url() const;

    /**
     * Set the file URL.
     */
    void setUrl(const QUrl &url);

    /**
     * Specify whether the file must be local.
     * The default is to allow both local and remote files.
     */
    void setLocalFileOnly(bool local);

    /**
     * Add a widget to the dialog.
     */
    void appendWidget(SingleFileValidatingWidget *widget);

protected Q_SLOTS:
    virtual void save() = 0;

protected:
    Ui::SingleFileResourceConfigDialog ui;
    KConfigDialogManager *mManager = nullptr;

private:
    void validate();
    void slotStatJobResult(KJob *);
    void writeConfig();
    void readConfig();
    KIO::StatJob *mStatJob = nullptr;
    SingleFileValidatingWidget *mAppendedWidget = nullptr;
    bool mDirUrlChecked = false;
    bool mMonitorEnabled = false;
    bool mLocalFileOnly = false;
    QPushButton *mOkButton = nullptr;
};

/**
 * Base class for widgets added to SingleFileResourceConfigDialogBase
 * using its appendWidget() method.
 *
 * Derived classes must implement validate() andQ_EMIT changed() when
 * appropriate.
 */
class AKONADI_SINGLEFILERESOURCE_EXPORT SingleFileValidatingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SingleFileValidatingWidget(QWidget *parent = nullptr);

    /**
     * Return whether the widget's value is valid when the dialog is
     * accepted.
     */
    virtual bool validate() const = 0;

Q_SIGNALS:
    /**
     * Signal emitted when the widget's value changes in a way which
     * might affect the result of validate().
     */
    void changed();
};
}

#endif
