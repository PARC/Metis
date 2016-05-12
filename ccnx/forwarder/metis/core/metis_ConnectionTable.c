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
 * @header MetisConnectionTable
 * @abstract Records all the current connections and references to them
 * @discussion
 *
 * Caveats:
 * - Need to expire connectionless connections (e.g. udp, ethernet) if not used after some period (case 765)
 *
 * @author Marc Mosko
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <LongBow/runtime.h>

#include <ccnx/forwarder/metis/core/metis_ConnectionTable.h>
#include <ccnx/forwarder/metis/io/metis_AddressPair.h>
#include <parc/algol/parc_Memory.h>
#include <parc/algol/parc_ArrayList.h>
#include <parc/algol/parc_HashCodeTable.h>
#include <parc/algol/parc_Hash.h>
#include <parc/algol/parc_TreeRedBlack.h>

struct metis_connection_table {
    // The main storage table that has a Destroy method.
    // The key is an unsigned int pointer.  We use an unsigned int pointer
    // because we want to be able to lookup by the id alone, and not have to
    // have the MetisIoOperations everywhere.
    PARCHashCodeTable *storageTableById;

    // The key is a MetisAddressPair
    // It does not have a destroy method for the data or key,
    // as they are derived from the storage table.
    PARCHashCodeTable *indexByAddressPair;

    // An iterable stucture organized by connection id.  The keys and
    // values are the same pointers as in storageTableById, so there
    // are no destructors in the tree.
    // The only reason to keep this tree is so we have an iterable list
    // of connections, which the hash table does not give us.
    PARCTreeRedBlack *listById;
};

static bool
metisConnectionTable_ConnectionIdEquals(const void *keyA, const void *keyB)
{
    unsigned idA = *((unsigned  *) keyA);
    unsigned idB = *((unsigned  *) keyB);
    return (idA == idB);
}

static int
metisConnectionTable_ConnectionIdCompare(const void *keyA, const void *keyB)
{
    unsigned idA = *((unsigned  *) keyA);
    unsigned idB = *((unsigned  *) keyB);
    if (idA < idB) {
        return -1;
    }
    if (idA > idB) {
        return +1;
    }
    return 0;
}

static bool
metisConnectionTable_AddressPairEquals(const void *keyA, const void *keyB)
{
    const MetisAddressPair *pairA = (const MetisAddressPair *) keyA;
    const MetisAddressPair *pairB = (const MetisAddressPair *) keyB;

    return metisAddressPair_Equals(pairA, pairB);
}

static HashCodeType
metisConnectionTable_ConnectionIdHashCode(const void *keyA)
{
    unsigned idA = *((unsigned  *) keyA);
    return parcHash32_Int32(idA);
}

static HashCodeType
metisConnectionTable_AddressPairHashCode(const void *keyA)
{
    const MetisAddressPair *pairA = (const MetisAddressPair *) keyA;
    return metisAddressPair_HashCode(pairA);
}


static void
metisConnectionTable_ConnectionIdDestroyer(void **dataPtr)
{
    unsigned *idA = (unsigned *) *dataPtr;
    parcMemory_Deallocate((void **) &idA);
    *dataPtr = NULL;
}

static void
metisConnectionTable_ConnectionDestroyer(void **dataPtr)
{
    metisConnection_Release((MetisConnection **) dataPtr);
}

MetisConnectionTable *
metisConnectionTable_Create()
{
    size_t initialSize = 16384;

    MetisConnectionTable *conntable = parcMemory_AllocateAndClear(sizeof(MetisConnectionTable));
    assertNotNull(conntable, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(MetisConnectionTable));

    conntable->storageTableById = parcHashCodeTable_Create_Size(metisConnectionTable_ConnectionIdEquals,
                                                                metisConnectionTable_ConnectionIdHashCode,
                                                                metisConnectionTable_ConnectionIdDestroyer,
                                                                metisConnectionTable_ConnectionDestroyer,
                                                                initialSize);

    // no key or data destroyer, this is an index into storageByid.
    conntable->indexByAddressPair = parcHashCodeTable_Create_Size(metisConnectionTable_AddressPairEquals,
                                                                  metisConnectionTable_AddressPairHashCode,
                                                                  NULL,
                                                                  NULL,
                                                                  initialSize);

    conntable->listById = parcTreeRedBlack_Create(metisConnectionTable_ConnectionIdCompare,
                                                  NULL,  // key free
                                                  NULL,  // key copy
                                                  NULL,  // value equals
                                                  NULL,  // value free
                                                  NULL); // value copy

    return conntable;
}


