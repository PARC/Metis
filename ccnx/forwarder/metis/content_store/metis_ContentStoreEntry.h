/*
 * Copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC)
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL XEROX OR PARC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * ################################################################################
 * #
 * # PATENT NOTICE
 * #
 * # This software is distributed under the BSD 2-clause License (see LICENSE
 * # file).  This BSD License does not make any patent claims and as such, does
 * # not act as a patent grant.  The purpose of this section is for each contributor
 * # to define their intentions with respect to intellectual property.
 * #
 * # Each contributor to this source code is encouraged to state their patent
 * # claims and licensing mechanisms for any contributions made. At the end of
 * # this section contributors may each make their own statements.  Contributor's
 * # claims and grants only apply to the pieces (source code, programs, text,
 * # media, etc) that they have contributed directly to this software.
 * #
 * # There is no guarantee that this section is complete, up to date or accurate. It
 * # is up to the contributors to maintain their portion of this section and up to
 * # the user of the software to verify any claims herein.
 * #
 * # Do not remove this header notification.  The contents of this section must be
 * # present in all distributions of the software.  You may only modify your own
 * # intellectual property statements.  Please provide contact information.
 * 
 * - Palo Alto Research Center, Inc
 * This software distribution does not grant any rights to patents owned by Palo
 * Alto Research Center, Inc (PARC). Rights to these patents are available via
 * various mechanisms. As of January 2016 PARC has committed to FRAND licensing any
 * intellectual property used by its contributions to this software. You may
 * contact PARC at cipo@parc.com for more information or visit http://www.ccnx.org
 */
/**
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#ifndef Metis_metis_ContentStoreEntry_h
#define Metis_metis_ContentStoreEntry_h

#include <ccnx/forwarder/metis/core/metis_Message.h>
#include <ccnx/forwarder/metis/content_store/metis_LruList.h>

struct metis_contentstore_entry;
typedef struct metis_contentstore_entry MetisContentStoreEntry;

/**
 * The max time allowed for an ExpiryTime. Will never be exceeded.
 */
extern const uint64_t metisContentStoreEntry_MaxExpiryTime;

/**
 * The max time allowed for an RecommendedCacheTime. Will never be exceeded.
 */
extern const uint64_t metisContentStoreEntry_MaxRecommendedCacheTime;


/**
 * Creates a new `MetisContentStoreEntry` instance, acquiring a reference to the supplied `MetisMessage`.
 *
 * @param message the message to store
 * @param lruList the LRU list that this entry will be stored in.
 * @return A newly created `MetisContentStoreEntry` instance that must eventually be released by calling
 *         {@link metisContentStoreEntry_Release}.
 *
 * @see metisContentStoreEntry_Release
 */
MetisContentStoreEntry *metisContentStoreEntry_Create(MetisMessage *objectMessage, MetisLruList *lruList);

/**
 * Returns a reference counted copy of the supplied `MetisContentStoreEntry`.
 *
 * @param original the MetisContentStoreEntry to return a reference to.
 * @return Reference counted copy, must call <code>metisContentStoreEntry_Destroy()</code> on it.
 */
MetisContentStoreEntry *metisContentStoreEntry_Acquire(const MetisContentStoreEntry *original);

/**
 * Releases one reference count and destroys object when reaches zero
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [in,out] entryPtr A pointer to an allocated MetisContentStoreEntry
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
void metisContentStoreEntry_Release(MetisContentStoreEntry **entryPtr);

/**
 * Returns a pointer to the contained {@link MetisMessage}.
 * The caller must called {@link metisMessage_Acquire()} if they want to keep a reference to the returned
 * message.
 *
 * @param storeEntry the MetisContentStoreEntry from which to retrieve the `MetisMessage` pointer.
 * @return the address of the `MetisMessage` contained in the storeEntry.
 * @see metisMessage_Acquire
 */
MetisMessage *metisContentStoreEntry_GetMessage(const MetisContentStoreEntry *storeEntry);

/**
 * Return true if the message stored in this `MetisContentStoreEntry` has an ExpiryTime.
 *
 * @param storeEntry the MetisContentStoreEntry containing the message.
 * @return true if the referenced message has an ExpiryTime. False, otherwise.
 */
bool metisContentStoreEntry_HasExpiryTimeTicks(const MetisContentStoreEntry *storeEntry);

/**
 * Return the ExpiryTime stored in this `MetisContentStoreEntry`.
 *
 * @param storeEntry the MetisContentStoreEntry from which to retrieve the `MetisMessage` pointer.
 * @return the address of the `MetisMessage` contained in the storeEntry.
 */
uint64_t metisContentStoreEntry_GetExpiryTimeTicks(const MetisContentStoreEntry *storeEntry);

/**
 * Return true if the message stored in this `MetisContentStoreEntry` has a RecommendedCacheTime.
 *
 * @param storeEntry the MetisContentStoreEntry containing the message.
 * @return true if the referenced message has a RecommendedCacheTime. False, otherwise.
 */
bool metisContentStoreEntry_HasRecommendedCacheTimeTicks(const MetisContentStoreEntry *storeEntry);

/**
 * Return the RecommendedCacheTime stored in this `MetisContentStoreEntry`.
 *
 * @param storeEntry the MetisContentStoreEntry from which to retrieve the `MetisMessage` pointer.
 * @return the address of the `MetisMessage` contained in the storeEntry.
 */
