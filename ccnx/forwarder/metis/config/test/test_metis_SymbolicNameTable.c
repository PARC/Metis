/*
 * Copyright (c) 2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC)
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
 * @copyright (c) 2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

// Include the file(s) containing the functions to be tested.
// This permits internal static functions to be visible to this Test Framework.
#include "../metis_SymbolicNameTable.c"

#include <stdio.h>
#include <parc/algol/parc_SafeMemory.h>
#include <LongBow/unit-test.h>

LONGBOW_TEST_RUNNER(metis_SymbolicNameTable)
{
    LONGBOW_RUN_TEST_FIXTURE(Global);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(metis_SymbolicNameTable)
{
    parcMemory_SetInterface(&PARCSafeMemoryAsPARCMemory);
    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(metis_SymbolicNameTable)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

// ==============================================================

LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, metisSymbolicNameTable_Create);
    LONGBOW_RUN_TEST_CASE(Global, metisSymbolicNameTable_Exists_True);
    LONGBOW_RUN_TEST_CASE(Global, metisSymbolicNameTable_Exists_False);
    LONGBOW_RUN_TEST_CASE(Global, metisSymbolicNameTable_Add_Unique);
    LONGBOW_RUN_TEST_CASE(Global, metisSymbolicNameTable_Add_Duplicate);
    LONGBOW_RUN_TEST_CASE(Global, metisSymbolicNameTable_Get_Exists);
    LONGBOW_RUN_TEST_CASE(Global, metisSymbolicNameTable_Get_Missing);
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

LONGBOW_TEST_CASE(Global, metisSymbolicNameTable_Create)
{
    MetisSymbolicNameTable *table = metisSymbolicNameTable_Create();
    assertNotNull(table, "Got null table");
    assertNotNull(table->symbolicNameTable, "Table did not have an inner hash table allocated");
    metisSymbolicNameTable_Destroy(&table);
}

LONGBOW_TEST_CASE(Global, metisSymbolicNameTable_Exists_True)
{
    MetisSymbolicNameTable *table = metisSymbolicNameTable_Create();
    metisSymbolicNameTable_Add(table, "foo", 3);
    bool exists = metisSymbolicNameTable_Exists(table, "foo");
    assertTrue(exists, "Failed to find existing key");
    metisSymbolicNameTable_Destroy(&table);
}

LONGBOW_TEST_CASE(Global, metisSymbolicNameTable_Exists_False)
{
    MetisSymbolicNameTable *table = metisSymbolicNameTable_Create();
    bool exists = metisSymbolicNameTable_Exists(table, "foo");
    assertFalse(exists, "Found non-existent key!");
    metisSymbolicNameTable_Destroy(&table);
}

LONGBOW_TEST_CASE(Global, metisSymbolicNameTable_Add_Unique)
{
    MetisSymbolicNameTable *table = metisSymbolicNameTable_Create();
    bool success = metisSymbolicNameTable_Add(table, "foo", 3);
    assertTrue(success, "Failed to add a unique key");
    metisSymbolicNameTable_Destroy(&table);
}

LONGBOW_TEST_CASE(Global, metisSymbolicNameTable_Add_Duplicate)
{
    MetisSymbolicNameTable *table = metisSymbolicNameTable_Create();
    metisSymbolicNameTable_Add(table, "foo", 3);
    bool failure = metisSymbolicNameTable_Add(table, "foo", 4);
    assertFalse(failure, "Should have failed to add a duplicate key");
    metisSymbolicNameTable_Destroy(&table);
}

LONGBOW_TEST_CASE(Global, metisSymbolicNameTable_Get_Exists)
{
    MetisSymbolicNameTable *table = metisSymbolicNameTable_Create();
    metisSymbolicNameTable_Add(table, "foo", 3);
    unsigned value = metisSymbolicNameTable_Get(table, "foo");
    assertTrue(value == 3, "Wrong value, expected %u got %u", 3, value);
    metisSymbolicNameTable_Destroy(&table);
}

LONGBOW_TEST_CASE(Global, metisSymbolicNameTable_Get_Missing)
{
    MetisSymbolicNameTable *table = metisSymbolicNameTable_Create();
    unsigned value = metisSymbolicNameTable_Get(table, "foo");
    assertTrue(value == UINT32_MAX, "Wrong value, expected %u got %u", UINT32_MAX, value);
    metisSymbolicNameTable_Destroy(&table);
}


// ==============================================================

int
main(int argc, char *argv[])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(metis_SymbolicNameTable);
    int exitStatus = longBowMain(argc, argv, testRunner, NULL);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}
