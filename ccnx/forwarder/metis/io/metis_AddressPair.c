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

#include <config.h>
#include <stdio.h>

#include <ccnx/forwarder/metis/io/metis_AddressPair.h>
#include <parc/algol/parc_Memory.h>
#include <parc/algol/parc_Hash.h>
#include <parc/algol/parc_Object.h>

#include <LongBow/runtime.h>

struct metis_address_pair {
    CPIAddress *local;
    CPIAddress *remote;
};

static void
_metisAddressPair_Destroy(MetisAddressPair **addressPairPtr)
{
    MetisAddressPair *pair = *addressPairPtr;

    cpiAddress_Destroy(&pair->local);
    cpiAddress_Destroy(&pair->remote);
}

parcObject_ExtendPARCObject(MetisAddressPair, _metisAddressPair_Destroy,
                            NULL, metisAddressPair_ToString, metisAddressPair_Equals, NULL, metisAddressPair_HashCode, NULL);

parcObject_ImplementAcquire(metisAddressPair, MetisAddressPair);

parcObject_ImplementRelease(metisAddressPair, MetisAddressPair);

MetisAddressPair *
metisAddressPair_Create(const CPIAddress *local, const CPIAddress *remote)
{
    assertNotNull(local, "Parameter local must be non-null");
    assertNotNull(remote, "Parameter remote must be non-null");

    MetisAddressPair *pair = parcObject_CreateInstance(MetisAddressPair);
    assertNotNull(pair, "Got null from parcObject_Create()");

    pair->local = cpiAddress_Copy(local);
    pair->remote = cpiAddress_Copy(remote);

    return pair;
}

bool
metisAddressPair_Equals(const MetisAddressPair *a, const MetisAddressPair *b)
{
    if (a == b) {
        return true;
    }
    if (a == NULL || b == NULL) {
        return false;
    }

    if (cpiAddress_Equals(a->local, b->local)) {
        if (cpiAddress_Equals(a->remote, b->remote)) {
            return true;
        }
    }

    return false;
}

bool
metisAddressPair_EqualsAddresses(const MetisAddressPair *a, const CPIAddress *local, const CPIAddress *remote)
{
    if (a == NULL || local == NULL || remote == NULL) {
        return false;
    }

    if (cpiAddress_Equals(a->local, local)) {
        if (cpiAddress_Equals(a->remote, remote)) {
            return true;
        }
    }

    return false;
}

char *
metisAddressPair_ToString(const MetisAddressPair *pair)
{
    assertNotNull(pair, "Parameter pair must be non-null");

    char *local = cpiAddress_ToString(pair->local);
    char *remote = cpiAddress_ToString(pair->remote);

    char *output;
    int failure = asprintf(&output, "{ .local=%s, .remote=%s }", local, remote);
    assertTrue(failure > -1, "Error on asprintf");

    parcMemory_Deallocate((void **) &local);
    parcMemory_Deallocate((void **) &remote);

    return output;
}

const CPIAddress *
metisAddressPair_GetLocal(const MetisAddressPair *pair)
{
    assertNotNull(pair, "Parameter pair must be non-null");
    return pair->local;
}

const CPIAddress *
metisAddressPair_GetRemote(const MetisAddressPair *pair)
{
    assertNotNull(pair, "Parameter pair must be non-null");
    return pair->remote;
}

/**
 * @function metisAddressPair_HashCode
 * @abstract Hash useful for tables.  Consistent with Equals.
 * @discussion
 *   Returns a non-cryptographic hash that is consistent with equals.  That is,
 *   if a == b, then hash(a) == hash(b).
 *
 * @param <#param1#>
 * @return <#return#>
 */
PARCHashCode
metisAddressPair_HashCode(const MetisAddressPair *pair)
{
    PARCHashCode hashpair[2];
    hashpair[0] = cpiAddress_HashCode(pair->local);
    hashpair[1] = cpiAddress_HashCode(pair->remote);
    return parcHashCode_Hash((const uint8_t *) hashpair, sizeof(hashpair));
//    return parcHash32_Data(hashpair, sizeof(hashpair));
}
