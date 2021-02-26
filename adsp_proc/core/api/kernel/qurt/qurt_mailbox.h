#ifndef QURT_MAILBOX_H
#define QURT_MAILBOX_H

/**
  @file qurt_mailbox.h
  @brief  Definitions, macros, and prototypes used for QuRT mailbox

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2015  by Qualcomm Technologies, Inc.  All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 ======================================================================*/


/* Definitions on typedef and return values */

#define   QURT_MAILBOX_ID_NULL               0
#define   QURT_MAILBOX_ERROR                -1
#define   QURT_MAILBOX_ID_ERROR             -2
#define   QURT_MAILBOX_NON_VALID_DATA       -3
#define   QURT_MAILBOX_FULL                 -4
#define   QURT_MAILBOX_DELETED              -5
#define   QURT_MAILBOX_RECEIVE_HALTED       -6
#define   QURT_MAILBOX_BANDWIDTH_LIMIT      -7


typedef enum {
        QURT_MAILBOX_AT_QURTOS=0,            // Receiver is QurtOS
        QURT_MAILBOX_AT_ROOTPD=1,            // Receiver is RootPD  (ASID=0)
        QURT_MAILBOX_AT_USERPD=2,            // Receiver is User PD (ASID!=0)
        QURT_MAILBOX_AT_SECUREPD=3,          // Receiver is Secure PD
} qurt_mailbox_receiver_cfg_t;  


typedef enum {
        QURT_MAILBOX_SEND_OVERWRITE=0,       // When there is already valid content, overwrite it
        QURT_MAILBOX_SEND_NON_OVERWRITE=1,   // When there is already valid content, return failure
} qurt_mailbox_send_option_t;  


typedef enum {
        QURT_MAILBOX_RECV_WAITING=0,          // When there is no valid content, wait for it 
        QURT_MAILBOX_RECV_NON_WAITING=1,      // When there is no valid content, return failure immediately
        QURT_MAILBOX_RECV_PEEK_NON_WAITING=2, // Read the content, but doesn't remove it from the mailbox. No waiting.
} qurt_mailbox_recv_option_t;  



/* Function prototype */

/**@ingroup qurt_mailbox_create
  Create Qurt mailbox
   
  @param name            Mailbox name up to 8 characters
  @param recv_opt        Configuration on the receiver process

  @return
  Mailbox ID             Mailbox Identifier
  QURT_MAILBOX_ID_NULL   NULL. Failure at creating mailbox

  @dependencies
  None.
*/
unsigned long long qurt_mailbox_create(char *name, qurt_mailbox_receiver_cfg_t recv_opt);


/**@ingroup qurt_mailbox_get_id
  Create Qurt mailbox
   
  @param name            Mailbox name up to 8 characters

  @return
  Mailbox ID             Mailbox Identifier
  QURT_MAILBOX_ID_NULL   NULL. Failure at getting mailbox ID

  @dependencies
  None.
*/
unsigned long long qurt_mailbox_get_id(char *name);


/**@ingroup qurt_mailbox_send
  Send data to Qurt mailbox
   
  @param mailbox_id   Mailbox Identifier
  @param send_opt     Option for mailbox send
  @param data         Data to send


  @return
  QURT_EOK                      Success
  QURT_MAILBOX_ID_ERROR         Mailbox ID Error.
  QURT_MAILBOX_ERROR            Other errors.
  QURT_MAILBOX_FULL             Valid data already exists, non-overwriting.
  QURT_MAILBOX_BANDWIDTH_LIMIT  Reach the bandwidth limitation   

  @dependencies
  None.
*/
int qurt_mailbox_send(unsigned long long mailbox_id, qurt_mailbox_send_option_t send_opt, unsigned long long data);


/**@ingroup qurt_mailbox_receive
  Send data to Qurt mailbox
   
  @param mailbox_id   Mailbox Identifier
  @param send_opt     Option for mailbox receiving
  @param data         Pointer to data buffer for receiving

  @return
  QURT_EOK                            Success
  QURT_MAILBOX_ID_ERROR               Mailbox ID Error.
  QURT_MAILBOX_ERROR                  Other errors.
  QURT_MAILBOX_NON_VALID_DATA         No current valid data, putting the previous content into buffer
  QURT_MAILBOX_RECEIVE_HALTED         The mailbox receiving is halted, and the waiting thread is woken up.
  QURT_MAILBOX_DELETED                The mailbox is deleted, and the waiting thread is woken up.

  @dependencies
  None.
*/
int qurt_mailbox_receive(unsigned long long mailbox_id, qurt_mailbox_recv_option_t recv_opt, unsigned long long *data);


/**@ingroup qurt_mailbox_delete
  Delete a Qurt mailbox

  A mailbox can only be deleted from the process that created the mailbox.
   
  @param mailbox_id   Mailbox Identifier

  @return
  QURT_EOK                   Success
  QURT_MAILBOX_ID_ERROR      Mailbox ID Error.
  QURT_MAILBOX_ERROR         Other errors.

  @dependencies
  None.
*/
int qurt_mailbox_delete(unsigned long long mailbox_id);


/**@ingroup qurt_mailbox_receive_halt
  Halt Qurt mailbox receiving and wake up waiting threads

  @param mailbox_id   Mailbox Identifier

  @return
  QURT_EOK                   Success
  QURT_MAILBOX_ID_ERROR      Mailbox ID Error.
  QURT_MAILBOX_ERROR         Other errors.

  @dependencies
  None.
*/
int qurt_mailbox_receive_halt(unsigned long long mailbox_id);



#endif // QURT_MAILBOX_H
