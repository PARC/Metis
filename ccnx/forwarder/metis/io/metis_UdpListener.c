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
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <ccnx/forwarder/metis/io/metis_UdpListener.h>
#include <ccnx/forwarder/metis/io/metis_UdpConnection.h>

#include <ccnx/forwarder/metis/core/metis_Forwarder.h>
#include <ccnx/forwarder/metis/core/metis_Connection.h>
#include <ccnx/forwarder/metis/tlv/metis_Tlv.h>
#include <ccnx/forwarder/metis/core/metis_Message.h>
#include <ccnx/forwarder/metis/messenger/metis_Messenger.h>

#include <LongBow/runtime.h>
#include <parc/algol/parc_Memory.h>

typedef struct metis_udp_stats {
    uint64_t framesIn;
    uint64_t framesError;
    uint64_t framesReceived;
} _MetisUdpStats;

struct metis_udp_listener {
    MetisForwarder *metis;
    MetisLogger *logger;

    //MetisNetworkEvent *udp_event;
    PARCEvent *udp_event;
    MetisSocketType udp_socket;
    uint16_t port;

    unsigned id;
    CPIAddress *localAddress;

    _MetisUdpStats stats;
};

static void              _destroy(MetisListenerOps **listenerOpsPtr);
static unsigned          _getInterfaceIndex(const MetisListenerOps *ops);
static const CPIAddress *_getListenAddress(const MetisListenerOps *ops);
static MetisEncapType    _getEncapType(const MetisListenerOps *ops);
static int               _getSocket(const MetisListenerOps *ops);

static MetisListenerOps udpTemplate = {
    .context           = NULL,
    .destroy           = &_destroy,
    .getInterfaceIndex = &_getInterfaceIndex,
    .getListenAddress  = &_getListenAddress,
    .getEncapType      = &_getEncapType,
    .getSocket         = &_getSocket
};

static void _readcb(int fd, PARCEventType what, void *udpVoid);

MetisListenerOps *
metisUdpListener_CreateInet6(MetisForwarder *metis, struct sockaddr_in6 sin6)
{
    MetisListenerOps *ops = NULL;

    MetisUdpListener *udp = parcMemory_AllocateAndClear(sizeof(MetisUdpListener));
    assertNotNull(udp, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(MetisUdpListener));
    udp->metis = metis;
    udp->logger = metisLogger_Acquire(metisForwarder_GetLogger(metis));
    udp->localAddress = cpiAddress_CreateFromInet6(&sin6);
    udp->id = metisForwarder_GetNextConnectionId(metis);

    udp->udp_socket = socket(AF_INET6, SOCK_DGRAM, 0);
    assertFalse(udp->udp_socket < 0, "Error opening UDP socket: (%d) %s", errno, strerror(errno));

    // Set non-blocking flag
    int flags = fcntl(udp->udp_socket, F_GETFL, NULL);
    assertTrue(flags != -1, "fcntl failed to obtain file descriptor flags (%d)", errno);
    int failure = fcntl(udp->udp_socket, F_SETFL, flags | O_NONBLOCK);
    assertFalse(failure, "fcntl failed to set file descriptor flags (%d)", errno);

    int one = 1;
    // don't hang onto address after listener has closed
    failure = setsockopt(udp->udp_socket, SOL_SOCKET, SO_REUSEADDR, (void *) &one, (socklen_t) sizeof(one));
    assertFalse(failure, "failed to set REUSEADDR on socket(%d)", errno);

    failure = bind(udp->udp_socket, (struct sockaddr *) &sin6, sizeof(sin6));
    if (failure == 0) {
        udp->udp_event = metisDispatcher_CreateNetworkEvent(metisForwarder_GetDispatcher(metis), true, _readcb, (void *) udp, udp->udp_socket);
        metisDispatcher_StartNetworkEvent(metisForwarder_GetDispatcher(metis), udp->udp_event);

        ops = parcMemory_AllocateAndClear(sizeof(MetisListenerOps));
        assertNotNull(ops, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(MetisListenerOps));
        memcpy(ops, &udpTemplate, sizeof(MetisListenerOps));
        ops->context = udp;

        if (metisLogger_IsLoggable(udp->logger, MetisLoggerFacility_IO, PARCLogLevel_Debug)) {
            char *str = cpiAddress_ToString(udp->localAddress);
            metisLogger_Log(udp->logger, MetisLoggerFacility_IO, PARCLogLevel_Debug, __func__,
                            "UdpListener %p created for address %s",
                            (void *) udp, str);
            parcMemory_Deallocate((void **) &str);
        }
    } else {
        if (metisLogger_IsLoggable(udp->logger, MetisLoggerFacility_IO, PARCLogLevel_Error)) {
            int myerrno = errno;
            char *str = cpiAddress_ToString(udp->localAddress);
            metisLogger_Log(udp->logger, MetisLoggerFacility_IO, PARCLogLevel_Error, __func__,
                            "Error binding UDP socket to address %s: (%d) %s", str, myerrno, strerror(myerrno));
            parcMemory_Deallocate((void **) &str);
        }

        close(udp->udp_socket);
        cpiAddress_Destroy(&udp->localAddress);
        metisLogger_Release(&udp->logger);
        parcMemory_Deallocate((void **) &udp);
    }

    return ops;
}

