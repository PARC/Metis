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
 * @file metis_TlvName.h
 * @brief Representation of the name
 *
 * It is stored as an array of TLV extents, one for each name segment.  This allows longest matching
 * prefix comparisons as well as equality comparisons.
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#ifndef Metis_metis_tlv_Name_h
#define Metis_metis_tlv_Name_h

#include <stdlib.h>
#include <stdbool.h>
#include <ccnx/common/ccnx_Name.h>

struct metis_tlv_name;
typedef struct metis_tlv_name MetisTlvName;

/**
 * Creates a name from packet memory
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [in] memory A pointer to the beginning of the Name TLV "value".
 * @param [in] The length of the "value"
 *
 * @retval non-null An allocated MetisTlvName
 * @retval null An error
 *
 * Example:
 * @code
 * {
 *    uint8_t encodedName[] = {0x00, 0x01, 0x00, 0x05, 'a', 'p', 'p', 'l', 'e', 0x00, 0x01, 0x00, 0x03, 'p', 'i', 'e'};
 *    MetisTlvName *name = metisTlvName_Create(encodedName, sizeof(encodedName));
 *    metisTlvName_Release(&name);
 * }
 * @endcode
 */
MetisTlvName *metisTlvName_Create(const uint8_t *memory, size_t length);

/**
 * Creates a Metis-sytle name from a CCNxName
 *
 * Converts a CCNxName to a Metis Name.  The Metis name has its own backing memory,
 * so it is independent of the CCNxName.
 *
 * @param [in] ccnxName An allocated CCNxName
 *
 * @retval non-null An allocated MetisTlvName
 * @retval null An error
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
MetisTlvName *metisTlvName_CreateFromCCNxName(const CCNxName *ccnxName);

/**
 * Releases one reference count, and frees memory after last reference
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [in,out] namePtr pointer to the name to free, *namePtr will be NULL'd
 *
 * Example:
 * @code
 * {
 *    uint8_t encodedName[] = {0x00, 0x01, 0x00, 0x05, 'a', 'p', 'p', 'l', 'e', 0x00, 0x01, 0x00, 0x03, 'p', 'i', 'e'};
 *    MetisTlvName *name = metisTlvName_Create(encodedName, sizeof(encodedName));
 *    metisTlvName_Release(&name);
 * }
 * @endcode
 */
void metisTlvName_Release(MetisTlvName **namePtr);

/**
 * Acquires a reference to the name
 *
 * The MetisTlvName wrapper is allocated but the underlying name memory is shared.
 * The returned pointer will not be the same as the original.
 *
 * @param [in] original The name to acquire a reference to
 *
 * @retval non-null A reference counted copy
 * @retval null An error
 *
 * Example:
 * @code
 * {
 *    uint8_t encodedName[] = {0x00, 0x01, 0x00, 0x05, 'a', 'p', 'p', 'l', 'e', 0x00, 0x01, 0x00, 0x03, 'p', 'i', 'e'};
 *    MetisTlvName *name = metisTlvName_Create(encodedName, sizeof(encodedName));
 *    MetisTlvName *copy = metisTlvName_Acquire(name);
 *    metisTlvName_Release(&name);
 *    metisTlvName_Release(&copy);
 * }
 * @endcode
 */
MetisTlvName *metisTlvName_Acquire(const MetisTlvName *original);

/**
 * Acquire a reference to the first name, but only use first 'segmentCount' name segments
 *
 * A reference to the underlying name segments is increased but a new MetisTlvName wrapper is
 * put around them.  This wrapper will only have 'segmentCount' name segments -- any name segments
 * after that are ignored.
 *
 * If segmentCount is longer than the name (e.g. UINT_MAX), it will be the same as the orignal
 * name, but have a different wrapper.  You sould probalby use metisTlvName_Acquire() in this case,
 * as it will avoid re-calculating the name's hash.
 *
 * This is a reference counted way to shorten a name, such as to store it as a shorter FIB entry.
 *
 * @param [in] original The name to acquire and slice
 * @param [in] segmentCount The number of segments, may be longer than the name
 *
 * @retval non-null A new slice
 * @retval null An error
 *
 * Example:
 * @code
 * {
 *    uint8_t encodedName[] = {0x00, 0x01, 0x00, 0x05, 'a', 'p', 'p', 'l', 'e', 0x00, 0x01, 0x00, 0x03, 'p', 'i', 'e'};
 *    MetisTlvName *name = metisTlvName_Create(encodedName, sizeof(encodedName));
 *    MetisTlvName *slice = metisTlvName_Slice(name, 1);
 *    // slice is only lci:/apple
 *    metisTlvName_Release(&name);
 *    metisTlvName_Release(&slice);
 * }
 * @endcode
 */
