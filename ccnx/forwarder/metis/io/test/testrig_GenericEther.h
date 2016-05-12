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
 * @file testrig_GenericEther.h
 * @brief A mockup of a platform Ethernet
 *
 * The mockup is connected to a socketpair, so you can read frames that the metis_EtherListener sends.
 * It also has an input queue so you can queue frames to be read by metis_EtherListener.
 *
 * This mockup implements the metis_GenericEther.h API plus two additional functions for the mockup.
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#ifndef Metis_testrig_GenericEther_h
#define Metis_testrig_GenericEther_h

#include <ccnx/forwarder/metis/io/metis_GenericEther.h>
#include <parc/algol/parc_Buffer.h>

/**
 * Returns the other end of a socketpair that mocks up the ethernet wire
 *
 * The mockup does not connect to a RAW or BPF socket, it connects to a socketpair.
 * This function gets the remote end of the socket pair, which is where you can read
 * frames that you send.
 *
 * DO NOT WRITE PACKETS HERE.  To queue packets for input, use mockGenericEther_QueueFrame().
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * @retval number System socketpair
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
int mockGenericEther_GetTestDescriptor(MetisGenericEther *ether);

/**
 * Queue an Ethernet frame to be read
 *
 * The mockup maintains an input queue (deque) for input frames.  These should be full
 * Ethernet frames (not including the frame check sequence).
 *
 * This stores a reference, so caller must also release the PARCBuffer.
 *
 * This function will not notify the etherSocket being watched by Libevent in Metis.
 * To notify Libevent, use mockGenericEther_Notify() after queuing packets.
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
void mockGenericEther_QueueFrame(MetisGenericEther *ether, PARCBuffer *ethernetFrame);

/**
 * Writes a byte to the etherSocket
 *
 * Tickles Libevent by writing a byte to the etherSocket.
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
void mockGenericEther_Notify(MetisGenericEther *ether);

/**
 * Convenience function to encapsulate a packet in an ethernet frame
 *
 * Creates a PARCBuffer that has an Ethernet header followed by a user-provided byte array.
 * Does not include the frame check sequence.
 *
 * @param [in] length The length of the ccnxPacket to be encapsulated
 * @param [in] ccnxPacket the byte array to put after the ethernet header
 * @param [in] dmac[6] The destination mac
 * @param [in] smac[6] The source mac
 * @param [in] ethertype The ethertype in host byte order
 *
 * @retval non-null An allocated PARCBuffer wrapping an ethernet frame, ready to read
 * @retval null An error
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
PARCBuffer *mockGenericEther_createFrame(size_t length, const uint8_t ccnxPacket[length], const uint8_t dmac[ETHER_ADDR_LEN], const uint8_t smac[ETHER_ADDR_LEN], uint16_t ethertype);

#endif // Metis_testrig_GenericEther_h
