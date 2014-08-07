/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */

/**
 * @page c_sim SIM Services
 *
 * @ref sim_interface.h "Click here for the API reference documentation."
 *
 * <HR>
 *
 * This file contains prototype definitions for high level SIM APIs.
 *
 * @ref le_sim_count <br>
 * @ref le_sim_create <br>
 * @ref le_sim_delete <br>
 * @ref le_sim_id <br>
 * @ref le_sim_auth <br>
 * @ref le_sim_state <br>
 *
 * A subscriber identity module or subscriber identification module (SIM) is an integrated circuit
 * that securely stores the international mobile subscriber identity (IMSI) and related key used
 * to identify and authenticate subscribers on M2M devices.
 *
 * Most SIM cards can store a number of SMS messages and phone book contacts.
 *
 * @section le_sim_count Counting the SIM card slots
 * @c le_sim_CountSlots() returns the number of SIM card sockets mounted on the device.
 *
 * @c le_sim_GetSelectedCard() returns the selected SIM card number.
 *
 * @section le_sim_create Creating a SIM object
 * You must create a SIM object by calling le_sim_Create() first; the card socket number must be specified with a parameter.
 * This passes the card socket number to create the required corresponding object
 * to the selected SIM card. Resources are automatically allocated for the SIM
 * object, which is referenced by @c le_sim_ObjRef_t type.
 *
 * When the SIM object is no longer needed, you must call @c le_sim_Delete() to free all
 * allocated ressources associated with the object.
 *
 * @section le_sim_delete Deleting a SIM object
 * To delete a SIM object, call @c le_sim_Delete(). This frees all the resources allocated for
 * the SIM object. If several users own the SIM object (e.g., several
 * handler functions registered for new state notifications), the SIM object will only
 * be actually deleted after the last user deletes the SIM object.
 *
 * @section le_sim_id SIM identification information
 * \b ICCID:
 * Each SIM is internationally identified by its integrated circuit card identifier (ICCID). ICCIDs
 * are stored in the SIM cards and engraved or printed on the SIM card body.
 * The ICCID is defined by the ITU-T recommendation E.118 as the
 * Primary Account Number. According to E.118, the number is up to 19 digits long, including a
 * single check digit calculated using the Luhn algorithm. However, the GSM Phase 1 (ETSI
 * Recommendation GSM 11.11) defined the ICCID length as 10 octets (20 digits) with
 * operator-specific structure.
 *
 * @c le_sim_GetICCID() API reads the identification number (ICCID).
 *
 * \b IMSI:
 * The International Mobile Subscriber Identity or IMSI is a unique identification associated with
 * all cellular networks. The IMSI is used in any mobile network that connects with other
 * networks. For GSM, UMTS and LTE network, this number is provisioned in the SIM card.
 *
 * An IMSI is usually presented as a 15 digit long number, but can be shorter. The first 3 digits
 * are the mobile country code (MCC), are followed by the mobile network code (MNC), either 2
 * digits (European standard) or 3 digits (North American standard). The length of the MNC depends
 * on the value of the MCC. The remaining digits are the mobile subscription identification number
 * (MSIN) within the network's customer base.
 *
 * @c le_sim_GetIMSI() API reads the international mobile subscriber identity (IMSI).
 *
 * \b Phone \b Number:
 * @c le_sim_GetSubscriberPhoneNumber() API reads the Phone Number associated to the SIM.
 * If the phone number has not been provisioned, it will return the empty string.
 *
 * @section le_sim_auth SIM Authentication
 * @c le_sim_EnterPIN() enters the PIN (Personal Identification Number) code that's
 * required before any Mobile equipment functionality can be used.
 *
 * @c le_sim_GetRemainingPINTries() returns the number of remaining PIN entry attempts
 * before the SIM will become blocked.
 *
 * @c le_sim_ChangePIN() must be called to change the PIN code.
 *
 * @c  le_sim_Lock() locks the SIM card: it enables requests for the PIN code.
 *
 * @c  le_sim_Unlock() unlocks the SIM card: it disables requests for the PIN code.
 *
 * @c le_sim_Unblock() unblocks the SIM card. The SIM card is blocked after X unsuccessful
 * attempts to enter the PIN. @c le_sim_Unblock() requires the PUK (Personal Unblocking) code
 * to set a new PIN code.
 *
 * @section le_sim_state SIM states
 * @c le_sim_IsPresent() API advises the SIM is inserted (and locked) or removed.
 *
 * @c le_sim_IsReady() API advises the SIM is ready (PIN code correctly entered
 * or not required).
 *
 * The le_sim_GetState() API retrieves the SIM state:
 * - LE_SIM_INSERTED      : SIM card is inserted and locked.
 * - LE_SIM_ABSENT        : SIM card is absent.
 * - LE_SIM_READY         : SIM card is inserted and unlocked.
 * - LE_SIM_BLOCKED       : SIM card is blocked.
 * - LE_SIM_BUSY          : SIM card is busy.
 * - LE_SIM_STATE_UNKNOWN : Unknown SIM state.
 *
 * A handler function must be registered to receive SIM's state notifications.
 * @c le_sim_AddNewStateHandler() API allows the User to register that handler.
 *
 * The handler must satisfy the following prototype:
 * typedef void(*le_sim_NewStateHandlerFunc_t)(le_sim_ObjRef_t sim);
 *
 * When a new SIM's state is notified, a SIM object is automatically created and the handler is
 * called.
 *
 * Call @c le_sim_GetState() to retrieve the new state of the SIM.
 *
 * @note If two (or more) applications have registered a handler function for notifications, they
 * will all receive it and will be passed the same SIM object reference.
 *
 * The application can uninstall the handler function by calling @c le_sim_RemoveNewStateHandler() API.
 * @note @c le_sim_RemoveNewStateHandler() API does not delete the SIM Object. The caller has to
 *       delete it.
 *
 *
 * <HR>
 *
 * Copyright (C) Sierra Wireless, Inc. 2014. Use of this work is subject
 * to license.
 */
