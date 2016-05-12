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
 * @file metis_Tlv.h
 * @brief The generic Tlv utilities.
 *
 * Provides generaic Tlv utilities, particularly for packets that have not been
 * decoded in to their skeleton.  Once packets are in the skeleton format, one should
 * use functions in metis_TlvSkeleton.
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#ifndef Metis_metis_Tlv_h
#define Metis_metis_Tlv_h

#include <stdlib.h>
#include <parc/security/parc_CryptoHash.h>
#include <ccnx/api/control/cpi_ControlMessage.h>
#include <parc/algol/parc_ByteArray.h>

#include <ccnx/forwarder/metis/tlv/metis_TlvExtent.h>
#include <ccnx/forwarder/metis/tlv/metis_TlvSkeleton.h>

/**
 * The TLV format
 *
 *   Mapping of the TLV format to a structure.  Remember to use
 *   <code>htons()</code> or <code>ntohs()</code> on the values
 *   if working in host byte order.
 *
 *   The 'length' is the length of the 'value', it does not include
 *   the T and L of the TLV.  A length of "0" is acceptable.
 *
 * @param type is in network byte order
 * @param length is in network byte order
 *
 * Example:
 * @code
 * {
 *    uint8_t *packet = // packet received from network
 *    size_t offset = // where you are in parsing the packet
 *
 *    MetisTlvType *tlv = (MetisTlvType *) (packet + offset);
 *    uint16_t type = htons(tlv->type);
 *    uint16_t v_length = htons(tlv->length);
 * }
 * @endcode
 */
typedef struct __attribute__ ((packed)) metis_tlv_type {
    uint16_t type;
    uint16_t length;
} MetisTlvType;


/**
 * Returns the length of the fixed header
 *
 * This is assumed to be the same for all versions.  At some point this will no longer be true
 * and metis will need to be re-factored.  This function works for V0 and V1 packets.
 *
 * @return positive The bytes of the fixed header.
 *
 * Example:
 * @code
 * {
 *    if (parcEventBuffer_GetLength(input) >= metisTlv_FixedHeaderLength()) {
 *       uint8_t fixedheader[metisTlv_FixedHeaderLength()];
 *       read(fd, &fixedheader, metisTlv_FixedHeaderLength());
 *       // process fixed header
 *    }
 * }
 * @endcode
 */
size_t metisTlv_FixedHeaderLength(void);

/**
 * Returns the length of all headers, which is the offset where the CCNx message starts
 *
 * Includes both the fixed header and the per hop headers.
 * Will return "0" for unknown packet version
 *
 * @param [in] packet Pointer to bytes 0 of the fixed header
 *
 * @retval positive The total header length (minimum 8)
 * @retval 0 Unsupported packet version or other error
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
size_t metisTlv_TotalHeaderLength(const uint8_t *packet);

/**
 * The total packet length based on the fixed header
 *
 * Parses the fixed header and returns the total packet length
 * Will return "0" for unknown packet version
 *
 * @param [in] packet Packet memory pointer
 *
 * @retval number Total packet length
 *
 * Example:
 * @code
 * {
 *    // in an example packet parser (does not handle any error conditions or partial reads)
 *    if (parcEventBuffer_GetLength(input) >= metisTlv_FixedHeaderLength()) {
 *        uint8_t fixedheader[metisTlv_FixedHeaderLength()];
 *        read(fd, &fixedheader, metisTlv_FixedHeaderLength());
 *
 *        size_t remainingBytes = metisTlv_TotalPacketLength(&fixedheader) -  metisTlv_FixedHeaderLength();
 *        if (parcEventBuffer_GetLength(input) >= remainingBytes) {
 *            uint8_t *packet = parcMemory_Allocate(metisTlv_TotalPacketLength(&fixedheader));
 *            read(fd, packet + metisTlv_FixedHeaderLength(), remainingBytes);
 *        }
 *    }
 * }
 * @endcode
 */
size_t metisTlv_TotalPacketLength(const uint8_t *packet);

/**
 * @function metisTlv_NameSegments
 * @abstract Treats the input as a TLV-encoded name, generating an output array of name segment extents
 * @discussion
 *   The outputArray is an ordered list of extents, giving the offset and length of each path segment.
 *   The lengths include the path segment type, length, and value.
 *
 *   Example:  Lets represent the name as a set of of tuples of T, L, and V:
 *             (t=1, len=4, value="help"), (t=1, len=2, value="me"), (t=7, len=10, value="understand")
 *             This name as 3 path segments  The first segment is of type 1, length 4, and value "help".
 *             The total length of that segment is 4 + 4 = 8, including the T and L.
 *             The outputArray would be { {.offset=0, .length=8}, {.offset=8, .length=6}, {.offset=14, .length=14} }.
 *             The outputLenght would be 3, because there are 3 elements in the array.
 *
 * @param name is a TLV-encoded name, not including the container name TLV
 * @param nameLength is the length of the name
 * @param outputArrayPtr is an allocated array of ordered extents, must be freed with <code>parcMemory_Deallocate()</code>
 * @param outputLengthPtr is the number of elements allocated in the array.
 *
 * Example:
 * @code
 * {
 *    MetisTlvExtent *extentArray;
 *    size_t arrayLength;
 *    uint8_t encodedName[] = "\x00\x01\x00\x05" "apple" "\x00\x01\x00\x03" "pie";
 *    metisTlv_NameSegments(encodedName, sizeof(encodedName), &extentArray, &arrayLength);
 *    // arrrayLength = 2
 *    // extentArray[1].offset = 4
 *    // extentArray[1].length = 5
 *    // etc.
 *    parcMemory_Deallocate(&extentArray);
 * }
 * @endcode
 */
void metisTlv_NameSegments(uint8_t *name, size_t nameLength, MetisTlvExtent **outputArrayPtr, size_t *outputLengthPtr);

/**
 * Given a CCNxControl packet, encode it in the proper schema
 *
 * Based on the dictionary schema version, will encode the control packet with the
 * correct encoder.
 *
 * @param [in] cpiControlMessage A an allocated control message
 *
 * @retval non-null An allocation wire format buffer
 * @retval null An error (likely unsupported schema version)
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
PARCBuffer *metisTlv_EncodeControlPlaneInformation(const CCNxControl *cpiControlMessage);

/**
 * Parse an extent as a VarInt
 *
 * The extent must be 1 to 8 bytes.
 *
 * @param [in] packet The packet memory pointer
 * @param [in] extent The byte extent of the varint buffer
 * @param [out] output The VarInt value
 *
 * @retval true The buffer was correctly parsed
 * @retval false The buffer was not parsed (likely extent length is 0 or greater than 8)
 *
 * Example:
 * @code
 * {
 *     uint8_t packet[] = { 0x00, 0x03, 0x00, 0x03, 0xa0, 0xa1, 0xa3 };
 *     MetisTlvExtent extent = { .offset = 4, .length = 3 };
 *     uint64_t output;
 *     metisTlv_ExtentToVarInt(packet, &extent, &output);
 *     // output = 0xa0a1a3
 * }
 * @endcode
 */
bool metisTlv_ExtentToVarInt(const uint8_t *packet, const MetisTlvExtent *extent, uint64_t *output);

#endif // Metis_metis_Tlv_h
