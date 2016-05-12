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
// Include the file(s) containing the functions to be tested.
// This permits internal static functions to be visible to this Test Framework.
#include "../metis_TlvSchemaV0.c"
#include "../metis_TlvSkeleton.c"
#include <LongBow/unit-test.h>
#include <parc/algol/parc_SafeMemory.h>
#include <parc/logging/parc_LogReporterTextStdout.h>

#include <ccnx/forwarder/metis/testdata/metis_TestDataV0.h>

static void
verifyInterestPerHop(MetisTlvSkeleton *skeleton)
{
    MetisTlvExtent extent = metisTlvSkeleton_GetHopLimit(skeleton);
    assertTrue(extent.offset == 12, "Incorrect hopLimit offset, expected %u got %u", 12, extent.offset);
    assertTrue(extent.length == 1, "Incorrect hopLimit length, expected %u got %u", 1, extent.length);
}

static void
verifyInterestSkeleton(MetisTlvSkeleton *skeleton)
{
    MetisTlvExtent nameExtent = metisTlvSkeleton_GetName(skeleton);
    assertTrue(nameExtent.offset == 37, "Incorrect name offset, expected %u got %u", 37, nameExtent.offset);
    assertTrue(nameExtent.length == 17, "Incorrect name length, expected %u got %u", 17, nameExtent.length);

    MetisTlvExtent keyidExtent = metisTlvSkeleton_GetKeyId(skeleton);
    assertTrue(keyidExtent.offset == 58, "Incorrect keyId offset, expected %u got %u", 58, keyidExtent.offset);
    assertTrue(keyidExtent.length == 4, "Incorrect keyId length, expected %u got %u", 4, keyidExtent.length);

    MetisTlvExtent objHashExtent = metisTlvSkeleton_GetObjectHash(skeleton);
    assertTrue(objHashExtent.offset == 66, "Incorrect objectHash offset, expected %u got %u", 66, objHashExtent.offset);
    assertTrue(objHashExtent.length == 6, "Incorrect objectHash length, expected %u got %u", 6, objHashExtent.length);

    MetisTlvExtent lifetimeExtent = metisTlvSkeleton_GetInterestLifetime(skeleton);
    assertTrue(lifetimeExtent.offset == 81, "Incorrect interestLifetime offset, expected %u got %u", 81, lifetimeExtent.offset);
    assertTrue(lifetimeExtent.length == 2, "Incorrect interestLifetime length, expected %u got %u", 2, lifetimeExtent.length);
}

static void
verifyObjectPerHop(MetisTlvSkeleton *skeleton)
{
    MetisTlvExtent hoplimitExtent = metisTlvSkeleton_GetHopLimit(skeleton);
    assertTrue(hoplimitExtent.offset == 12, "Incorrect hopLimit offset, expected %u got %u", 12, hoplimitExtent.offset);
    assertTrue(hoplimitExtent.length == 1, "Incorrect hopLimit length, expected %u got %u", 1, hoplimitExtent.length);
}

static void
verifyObjectSkeleton(MetisTlvSkeleton *skeleton)
{
    MetisTlvExtent nameExtent = metisTlvSkeleton_GetName(skeleton);
    assertTrue(nameExtent.offset == metisTestDataV0_EncodedObject_name.offset, "Incorrect name offset, expected %u got %u", metisTestDataV0_EncodedObject_name.offset, nameExtent.offset);
    assertTrue(nameExtent.length == metisTestDataV0_EncodedObject_name.length, "Incorrect name length, expected %u got %u", metisTestDataV0_EncodedObject_name.length, nameExtent.length);

    MetisTlvExtent keyidExtent = metisTlvSkeleton_GetKeyId(skeleton);
    assertTrue(keyidExtent.offset == metisTestDataV0_EncodedObject_keyid.offset, "Incorrect keyId offset, expected %u got %u", metisTestDataV0_EncodedObject_keyid.offset, keyidExtent.offset);
    assertTrue(keyidExtent.length == metisTestDataV0_EncodedObject_keyid.length, "Incorrect keyId length, expected %u got %u", metisTestDataV0_EncodedObject_keyid.length, keyidExtent.length);
}