/** @file sim_interface.h
 *
 * Legato @ref c_sim include file.
 *
 * Copyright (C) Sierra Wireless, Inc. 2014. Use of this work is subject to license.
 */

#ifndef SIM_INTERFACE_H_INCLUDE_GUARD
#define SIM_INTERFACE_H_INCLUDE_GUARD


#include "legato.h"

// User customizable include file
#include "userInclude.h"


//--------------------------------------------------------------------------------------------------
/**
 * Start the service for the client main thread
 */
//--------------------------------------------------------------------------------------------------
void le_sim_StartClient
(
    const char* serviceInstanceName
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Stop the service for the current client thread
 */
//--------------------------------------------------------------------------------------------------
void le_sim_StopClient
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Reference type for le_sim_NewStateHandler handler ADD/REMOVE functions
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_sim_NewStateHandler* le_sim_NewStateHandlerRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * Handler for sim state changes.
 *
 *
 * @param simRef
 *        SIM reference.
 * @param contextPtr
 */
//--------------------------------------------------------------------------------------------------
typedef void (*le_sim_NewStateHandlerFunc_t)
(
    le_sim_ObjRef_t simRef,
    void* contextPtr
);

//--------------------------------------------------------------------------------------------------
/**
 * le_sim_NewStateHandler handler ADD function
 */
//--------------------------------------------------------------------------------------------------
le_sim_NewStateHandlerRef_t le_sim_AddNewStateHandler
(
    le_sim_NewStateHandlerFunc_t handlerPtr,
        ///< [IN]

    void* contextPtr
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * le_sim_NewStateHandler handler REMOVE function
 */
//--------------------------------------------------------------------------------------------------
void le_sim_RemoveNewStateHandler
(
    le_sim_NewStateHandlerRef_t addHandlerRef
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Count the number of SIM card slots.
 *
 * @return Number of the SIM card slots mounted on the device.
 */
//--------------------------------------------------------------------------------------------------
uint32_t le_sim_CountSlots
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the current selected card.
 *
 * @return Number of the current selected SIM card.
 */
//--------------------------------------------------------------------------------------------------
uint32_t le_sim_GetSelectedCard
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Create a SIM object.
 *
 * @return Reference to the SIM object.
 *
 * @note
 *      On failure, the process exits, so no need to check the return value for validity
 */
//--------------------------------------------------------------------------------------------------
le_sim_ObjRef_t le_sim_Create
(
    uint32_t cardNum
        ///< [IN]
        ///< The SIM card number. (1 or 2, it depends on the platform).
);

//--------------------------------------------------------------------------------------------------
/**
 * Delete a SIM object.
 *
 * This frees all the resources allocated for
 * the SIM object. If several users own the SIM object (e.g., several
 * handler functions registered for new state notifications), the SIM object will only
 * be actually deleted after the last user deletes the SIM object.
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
void le_sim_Delete
(
    le_sim_ObjRef_t simRef
        ///< [IN]
        ///< The SIM object.
);

//--------------------------------------------------------------------------------------------------
/**
 * Retrieve the slot number of the SIM card.
 *
 * @return Slot number of the SIM card.
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
uint32_t le_sim_GetSlotNumber
(
    le_sim_ObjRef_t simRef
        ///< [IN]
        ///< The SIM object.
);

//--------------------------------------------------------------------------------------------------
/**
 * Retrieves the integrated circuit card identifier (ICCID) of the SIM card (20 digits)
 *
 * @return LE_OK             ICCID was successfully retrieved.
 * @return LE_OVERFLOW       iccidPtr buffer was too small for the ICCID.
 * @return LE_NOT_POSSIBLE   ICCID could not be retrieved.
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_GetICCID
(
    le_sim_ObjRef_t simRef,
        ///< [IN]
        ///< The SIM object.

    char* iccid,
        ///< [OUT]
        ///< ICCID

    size_t iccidNumElements
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Retrieves the identification number (IMSI) of the SIM card. (max 15 digits)
 *
 * @return LE_OK            IMSI was successfully retrieved.
 * @return LE_OVERFLOW       imsiPtr buffer was too small for the IMSI.
 * @return LE_NOT_POSSIBLE  IMSI could not be retrieved.
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_GetIMSI
(
    le_sim_ObjRef_t simRef,
        ///< [IN]
        ///< The SIM object.

    char* imsi,
        ///< [OUT]
        ///< IMSI

    size_t imsiNumElements
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Verify if the SIM card is present or not.
 *
 * @return true   SIM card is present.
 * @return false  SIM card is absent
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
bool le_sim_IsPresent
(
    le_sim_ObjRef_t simRef
        ///< [IN]
        ///< The SIM object.
);

//--------------------------------------------------------------------------------------------------
/**
 * Verify if the SIM is ready (PIN code correctly inserted or not
 * required).
 *
 * @return true   PIN is correctly inserted or not required.
 * @return false  PIN must be inserted
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
bool le_sim_IsReady
(
    le_sim_ObjRef_t simRef
        ///< [IN]
        ///< The SIM object.
);

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to enter the PIN code.
 *
 * @return LE_NOT_FOUND     Function failed to select the SIM card for this operation.
 * @return LE_UNDERFLOW     PIN code is not long enough (min 4 digits).
 * @return LE_NOT_POSSIBLE  Function failed to enter the PIN code.
 * @return LE_OK            Function succeeded.
 *
 * @note If PIN code is too long (max 8 digits), it is a fatal error, the
 *       function will not return.
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_EnterPIN
(
    le_sim_ObjRef_t simRef,
        ///< [IN]
        ///< The SIM object.

    const char* pin
        ///< [IN]
        ///< The PIN code.
);

//--------------------------------------------------------------------------------------------------
/**
 * Change the PIN code.
 *
 * @return LE_NOT_FOUND     Function failed to select the SIM card for this operation.
 * @return LE_UNDERFLOW     PIN code is/are not long enough (min 4 digits).
 * @return LE_NOT_POSSIBLE  Function failed to change the PIN code.
 * @return LE_OK            Function succeeded.
 *
 * @note If PIN code is too long (max 8 digits), it is a fatal error, the
 *       function will not return.
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_ChangePIN
(
    le_sim_ObjRef_t simRef,
        ///< [IN]
        ///< The SIM object.

    const char* oldpin,
        ///< [IN]
        ///< The old PIN code.

    const char* newpin
        ///< [IN]
        ///< The new PIN code.
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the number of remaining PIN insertion tries.
 *
 * @return LE_NOT_FOUND     Function failed to select the SIM card for this operation.
 * @return LE_NOT_POSSIBLE  Function failed to get the number of remaining PIN insertion tries.
 * @return A positive value Function succeeded. The number of remaining PIN insertion tries.
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
int32_t le_sim_GetRemainingPINTries
(
    le_sim_ObjRef_t simRef
        ///< [IN]
        ///< The SIM object.
);

//--------------------------------------------------------------------------------------------------
/**
 * Unlock the SIM card: it disables the request of the PIN code.
 *
 * @return LE_NOT_FOUND     Function failed to select the SIM card for this operation.
 * @return LE_UNDERFLOW     PIN code is not long enough (min 4 digits).
 * @return LE_NOT_POSSIBLE  Function failed to unlock the SIM card.
 * @return LE_OK            Function succeeded.
 *
 * @note If PIN code is too long (max 8 digits), it is a fatal error, the
 *       function will not return.
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_Unlock
(
    le_sim_ObjRef_t simRef,
        ///< [IN]
        ///< The SIM object.

    const char* pin
        ///< [IN]
        ///< The PIN code.
);

//--------------------------------------------------------------------------------------------------
/**
 * Lock the SIM card: it enables the request of the PIN code.
 *
 * @return LE_NOT_FOUND     Function failed to select the SIM card for this operation.
 * @return LE_UNDERFLOW     PIN code is not long enough (min 4 digits).
 * @return LE_NOT_POSSIBLE  Function failed to unlock the SIM card.
 * @return LE_OK            Function succeeded.
 *
 * @note If PIN code is too long (max 8 digits), it is a fatal error, the
 *       function will not return.
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_Lock
(
    le_sim_ObjRef_t simRef,
        ///< [IN]
        ///< The SIM object.

    const char* pin
        ///< [IN]
        ///< The PIN code.
);

//--------------------------------------------------------------------------------------------------
/**
 * Unblock the SIM card.
 *
 * @return LE_NOT_FOUND     Function failed to select the SIM card for this operation.
 * @return LE_UNDERFLOW     PIN code is not long enough (min 4 digits).
 * @return LE_OUT_OF_RANGE  PUK code length is not correct (8 digits).
 * @return LE_NOT_POSSIBLE  Function failed to unblock the SIM card.
 * @return LE_OK            Function succeeded.
 *
 * @note If new PIN or puk code are too long (max 8 digits), it is a fatal error, the
 *       function will not return.
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_Unblock
(
    le_sim_ObjRef_t simRef,
        ///< [IN]
        ///< The SIM object.

    const char* puk,
        ///< [IN]
        ///< The PUK code.

    const char* newpin
        ///< [IN]
        ///< The PIN code.
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the SIM state.
 *
 * @return Current SIM state.
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_sim_States_t le_sim_GetState
(
    le_sim_ObjRef_t simRef
        ///< [IN]
        ///< The SIM object.
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the SIM Phone Number.
 *
 * @return
 *      - LE_OK on success
 *      - LE_OVERFLOW if the Phone Number can't fit in phoneNumberStr
 *      - LE_NOT_POSSIBLE on any other failure
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_GetSubscriberPhoneNumber
(
    le_sim_ObjRef_t simRef,
        ///< [IN]
        ///< The SIM object.

    char* phoneNumberStr,
        ///< [OUT]
        ///< The phone Number.

    size_t phoneNumberStrNumElements
        ///< [IN]
);


#endif // SIM_INTERFACE_H_INCLUDE_GUARD