MetisListenerOps *
metisUdpListener_CreateInet(MetisForwarder *metis, struct sockaddr_in sin)
{
    MetisListenerOps *ops = NULL;

    MetisUdpListener *udp = parcMemory_AllocateAndClear(sizeof(MetisUdpListener));
    assertNotNull(udp, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(MetisUdpListener));
    udp->metis = metis;
    udp->logger = metisLogger_Acquire(metisForwarder_GetLogger(metis));
    udp->localAddress = cpiAddress_CreateFromInet(&sin);
    udp->id = metisForwarder_GetNextConnectionId(metis);

    udp->udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    assertFalse(udp->udp_socket < 0, "Error opening UDP socket: (%d) %s", errno, strerror(errno));

    // Set non-blocking flag
    int flags = fcntl(udp->udp_socket, F_GETFL, NULL);
    assertTrue(flags != -1, "fcntl failed to obtain file descriptor flags (%d)", errno);
    int failure = fcntl(udp->udp_socket, F_SETFL, flags | O_NONBLOCK);
    assertFalse(failure, "fcntl failed to set file descriptor flags (%d)", errno);

    int one = 1;
    // don't hang onto address after listener has closed
    failure = setsockopt(udp->udp_socket, SOL_SOCKET, SO_REUSEADDR, (void *) &one, (socklen_t) sizeof(one));
    assertFalse(failure, "failed to set REUSEADDR on socket(%d)", errno);

    failure = bind(udp->udp_socket, (struct sockaddr *) &sin, sizeof(sin));
    if (failure == 0) {
        udp->udp_event = metisDispatcher_CreateNetworkEvent(metisForwarder_GetDispatcher(metis), true, _readcb, (void *) udp, udp->udp_socket);
        metisDispatcher_StartNetworkEvent(metisForwarder_GetDispatcher(metis), udp->udp_event);

        ops = parcMemory_AllocateAndClear(sizeof(MetisListenerOps));
        assertNotNull(ops, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(MetisListenerOps));
        memcpy(ops, &udpTemplate, sizeof(MetisListenerOps));
        ops->context = udp;

        if (metisLogger_IsLoggable(udp->logger, MetisLoggerFacility_IO, PARCLogLevel_Debug)) {
            char *str = cpiAddress_ToString(udp->localAddress);
            metisLogger_Log(udp->logger, MetisLoggerFacility_IO, PARCLogLevel_Debug, __func__,
                            "UdpListener %p created for address %s",
                            (void *) udp, str);
            parcMemory_Deallocate((void **) &str);
        }
    } else {
        if (metisLogger_IsLoggable(udp->logger, MetisLoggerFacility_IO, PARCLogLevel_Error)) {
            int myerrno = errno;
            char *str = cpiAddress_ToString(udp->localAddress);
            metisLogger_Log(udp->logger, MetisLoggerFacility_IO, PARCLogLevel_Error, __func__,
                            "Error binding UDP socket to address %s: (%d) %s", str, myerrno, strerror(myerrno));
            parcMemory_Deallocate((void **) &str);
        }

        close(udp->udp_socket);
        cpiAddress_Destroy(&udp->localAddress);
        metisLogger_Release(&udp->logger);
        parcMemory_Deallocate((void **) &udp);
    }

    return ops;
}