LONGBOW_TEST_RUNNER(metis_TlvSchemaV0)
{
    // The following Test Fixtures will run their corresponding Test Cases.
    // Test Fixtures are run in the order specified, but all tests should be idempotent.
    // Never rely on the execution order of tests or share state between them.
    LONGBOW_RUN_TEST_FIXTURE(Global);
    LONGBOW_RUN_TEST_FIXTURE(Local);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(metis_TlvSchemaV0)
{
    parcMemory_SetInterface(&PARCSafeMemoryAsPARCMemory);
    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(metis_TlvSchemaV0)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, metisTlvSchemaV0_ComputeContentObjectHash);
    LONGBOW_RUN_TEST_CASE(Global, metisTlvSchemaV0_Skeleton_Interest);
    LONGBOW_RUN_TEST_CASE(Global, metisTlvSchemaV0_Skeleton_Object);
    LONGBOW_RUN_TEST_CASE(Global, metisTlvSchemaV0_Skeleton_Control);

    LONGBOW_RUN_TEST_CASE(Global, metisTlvSchemaV0_IsPacketTypeInterest_True);
    LONGBOW_RUN_TEST_CASE(Global, metisTlvSchemaV0_IsPacketTypeContentObject_True);
    LONGBOW_RUN_TEST_CASE(Global, metisTlvSchemaV0_IsPacketTypeInterest_False);
    LONGBOW_RUN_TEST_CASE(Global, metisTlvSchemaV0_IsPacketTypeContentObject_False);

    LONGBOW_RUN_TEST_CASE(Global, metisTlvSchemaV0_EncodeControlPlaneInformation);
}

