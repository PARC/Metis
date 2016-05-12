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
 * @header Metis Dispatcher
 * @abstract The dispatcher is the event loop run by MetisForwarder.
 * @discussion
 *     These functions manage listeners, timers, and network events inside
 *     the event loop.
 *
 *     Curently, it is a thin wrapper around an event so we don't have to
 *     expose that implementation detail to other modules.
 *
 * @author Marc Mosko
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#ifndef Metis_metis_Dispatcher_h
#define Metis_metis_Dispatcher_h

#include <sys/socket.h>
#include <stdbool.h>

struct metis_dispatcher;
typedef struct metis_dispatcher MetisDispatcher;

#include <parc/algol/parc_Memory.h>
#include <parc/algol/parc_EventScheduler.h>
#include <parc/algol/parc_Event.h>
#include <parc/algol/parc_EventTimer.h>
#include <parc/algol/parc_EventSignal.h>
#include <parc/algol/parc_EventQueue.h>
#include <parc/algol/parc_EventSocket.h>

#include <ccnx/forwarder/metis/core/metis_Logger.h>

PARCEventScheduler *metisDispatcher_GetEventScheduler(MetisDispatcher *dispatcher);
/**
 * Creates an event dispatcher
 *
 * Event dispatcher based on PARCEvent
 *
 * @return non-null Allocated event dispatcher
 * @return null An error
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
MetisDispatcher *metisDispatcher_Create(MetisLogger *logger);

/**
 * Destroys event dispatcher
 *
 * Caller is responsible for destroying call events before destroying
 * the event dispatcher.
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * @return <#value#> <#explanation#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
void metisDispatcher_Destroy(MetisDispatcher **dispatcherPtr);

/**
 * @function metisDispatcher_Stop
 * @abstract Called from a different thread, tells the dispatcher to stop
 * @discussion
 *   Called from a user thread or from an interrupt handler.
 *   Does not block.  Use <code>metisDispatcher_WaitForStopped()</code> to
 *   block until stopped after calling this.
 *
 * @param <#param1#>
 * @return <#return#>
 */
void metisDispatcher_Stop(MetisDispatcher *dispatcher);

/**
 * @function metisDispatcher_WaitForStopped
 * @abstract Blocks until dispatcher in stopped state
 * @discussion
 *   Used after <code>metisDispatcher_Stop()</code> to wait for stop.
 *
 * @param <#param1#>
 * @return <#return#>
 */
void metisDispatcher_WaitForStopped(MetisDispatcher *dispatcher);

/**
 * @function metisDispatcher_Run
 * @abstract Runs the forwarder, blocks.
 * @discussion
 *   <#Discussion#>
 *
 * @param <#param1#>
 */
void metisDispatcher_Run(MetisDispatcher *dispatcher);

/**
 * @function metisDispatcher_RunDuration
 * @abstract Runs forwarder for at most duration, blocks.
 * @discussion
 *   Blocks running the forwarder for a duration.  May be called
 *   iteratively to keep running.  Duration is a minimum, actual
 *   runtime may be slightly longer.
 *
 * @param <#param1#>
 */
void metisDispatcher_RunDuration(MetisDispatcher *dispatcher, struct timeval *duration);

/**
 * @header metisDispatcher_RunCount
 * @abstract Run the event loop for the given count cycles
 * @discussion
 *     Runs the event loop for the given number of cycles, blocking
 *     until done.  May be called sequentially over and over.
 *
 */
void metisDispatcher_RunCount(MetisDispatcher *dispatcher, unsigned count);

typedef int MetisSocketType;

typedef struct evconnlistener MetisListener;

/**
 * @typedef MetisListenerCallback
 * @abstract Callback function typedef for a stream listener
 *
 * @constant listener is the object created by <code>metisForwarder_NewBind()</code> that received the client connection
 * @constant client_socket is the client socket
 * @constant user_data is the user_data passed to <code>metisForwarder_NewBind()</code>
 * @constant client_addr is the client address
 * @constant socklen is the length of client_addr
 * @discussion <#Discussion#>
 */
typedef void (MetisListenerCallback)(MetisListener *listener, MetisSocketType client_socket,
                                     struct sockaddr *client_addr, int socklen, void *user_data);

/**
 * @header metisForwarder_NewBind
 * @abstract Allocate a new stream listener
 * @discussion
 *     The server socket will be freed when closed and will be reusable.
 *
 * @param metis the forwarder that owns the event loop
 * @param cb is the callback for a new connection
 * @param user_data is opaque user data passed to the callback
 * @param backlog is the listen() depth, may use -1 for a default value
 * @param sa is the socket address to bind to (INET, INET6, LOCAL)
 * @param socklen is the sizeof the actual sockaddr (e.g. sizeof(sockaddr_un))
 */
PARCEventSocket *metisDispatcher_CreateListener(MetisDispatcher *dispatcher,
                                                PARCEventSocket_Callback *callback, void *user_data,
                                                int backlog, const struct sockaddr *sa, int socklen);

void metisDispatcher_DestroyListener(MetisDispatcher *dispatcher, PARCEventSocket **listenerPtr);

typedef struct event MetisTimerEvent;
typedef struct event MetisNetworkEvent;
typedef struct event MetisSignalEvent;

/**
 * @typedef MetisEventCallback
 * @abstract A network event or a timer callback
 * @constant fd The file descriptor associated with the event, may be -1 for timers
 * @constant which_event is a bitmap of the MetisEventType
 * @constant user_data is the user_data passed to <code>MetisForwarder_CreateEvent()</code>
 * @discussion <#Discussion#>
 */
typedef void (MetisEventCallback)(MetisSocketType fd, short which_event, void *user_data);