MetisTlvName *metisTlvName_Slice(const MetisTlvName *original, size_t segmentCount);

/**
 * A hash value for use in hash tables
 *
 * Will only be calculated once, then cached inside the MetisTlvName.
 *
 * @param [in] name The name to hash
 *
 * @retval number A hash value for use in hash tables
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
uint32_t metisTlvName_HashCode(const MetisTlvName *name);

/**
 * Determine if two MetisTlvName instances are equal.
 *
 * Two MetisTlvName instances are equal if, and only if,
 * both objects have a name and they are equal.
 *
 * The following equivalence relations on non-null `MetisTlvName` instances are maintained:
 *
 *  * It is reflexive: for any non-null reference value x, `MetisTlvName_Equals(x, x)`
 *      must return true.
 *
 *  * It is symmetric: for any non-null reference values x and y,
 *    `metisTlvName_Equals(x, y)` must return true if and only if
 *        `metisTlvName_Equals(y, x)` returns true.
 *
 *  * It is transitive: for any non-null reference values x, y, and z, if
 *        `metisTlvName_Equals(x, y)` returns true and
 *        `metisTlvName_Equals(y, z)` returns true,
 *        then  `metisTlvName_Equals(x, z)` must return true.
 *
 *  * It is consistent: for any non-null reference values x and y, multiple
 *      invocations of `metisTlvName_Equals(x, y)` consistently return true or
 *      consistently return false.
 *
 *  * For any non-null reference value x, `metisTlvName_Equals(x, NULL)` must
 *      return false.
 *
 * @param a A pointer to a `MetisTlvName` instance.
 * @param b A pointer to a `MetisTlvName` instance.
 * @return true if the two `MetisTlvName` instances are equal.
 *
 * Example:
 * @code
 * {
 *    MetisTlvName *a = metisTlvName_Create();
 *    MetisTlvName *b = metisTlvName_Create();
 *
 *    if (metisTlvName_Equals(a, b)) {
 *        // true
 *    } else {
 *        // false
 *    }
 * }
 * @endcode
 */
bool metisTlvName_Equals(const MetisTlvName *a, const MetisTlvName *b);

/**
 * Compares two names and returns their ordering
 *
 *   If |A| < |B| or |A|=|B| & A < B, return -1
 *   If A = B, return 0
 *   If |A| > |B| or |A|=|B| & A > B, return +1
 *
 * @param [in] a The first name
 * @param [in] b The second name
 *
 * @retval negative If |A| < |B| or |A|=|B| & A < B
 * @retval zero If A = B, return 0
 * @retval positive If |A| > |B| or |A|=|B| & A > B
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
int metisTlvName_Compare(const MetisTlvName *a, const MetisTlvName *b);

/**
 * @function metsName_StartsWith
 * @abstract Tests if name starts with prefix
 * @discussion
 *   Byte-by-byte prefix comparison
 *
 * @return True if the name is equal to or begins with prefix
 */

/**
 * Determines if name begins with prefix
 *
 * Returns true if the given name begins or equals the given prefix.
 *
 * @param [in] name The name to test (must be non-null)
 * @param [in] prefix The prefix to check the name against (must be non-null)
 *
 * @retval true name is equal to or prefixed by prefix
 * @retval false prefix is unreleated to name
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
bool metisTlvName_StartsWith(const MetisTlvName *name, const MetisTlvName *prefix);

/**
 * The number of name segments in the name
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [in] name An allocated MetisTlvName
 *
 * @retval number The number of name segments
 *
 * Example:
 * @code
 * {
 *    uint8_t encodedName[] = {0x00, 0x01, 0x00, 0x05, 'a', 'p', 'p', 'l', 'e', 0x00, 0x01, 0x00, 0x03, 'p', 'i', 'e'};
 *    MetisTlvName *name = metisTlvName_Create(encodedName, sizeof(encodedName));
 *    size_t count = metisTlvName_SegmentCount(name);
 *    // count = 2
 *    metisTlvName_Release(&name);
 * }
 * @endcode
 */
size_t metisTlvName_SegmentCount(const MetisTlvName *name);

/**
 * Converts a MetisTlvName to a CCNxName
 *
 * The new name will use its own memory unrelated to the MetisTlvName.
 * You must release the reference to the CCNxName when done with it.
 *
 * @param [in] name An allocated MetisTlvName
 *
 * @retval non-null An allocated CCNxName
 * @retval null An error
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
CCNxName *metisTlvName_ToCCNxName(const MetisTlvName *name);
#endif // Metis_metis_tlv_Name_h