void
metisConnectionTable_Destroy(MetisConnectionTable **conntablePtr)
{
    assertNotNull(conntablePtr, "Parameter must be non-null double pointer");
    assertNotNull(*conntablePtr, "Parameter must dereference to non-null pointer");

    MetisConnectionTable *conntable = *conntablePtr;

    parcTreeRedBlack_Destroy(&conntable->listById);
    parcHashCodeTable_Destroy(&conntable->indexByAddressPair);
    parcHashCodeTable_Destroy(&conntable->storageTableById);
    parcMemory_Deallocate((void **) &conntable);
    *conntablePtr = NULL;
}

/**
 * @function metisConnectionTable_Add
 * @abstract Add a connection, takes ownership of memory
 * @discussion
 *   <#Discussion#>
 *
 * @param <#param1#>
 */
void
metisConnectionTable_Add(MetisConnectionTable *table, MetisConnection *connection)
{
    assertNotNull(table, "Parameter table must be non-null");
    assertNotNull(connection, "Parameter connection must be non-null");

    unsigned *connectionIdKey = parcMemory_Allocate(sizeof(unsigned));
    assertNotNull(connectionIdKey, "parcMemory_Allocate(%zu) returned NULL", sizeof(unsigned));
    *connectionIdKey = metisConnection_GetConnectionId(connection);

    if (parcHashCodeTable_Add(table->storageTableById, connectionIdKey, connection)) {
        parcHashCodeTable_Add(table->indexByAddressPair, (void *) metisConnection_GetAddressPair(connection), connection);
        parcTreeRedBlack_Insert(table->listById, connectionIdKey, connection);
    } else {
        trapUnexpectedState("Could not add connection id %u -- is it a duplicate?", *connectionIdKey);
    }
}

/**
 * @function metisConnectionTable_Remove
 * @abstract Removes the connection, calling Destroy on our copy
 * @discussion
 *   <#Discussion#>
 *
 * @param <#param1#>
 */
void
metisConnectionTable_Remove(MetisConnectionTable *table, const MetisConnection *connection)
{
    assertNotNull(table, "Parameter table must be non-null");
    assertNotNull(connection, "Parameter connection must be non-null");

    unsigned connid = metisConnection_GetConnectionId(connection);

    parcTreeRedBlack_Remove(table->listById, &connid);
    parcHashCodeTable_Del(table->indexByAddressPair, metisConnection_GetAddressPair(connection));
    parcHashCodeTable_Del(table->storageTableById, &connid);
}

void
metisConnectionTable_RemoveById(MetisConnectionTable *table, unsigned id)
{
    assertNotNull(table, "Parameter table must be non-null");
    const MetisConnection *connection = metisConnectionTable_FindById(table, id);
    if (connection) {
        metisConnectionTable_Remove(table, connection);
    }
}

const MetisConnection *
metisConnectionTable_FindByAddressPair(MetisConnectionTable *table, const MetisAddressPair *pair)
{
    assertNotNull(table, "Parameter table must be non-null");
    return (MetisConnection *) parcHashCodeTable_Get(table->indexByAddressPair, pair);
}

const MetisConnection *
metisConnectionTable_FindById(MetisConnectionTable *table, unsigned id)
{
    assertNotNull(table, "Parameter table must be non-null");
    return (MetisConnection *) parcHashCodeTable_Get(table->storageTableById, &id);
}


MetisConnectionList *
metisConnectionTable_GetEntries(const MetisConnectionTable *table)
{
    assertNotNull(table, "Parameter table must be non-null");
    MetisConnectionList *list = metisConnectionList_Create();

    PARCArrayList *values = parcTreeRedBlack_Values(table->listById);
    for (size_t i = 0; i < parcArrayList_Size(values); i++) {
        MetisConnection *original = parcArrayList_Get(values, i);
        metisConnectionList_Append(list, original);
    }
    parcArrayList_Destroy(&values);
    return list;
}
