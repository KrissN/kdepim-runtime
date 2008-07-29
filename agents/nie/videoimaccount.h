/*
 *
 * $Id: $
 *
 * This file is part of the Nepomuk KDE project.
 * Copyright (C) 2007 Sebastian Trueg <trueg@kde.org>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * See the file "COPYING" for the exact licensing terms.
 */

/*
 * This file has been generated by the Nepomuk Resource class generator.
 * DO NOT EDIT THIS FILE.
 * ANY CHANGES WILL BE LOST.
 */

#ifndef _VIDEOIMACCOUNT_H_
#define _VIDEOIMACCOUNT_H_

namespace Nepomuk {
}

#include "audioimaccount.h"
#include <nepomuk/nepomuk_export.h>

namespace Nepomuk {

/**
 * An account in an instant messaging system capable of video conversations. 
 */
    class NEPOMUK_EXPORT VideoIMAccount : public AudioIMAccount
    {
    public:
        /**
         * Create a new empty and invalid VideoIMAccount instance
         */
        VideoIMAccount();
        /**
         * Default copy constructor
         */
        VideoIMAccount( const VideoIMAccount& );
        VideoIMAccount( const Resource& );
        /**
         * Create a new VideoIMAccount instance representing the resource
         * referenced by \a uriOrIdentifier.
         */
        VideoIMAccount( const QString& uriOrIdentifier );
        /**
         * Create a new VideoIMAccount instance representing the resource
         * referenced by \a uri.
         */
        VideoIMAccount( const QUrl& uri );
        ~VideoIMAccount();

        VideoIMAccount& operator=( const VideoIMAccount& );

            /**
             * Retrieve a list of all available VideoIMAccount resources. 
             * This list consists of all resource of type VideoIMAccount that 
             * are stored in the local Nepomuk meta data storage and any changes 
             * made locally. Be aware that in some cases this list can get very 
             * big. Then it might be better to use libKNep directly. 
             */
            static QList<VideoIMAccount> allVideoIMAccounts();


        /**
         * \return The URI of the resource type that is used in VideoIMAccount instances.
         */
        static QString resourceTypeUri();

    protected:
       VideoIMAccount( const QString& uri, const QUrl& type );
       VideoIMAccount( const QUrl& uri, const QUrl& type );
   };
}

#endif
