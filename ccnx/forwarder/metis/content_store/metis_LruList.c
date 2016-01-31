/*
 * Copyright (c) 2013-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Patent rights are not granted under this agreement. Patent rights are
 *       available under FRAND terms.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL XEROX or PARC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <config.h>
#include <stdio.h>
#include <sys/queue.h>

#include <ccnx/forwarder/metis/content_store/metis_LruList.h>
#include <parc/algol/parc_Memory.h>
#include <LongBow/runtime.h>

struct metis_lru_list_entry {
    void *userData;

    // always set to the list
    MetisLruList *parentList;

    // indicates if the Entry is currently in the list
    bool inList;

    TAILQ_ENTRY(metis_lru_list_entry)  list;
};

// this defines the TAILQ structure so we can access the tail pointer
TAILQ_HEAD(metis_lru_s, metis_lru_list_entry);

struct metis_lru_list {
    struct metis_lru_s head;
    size_t itemsInList;
};

void
metisLruList_EntryDestroy(MetisLruListEntry **entryPtr)
{
    assertNotNull(entryPtr, "Parameter entryPtr must be non-null double pointer");

    MetisLruListEntry *entry = *entryPtr;
    if (entry->inList) {
        TAILQ_REMOVE(&entry->parentList->head, entry, list);
        assertTrue(entry->parentList->itemsInList > 0, "Invalid state, removed entry from list, but itemsInList is 0");
        entry->parentList->itemsInList--;
    }

    parcMemory_Deallocate((void **) &entry);
    *entryPtr = NULL;
}

void
metisLruList_EntryMoveToHead(MetisLruListEntry *entry)
{
    assertNotNull(entry, "Parameter entry must be non-null");

    TAILQ_REMOVE(&entry->parentList->head, entry, list);
    TAILQ_INSERT_HEAD(&entry->parentList->head, entry, list);
}

void *
metisLruList_EntryGetData(MetisLruListEntry *entry)
{
    return entry->userData;
}

MetisLruList *
metisLruList_Create()
{
    MetisLruList *list = parcMemory_AllocateAndClear(sizeof(MetisLruList));
    assertNotNull(list, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(MetisLruList));
    list->itemsInList = 0;
    TAILQ_INIT(&list->head);
    return list;
}

void
metisLruList_Destroy(MetisLruList **lruPtr)
{
    assertNotNull(lruPtr, "Parameter lruPtr must be non-null double pointer");

    MetisLruList *lru = *lruPtr;

    MetisLruListEntry *entry = TAILQ_FIRST(&lru->head);
    while (entry != NULL) {
        MetisLruListEntry *next = TAILQ_NEXT(entry, list);
        metisLruList_EntryDestroy(&entry);
        entry = next;
    }

    parcMemory_Deallocate((void **) &lru);
    *lruPtr = NULL;
}

MetisLruListEntry *
metisLruList_NewHeadEntry(MetisLruList *lru, void *data)
{
    assertNotNull(lru, "Parameter lru must be non-null");
    assertNotNull(data, "Parameter data must be non-null");

    MetisLruListEntry *entry = parcMemory_AllocateAndClear(sizeof(MetisLruListEntry));
    assertNotNull(entry, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(MetisLruListEntry));
    entry->userData = data;
    entry->parentList = lru;
    entry->inList = true;

    TAILQ_INSERT_HEAD(&lru->head, entry, list);
    lru->itemsInList++;

    return entry;
}

MetisLruListEntry *
metisLruList_PopTail(MetisLruList *lru)
{
    assertNotNull(lru, "Parameter lru must be non-null");

    MetisLruListEntry *entry = TAILQ_LAST(&lru->head, metis_lru_s);

    if (entry) {
        assertTrue(lru->itemsInList > 0, "Invalid state, removed entry from list, but itemsInList is 0");
        lru->itemsInList--;
        TAILQ_REMOVE(&lru->head, entry, list);
        entry->inList = false;
    }

    return entry;
}

size_t
metisLruList_Length(const MetisLruList *lru)
{
    assertNotNull(lru, "Parameter lru must be non-null");
    return lru->itemsInList;
}