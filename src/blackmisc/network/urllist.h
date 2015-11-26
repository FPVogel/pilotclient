/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_NETWORK_NETWORKLOCATIONLIST_H
#define BLACKMISC_NETWORK_NETWORKLOCATIONLIST_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/network/url.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include <QReadWriteLock>

namespace BlackMisc
{
    namespace Network
    {
        //! Value object encapsulating a list of servers.
        class BLACKMISC_EXPORT CUrlList :
            public CSequence<CUrl>,
            public BlackMisc::Mixin::MetaType<CUrlList>
        {
        public:
            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CUrlList)

            //! Default constructor.
            CUrlList();

            //! Copy constructor (because of mutex)
            CUrlList(const CUrlList &other);

            //! Copy assignment (because of mutex)
            CUrlList &operator =(const CUrlList &other);

            //! By list of URLs
            explicit CUrlList(const QStringList &listOfUrls, bool removeDuplicates = true);

            //! Construct from a base class object.
            CUrlList(const CSequence<CUrl> &other);

            //! Random location for distributed load
            CUrl getRandomUrl() const;

            //! Random location for distributed load, tested
            CUrl getRandomWorkingUrl(int maxTrials = 2) const;

            //! Random location for distributed load
            CUrl getRandomWithout(const CUrlList &exclude) const;

            //! Round robin with random start point
            CUrl getNextUrl(bool randomStart = true) const;

            //! Round robin with random start point
            CUrl getNextUrlWithout(const CUrlList &exclude, bool randomStart = true) const;

            //! Append path to all URLs
            CUrlList appendPath(const QString &path) const;

            //! To formatted String
            QString convertToQString(const QString &separator, bool i18n = false) const;

            //! URLs without duplicates
            CUrlList getWithoutDuplicates() const;

            //! Remove duplicated URL and return number of removed elements
            int removeDuplicates();

        private:
            mutable int m_currentIndexDistributedLoad = -1; //!< index for random access
            mutable QReadWriteLock m_lock;  //!< lock (because of mutable members)
        };

        //! URL list with fail support
        class BLACKMISC_EXPORT CFailoverUrlList : public CUrlList
        {
        public:
            //! Default constructor.
            CFailoverUrlList(int maxTrials = 2);

            //! By list of URLs
            explicit CFailoverUrlList(const QStringList &listOfUrls, int maxTrials = 2);

            //! Construct from a base class object.
            CFailoverUrlList(const CSequence<CUrl> &other, int maxTrials = 2);

            //! All failed URLs
            const CUrlList &getFailedUrls() const { return m_failedUrls; }

            //! Get without the failed URLs
            CUrlList getWithoutFailed() const;

            //! Failed URL
            bool addFailedUrl(const CUrl &failedUrl);

            //! More URLs to try
            bool hasMoreUrlsToTry() const;

            //! Next working URL, test if it can be connected
            CUrl getNextWorkingUrl(const CUrl &failedUrl = CUrl(), bool random = true);

            //! Reset failed URL, allows to set an optional new number of max.trials
            void reset(int maxTrials = -1);

        private:
            int m_maxTrials = 2; //!< number of max trials
            CUrlList m_failedUrls;
        };

    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Network::CUrlList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Network::CUrl>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Network::CUrl>)

#endif //guard