LONGBOW_TEST_FIXTURE_SETUP(Global)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(Global)
{
    uint32_t outstandingAllocations = parcSafeMemory_ReportAllocation(STDERR_FILENO);
    if (outstandingAllocations != 0) {
        printf("%s leaks memory by %d allocations\n", longBowTestCase_GetName(testCase), outstandingAllocations);
        return LONGBOW_STATUS_MEMORYLEAK;
    }
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_CASE(Global, metisTlvSchemaV0_ComputeContentObjectHash)
{
    _MetisTlvFixedHeaderV0 *hdr = (_MetisTlvFixedHeaderV0 *) metisTestDataV0_EncodedObject;
    size_t headerLength = htons(hdr->headerLength);
    size_t endHeaders = FIXED_HEADER_LEN + headerLength;
    size_t endPacket = metisTlv_TotalPacketLength((uint8_t *) hdr);

    uint8_t *start = &metisTestDataV0_EncodedObject[endHeaders];
    size_t length = endPacket - endHeaders;

    PARCCryptoHasher *hasher = parcCryptoHasher_Create(PARC_HASH_SHA256);
    parcCryptoHasher_Init(hasher);
    parcCryptoHasher_UpdateBytes(hasher, start, length);

    PARCCryptoHash *hash_truth = parcCryptoHasher_Finalize(hasher);

    PARCCryptoHash *hash_test = _computeContentObjectHash(metisTestDataV0_EncodedObject);

    assertTrue(parcCryptoHash_Equals(hash_truth, hash_test),
               "Content object digests did not match: truth %s test %s",
               parcBuffer_ToString(parcCryptoHash_GetDigest(hash_truth)),
               parcBuffer_ToString(parcCryptoHash_GetDigest(hash_test)));

    parcCryptoHash_Release(&hash_truth);
    parcCryptoHash_Release(&hash_test);
    parcCryptoHasher_Release(&hasher);
}

LONGBOW_TEST_CASE(Global, metisTlvSchemaV0_Skeleton_Interest)
{
    MetisTlvSkeleton opaque;
    _InternalSkeleton *skeleton = (_InternalSkeleton *) &opaque;
    PARCLogReporter *reporter = parcLogReporterTextStdout_Create();
    MetisLogger *logger = metisLogger_Create(reporter, parcClock_Wallclock());
    parcLogReporter_Release(&reporter);
    _initialize(skeleton, &MetisTlvSchemaV0_Ops, metisTestDataV0_EncodedInterest, logger);
    _parse(&opaque);
    verifyInterestPerHop(&opaque);
    verifyInterestSkeleton(&opaque);
    metisLogger_Release(&logger);
}

LONGBOW_TEST_CASE(Global, metisTlvSchemaV0_Skeleton_Object)
{
    MetisTlvSkeleton opaque;
    _InternalSkeleton *skeleton = (_InternalSkeleton *) &opaque;
    PARCLogReporter *reporter = parcLogReporterTextStdout_Create();
    MetisLogger *logger = metisLogger_Create(reporter, parcClock_Wallclock());
    parcLogReporter_Release(&reporter);
    _initialize(skeleton, &MetisTlvSchemaV0_Ops, metisTestDataV0_EncodedObject, logger);
    _parse(&opaque);
    verifyObjectPerHop(&opaque);
    verifyObjectSkeleton(&opaque);
    metisLogger_Release(&logger);
}

LONGBOW_TEST_CASE(Global, metisTlvSchemaV0_Skeleton_Control)
{
    MetisTlvSkeleton opaque;
    _InternalSkeleton *skeleton = (_InternalSkeleton *) &opaque;
    PARCLogReporter *reporter = parcLogReporterTextStdout_Create();
    MetisLogger *logger = metisLogger_Create(reporter, parcClock_Wallclock());
    parcLogReporter_Release(&reporter);
    _initialize(skeleton, &MetisTlvSchemaV0_Ops, metisTestDataV0_CPIMessage, logger);
    _parse(&opaque);

    MetisTlvExtent cpiExtent = metisTlvSkeleton_GetCPI(&opaque);
    assertTrue(cpiExtent.offset == 12, "cpi offset wrong, got %u expected %u", cpiExtent.offset, 12);
    assertTrue(cpiExtent.length == 47, "cpi length wrong, got %u expected %u", cpiExtent.length, 47);
    metisLogger_Release(&logger);
}

LONGBOW_TEST_CASE(Global, metisTlvSchemaV0_IsPacketTypeInterest_True)
{
    bool result = _isPacketTypeInterest(metisTestDataV0_EncodedInterest);
    assertTrue(result, "Interest packet type did not return true for IsInterest test");
}

LONGBOW_TEST_CASE(Global, metisTlvSchemaV0_IsPacketTypeContentObject_True)
{
    bool result = _isPacketTypeContentObject(metisTestDataV0_EncodedObject);
    assertTrue(result, "ContentObject packet type did not return true for IsContentObject test");
}

LONGBOW_TEST_CASE(Global, metisTlvSchemaV0_IsPacketTypeInterest_False)
{
    bool result = _isPacketTypeInterest(metisTestDataV0_EncodedObject);
    assertFalse(result, "ContentObject packet type did not return false for IsInterest test");
}

LONGBOW_TEST_CASE(Global, metisTlvSchemaV0_IsPacketTypeContentObject_False)
{
    bool result = _isPacketTypeContentObject(metisTestDataV0_EncodedInterest);
    assertFalse(result, "Interest packet type did not return false for IsContentObject test");
}

LONGBOW_TEST_CASE(Global, metisTlvSchemaV0_EncodeControlPlaneInformation)
{
    CCNxControl *control = ccnxControl_CreateRouteListRequest();
    PARCBuffer *buffer = _encodeControlPlaneInformation(control);
    PARCBuffer *truth = parcBuffer_Flip(parcBuffer_PutArray(parcBuffer_Allocate(sizeof(metisTestDataV0_CPIMessage)), sizeof(metisTestDataV0_CPIMessage), metisTestDataV0_CPIMessage));

    assertTrue(parcBuffer_Equals(truth, buffer), "Buffers not equal")
    {
        printf("expected:\n");
        parcBuffer_Display(truth, 3);
        printf("got:\n");
        parcBuffer_Display(buffer, 3);
    }
    ccnxControl_Release(&control);
    parcBuffer_Release(&truth);
    parcBuffer_Release(&buffer);
}


// ======================================================

LONGBOW_TEST_FIXTURE(Local)
{
    // computeHash and parseName Auth are tested called through by other tests

    LONGBOW_RUN_TEST_CASE(Local, _parseInterestV0);
    LONGBOW_RUN_TEST_CASE(Local, _parseObjectV0);
    LONGBOW_RUN_TEST_CASE(Local, _parsePerHopV0_Interest);
    LONGBOW_RUN_TEST_CASE(Local, _parsePerHopV0_Object);
}

LONGBOW_TEST_FIXTURE_SETUP(Local)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(Local)
{
    uint32_t outstandingAllocations = parcSafeMemory_ReportAllocation(STDERR_FILENO);
    if (outstandingAllocations != 0) {
        printf("%s leaks memory by %d allocations\n", longBowTestCase_GetName(testCase), outstandingAllocations);
        return LONGBOW_STATUS_MEMORYLEAK;
    }
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_CASE(Local, _parseInterestV0)
{
    MetisTlvSkeleton skeleton;
    memset(&skeleton, 0, sizeof(skeleton));
    PARCLogReporter *reporter = parcLogReporterTextStdout_Create();
    MetisLogger *logger = metisLogger_Create(reporter, parcClock_Wallclock());
    parcLogReporter_Release(&reporter);
    _initialize((_InternalSkeleton *) &skeleton, &MetisTlvSchemaV0_Ops, metisTestDataV0_EncodedInterest, logger);

    _MetisTlvFixedHeaderV0 *hdr = (_MetisTlvFixedHeaderV0 *) metisTestDataV0_EncodedInterest;
    size_t headerLength = htons(hdr->headerLength);
    size_t endHeaders = FIXED_HEADER_LEN + headerLength;
    size_t endPacket = metisTlv_TotalPacketLength((uint8_t *) hdr);

    _parseInterestV0(metisTestDataV0_EncodedInterest, endHeaders, endPacket, &skeleton);
    verifyInterestSkeleton(&skeleton);

    metisLogger_Release(&logger);
}

LONGBOW_TEST_CASE(Local, _parseObjectV0)
{
    MetisTlvSkeleton skeleton;
    memset(&skeleton, 0, sizeof(skeleton));
    PARCLogReporter *reporter = parcLogReporterTextStdout_Create();
    MetisLogger *logger = metisLogger_Create(reporter, parcClock_Wallclock());
    parcLogReporter_Release(&reporter);
    _initialize((_InternalSkeleton *) &skeleton, &MetisTlvSchemaV0_Ops, metisTestDataV0_EncodedInterest, logger);

    _MetisTlvFixedHeaderV0 *hdr = (_MetisTlvFixedHeaderV0 *) metisTestDataV0_EncodedObject;
    size_t headerLength = htons(hdr->headerLength);
    size_t endHeaders = FIXED_HEADER_LEN + headerLength;
    size_t endPacket = metisTlv_TotalPacketLength((uint8_t *) hdr);

    _parseObjectV0(metisTestDataV0_EncodedObject, endHeaders, endPacket, &skeleton);
    verifyObjectSkeleton(&skeleton);
    metisLogger_Release(&logger);
}

LONGBOW_TEST_CASE(Local, _parsePerHopV0_Interest)
{
    MetisTlvSkeleton skeleton;
    memset(&skeleton, 0, sizeof(skeleton));
    PARCLogReporter *reporter = parcLogReporterTextStdout_Create();
    MetisLogger *logger = metisLogger_Create(reporter, parcClock_Wallclock());
    parcLogReporter_Release(&reporter);
    _initialize((_InternalSkeleton *) &skeleton, &MetisTlvSchemaV0_Ops, metisTestDataV0_EncodedInterest, logger);

    _parsePerHopV0(metisTestDataV0_EncodedInterest, 8, 29, &skeleton);
    verifyInterestPerHop(&skeleton);
    metisLogger_Release(&logger);
}

LONGBOW_TEST_CASE(Local, _parsePerHopV0_Object)
{
    MetisTlvSkeleton skeleton;
    memset(&skeleton, 0, sizeof(skeleton));
    PARCLogReporter *reporter = parcLogReporterTextStdout_Create();
    MetisLogger *logger = metisLogger_Create(reporter, parcClock_Wallclock());
    parcLogReporter_Release(&reporter);
    _initialize((_InternalSkeleton *) &skeleton, &MetisTlvSchemaV0_Ops, metisTestDataV0_EncodedInterest, logger);

    _parsePerHopV0(metisTestDataV0_EncodedObject, 8, 29, &skeleton);
    verifyObjectPerHop(&skeleton);
    metisLogger_Release(&logger);
}

int
main(int argc, char *argv[])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(metis_TlvSchemaV0);
    int exitStatus = longBowMain(argc, argv, testRunner, NULL);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}