uint64_t metisContentStoreEntry_GetRecommendedCacheTimeTicks(const MetisContentStoreEntry *storeEntry);

/**
 * A signum function comparing two `MetisContentStoreEntry` instances, using their
 * RecommendedCacheTime and, if necessary, the addresses of the referenced MetisMessage. In other words, if
 * two ContentStoreEntries have the same RecommendedCacheTime, the comparison will then be made on the
 * memory addresses of the MetisMessages referenced by the MetisContentStoreEntrys. So, the only way two
 * MetisContentStoreEntrys will compare equally (0) is if they both have the same RecommendedCacheTime and reference
 * the same MetisMessage.
 *
 * Used to determine the ordering relationship of two `MetisContentStoreEntry` instances.
 * This is used by the {@link MetisTimeOrderedList} to keep a list of MetisContentStoreEntrys, sorted by
 * RecommendedCacheTime.
 *
 * @param [in] storeEntry1 A pointer to a `MetisContentStoreEntry` instance.
 * @param [in] storeEntry2 A pointer to a `MetisContentStoreEntry` instance to be compared to `storeEntry1`.
 *
 * @return 0 if `storeEntry1` and `storeEntry2` are equivalent
 * @return < 0 if `storeEntry1` < `storeEntry2`
 * @return > 0 if `storeEntry1` > `storeEntry2`
 *
 * Example:
 * @code
 * {
 *     MetisContentStoreEntry *entry1 = metisContentStoreEntry_Create(...);
 *     MetisContentStoreEntry *entry2 = metisContentStoreEntry_Create(...);
 *
 *     int val = metisContentStoreEntry_CompareRecommendedCacheTime(entry1, entry2);
 *     if (val < 0) {
 *         // entry1 has a lower RecommendedCacheTime, or the same RecommendedCacheTime as entry2 and a different message.
 *     } else if (val > 0) {
 *         // entry2 has a lower RecommendedCacheTime, or the same RecommendedCacheTime as entry1 and a different message.
 *     } else {
 *         // entry1 and entry2 have the same RecommendedCacheTime AND the same message.
 *     }
 *
 *     metisContentStoreEntry_Release(&entry1);
 *     metisContentStoreEntry_Release(&entry2);
 *
 * }
 * @endcode
 * @see `metisContentStoreEntry_CompareExpiryTime`
 */
int metisContentStoreEntry_CompareRecommendedCacheTime(const MetisContentStoreEntry *storeEntry1, const MetisContentStoreEntry *storeEntry2);

/**
 * A signum function comparing two `MetisContentStoreEntry` instances, using their
 * ExpiryTime and, if necessary, the addresses of the referenced MetisMessage. In other words, if
 * two ContentStoreEntries have the same ExpiryTime, the comparison will then be made on the
 * memory addresses of the MetisMessages referenced by the MetisContentStoreEntrys. So, the only way two
 * MetisContentStoreEntrys will compare equally (0) is if they both have the same ExpiryTime and reference
 * the same MetisMessage.
 *
 * Used to determine the ordering relationship of two `MetisContentStoreEntry` instances.
 * This is used by the {@link MetisTimeOrderedList} to keep a list of MetisContentStoreEntrys, sorted by
 * ExpiryTime.
 *
 * @param [in] storeEntry1 A pointer to a `MetisContentStoreEntry` instance.
 * @param [in] storeEntry2 A pointer to a `MetisContentStoreEntry` instance to be compared to `storeEntry1`.
 *
 * @return 0 if `storeEntry1` and `storeEntry2` are equivalent
 * @return < 0 if `storeEntry1` < `storeEntry2`
 * @return > 0 if `storeEntry1` > `storeEntry2`
 *
 * Example:
 * @code
 * {
 *     MetisContentStoreEntry *entry1 = metisContentStoreEntry_Create(...);
 *     MetisContentStoreEntry *entry2 = metisContentStoreEntry_Create(...);
 *
 *     int val = metisContentStoreEntry_CompareExpiryTime(entry1, entry2);
 *     if (val < 0) {
 *         // entry1 has a lower ExpiryTime, or the same ExpiryTime as entry2 and a different message.
 *     } else if (val > 0) {
 *         // entry2 has a lower ExpiryTime, or the same ExpiryTime as entry1 and a different message.
 *     } else {
 *         // entry1 and entry2 have the same ExpiryTime AND the same message.
 *     }
 *
 *     metisContentStoreEntry_Release(&entry1);
 *     metisContentStoreEntry_Release(&entry2);
 *
 * }
 * @endcode
 * @see `metisContentStoreEntry_CompareRecommendedCacheTime`
 */
int metisContentStoreEntry_CompareExpiryTime(const MetisContentStoreEntry *storeEntry1, const MetisContentStoreEntry *storeEntry2);

/**
 * Move this entry to the head of the LRU list
 *
 * Moves the entry to the head of the LRU list it was created with
 *
 * @param [in] storeEntry An allocated MetisContenstoreEntry
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
void metisContentStoreEntry_MoveToHead(MetisContentStoreEntry *storeEntry);
#endif // Metis_metis_ContentStoreEntry_h
