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
 * Main entry point for control program.  Handles parsing the optional --keystore and --password options,
 * then dispatches the command.
 *
 * @author Marc Mosko, Alan Walendowski, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#include <config.h>

#include <stdbool.h>
#include <stdint.h>
#include <strings.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>

#include <LongBow/runtime.h>
#include <string.h>

#include <parc/security/parc_Security.h>
#include <parc/security/parc_IdentityFile.h>

#include <parc/algol/parc_Memory.h>
#include <parc/algol/parc_SafeMemory.h>
#include <parc/algol/parc_List.h>
#include <parc/algol/parc_ArrayList.h>

#include <ccnx/api/control/cpi_ManageLinks.h>

#include <ccnx/api/control/cpi_Forwarding.h>
#include <ccnx/forwarder/metis/core/metis_Forwarder.h>
#include <ccnx/forwarder/metis/core/metis_Dispatcher.h>

#include <ccnx/api/ccnx_Portal/ccnx_Portal.h>
#include <ccnx/api/ccnx_Portal/ccnx_PortalRTA.h>

#include <ccnx/common/ccnx_KeystoreUtilities.h>
#include <ccnx/transport/common/transport_MetaMessage.h>

#include <ccnx/forwarder/metis/metis_About.h>
#include <ccnx/forwarder/metis/config/metis_ControlState.h>
#include <ccnx/forwarder/metis/config/metisControl_Root.h>

#include <errno.h>

typedef struct metis_control_main_state {
    KeystoreParams *keystoreParams;
    CCNxPortal *controlPortal;
    MetisControlState *controlState;
} MetisControlMainState;

static void
_displayForwarderLogo(void)
{
    printf("%s\n", metisAbout_About());

    printf("            __  __        _    _\n");
    printf("           |  \\/  |  ___ | |_ (_) ___\n");
    printf("           | |\\/| | / _ \\| __|| |/ __|\n");
    printf("           | |  | ||  __/| |_ | |\\__ \\\n");
    printf("           |_|  |_| \\___| \\__||_||___/\n");

    printf("\n");
}

static void
_displayUsage(char *programName)
{
    printf("Usage: %s -h\n", programName);
    printf("       %s [--k|--keystore <keystore file name>] [--p|--password <keystore password>] [commands]\n", programName);
    printf("\n");
    printf("Metis is the CCNx 1.0 forwarder, which runs on each end system and as a software forwarder\n");
    printf("on intermediate systems.  metis_control is the program to configure the forwarder, metis_daemon.\n");
    printf("\n");
    printf("Options:\n");
    printf("-h              = This help screen\n");
    printf("-k | --keystore = Specify the path of the PKCS12 keystore (default ~/.ccnx/.ccnx_keystore.p12)\n");
    printf("-p | --password = keystore password (default to prompt user)\n");
    printf("commands        = configuration line to send to metis (use 'help' for list)\n");
    printf("\n");
}

static int
_parseArgs(int argc, char *argv[], char **keystorePath, char **keystorePassword, PARCList *commandList)
{
    static struct option longFormOptions[] = {
        { "help",     no_argument,       0, 'h' },
        { "keystore", required_argument, 0, 'k' },
        { "password", required_argument, 0, 'p' },
        { 0,          0,                 0, 0   }
    };

    int c;

    while (1) {
        // getopt_long stores the option index here.
        int optionIndex = 0;

        c = getopt_long(argc, argv, "hk:p:", longFormOptions, &optionIndex);

        // Detect the end of the options.
        if (c == -1) {
            break;
        }

        switch (c) {
            case 'k':
                *keystorePath = optarg;
                break;

            case 'p':
                *keystorePassword = optarg;
                break;

            case 'h':
            default:
                _displayUsage(argv[0]);
                return 0;
        }
    }

    // Any remaining parameters get put in the command list.
    if (optind < argc) {
        while (optind < argc) {
            parcList_Add(commandList, argv[optind]);
            optind++;
        }
    }

    return 1;
}

