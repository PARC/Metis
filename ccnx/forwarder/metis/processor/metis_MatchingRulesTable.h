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
 * @header metis_MatchingRulesTable
 * @abstract A generic table (void *) that matches a MetisMessage according to the CCNx 1.0 rules
 * @discussion
 *     Matching is done based on Name, Name + KeyId, or Name + ContentObjectHash.
 *     The table key is always a MetisMessage.
 *
 *     When used in the PIT, one calls <code>metisMatchingRulesTable_AddToBestTable()</code> to
 *     add an interest to the "best" (i.e. most restrictive match) table, then calls
 *     <code>metisMatchingRulesTable_GetUnion()</code> on a content object to match against
 *     all of them.
 *
 *     When used in a ContentStore, one calls <code>metisMatchingRulesTable_AddToAllTables()</code>
 *     to index a Content Object in all the tables.  one then calls <code>metisMatchingRulesTable_Get()</code>
 *     with an Interest to do the "best" matching (i.e by hash first, then keyid, then just by name).
 *
 * @author Mosko, Marc <Marc.Mosko@parc.com> on 11/29/13.
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#ifndef Metis_metis_MatchingRulesTable_h
#define Metis_metis_MatchingRulesTable_h

#include <parc/algol/parc_HashCodeTable.h>
#include <ccnx/forwarder/metis/core/metis_Message.h>
#include <parc/algol/parc_ArrayList.h>

struct metis_matching_rules_table;
typedef struct metis_matching_rules_table MetisMatchingRulesTable;

/**
 * Creates a MetisMatchigRulesTable and specifies the function to call to de-allocate an entry
 *
 * The datadestroyer will be called when an entry is removed from a table. It may be NULL.
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * @retval <#value#> <#explanation#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
MetisMatchingRulesTable *metisMatchingRulesTable_Create(PARCHashCodeTable_Destroyer dataDestroyer);

/**
 * Destroys the table and removes all stored elements.
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * @retval <#value#> <#explanation#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
void metisMatchingRulesTable_Destroy(MetisMatchingRulesTable **tablePtr);

/**
 * @function metisMatchingRulesTable_Get
 * @abstract Returns the data item that best matches the message.
 * @discussion
 *   Indexed by NameAndContentObjectHash, NameAndKeyId, and Name, in that order.
 *
 * @param <#param1#>
 * @return NULL if nothing matches, otherwise the stored value
 */
void *metisMatchingRulesTable_Get(const MetisMatchingRulesTable *table, const MetisMessage *message);

/**
 * @function metisMatchingRulesTable_GetUnion
 * @abstract Returns matching data items from all index tables.
 * @discussion
 *   The PARCArrayList does not have an item destructor, so destroying it will not affect
 *   the underlying data.
 *
 *
 * @param <#param1#>
 * @return Will not be NULL, but may be empty
 */
PARCArrayList *metisMatchingRulesTable_GetUnion(const MetisMatchingRulesTable *table, const MetisMessage *message);

/**
 * @function metisMatchingRulesTable_Add
 * @abstract Adds the data to the best table
 * @discussion
 *   The key must be derived from the data and destroyed when the data is destroyed.  Only the data
 *   destroyer is called.
 *
 *   No duplicates are allowed, will return false if not added.
 *
 * @param <#param1#>
 * @return true if unique key and added, false if duplicate and no action taken.
 */
bool metisMatchingRulesTable_AddToBestTable(MetisMatchingRulesTable *rulesTable, MetisMessage *key, void *data);

/**
 * @function metisMatchingRulesTable_AddToAllTables
 * @abstract Adds the key and data to all tables
 * @discussion
 *   duplicates are not added
 *
 * @param <#param1#>
 */
void metisMatchingRulesTable_AddToAllTables(MetisMatchingRulesTable *rulesTable, MetisMessage *key, void *data);

/**
 * @function metisMatchingRulesTable_Remove
 * @abstract Removes the matching entry from the best match table, calling the destroyer on the data.
 * @discussion
 *   <#Discussion#>
 *
 * @param <#param1#>
 */
void metisMatchingRulesTable_RemoveFromBest(MetisMatchingRulesTable *rulesTable, const MetisMessage *message);

/**
 * @function metisMatchingRulesTable_RemoveFromAll
 * @abstract Removes the message from all tables
 * @discussion
 *   <#Discussion#>
 *
 * @param <#param1#>
 * @return <#return#>
 */
void metisMatchingRulesTable_RemoveFromAll(MetisMatchingRulesTable *rulesTable, const MetisMessage *message);
#endif // Metis_metis_MatchingRulesTable_h
