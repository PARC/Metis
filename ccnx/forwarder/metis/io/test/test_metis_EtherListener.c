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

// force the use of the generic ethernet mockup
#include "testrig_GenericEther.c"
#include "../metis_EtherListener.c"
#include "testrig_GenericEther.h"

#include <parc/algol/parc_SafeMemory.h>

#include <LongBow/unit-test.h>

#include <ccnx/forwarder/metis/testdata/metis_TestDataV1.h>


typedef struct test_data {
    MetisForwarder *metis;
    MetisListenerOps *ops;
} TestData;

static void
commonSetup(const LongBowTestCase *testCase, uint16_t ethertype)
{
    TestData *data = parcMemory_AllocateAndClear(sizeof(TestData));

    data->metis = metisForwarder_Create(NULL);
    data->ops = metisEtherListener_Create(data->metis, "test0", ethertype);

    // crank the libevent handle
    metisDispatcher_RunDuration(metisForwarder_GetDispatcher(data->metis), &((struct timeval) {0, 10000}));

    longBowTestCase_SetClipBoardData(testCase, data);
}

static void
commonTeardown(const LongBowTestCase *testCase)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);

    metisDispatcher_RunDuration(metisForwarder_GetDispatcher(data->metis), &((struct timeval) {0, 10000}));
    data->ops->destroy(&data->ops);
    metisForwarder_Destroy(&data->metis);

    parcMemory_Deallocate((void **) &data);
}

// ============================================================

LONGBOW_TEST_RUNNER(metis_EtherListener)
{
    // The following Test Fixtures will run their corresponding Test Cases.
    // Test Fixtures are run in the order specified, but all tests should be idempotent.
    // Never rely on the execution order of tests or share state between them.
    LONGBOW_RUN_TEST_FIXTURE(Global);
    LONGBOW_RUN_TEST_FIXTURE(Local);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(metis_EtherListener)
{
    parcMemory_SetInterface(&PARCSafeMemoryAsPARCMemory);
    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(metis_EtherListener)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

// ============================================================

LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, metisEtherListener_Create);
}