/**
 * @function metisDispatcher_CreateTimer
 * @abstract Creates a MetisEvent for use as a timer.
 * @discussion
 *
 *   When created, the timer is idle and you need to call <code>metisForwarder_StartTimer()</code>
 *
 * @param isPeriodic means the timer will fire repeatidly, otherwise it is a one-shot and
 *         needs to be set again with <code>metisDispatcher_StartTimer()</code>
 * @return <#return#>
 */
PARCEventTimer *metisDispatcher_CreateTimer(MetisDispatcher *dispatcher, bool isPeriodic, PARCEvent_Callback *callback, void *userData);

/**
 * @function metisDispatcher_StartTimer
 * @abstract Starts the timer with the given delay.
 * @discussion
 *   If the timer is periodic, it will keep firing with the given delay
 *
 * @param <#param1#>
 * @return <#return#>
 */
void metisDispatcher_StartTimer(MetisDispatcher *dispatcher, PARCEventTimer *timerEvent, struct timeval *delay);

void metisDispatcher_StopTimer(MetisDispatcher *dispatcher, PARCEventTimer *timerEvent);

/**
 * @function metisDispatcher_DestroyTimerEvent
 * @abstract Cancels the timer and frees the event
 * @discussion
 *   <#Discussion#>
 *
 * @param <#param1#>
 * @return <#return#>
 */
void metisDispatcher_DestroyTimerEvent(MetisDispatcher *dispatcher, PARCEventTimer **eventPtr);

/**
 * @function metisDispatcher_CreateNetworkEvent
 * @abstract Creates a network event callback on the socket
 * @discussion
 *   May be used on any sort of file descriptor or socket.  The event is edge triggered and non-reentrent.
 *   This means you need to drain the events off the socket, as the callback will not be called again
 *   until a new event arrives.
 *
 *   When created, the event is idle and you need to call <code>metisForwarder_StartNetworkEvent()</code>
 *
 * @param isPersistent means the callback will keep firing with new events, otherwise its a one-shot
 * @param fd is the socket to monitor
 * @return <#return#>
 */
//MetisNetworkEvent *metisDispatcher_CreateNetworkEvent(MetisDispatcher *dispatcher, bool isPersistent, MetisEventCallback *callback, void *userData, MetisSocketType fd);
PARCEvent *metisDispatcher_CreateNetworkEvent(MetisDispatcher *dispatcher, bool isPersistent, PARCEvent_Callback *callback, void *userData, int fd);

//void metisDispatcher_StartNetworkEvent(MetisDispatcher *dispatcher, MetisNetworkEvent *event);
//void metisDispatcher_StopNetworkEvent(MetisDispatcher *dispatcher, MetisNetworkEvent *event);
void metisDispatcher_StartNetworkEvent(MetisDispatcher *dispatcher, PARCEvent *event);
void metisDispatcher_StopNetworkEvent(MetisDispatcher *dispatcher, PARCEvent *event);

//void metisDispatcher_DestroyNetworkEvent(MetisDispatcher *dispatcher, MetisNetworkEvent **eventPtr);
void metisDispatcher_DestroyNetworkEvent(MetisDispatcher *dispatcher, PARCEvent **eventPtr);

/**
 * @function metisDispatcher_CreateSignalEvent
 * @abstract Creates a signal trap
 * @discussion
 *   May be used on catchable signals.  The event is edge triggered and non-reentrent.  Signal events are persistent.
 *
 *   When created, the signal trap is idle and you need to call <code>metisForwarder_StartSignalEvent()</code>
 *
 * @param signal is the system signal to monitor (e.g. SIGINT).
 * @return <#return#>
 */
PARCEventSignal *metisDispatcher_CreateSignalEvent(MetisDispatcher *dispatcher, PARCEventSignal_Callback *callback, void *userData, int signal);

void metisDispatcher_DestroySignalEvent(MetisDispatcher *dispatcher, PARCEventSignal **eventPtr);

void metisDispatcher_StartSignalEvent(MetisDispatcher *dispatcher, PARCEventSignal *event);
void metisDispatcher_StopSignalEvent(MetisDispatcher *dispatcher, PARCEventSignal *event);

// =============
// stream buffers

#include <ccnx/forwarder/metis/core/metis_StreamBuffer.h>
#include <ccnx/forwarder/metis/io/metis_AddressPair.h>

/**
 * @function metisDispatcher_CreateStreamBuffer
 * @abstract Creates a high-function buffer around a stream socket
 * @discussion
 *   <#Discussion#>
 *
 * @param <#param1#>
 * @return <#return#>
 */
PARCEventQueue *metisDispatcher_CreateStreamBufferFromSocket(MetisDispatcher *dispatcher, MetisSocketType fd);

/**
 * @function metisDispatcher_StreamBufferConnect
 * @abstract Create a TCP tunnel to a remote peer
 * @discussion
 *   For TCP, both address pairs need to be the same address family: both INET or both INET6.  The remote
 *   address must have the complete socket information (address, port).  The local socket could be wildcarded or
 *   may specify down to the (address, port) pair.
 *
 *   If the local address is IPADDR_ANY and the port is 0, then it is a normal call to "connect" that will use
 *   whatever local IP address and whatever local port for the connection.  If either the address or port is
 *   set, the local socket will first be bound (via bind(2)), and then call connect().
 *
 *   It is unlikely that the buffer will be connected by the time the function returns.  The eventCallback will
 *   fire once the remote system accepts the conneciton.
 *
 * @param <#param1#>
 * @return NULL on error, otherwise a streambuffer.
 */
PARCEventQueue *metisDispatcher_StreamBufferConnect(MetisDispatcher *dispatcher, const MetisAddressPair *pair);
#endif // Metis_metis_Dispatcher_h
