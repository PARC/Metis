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
 * A name built around the TLV representation.
 *
 * A common operation is to get a sub-string of the name, specifically prefixes.  Use
 * metisTlvName_Slice() for that.
 *
 * To be efficient about Slice(), the reference counters are pointers, so every allocated
 * copy shares the reference counter and all the allocated memory.  Each Slice() will do
 * one malloc for the new shell and do a shallow memcpy of the struct.  Destroy will always
 * free the shell, but will only free the guts when the shared reference count goes to zero.
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#include <config.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include <parc/algol/parc_Memory.h>
#include <parc/algol/parc_Hash.h>
#include <parc/algol/parc_BufferComposer.h>

#include <ccnx/forwarder/metis/tlv/metis_TlvName.h>
#include <ccnx/forwarder/metis/tlv/metis_Tlv.h>
#include <ccnx/forwarder/metis/tlv/metis_TlvNameCodec.h>

#include <LongBow/runtime.h>

struct metis_tlv_name {
    uint8_t *memory;
    size_t memoryLength;

    // the refcount is shared between all copies
    unsigned  *refCountPtr;

    // the memory extents of each path segment
    MetisTlvExtent *segmentArray;
    size_t segmentArrayLength;

    // hashes of the name through different prefix lengths
    // It is allocated out to the limit (same assegmentArrayLength),
    // but only computed so far through segmentCumulativeHashArrayLength
    // This is to avoid computing the hash over unnecessary suffix segments.
    size_t segmentCumulativeHashArrayLimit;

    // the cumulative hash array length is shared between all copies, so if
    // one copy extends the array, all copies see it
    size_t *segmentCumulativeHashArrayLengthPtr;
    uint32_t *segmentCumulativeHashArray;
};

// =====================================================

static unsigned
_getRefCount(const MetisTlvName *name)
{
    return *name->refCountPtr;
}

static void
_incrementRefCount(MetisTlvName *name)
{
    assertTrue(*name->refCountPtr > 0, "Illegal State: Trying to increment a 0 refcount!");
    (*name->refCountPtr)++;
}

static void
_decrementRefCount(MetisTlvName *name)
{
    assertTrue(*name->refCountPtr > 0, "Illegal State: Trying to decrement a 0 refcount!");
    (*name->refCountPtr)--;
}

// ============================================================================

/**
 * Common parts of setting up a MetisTlvName after the backing memory has been allocated and copied in to.
 *
 * PRECONDITIONS: name->memory and name->memoryLength set
 */
static void
_setup(MetisTlvName *name)
{
    name->refCountPtr = parcMemory_Allocate(sizeof(unsigned));
    assertNotNull(name->refCountPtr, "parcMemory_Allocate(%zu) returned NULL", sizeof(unsigned));
    *name->refCountPtr = 1;

    metisTlv_NameSegments(name->memory, name->memoryLength, &name->segmentArray, &name->segmentArrayLength);

    name->segmentCumulativeHashArray = parcMemory_Allocate(name->segmentArrayLength * sizeof(uint32_t));
    assertNotNull(name->segmentCumulativeHashArray, "parcMemory_Allocate(%zu) returned NULL", name->segmentArrayLength * sizeof(uint32_t));

    name->segmentCumulativeHashArrayLengthPtr = parcMemory_Allocate(sizeof(size_t));
    assertNotNull(name->segmentCumulativeHashArrayLengthPtr, "parcMemory_Allocate(%zu) returned NULL", sizeof(size_t));

    *name->segmentCumulativeHashArrayLengthPtr = 1;
    name->segmentCumulativeHashArrayLimit = name->segmentArrayLength;


    if (name->segmentArrayLength > 0) {
        // always hash the 1st name component.  This is needed as the initial case
        // to do the cumulative hashes in metisTlvName_HashCode
        name->segmentCumulativeHashArray[0] = parcHash32_Data(&name->memory[name->segmentArray[0].offset], name->segmentArray[0].length);
    }
}

MetisTlvName *
metisTlvName_Create(const uint8_t *memory, size_t memoryLength)
{
    MetisTlvName *name = parcMemory_AllocateAndClear(sizeof(MetisTlvName));
    assertNotNull(name, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(MetisTlvName));

    name->memory = parcMemory_Allocate(memoryLength);
    assertNotNull(name->memory, "parcMemory_Allocate(%zu) returned NULL", memoryLength);

    memcpy(name->memory, memory, memoryLength);
    name->memoryLength = memoryLength;

    _setup(name);

    return name;
}

MetisTlvName *
metisTlvName_CreateFromCCNxName(const CCNxName *ccnxName)
{
    // to avoid reallocs, calculate the exact size we need
    size_t memoryLength = 0;
    for (size_t i = 0; i < ccnxName_GetSegmentCount(ccnxName); i++) {
        CCNxNameSegment *segment = ccnxName_GetSegment(ccnxName, i);
        memoryLength += 4 + ccnxNameSegment_Length(segment);
    }

    MetisTlvName *name = parcMemory_AllocateAndClear(sizeof(MetisTlvName));
    assertNotNull(name, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(MetisTlvName));

    name->memoryLength = memoryLength;
    name->memory = parcMemory_Allocate(memoryLength);
    assertNotNull(name->memory, "parcMemory_Allocate(%zu) returned NULL", memoryLength);

    uint8_t *p = name->memory;
    uint8_t *end = p + memoryLength;
    for (size_t i = 0; i < ccnxName_GetSegmentCount(ccnxName); i++) {
        CCNxNameSegment *segment = ccnxName_GetSegment(ccnxName, i);
        uint16_t type = ccnxNameSegment_GetType(segment);
        uint16_t length = ccnxNameSegment_Length(segment);

        *(uint16_t *) p = htons(type);
        p += 2;

        *(uint16_t *) p = htons(length);
        p += 2;

        if (length >0) {
            PARCBuffer *buffer = ccnxNameSegment_GetValue(segment);
            uint8_t *overlay = parcBuffer_Overlay(buffer, 0);
            memcpy(p, overlay, length);

            p += length;
        }

        // sanity check
        assertTrue(p <= end, "Wrote past the end of buffer, now at %p end at %p", p, end);
    }

    _setup(name);
    return name;
}