LONGBOW_TEST_FIXTURE_SETUP(Global)
{
    commonSetup(testCase, 0x0801);
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(Global)
{
    commonTeardown(testCase);
    uint32_t outstandingAllocations = parcSafeMemory_ReportAllocation(STDERR_FILENO);
    if (outstandingAllocations != 0) {
        printf("%s leaks memory by %d allocations\n", longBowTestCase_GetName(testCase), outstandingAllocations);
        return LONGBOW_STATUS_MEMORYLEAK;
    }
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_CASE(Global, metisEtherListener_Create)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    assertNotNull(data->ops, "Null return from metisEtherListener_Create");
}

// ============================================================

LONGBOW_TEST_FIXTURE(Local)
{
    LONGBOW_RUN_TEST_CASE(Local, metisEtherListener_Destroy);
    LONGBOW_RUN_TEST_CASE(Local, metisEtherListener_OpsDestroy);
    LONGBOW_RUN_TEST_CASE(Local, metisEtherListener_OpsGetInterfaceIndex);
    LONGBOW_RUN_TEST_CASE(Local, metisEtherListener_OpsGetListenAddress);
    LONGBOW_RUN_TEST_CASE(Local, metisEtherListener_OpsGetEncapType);

    LONGBOW_RUN_TEST_CASE(Local, _metisEtherListener_ReadCallback);
    LONGBOW_RUN_TEST_CASE(Local, _metisEtherListener_ReadCallback_FragmentBegin);
    LONGBOW_RUN_TEST_CASE(Local, _metisEtherListener_ReadCallback_FragmentEnd);


    LONGBOW_RUN_TEST_CASE(Local, _metisEtherListener_ReadEtherFrame_EmptyQueue);
    LONGBOW_RUN_TEST_CASE(Local, _metisEtherListener_ReadEtherFrame_PacketWaiting);

    LONGBOW_RUN_TEST_CASE(Local, _metisEtherListener_IsOurSourceAddress_True);
    LONGBOW_RUN_TEST_CASE(Local, _metisEtherListener_IsOurSourceAddress_False);
    LONGBOW_RUN_TEST_CASE(Local, _metisEtherListener_IsOurDestinationAddress_Unicast);
    LONGBOW_RUN_TEST_CASE(Local, _metisEtherListener_IsOurDestinationAddress_Group);
    LONGBOW_RUN_TEST_CASE(Local, _metisEtherListener_IsOurDestinationAddress_Broadcast);
    LONGBOW_RUN_TEST_CASE(Local, _metisEtherListener_IsOurDestinationAddress_False);
    LONGBOW_RUN_TEST_CASE(Local, _metisEtherListener_IsOurProtocol);
    LONGBOW_RUN_TEST_CASE(Local, _metisEtherListener_ParseEtherFrame);


    LONGBOW_RUN_TEST_CASE(Local, _metisEtherListener_FillInEthernetAddresses);
    LONGBOW_RUN_TEST_CASE(Local, _metisEtherListener_ReleaseEthernetAddresses);
}

LONGBOW_TEST_FIXTURE_SETUP(Local)
{
    commonSetup(testCase, 0x0801);
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(Local)
{
    commonTeardown(testCase);
    uint32_t outstandingAllocations = parcSafeMemory_ReportAllocation(STDERR_FILENO);
    if (outstandingAllocations != 0) {
        printf("%s leaks memory by %d allocations\n", longBowTestCase_GetName(testCase), outstandingAllocations);
        return LONGBOW_STATUS_MEMORYLEAK;
    }
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_CASE(Local, metisEtherListener_Destroy)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    MetisListenerOps *ops = metisEtherListener_Create(data->metis, "fake0", 0x0801);

    _metisEtherListener_Destroy((_MetisEtherListener **) &ops->context);
    assertNull(ops->context, "Destory did not null context");
    parcMemory_Deallocate((void **) &ops);
}

LONGBOW_TEST_CASE(Local, metisEtherListener_OpsDestroy)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    MetisListenerOps *ops = metisEtherListener_Create(data->metis, "fake1", 0x0801);
    _metisEtherListener_OpsDestroy(&ops);
    assertNull(ops, "OpsDestroy did not null ops");
}

LONGBOW_TEST_CASE(Local, metisEtherListener_OpsGetInterfaceIndex)
{
    testUnimplemented("");
}

LONGBOW_TEST_CASE(Local, metisEtherListener_OpsGetListenAddress)
{
    testUnimplemented("");
}

LONGBOW_TEST_CASE(Local, metisEtherListener_OpsGetEncapType)
{
    testUnimplemented("");
}

LONGBOW_TEST_CASE(Local, _metisEtherListener_ReadCallback)
{
    testUnimplemented("");
}

/*
 * Read only a B frame, so its not a complete reassembly
 */
LONGBOW_TEST_CASE(Local, _metisEtherListener_ReadCallback_FragmentBegin)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    _MetisEtherListener *etherListener = data->ops->context;

    uint8_t headerArray[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0x08, 0x01 };

    PARCBuffer *frameBuffer = parcBuffer_Allocate(sizeof(struct ether_header) + sizeof(metisTestDataV1_HopByHopFrag_Begin));
    parcBuffer_PutArray(frameBuffer, sizeof(struct ether_header), headerArray);
    parcBuffer_PutArray(frameBuffer, sizeof(metisTestDataV1_HopByHopFrag_Begin), metisTestDataV1_HopByHopFrag_Begin);

    parcBuffer_Flip(frameBuffer);

    mockGenericEther_QueueFrame(etherListener->genericEther, frameBuffer);

    _metisEtherListener_ReadCallback(0, PARCEventType_Read, data->ops->context);

    assertTrue(etherListener->stats.framesIn == 1, "Wrong framesIn count, expected 1 got %" PRIu64, etherListener->stats.framesIn);
    assertTrue(etherListener->stats.framesReceived == 1, "Wrong framesReceived count, expected 1 got %" PRIu64, etherListener->stats.framesReceived);
    assertTrue(etherListener->stats.framesReassembled == 0, "Wrong framesReassembled count, expected 0 got %" PRIu64, etherListener->stats.framesReassembled);

    parcBuffer_Release(&frameBuffer);
}

/*
 * Read a B and middle and E frame, so it is complete reassembly
 */
LONGBOW_TEST_CASE(Local, _metisEtherListener_ReadCallback_FragmentEnd)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    _MetisEtherListener *etherListener = data->ops->context;

    uint8_t headerArray[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0x08, 0x01 };

    PARCBuffer *frameBuffer = parcBuffer_Allocate(sizeof(struct ether_header) + sizeof(metisTestDataV1_HopByHopFrag_BeginEnd));
    parcBuffer_PutArray(frameBuffer, sizeof(struct ether_header), headerArray);
    parcBuffer_PutArray(frameBuffer, sizeof(metisTestDataV1_HopByHopFrag_BeginEnd), metisTestDataV1_HopByHopFrag_BeginEnd);

    parcBuffer_Flip(frameBuffer);

    mockGenericEther_QueueFrame(etherListener->genericEther, frameBuffer);

    _metisEtherListener_ReadCallback(0, PARCEventType_Read, data->ops->context);

    assertTrue(etherListener->stats.framesIn == 1, "Wrong framesIn count, expected 1 got %" PRIu64, etherListener->stats.framesIn);
    assertTrue(etherListener->stats.framesReceived == 1, "Wrong framesReceived count, expected 1 got %" PRIu64, etherListener->stats.framesReceived);
    assertTrue(etherListener->stats.framesReassembled == 1, "Wrong framesReassembled count, expected 1 got %" PRIu64, etherListener->stats.framesReassembled);

    parcBuffer_Release(&frameBuffer);
}