static void
metisUdpListener_Destroy(MetisUdpListener **listenerPtr)
{
    assertNotNull(listenerPtr, "Parameter must be non-null double pointer");
    assertNotNull(*listenerPtr, "Parameter must derefernce to non-null pointer");

    MetisUdpListener *udp = *listenerPtr;

    if (metisLogger_IsLoggable(udp->logger, MetisLoggerFacility_IO, PARCLogLevel_Debug)) {
        metisLogger_Log(udp->logger, MetisLoggerFacility_IO, PARCLogLevel_Debug, __func__,
                        "UdpListener %p destroyed",
                        (void *) udp);
    }

    close(udp->udp_socket);
    cpiAddress_Destroy(&udp->localAddress);
    metisDispatcher_DestroyNetworkEvent(metisForwarder_GetDispatcher(udp->metis), &udp->udp_event);
    metisLogger_Release(&udp->logger);
    parcMemory_Deallocate((void **) &udp);
    *listenerPtr = NULL;
}

static void
_destroy(MetisListenerOps **listenerOpsPtr)
{
    MetisListenerOps *ops = *listenerOpsPtr;
    MetisUdpListener *udp = (MetisUdpListener *) ops->context;
    metisUdpListener_Destroy(&udp);
    parcMemory_Deallocate((void **) &ops);
    *listenerOpsPtr = NULL;
}

static unsigned
_getInterfaceIndex(const MetisListenerOps *ops)
{
    MetisUdpListener *udp = (MetisUdpListener *) ops->context;
    return udp->id;
}

static const CPIAddress *
_getListenAddress(const MetisListenerOps *ops)
{
    MetisUdpListener *udp = (MetisUdpListener *) ops->context;
    return udp->localAddress;
}

static MetisEncapType
_getEncapType(const MetisListenerOps *ops)
{
    return METIS_ENCAP_UDP;
}

static int
_getSocket(const MetisListenerOps *ops)
{
    MetisUdpListener *udp = (MetisUdpListener *) ops->context;
    return (int) udp->udp_socket;
}

static void
_logStats(MetisUdpListener *udp, PARCLogLevel level)
{
    if (metisLogger_IsLoggable(udp->logger, MetisLoggerFacility_IO, level)) {
        metisLogger_Log(udp->logger, MetisLoggerFacility_IO, level, __func__,
                        "UdpListener %p frames in %" PRIu64 ", errors %" PRIu64 " ok %" PRIu64,
                        (void *) udp,
                        udp->stats.framesIn,
                        udp->stats.framesError,
                        udp->stats.framesReceived);
    }
}


// =====================================================================

/**
 * @function peekMesageLength
 * @abstract Peek at the next packet to learn its length by reading the fixed header
 * @discussion
 *   <#Discussion#>
 *
 * @param <#param1#>
 * @return <#return#>
 */
static size_t
_peekMessageLength(MetisUdpListener *udp, int fd, struct sockaddr *peerIpAddress, socklen_t *peerIpAddressLengthPtr)
{
    size_t packetLength = 0;

    uint8_t fixedHeader[metisTlv_FixedHeaderLength()];

    // peek at the UDP packet and read in the fixed header.
    // Also returns the socket information for the remote peer

    ssize_t readLength = recvfrom(fd, fixedHeader, metisTlv_FixedHeaderLength(), MSG_PEEK, (struct sockaddr *) peerIpAddress, peerIpAddressLengthPtr);

	if( readLength == -1 ){
	  return -1;
	}

    if (readLength == metisTlv_FixedHeaderLength()) {
        packetLength = metisTlv_TotalPacketLength(fixedHeader);
    } else {
        if (metisLogger_IsLoggable(udp->logger, MetisLoggerFacility_IO, PARCLogLevel_Warning)) {
            metisLogger_Log(udp->logger, MetisLoggerFacility_IO, PARCLogLevel_Warning, __func__,
                            "read %zu bytes from fd %d, wrong size for a FixedHeader",
                            readLength,
                            fd);
        }

        if (readLength < 0) {
            if (metisLogger_IsLoggable(udp->logger, MetisLoggerFacility_IO, PARCLogLevel_Error)) {
                metisLogger_Log(udp->logger, MetisLoggerFacility_IO, PARCLogLevel_Error, __func__,
                                "Error reading fd %d: (%d) %s", fd, errno, strerror(errno));
            }
        }
    }

    return packetLength;
}

