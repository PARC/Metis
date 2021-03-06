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
//  metis_Notifications.h
//  Libccnx
//
//  Created by Mosko, Marc <Marc.Mosko@parc.com> on 11/20/13.

/**
 * The EventMessenger is the system that messages events between
 * producers and consumers.
 *
 * Events are delivered in a deferred event cycle to avoid event callbacks
 * firing when the event generator is still running.
 */

#ifndef Metis_metis_Messenger_h
#define Metis_metis_Messenger_h

#include <ccnx/forwarder/metis/core/metis_Dispatcher.h>
#include <ccnx/forwarder/metis/messenger/metis_Missive.h>
#include <ccnx/forwarder/metis/messenger/metis_MessengerRecipient.h>

struct metis_messenger;
typedef struct metis_messenger MetisMessenger;

/**
 * @function metisEventmessenger_Create
 * @abstract Creates an event notification system
 * @discussion
 *   Typically there's only one of these managed by metisForwarder.
 *
 * @param dispatcher is the event dispatcher to use to schedule events.
 * @return <#return#>
 */
MetisMessenger *metisMessenger_Create(MetisDispatcher *dispatcher);

/**
 * @function metisEventMessenger_Destroy
 * @abstract Destroys the messenger system, no notification is sent
 * @discussion
 *   <#Discussion#>
 *
 * @param <#param1#>
 * @return <#return#>
 */
void metisMessenger_Destroy(MetisMessenger **messengerPtr);

/**
 * @function metisEventMessenger_Send
 * @abstract Send an event message, takes ownership of the event memory
 * @discussion
 *   <#Discussion#>
 *
 * @param <#param1#>
 */
void metisMessenger_Send(MetisMessenger *messenger, MetisMissive *missive);

/**
 * @function metisEventMessenger_Register
 * @abstract Receive all event messages
 * @discussion
 *   <#Discussion#>
 *
 * @param <#param1#>
 * @return <#return#>
 */
void metisMessenger_Register(MetisMessenger *messenger, const MetisMessengerRecipient *recipient);

/**
 * @function metisEventMessenger_Unregister
 * @abstract Stop receiving event messages
 * @discussion
 *   <#Discussion#>
 *
 * @param <#param1#>
 * @return <#return#>
 */
void metisMessenger_Unregister(MetisMessenger *messenger, const MetisMessengerRecipient *recipient);
#endif // Metis_metis_Messenger_h