LONGBOW_TEST_CASE(Local, _metisEtherListener_ReadEtherFrame_PacketWaiting)
{
    // create a frame and queue it
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    _MetisEtherListener *etherListener = data->ops->context;

    // Ethernet frame addressed to us with a 0-length CCNx TLV packet (i.e. just the fixed header)
    uint8_t frame[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 't',  'e', 's', 't', '0', 0xA0, 0x08, 0x01,
                        0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00 };

    PARCBuffer *frameBuffer = parcBuffer_Wrap(frame, sizeof(frame), 0, sizeof(frame));

    mockGenericEther_QueueFrame(etherListener->genericEther, frameBuffer);

    PARCEventBuffer *buffer = _metisEtherListener_ReadEtherFrame(etherListener);
    assertNotNull(buffer, "Got null buffer from ReadEtherFrame with a frame queued");

    assertTrue(parcEventBuffer_GetLength(buffer) == sizeof(frame), "Wrong size, got %zu expected %zu", parcEventBuffer_GetLength(buffer), sizeof(frame));

    parcEventBuffer_Destroy(&buffer);
    parcBuffer_Release(&frameBuffer);
}


LONGBOW_TEST_CASE(Local, _metisEtherListener_ReadEtherFrame_EmptyQueue)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    _MetisEtherListener *etherListener = data->ops->context;
    PARCEventBuffer *buffer = _metisEtherListener_ReadEtherFrame(etherListener);
    assertNull(buffer, "Should get null buffer from ReadEtherFrame without a frame queued");
}

LONGBOW_TEST_CASE(Local, _metisEtherListener_IsOurSourceAddress_True)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    _MetisEtherListener *etherListener = data->ops->context;

    uint8_t frame[] = { 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 't', 'e', 's', 't', '0', 0x06, 0x08, 0x01 };
    struct ether_header *header = (struct ether_header *) frame;

    bool success = _metisEtherListener_IsOurSourceAddress(etherListener, header);
    assertTrue(success, "Did not match our source address.");
}

LONGBOW_TEST_CASE(Local, _metisEtherListener_IsOurSourceAddress_False)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    _MetisEtherListener *etherListener = data->ops->context;

    uint8_t frame[] = { 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0x11, 0x22, 0x33, 0x44, 0x05, 0x06, 0x08, 0x01 };
    struct ether_header *header = (struct ether_header *) frame;

    bool success = _metisEtherListener_IsOurSourceAddress(etherListener, header);
    assertFalse(success, "Should not match our address.");
}

LONGBOW_TEST_CASE(Local, _metisEtherListener_IsOurDestinationAddress_Unicast)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    _MetisEtherListener *etherListener = data->ops->context;

    uint8_t frame[] = { 't', 'e', 's', 't', '0', 0x06, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0x08, 0x01 };
    struct ether_header *header = (struct ether_header *) frame;

    bool success = _metisEtherListener_IsOurDestinationAddress(etherListener, header);
    assertTrue(success, "Did not match our destination address.");
}

LONGBOW_TEST_CASE(Local, _metisEtherListener_IsOurDestinationAddress_Group)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    _MetisEtherListener *etherListener = data->ops->context;

    uint8_t frame[] = { 0x01, 0x00, 0x5e, 0x00, 0x17, 0xaa, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0x08, 0x01 };
    struct ether_header *header = (struct ether_header *) frame;

    bool success = _metisEtherListener_IsOurDestinationAddress(etherListener, header);
    assertTrue(success, "Did not match group address.");
}

LONGBOW_TEST_CASE(Local, _metisEtherListener_IsOurDestinationAddress_Broadcast)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    _MetisEtherListener *etherListener = data->ops->context;

    uint8_t frame[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0x08, 0x01 };
    struct ether_header *header = (struct ether_header *) frame;

    bool success = _metisEtherListener_IsOurDestinationAddress(etherListener, header);
    assertTrue(success, "Did not match broadcast address.");
}

LONGBOW_TEST_CASE(Local, _metisEtherListener_IsOurDestinationAddress_False)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    _MetisEtherListener *etherListener = data->ops->context;

    uint8_t frame[] = { 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0x08, 0x01 };
    struct ether_header *header = (struct ether_header *) frame;

    bool success = _metisEtherListener_IsOurDestinationAddress(etherListener, header);
    assertFalse(success, "Should not match one of our addresses.");
}


LONGBOW_TEST_CASE(Local, _metisEtherListener_IsOurProtocol)
{
    testUnimplemented("");
}

LONGBOW_TEST_CASE(Local, _metisEtherListener_ParseEtherFrame)
{
    testUnimplemented("");
}

LONGBOW_TEST_CASE(Local, _metisEtherListener_FillInEthernetAddresses)
{
    testUnimplemented("");
}

LONGBOW_TEST_CASE(Local, _metisEtherListener_ReleaseEthernetAddresses)
{
    testUnimplemented("");
}

int
main(int argc, char *argv[])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(metis_EtherListener);
    int exitStatus = longBowMain(argc, argv, testRunner, NULL);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}
