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

/*
 * hard-coded in port 49009 on localhost
 */

// Include the file(s) containing the functions to be tested.
// This permits internal static functions to be visible to this Test Framework.
#include "../metis_TcpListener.c"
#include <LongBow/unit-test.h>
#include <ccnx/forwarder/metis/tlv/metis_Tlv.h>
#include <parc/algol/parc_SafeMemory.h>

#include <ccnx/forwarder/metis/testdata/metis_TestDataV0.h>

#include <parc/algol/parc_Network.h>

// for inet_pton
#include <arpa/inet.h>

#include <signal.h>

struct test_set {
    CPIAddress *listenAddress;
    MetisForwarder *metis;
    MetisListenerOps *ops;
} TestSet;

static void
setupInetListener()
{
    struct sockaddr_in addr;

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(49009);
    inet_pton(AF_INET, "127.0.0.1", &(addr.sin_addr));

    TestSet.metis = metisForwarder_Create(NULL);
    TestSet.ops = metisTcpListener_CreateInet(TestSet.metis, addr);
    TestSet.listenAddress = cpiAddress_CreateFromInet(&addr);

    // crank the event scheduler once
    metisDispatcher_RunDuration(metisForwarder_GetDispatcher(TestSet.metis), &((struct timeval) {0, 10000}));
}

static void
setupInet6Listener()
{
    struct sockaddr_in6 addr;

    memset(&addr, 0, sizeof(addr));
    addr.sin6_family = AF_INET6;
    addr.sin6_port = htons(49009);

    // "::1" is the ipv6 loopback address
    inet_pton(AF_INET6, "::1", &(addr.sin6_addr));

    TestSet.metis = metisForwarder_Create(NULL);
    TestSet.ops = metisTcpListener_CreateInet6(TestSet.metis, addr);
    TestSet.listenAddress = cpiAddress_CreateFromInet6(&addr);

    // crank the event scheduler once
    metisDispatcher_RunDuration(metisForwarder_GetDispatcher(TestSet.metis), &((struct timeval) {0, 10000}));
}

static void
teardownListener()
{
    cpiAddress_Destroy(&TestSet.listenAddress);
    TestSet.ops->destroy(&TestSet.ops);
    metisForwarder_Destroy(&TestSet.metis);
}

struct sigaction save_sigchld;
struct sigaction save_sigpipe;

static void
blockSigChild()
{
    struct sigaction ignore_action;
    ignore_action.sa_handler = SIG_IGN;
    sigemptyset(&ignore_action.sa_mask);
    ignore_action.sa_flags = 0;

    sigaction(SIGCHLD, NULL, &save_sigchld);
    sigaction(SIGPIPE, NULL, &save_sigpipe);

    sigaction(SIGCHLD, &ignore_action, NULL);
    sigaction(SIGPIPE, &ignore_action, NULL);
}

static void
unblockSigChild()
{
    sigaction(SIGCHLD, &save_sigchld, NULL);
    sigaction(SIGPIPE, &save_sigpipe, NULL);
}