static MetisMessage *
_readMessage(MetisForwarder *metis, unsigned connid, int fd, size_t packetLength)
{
    PARCEventBuffer *readbuffer = parcEventBuffer_Create();
    int readLength = parcEventBuffer_ReadFromFileDescriptor(readbuffer, fd, packetLength);

    MetisMessage *message = NULL;
    if (readLength == packetLength) {
        message = metisMessage_CreateFromBuffer(connid, metisForwarder_GetTicks(metis), readbuffer, metisForwarder_GetLogger(metis));

        // because metisMessage_CreateFromBuffer takes ownership of readbuffer, if there is
        // an error and it returns null, metisMessage_CreateFromBuffer will destroy the readbuffer.
    } else {
        parcEventBuffer_Destroy(&readbuffer);

        if (metisLogger_IsLoggable(metisForwarder_GetLogger(metis), MetisLoggerFacility_IO, PARCLogLevel_Warning)) {
            metisLogger_Log(metisForwarder_GetLogger(metis), MetisLoggerFacility_IO, PARCLogLevel_Warning, __func__,
                            "read %d bytes from fd %d, expected %zu",
                            readLength,
                            fd,
                            packetLength);
        }

        if (readLength < 0) {
            if (metisLogger_IsLoggable(metisForwarder_GetLogger(metis), MetisLoggerFacility_IO, PARCLogLevel_Error)) {
                metisLogger_Log(metisForwarder_GetLogger(metis), MetisLoggerFacility_IO, PARCLogLevel_Error, __func__,
                                "Error reading fd %d: (%d) %s", fd, errno, strerror(errno));
            }
        }
    }

    return message;
}

/**
 * @function _constructAddressPair
 * @abstract Creates the address pair that uniquely identifies the connection
 * @discussion
 *   The peerIpAddress must be of AF_INET or AF_INET6 family.
 *
 * @param <#param1#>
 * @return Allocated MetisAddressPair, must be destroyed
 */
static MetisAddressPair *
_constructAddressPair(MetisUdpListener *udp, struct sockaddr *peerIpAddress, socklen_t peerIpAddressLength)
{
    CPIAddress *remoteAddress;

    switch (peerIpAddress->sa_family) {
        case AF_INET:
            remoteAddress = cpiAddress_CreateFromInet((struct sockaddr_in *) peerIpAddress);
            break;

        case AF_INET6:
            remoteAddress = cpiAddress_CreateFromInet6((struct sockaddr_in6 *) peerIpAddress);
            break;

        default:
            trapIllegalValue(peerIpAddress, "Peer address unrecognized family for IP: %d", peerIpAddress->sa_family);
    }

    MetisAddressPair *pair = metisAddressPair_Create(udp->localAddress, remoteAddress);
    cpiAddress_Destroy(&remoteAddress);

    return pair;
}

/**
 * @function _lookupConnectionId
 * @abstract  Lookup a connection in the connection table
 * @discussion
 *   Looks up the connection in the connection table and returns the connection id if it exists.
 *
 * @param outputConnectionIdPtr is the output parameter
 * @return true if connection found and outputConnectionIdPtr set
 */
static bool
_lookupConnectionId(MetisUdpListener *udp, MetisAddressPair *pair, unsigned *outputConnectionIdPtr)
{
    MetisConnectionTable *connTable = metisForwarder_GetConnectionTable(udp->metis);

    const MetisConnection *conn = metisConnectionTable_FindByAddressPair(connTable, pair);
    if (conn) {
        *outputConnectionIdPtr = metisConnection_GetConnectionId(conn);
        return true;
    }

    return false;
}

/**
 * @function _createNewConnection
 * @abstract Creates a new Metis connection for the peer
 * @discussion
 *   PRECONDITION: you know there's not an existing connection with the address pair
 *
 *   Creates a new connection and adds it to the connection table.
 *
 * @param <#param1#>
 * @return The connection id for the new connection
 */
static unsigned
_createNewConnection(MetisUdpListener *udp, int fd, const MetisAddressPair *pair)
{
    // if peerIpAddress is localhost, it should be a local connection (case 824)
    bool isLocal = false;

    // metisUdpConnection_Create takes ownership of the pair
    MetisIoOperations *ops = metisUdpConnection_Create(udp->metis, fd, pair, isLocal);
    MetisConnection *conn = metisConnection_Create(ops);

    metisConnectionTable_Add(metisForwarder_GetConnectionTable(udp->metis), conn);
    unsigned connid = metisIoOperations_GetConnectionId(ops);

    return connid;
}

