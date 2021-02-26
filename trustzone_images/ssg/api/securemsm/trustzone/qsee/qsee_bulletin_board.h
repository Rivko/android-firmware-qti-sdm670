/**
@file qsee_bulletin_board.h
@brief Provides simple message posting mechanism between QSEE applications
*/
/*===========================================================================
   Copyright (c) 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

when         who      what, where, why
--------     ---      ------------------------------------
Jun/01/2015  el       Initial version.
===========================================================================*/
#ifndef QSEE_BULLETIN_BOARD_H
#define QSEE_BULLETIN_BOARD_H


/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <stdint.h>

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define MAX_CATEGORY_NAME_LEN 8
#define MAX_TA_NAME_LEN       128               /* distinguished name */

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef struct qsee_bulletin_board_note_
{
    uint8_t* data;
    uint32_t size;

    char category[MAX_CATEGORY_NAME_LEN+1];

} qsee_bulletin_board_note;


typedef struct qsee_ta_name_
{
    char name[MAX_TA_NAME_LEN+1];

} qsee_ta_name;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
 * @brief Post a note to QSEE bulletin board, under a specific category.
 *
 *        QSEE bulletin board contains several categories, each category only
 *        holds the recent note that was posted to that category, overwriting
 *        the previous one.
 *        Notes, cannot be deleted from the bulletin board and are available
 *        until they're overwritten by newer note or the device reboots.
 *
 *        No permission model is applied on the bulletin board.
 *        Every QSEE application can post notes under any category.
 *
 * @param[in] note        the note to post.
 *
 *                        note->data shall point to a buffer that holds the
 *                        data to post.
 *
 *                        note->size indicates the size of the data buffer.
 *
 *                        note->category shall be non-empty, NULL terminated
 *                        ascii string that holds the name of the category under
 *                        which the note will get posted.
 *
 * @return 0 on success, negative on failure (values are taken from IxErrno.h)
*/
int qsee_bulletin_board_post(const qsee_bulletin_board_note* note);


/**
 * @brief Read a note from QSEE bulletin board.
 *
 *        This function reads a note (if available) from a specific category.
 *
 *        No permission model is applied on the bulletin board.
 *        Every QSEE application can read all available notes from all categories.
 *
 * @param[inout] note     structure to hold the read note.
 *
 *                        note->data shall point to a buffer that has enough space
 *                        to hold the note from the desired category.
 *
 *                        note->size is a bi-directional parameter. when used as input
 *                        it indicates the size of the buffer.
 *                        when the function returns, it will hold the amount of bytes
 *                        that were actually copied into the user provided buffer.
 *
 *                        note->category shall hold the name of the category which
 *                        its note is desired.
 *
 * @param[inout] sender_name     pointer to a structure that will hold the name of the TA
 *                               which posted this note.
 *
 * @return 0 on success, negative on failure (values are taken from IxErrno.h)
*/
int qsee_bulletin_board_read(qsee_bulletin_board_note* note, qsee_ta_name* sender_name);


#endif // QSEE_BULLETIN_BOARD_H
