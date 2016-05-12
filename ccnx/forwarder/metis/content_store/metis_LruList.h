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
/**
 * @file metis_LruList.h
 * @brief Maintains an LRU for the content store
 *
 * An LRU list is make up of LRU entries.  The entries are bound to the list.
 * The user of the list is reponsible for knowing when there's too many things and
 * wants to remove one.  The LRU list will grow without bound otherwise.
 *
 * The LRU list is meant to be used as an auxiliary data structure, not the primary
 * storage of data elements.
 *
 * Example Usage:
 *
 * <code>
 *  myClass_Create() {
 *      ...
 *      me->lru = metisLruList_Create();
 *      ...
 *  }
 *
 *  myClass_AddAnItem(MyClass *me, ...) {
 *     size_t length = metisLruList_Length(me->lru);
 *     if( lenth == me->limit )
 *        myClass_Overflow(me);
 *
 *     item_Create(me, ...);
 *  }
 *
 *  myClass_Overflow(MyClass *me) {
 *     MetisLruEntry *entry = metisLruList_PopTail(me->lru);
 *
 *     // entry could be NULL, if the list is empty, but you probalby wouldnt have overflow then...
 *     Item *item = (Item *) metisLruEntry_GetData(entry);
 *     item_Destroy(&item);
 *  }
 *
 *  myClass_Destroy(MyClass *me) {
 *     // this does not destroy the void * data in the entries, just the entries.
 *     metisLruList_Destroy(&me->lru);
 *     // destroy all the items from some other knowledge
 *  }
 *
 *  item_Create(MyClass *me, ...) {
 *     item = malloc(sizeof(Item));
 *     MetisLruEntry *entry = metisLruList_NewHeadEntry(me->lru, item);
 *     item->lruEntry = entry;
 *     ...
 *  }
 *
 *  item_Use(Item *item) {
 *     metisLruEntry_MoveToHead(item->lruEntry);
 *     ...
 *  }
 *
 *  item_Destroy(Item *item) {
 *     // removes the entry from the list, if its in a list
 *     metisLruEntry_Destroy(&item->lruEntry);
 *     free(item);
 *  }
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#ifndef Metis_metis_LruList_h
#define Metis_metis_LruList_h

struct metis_lru_list_entry;
typedef struct metis_lru_list_entry MetisLruListEntry;

struct metis_lru_list;
typedef struct metis_lru_list MetisLruList;

/**
 * @function metisLruEntry_Destroy
 * @abstract Destroys and element.  This will also remove it from the list.
 * @discussion
 *   <#Discussion#>
 *
 * @param <#param1#>
 * @return <#return#>
 */
void metisLruList_EntryDestroy(MetisLruListEntry **entryPtr);

/**
 * @function <#FunctionName#>
 * @abstract <#OneLineDescription#>
 * @discussion
 *   <#Discussion#>
 *
 * @param <#param1#>
 * @return <#return#>
 */
void metisLruList_EntryMoveToHead(MetisLruListEntry *entry);

/**
 * @function metisLruEntry_GetData
 * @abstract Returns the user-supplied opaque data when the entry was created
 * @discussion
 *   <#Discussion#>
 *
 * @param <#param1#>
 * @return <#return#>
 */
void *metisLruList_EntryGetData(MetisLruListEntry *entry);

/**
 * @function metisLruList_Create
 * @abstract Creates a new Least-Recently-Used list
 * @discussion
 *   <#Discussion#>
 *
 * @param <#param1#>
 * @return <#return#>
 */
MetisLruList *metisLruList_Create();

/**
 * @function metisLruList_Destroy
 * @abstract Destroys a list and frees all the elements in it
 * @discussion
 *   <#Discussion#>
 *
 * @param <#param1#>
 */
void metisLruList_Destroy(MetisLruList **listPtr);

/**
 * Returns the number of items in the list
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [in] lru An allocated MetisLruList
 *
 * @retval number The number of items in the LRU list
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
size_t metisLruList_Length(const MetisLruList *lru);

/**
 * @function metisLruList_NewHeadEntry
 * @abstract Creates a new entry for the list.  It is inserted at the head of the list.
 * @discussion
 *   <#Discussion#>
 *
 * @param <#param1#>
 * @return <#return#>
 */
MetisLruListEntry *metisLruList_NewHeadEntry(MetisLruList *lru, void *data);

/**
 * @function metisLruList_PopTail
 * @abstract Removes the tail element from the list and returns it to the user
 * @discussion
 *   Pops the tail element.  The user should examine its data to destroy their
 *   tail object, then call <code>MetisLruEntry_Destroy()</code> to free the
 *   LRU entry.
 *
 * @param <#param1#>
 * @return The tail element, or NULL for an empty list
 */
MetisLruListEntry *metisLruList_PopTail(MetisLruList *list);
#endif // Metis_metis_LruList_h
