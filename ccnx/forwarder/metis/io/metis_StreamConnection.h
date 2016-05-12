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

//
//  metis_ListenerStream.h
//  Libccnx
//
//  Created by Mosko, Marc <Marc.Mosko@parc.com> on 11/19/13.

/**
 * Methods common to TCP and PF_LOCAL stream-based listeners
 */

#ifndef Metis_metis_StreamConnection_h
#define Metis_metis_StreamConnection_h

#include <ccnx/forwarder/metis/io/metis_IoOperations.h>
#include <ccnx/forwarder/metis/io/metis_AddressPair.h>
#include <ccnx/forwarder/metis/core/metis_Forwarder.h>
#include <ccnx/api/control/cpi_Address.h>

/**
 * @function metisStreamConnection_AcceptConnection
 * @abstract Receive a connection from a remote peer
 * @discussion
 *   We are the "server side" of the stream connection, so we need to accept the client connection
 *   and setup state for her.
 *
 * @param <#param1#>
 * @return <#return#>
 */
MetisIoOperations *metisStreamConnection_AcceptConnection(MetisForwarder *metis, int fd, MetisAddressPair *pair, bool isLocal);

/**
 * @function metisStreamConnection_OpenConnection
 * @abstract Initiate a connection to a remote peer
 * @discussion
 *   We are the "client side" of the stream connection.  We'll create state for the peer, but it will
 *   be in the "down" state until the connection establishes.
 *
 *   For TCP, both address pairs need to be the same address family: both INET or both INET6.  The remote
 *   address must have the complete socket information (address, port).  The local socket could be wildcarded or
 *   may specify down to the (address, port) pair.
 *
 *   If the local address is IPADDR_ANY and the port is 0, then it is a normal call to "connect" that will use
 *   whatever local IP address and whatever local port for the connection.  If either the address or port is
 *   set, the local socket will first be bound (via bind(2)), and then call connect().
 *
 *   AF_UNIX is not yet supported
 *
 *   If there's an error binding to the specified address or connecting to the remote address,
 *   will return NULL.
 *
 * @param pair (takes ownership of this) is the complete socket pair of (address, port) for each end, if INET or INET6.
 * @return NULL on error, otherwise the connections IO operations.
 */
MetisIoOperations *metisStreamConnection_OpenConnection(MetisForwarder *metis, MetisAddressPair *pair, bool isLocal);
#endif // Metis_metis_StreamConnection_h