void
metisTlvName_Release(MetisTlvName **namePtr)
{
    assertNotNull(namePtr, "Parameter must be non-null double pointer");
    assertNotNull(*namePtr, "Parameter must dereference to non-null pointer");

    MetisTlvName *name = *namePtr;
    _decrementRefCount(name);
    if (_getRefCount(name) == 0) {
        parcMemory_Deallocate((void **) &(name->refCountPtr));
        parcMemory_Deallocate((void **) &(name->segmentArray));
        parcMemory_Deallocate((void **) &(name->segmentCumulativeHashArray));
        parcMemory_Deallocate((void **) &(name->segmentCumulativeHashArrayLengthPtr));
        parcMemory_Deallocate((void **) &(name->memory));
    }
    parcMemory_Deallocate((void **) &name);
    *namePtr = NULL;
}

MetisTlvName *
metisTlvName_Acquire(const MetisTlvName *original)
{
    return metisTlvName_Slice(original, UINT_MAX);
}

MetisTlvName *
metisTlvName_Slice(const MetisTlvName *original, size_t segmentCount)
{
    assertNotNull(original, "Parameter must be non-null");
    MetisTlvName *copy = parcMemory_AllocateAndClear(sizeof(MetisTlvName));
    assertNotNull(copy, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(MetisTlvName));

    memcpy(copy, original, sizeof(MetisTlvName));
    _incrementRefCount(copy);

    copy->segmentArrayLength = (copy->segmentArrayLength < segmentCount) ? copy->segmentArrayLength : segmentCount;

    // for equality to work, we need to shorten the MemoryLength to the amount
    // actually used by the number of segments.
    size_t startOfLastSegment = copy->segmentArray[ copy->segmentArrayLength - 1 ].offset;
    size_t lengthOfLastSegment = copy->segmentArray[ copy->segmentArrayLength - 1 ].length;

    copy->memoryLength = startOfLastSegment + lengthOfLastSegment;

    return copy;
}

uint32_t
metisTlvName_HashCode(const MetisTlvName *name)
{
    if ((name == NULL) || (name->segmentArrayLength == 0)) {
        return 0;
    }

    size_t lastSegment = name->segmentArrayLength - 1;

    if (lastSegment >= *name->segmentCumulativeHashArrayLengthPtr) {
        // we have not yet computed this, so lets do it now!
        // Note that we go up to and including lastSegment in the for loop.  lastSegment is not a "length", it is
        // the actual array index.
        for (size_t i = (*name->segmentCumulativeHashArrayLengthPtr); i <= lastSegment; i++) {
            // the -1 is ok in the 3rd argument, because segmentCumulativeHashArrayLength always has length at least 1
            // if there are any name components to hash.
            name->segmentCumulativeHashArray[i] = parcHash32_Data_Cumulative(&name->memory[ name->segmentArray[i].offset ],
                                                                             name->segmentArray[i].length,
                                                                             name->segmentCumulativeHashArray[i - 1]);
        }
        *name->segmentCumulativeHashArrayLengthPtr = lastSegment + 1;
    }

    return name->segmentCumulativeHashArray[lastSegment];
}

bool
metisTlvName_Equals(const MetisTlvName *a, const MetisTlvName *b)
{
    assertNotNull(a, "Parameter a must be non-null");
    assertNotNull(b, "Parameter b must be non-null");

    if (a->memoryLength == b->memoryLength) {
        return (memcmp(a->memory, b->memory, a->memoryLength) == 0);
    }
    return false;
}

int
metisTlvName_Compare(const MetisTlvName *a, const MetisTlvName *b)
{
    if (a == NULL && b == NULL) {
        return 0;
    }
    if (a == NULL) {
        return -1;
    }
    if (b == NULL) {
        return +1;
    }

    if (a->memoryLength < b->memoryLength) {
        return -1;
    }

    if (a->memoryLength > b->memoryLength) {
        return +1;
    }

    return memcmp(a->memory, b->memory, a->memoryLength);
}

bool
metisTlvName_StartsWith(const MetisTlvName *name, const MetisTlvName *prefix)
{
    assertNotNull(name, "Parameter name must be non-null");
    assertNotNull(prefix, "Parameter prefix must be non-null");

    if (prefix->memoryLength <= name->memoryLength) {
        return (memcmp(prefix->memory, name->memory, prefix->memoryLength) == 0);
    }

    return false;
}

size_t
metisTlvName_SegmentCount(const MetisTlvName *name)
{
    assertNotNull(name, "Parameter name must be non-null");
    return name->segmentArrayLength;
}

CCNxName *
metisTlvName_ToCCNxName(const MetisTlvName *name)
{
    return metisTlvNameCodec_Decode(name->memory, 0, name->memoryLength);
}