static CCNxMetaMessage *
_writeAndReadMessage(void *mainStatePtr, CCNxMetaMessage *msg)
{
    assertNotNull(mainStatePtr, "Parameter mainStatePtr must be non-null");
    assertNotNull(msg, "Parameter msg must be non-null");

    MetisControlMainState *mainState = mainStatePtr;

    if (ccnxPortal_Send(mainState->controlPortal, msg, CCNxStackTimeout_Never)) {
        CCNxMetaMessage *result = ccnxPortal_Receive(mainState->controlPortal, CCNxStackTimeout_Never);
        assertTrue(result != NULL, "Error reading response from Portal: (%d) %s\nb", errno, strerror(errno));

        return result;
    }

    return NULL;
}

static CCNxPortal *
_createPortalWithKeystore(const char *keystoreName, const char *keystorePassword)
{
    PARCIdentityFile *identityFile = parcIdentityFile_Create(keystoreName, keystorePassword);
    PARCIdentity *identity = parcIdentity_Create(identityFile, PARCIdentityFileAsPARCIdentity);
    CCNxPortalFactory *portalFactory = ccnxPortalFactory_Create(identity);

    CCNxPortal *result = ccnxPortalFactory_CreatePortal(portalFactory, ccnxPortalRTA_Message);

    ccnxPortalFactory_Release(&portalFactory);
    parcIdentity_Release(&identity);
    parcIdentityFile_Release(&identityFile);

    return result;
}

static MetisControlMainState
_openKeyStore(char *keystorePath, char *keystorePassword)
{
    MetisControlMainState mainState;

    if (keystorePassword == NULL) {
        keystorePassword = ccnxKeystoreUtilities_ReadPassword();

        mainState.keystoreParams = ccnxKeystoreUtilities_OpenFile(keystorePath, keystorePassword);
        parcMemory_Deallocate((void *) &keystorePassword);
    } else {
        mainState.keystoreParams = ccnxKeystoreUtilities_OpenFile(keystorePath, keystorePassword);
    }

    return mainState;
}

int
main(int argc, char *argv[])
{
    _displayForwarderLogo();

    if (argc == 2 && strcmp("-h", argv[1]) == 0) {
        _displayUsage(argv[0]);
        exit(EXIT_SUCCESS);
    }

    char *keystorePath = NULL;
    char *keystorePassword = NULL;

    PARCList *commands = parcList(parcArrayList_Create(NULL), PARCArrayListAsPARCList);

    // Extract optional keystore and password, and optional commands.
    if (!_parseArgs(argc, argv, &keystorePath, &keystorePassword, commands)) {
        parcList_Release(&commands);
        exit(EXIT_FAILURE);
    }

    if (keystorePath == NULL) {
        printf("No keystore specified. Will try default.\n");
    } else {
        printf("Using keystore: %s\n", keystorePath);
    }

    parcSecurity_Init();

    MetisControlMainState mainState = _openKeyStore(keystorePath, keystorePassword);
    if (mainState.keystoreParams == NULL) {
        printf("Could not open keystore '%s'\n", keystorePath == NULL ? "~/.ccnx/.ccnx_keystore.p12" : keystorePath);
        exit(EXIT_FAILURE);
    }

    mainState.controlPortal = _createPortalWithKeystore(ccnxKeystoreUtilities_GetFileName(mainState.keystoreParams),
                                                        ccnxKeystoreUtilities_GetPassword(mainState.keystoreParams));
    parcSecurity_Fini();

    mainState.controlState = metisControlState_Create(&mainState, _writeAndReadMessage);

    metisControlState_RegisterCommand(mainState.controlState, metisControlRoot_HelpCreate(mainState.controlState));
    metisControlState_RegisterCommand(mainState.controlState, metisControlRoot_Create(mainState.controlState));

    if (parcList_Size(commands) > 0) {
        metisControlState_DispatchCommand(mainState.controlState, commands);
    } else {
        metisControlState_Interactive(mainState.controlState);
    }

    parcList_Release(&commands);

    metisControlState_Destroy(&mainState.controlState);

    keystoreParams_Destroy(&mainState.keystoreParams);

    ccnxPortal_Release(&mainState.controlPortal);

    return EXIT_SUCCESS;
}
