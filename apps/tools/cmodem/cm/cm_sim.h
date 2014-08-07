
// -------------------------------------------------------------------------------------------------
/**
 *  @file cm_sim.h
 *
 *  Handle sim related functionality
 *
 *  Copyright (C) Sierra Wireless, Inc. 2014. All rights reserved. Use of this work is subject to license.
 */
// -------------------------------------------------------------------------------------------------

#ifndef CMODEM_SIM_INCLUDE_GUARD
#define CMODEM_SIM_INCLUDE_GUARD


// -------------------------------------------------------------------------------------------------
/**
*  Print the sim help text to stdout.
*/
// -------------------------------------------------------------------------------------------------
void cm_sim_PrintSimHelp
(
    void
);


// -------------------------------------------------------------------------------------------------
/**
*  This function will attempt to get the SIM state.
*
*  @return EXIT_SUCCESS if the call was successful, EXIT_FAILURE otherwise.
*/
// -------------------------------------------------------------------------------------------------
int cm_sim_GetSimStatus
(
    void
);


// -------------------------------------------------------------------------------------------------
/**
*  This function will enter the pin code for the sim.
*
*  @return EXIT_SUCCESS if the call was successful, EXIT_FAILURE otherwise.
*/
// -------------------------------------------------------------------------------------------------
int cm_sim_EnterPin
(
    const char * pin    ///< [IN] PIN code
);


// -------------------------------------------------------------------------------------------------
/**
*  This function will change the pin code for the sim.
*
*  @return EXIT_SUCCESS if the call was successful, EXIT_FAILURE otherwise.
*/
// -------------------------------------------------------------------------------------------------
int cm_sim_ChangePin
(
    const char * oldPinPtr,     ///< [IN] Old PIN code
    const char * newPinPtr      ///< [IN] New PIN code
);


// -------------------------------------------------------------------------------------------------
/**
*  This function will lock the sim.
*
*  @return EXIT_SUCCESS if the call was successful, EXIT_FAILURE otherwise.
*/
// -------------------------------------------------------------------------------------------------
int cm_sim_LockSim
(
    const char * pin    ///< [IN] PIN code
);


// -------------------------------------------------------------------------------------------------
/**
*  This function will unlock the sim.
*
*  @return EXIT_SUCCESS if the call was successful, EXIT_FAILURE otherwise.
*/
// -------------------------------------------------------------------------------------------------
int cm_sim_UnlockSim
(
    const char * pin    ///< [IN] PIN code
);


// -------------------------------------------------------------------------------------------------
/**
*  This function will unblock the sim.
*
*  @return EXIT_SUCCESS if the call was successful, EXIT_FAILURE otherwise.
*/
// -------------------------------------------------------------------------------------------------
int cm_sim_UnblockSim
(
    const char * puk,       ///< [IN] PUK code
    const char * newpin     ///< [IN] New PIN code
);


// -------------------------------------------------------------------------------------------------
/**
*  This function will store the pin in the configdb.
*
*  @return EXIT_SUCCESS if the call was successful, EXIT_FAILURE otherwise.
*/
// -------------------------------------------------------------------------------------------------
int cm_sim_StorePin
(
    const char * pin    ///< [IN] PIN code
);


#endif