static void
_receivePacket(MetisUdpListener *udp, int fd, size_t packetLength, struct sockaddr_storage *peerIpAddress, socklen_t peerIpAddressLength)
{
    unsigned connid = 0;
    MetisAddressPair *pair = _constructAddressPair(udp, (struct sockaddr *) peerIpAddress, peerIpAddressLength);
    bool foundConnection = _lookupConnectionId(udp, pair, &connid);

    if (!foundConnection) {
        connid = _createNewConnection(udp, fd, pair);
    }

    metisAddressPair_Release(&pair);

    MetisMessage *message = _readMessage(udp->metis, connid, fd, packetLength);

    if (message) {
        udp->stats.framesReceived++;

        if (metisLogger_IsLoggable(udp->logger, MetisLoggerFacility_IO, PARCLogLevel_Debug)) {
            metisLogger_Log(udp->logger, MetisLoggerFacility_IO, PARCLogLevel_Debug, __func__,
                            "read %zu bytes from fd %d sa %s:%d connid %d",
                            metisMessage_Length(message),
                            fd,
                            inet_ntoa(((struct sockaddr_in *) peerIpAddress)->sin_addr),
                            ntohs(((struct sockaddr_in *) peerIpAddress)->sin_port),
                            connid);
        }

        _logStats(udp, PARCLogLevel_Debug);

        metisForwarder_Receive(udp->metis, message);
    } else {
        udp->stats.framesError++;
        if (metisLogger_IsLoggable(udp->logger, MetisLoggerFacility_IO, PARCLogLevel_Warning)) {
            metisLogger_Log(udp->logger, MetisLoggerFacility_IO, PARCLogLevel_Warning, __func__,
                            "Could not parse frame from fd %d, discarding", fd);
        }
        _logStats(udp, PARCLogLevel_Warning);
    }
}

static void
_readFrameToDiscard(MetisUdpListener *udp, int fd)
{
    // we need to discard the frame.  Read 1 byte.  This will clear it off the stack.
    uint8_t buffer;
    ssize_t nread = read(fd, &buffer, 1);

    udp->stats.framesError++;

    if (nread == 1) {
        if (metisLogger_IsLoggable(udp->logger, MetisLoggerFacility_IO, PARCLogLevel_Debug)) {
            metisLogger_Log(udp->logger, MetisLoggerFacility_IO, PARCLogLevel_Debug, __func__,
                            "Discarded frame from fd %d", fd);
        }
        _logStats(udp, PARCLogLevel_Debug);
    } else if (nread < 0) {
        if (metisLogger_IsLoggable(udp->logger, MetisLoggerFacility_IO, PARCLogLevel_Error)) {
            metisLogger_Log(udp->logger, MetisLoggerFacility_IO, PARCLogLevel_Error, __func__,
                            "Error trying to discard frame from fd %d: (%d) %s", fd, errno, strerror(errno));
        }
        _logStats(udp, PARCLogLevel_Error);
    }
}

static void
_readcb(int fd, PARCEventType what, void *udpVoid)
{
    MetisUdpListener *udp = (MetisUdpListener *) udpVoid;

    if (metisLogger_IsLoggable(udp->logger, MetisLoggerFacility_IO, PARCLogLevel_Debug)) {
        metisLogger_Log(udp->logger, MetisLoggerFacility_IO, PARCLogLevel_Debug, __func__,
                        "%s socket %d what %s%s%s%s data %p",
                        __func__, fd,
                        (what & PARCEventType_Timeout) ? " timeout" : "",
                        (what & PARCEventType_Read)    ? " read" : "",
                        (what & PARCEventType_Write)   ? " write" : "",
                        (what & PARCEventType_Signal)  ? " signal" : "",
                        udpVoid);
    }

    if (what & PARCEventType_Read) {
        udp->stats.framesIn++;
        struct sockaddr_storage peerIpAddress;
        socklen_t peerIpAddressLength = sizeof(peerIpAddress);

        size_t packetLength = _peekMessageLength(udp, fd, (struct sockaddr *) &peerIpAddress, &peerIpAddressLength);

        if (packetLength > 0) {
            _receivePacket(udp, fd, packetLength, &peerIpAddress, peerIpAddressLength);
        } else {
            _readFrameToDiscard(udp, fd);
        }
    }
}