LONGBOW_TEST_RUNNER(metis_TcpListener)
{
    // The following Test Fixtures will run their corresponding Test Cases.
    // Test Fixtures are run in the order specified, but all tests should be idempotent.
    // Never rely on the execution order of tests or share state between them.
    LONGBOW_RUN_TEST_FIXTURE(Global_Inet);
    LONGBOW_RUN_TEST_FIXTURE(Global_Inet6);
    LONGBOW_RUN_TEST_FIXTURE(Local);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(metis_TcpListener)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(metis_TcpListener)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

// ===========================================================================

LONGBOW_TEST_FIXTURE(Global_Inet)
{
    LONGBOW_RUN_TEST_CASE(Global_Inet, metisListenerTcp_CreateInet);
    LONGBOW_RUN_TEST_CASE(Global_Inet, metisListenerTcp_Connect);
    LONGBOW_RUN_TEST_CASE(Global_Inet, metisListenerTcp_SendPacket);
}

LONGBOW_TEST_FIXTURE_SETUP(Global_Inet)
{
    setupInetListener();
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(Global_Inet)
{
    teardownListener();
    if (parcSafeMemory_ReportAllocation(STDOUT_FILENO) != 0) {
        return LONGBOW_STATUS_MEMORYLEAK;
    }
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_CASE(Global_Inet, metisListenerTcp_CreateInet)
{
    // now verify that we are listening
    // tcp4       0      0  127.0.0.1.49009        *.*                    LISTEN

    blockSigChild();
    FILE *fp = popen("netstat -an -p tcp", "r");
    assertNotNull(fp, "Got null opening netstat for reading");

    char str[1035];
    bool found = false;
    while (fgets(str, sizeof(str) - 1, fp) != NULL) {
        if (strstr(str, "127.0.0.1.49009") != NULL) {
            found = true;
            break;
        }
        if (strstr(str, "127.0.0.1:49009") != NULL) {
            found = true;
            break;
        }
    }

    pclose(fp);
    unblockSigChild();

    if (!found) {
        int ret = system("netstat -an -p tcp");
        assertTrue(ret > -1, "Error on system call");
    }

    assertTrue(found, "Did not find 127.0.0.1.49009 in netstat output");
}

LONGBOW_TEST_CASE(Global_Inet, metisListenerTcp_Connect)
{
    int fd = socket(PF_INET, SOCK_STREAM, 0);
    assertFalse(fd < 0, "Error on socket: (%d) %s", errno, strerror(errno));

    struct sockaddr_in serverAddress;
    cpiAddress_GetInet(TestSet.listenAddress, &serverAddress);

    int failure = connect(fd, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
    assertFalse(failure, "Error on connect: (%d) %s", errno, strerror(errno));

    metisDispatcher_RunDuration(metisForwarder_GetDispatcher(TestSet.metis), &((struct timeval) {0, 10000}));

    struct sockaddr_in connectAddress;
    socklen_t connectAddressLength = sizeof(connectAddress);
    failure = getsockname(fd, (struct sockaddr *) &connectAddress, &connectAddressLength);
    assertFalse(failure, "Error on getsockname: (%d) %s", errno, strerror(errno));
    assertTrue(connectAddressLength == sizeof(struct sockaddr_in),
               "connectAddressLength wrong size, expected %zu got %u",
               sizeof(struct sockaddr_in), connectAddressLength);

    // make sure its in the connection table
    MetisConnectionTable *table = metisForwarder_GetConnectionTable(TestSet.metis);
    CPIAddress *remote = cpiAddress_CreateFromInet(&connectAddress);
    MetisAddressPair *pair = metisAddressPair_Create(TestSet.listenAddress, remote);
    const MetisConnection *conn = metisConnectionTable_FindByAddressPair(table, pair);
    assertNotNull(conn, "Did not find connection in connection table");

    cpiAddress_Destroy(&remote);
    metisAddressPair_Release(&pair);

    close(fd);
}

LONGBOW_TEST_CASE(Global_Inet, metisListenerTcp_SendPacket)
{
    int fd = socket(PF_INET, SOCK_STREAM, 0);
    assertFalse(fd < 0, "Error on socket: (%d) %s", errno, strerror(errno));

    struct sockaddr_in serverAddress;
    cpiAddress_GetInet(TestSet.listenAddress, &serverAddress);

    int failure = connect(fd, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
    assertFalse(failure, "Error on connect: (%d) %s", errno, strerror(errno));

    metisDispatcher_RunDuration(metisForwarder_GetDispatcher(TestSet.metis), &((struct timeval) {0, 10000}));

    ssize_t write_length = write(fd, metisTestDataV0_InterestWithName, sizeof(metisTestDataV0_InterestWithName));
    assertFalse(write_length < 0, "Error on write: (%d) %s", errno, strerror(errno));
    assertTrue(write_length == sizeof(metisTestDataV0_InterestWithName), "Got partial write, expected %zu got %zd", sizeof(metisTestDataV0_InterestWithName), write_length);

    metisDispatcher_RunDuration(metisForwarder_GetDispatcher(TestSet.metis), &((struct timeval) {0, 10000}));

    close(fd);
}

// ===========================================================================

LONGBOW_TEST_FIXTURE(Global_Inet6)
{
    LONGBOW_RUN_TEST_CASE(Global_Inet6, metisListenerTcp_CreateInet6);
}

LONGBOW_TEST_FIXTURE_SETUP(Global_Inet6)
{
    setupInet6Listener();
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(Global_Inet6)
{
    teardownListener();
    if (parcSafeMemory_ReportAllocation(STDOUT_FILENO) != 0) {
        return LONGBOW_STATUS_MEMORYLEAK;
    }
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_CASE(Global_Inet6, metisListenerTcp_CreateInet6)
{
    // now verify that we are listening
    // tcp6       0      0  ::1.49009              *.*                    LISTEN

    blockSigChild();
    FILE *fp = popen("netstat -an -p tcp", "r");
    assertNotNull(fp, "Got null opening netstat for reading");

    char str[1035];
    bool found = false;
    while (fgets(str, sizeof(str) - 1, fp) != NULL) {
        if (strstr(str, "::1.49009") != NULL) {
            found = true;
            break;
        }
        if (strstr(str, "::1:49009") != NULL) {
            found = true;
            break;
        }
    }

    pclose(fp);
    unblockSigChild();

    if (!found) {
        int ret = system("netstat -an -p tcp");
        assertTrue(ret > -1, "Error on system call");
    }

    assertTrue(found, "Did not find ::1.49009 in netstat output");
}

// ===========================================================================

LONGBOW_TEST_FIXTURE(Local)
{
    LONGBOW_RUN_TEST_CASE(Local, metisListenerTcp_Listen);
}

LONGBOW_TEST_FIXTURE_SETUP(Local)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(Local)
{
    if (parcSafeMemory_ReportAllocation(STDOUT_FILENO) != 0) {
        return LONGBOW_STATUS_MEMORYLEAK;
    }
    return LONGBOW_STATUS_SUCCEEDED;
}

/**
 * Create a TCP INET listener then connect to it.
 */
LONGBOW_TEST_CASE(Local, metisListenerTcp_Listen)
{
    setupInetListener();

    struct sockaddr_in addr_remote;

    memset(&addr_remote, 0, sizeof(addr_remote));
    addr_remote.sin_family = AF_INET;
    addr_remote.sin_port = htons(49010);
    inet_pton(AF_INET, "127.0.0.1", &(addr_remote.sin_addr));

    _MetisTcpListener *tcp = (_MetisTcpListener *) TestSet.ops->context;

    int fds[2];
    int failure = socketpair(AF_LOCAL, SOCK_STREAM, 0, fds);
    assertFalse(failure, "Failed with socketpair: (%d) %s", errno, strerror(errno));

    _metisTcpListener_Listen(fds[0], (struct sockaddr *) &addr_remote, sizeof(addr_remote), tcp);

    // now verify the connection is in the connection table
    MetisConnectionTable *table = metisForwarder_GetConnectionTable(TestSet.metis);
    CPIAddress *remote = cpiAddress_CreateFromInet(&addr_remote);
    MetisAddressPair *pair = metisAddressPair_Create(tcp->localAddress, remote);
    const MetisConnection *conn = metisConnectionTable_FindByAddressPair(table, pair);
    assertNotNull(conn, "Did not find connection in connection table");

    cpiAddress_Destroy(&remote);
    metisAddressPair_Release(&pair);
    teardownListener();
}

int
main(int argc, char *argv[])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(metis_TcpListener);
    int exitStatus = longBowMain(argc, argv, testRunner, NULL);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}
