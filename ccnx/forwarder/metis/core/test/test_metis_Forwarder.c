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
#include "../metis_Forwarder.c"
#include <LongBow/unit-test.h>
#include <parc/algol/parc_SafeMemory.h>

LONGBOW_TEST_RUNNER(metis_Forwarder)
{
    // The following Test Fixtures will run their corresponding Test Cases.
    // Test Fixtures are run in the order specified, but all tests should be idempotent.
    // Never rely on the execution order of tests or share state between them.
    LONGBOW_RUN_TEST_FIXTURE(Global);
    LONGBOW_RUN_TEST_FIXTURE(Local);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(metis_Forwarder)
{
    parcMemory_SetInterface(&PARCSafeMemoryAsPARCMemory);
    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(metis_Forwarder)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, byteArrayToUnsignedLong);
    LONGBOW_RUN_TEST_CASE(Global, metisForwarder_Create);
    LONGBOW_RUN_TEST_CASE(Global, metisForwarder_Destroy);
    LONGBOW_RUN_TEST_CASE(Global, metisForwarder_GetDispatcher);
    LONGBOW_RUN_TEST_CASE(Global, metisForwarder_GetMessenger);
    LONGBOW_RUN_TEST_CASE(Global, metisForwarder_GetNextConnectionId);
    LONGBOW_RUN_TEST_CASE(Global, metisForwarder_GetTicks);
    LONGBOW_RUN_TEST_CASE(Global, metisForwarder_Log);
    LONGBOW_RUN_TEST_CASE(Global, metisForwarder_Receive);
    LONGBOW_RUN_TEST_CASE(Global, metis_run);
    LONGBOW_RUN_TEST_CASE(Global, metis_stop);


    LONGBOW_RUN_TEST_CASE(Global, metisForwarder_NanosToTicks_1sec);
    LONGBOW_RUN_TEST_CASE(Global, metisForwarder_NanosToTicks_1msec);
    LONGBOW_RUN_TEST_CASE(Global, metisForwarder_NanosToTicks_LessThanHz);

    LONGBOW_RUN_TEST_CASE(Global, metisForwarder_TicksToNanos_1sec);
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

LONGBOW_TEST_CASE(Global, byteArrayToUnsignedLong)
{
    testUnimplemented("This test is unimplemented");
}

LONGBOW_TEST_CASE(Global, metisForwarder_Create)
{
    testUnimplemented("This test is unimplemented");
}

LONGBOW_TEST_CASE(Global, metisForwarder_Destroy)
{
    testUnimplemented("This test is unimplemented");
}

LONGBOW_TEST_CASE(Global, metisForwarder_GetDispatcher)
{
    testUnimplemented("This test is unimplemented");
}

LONGBOW_TEST_CASE(Global, metisForwarder_GetMessenger)
{
    testUnimplemented("This test is unimplemented");
}

LONGBOW_TEST_CASE(Global, metisForwarder_GetNextConnectionId)
{
    testUnimplemented("This test is unimplemented");
}

LONGBOW_TEST_CASE(Global, metisForwarder_GetTicks)
{
    MetisForwarder *metis = metisForwarder_Create(NULL);
    MetisDispatcher *dispatcher = metisForwarder_GetDispatcher(metis);

    int msec = 50;
    struct timeval duration = { 0, msec * 1000 };

    // run for a bit to get things primed
    metisDispatcher_RunDuration(dispatcher, &duration);

    MetisTicks t0 = metisForwarder_GetTicks(metis);
    metisDispatcher_RunDuration(dispatcher, &duration);
    MetisTicks t1 = metisForwarder_GetTicks(metis);

    int64_t tickDelta = (int64_t) (t1 - t0);
    int64_t tickError = llabs(msec - tickDelta);

    metisForwarder_Destroy(&metis);

    printf("tickError = %" PRId64 "\n", tickError);

    // Test we're somewhere in the ballpark, betwen 40msec - 60msec
    assertTrue(tickError <= 10, "tickError %" PRId64
       " (tickDelta %" PRId64 ")",
       tickError, tickDelta);

}

LONGBOW_TEST_CASE(Global, metisForwarder_Log)
{
    testUnimplemented("This test is unimplemented");
}

LONGBOW_TEST_CASE(Global, metisForwarder_Receive)
{
    testUnimplemented("This test is unimplemented");
}

LONGBOW_TEST_CASE(Global, metis_run)
{
    testUnimplemented("This test is unimplemented");
}

LONGBOW_TEST_CASE(Global, metis_stop)
{
    testUnimplemented("This test is unimplemented");
}

LONGBOW_TEST_CASE(Global, metisForwarder_NanosToTicks_1sec)
{
    // 1 second
    uint64_t nanos = 1000000000ULL;
    MetisTicks t = metisForwarder_NanosToTicks(nanos);

    assertTrue(t == METISHZ, "1 second in nanos did not equal METISHZ, expected %d, got %" PRIu64, METISHZ, t);
}

LONGBOW_TEST_CASE(Global, metisForwarder_NanosToTicks_1msec)
{
    // 1 second
    uint64_t nanos = 1000000ULL;
    MetisTicks t = metisForwarder_NanosToTicks(nanos);
    MetisTicks expected = METISHZ / 1000;
    if (expected == 0) {
        expected = 1;
    }

    assertTrue(t == expected, "1 msec in nanos did not equal METISHZ/1000, expected %" PRIu64 ", got %" PRIu64, expected, t);
}

LONGBOW_TEST_CASE(Global, metisForwarder_NanosToTicks_LessThanHz)
{
    // 1 second
    uint64_t nanos = 1ULL;
    MetisTicks t = metisForwarder_NanosToTicks(nanos);
    MetisTicks expected = 1;

    assertTrue(t == expected, "1 nsec in nanos did not equal 1, expected %" PRIu64 ", got %" PRIu64, expected, t);
}

LONGBOW_TEST_CASE(Global, metisForwarder_TicksToNanos_1sec)
{
    MetisTicks t = METISHZ;
    uint64_t expected = ((1000000000ULL) * t / METISHZ);

    uint64_t nanos = metisForwarder_TicksToNanos(t);
    assertTrue(nanos == expected, "METISHZ ticks does not equal 1sec, expected %" PRIu64 ", got %" PRIu64, expected, nanos);
}

// ======================================================================

LONGBOW_TEST_FIXTURE(Local)
{
    LONGBOW_RUN_TEST_CASE(Local, metisForwarder_Seed);
    LONGBOW_RUN_TEST_CASE(Local, signal_cb);
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

LONGBOW_TEST_CASE(Local, metisForwarder_Seed)
{
    testUnimplemented("This test is unimplemented");
}

LONGBOW_TEST_CASE(Local, signal_cb)
{
    testUnimplemented("This test is unimplemented");
}

int
main(int argc, char *argv[])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(metis_Forwarder);
    int exitStatus = longBowMain(argc, argv, testRunner, NULL);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}
