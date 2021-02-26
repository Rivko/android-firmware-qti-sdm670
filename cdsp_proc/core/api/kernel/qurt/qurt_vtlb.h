/*=============================================================================

                                    qurt_vtlb.h

GENERAL DESCRIPTION

EXTERNAL FUNCTIONS
        None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
        None.

             Copyright (c) 2013, 2019  by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/
#ifndef QURT_VTLB_H
#define QURT_VTLB_H

/*
||  Names starting with "qurt_i_vtlb" are the internal low-level functions.
||  These should be considered subject to change.
*/

int qurt_i_vtlb_entry_create(unsigned *pIndex,
                             unsigned tlb_lo,
                             unsigned tlb_hi,
                             unsigned extension);

int qurt_i_vtlb_entry_delete(unsigned index);

int qurt_i_vtlb_entry_read(unsigned index, unsigned *tlbinfo);

int qurt_i_vtlb_entry_write(unsigned index, unsigned tlb_lo, unsigned tlb_hi, unsigned extension);

int qurt_i_vtlb_entry_probe(const void *vaddr, unsigned *tlbinfo, unsigned *pIndex);

int qurt_i_vtlb_entry_split(unsigned index, unsigned *split_indexes);

int qurt_i_vtlb_entry_merge(unsigned index, unsigned *freed_indexes);    // TBD -- is this the right prototype?

int qurt_i_vtlb_statistics(unsigned *stats); // Returns stats[0] -- total number of VTLB entries
                                             //         stats[1] -- number of available VTLB entries
                                             //         stats[2] -- max size of VTLB tree since boot

//can return index to an entry that was specialed, change it to take addresses instead of pages
int qurt_i_vtlb_set_special(int index, unsigned pageno, unsigned asid, unsigned size);

int qurt_i_vtlb_queue_ppage(unsigned pageno, unsigned vtlb_index);

int qurt_i_vtlb_asid_state(unsigned asid, unsigned new_state);

#define QURT_VTLB_EXT_DEFAULT 0
#define QURT_VTLB_EXT_LOCKED  1

#define QURT_VTLB_ERR_OVERLAP           64
#define QURT_VTLB_ERR_TREE_NO_SPACE     65
#define QURT_VTLB_ERR_ILLEGAL           66

#endif // QURT_VTLB_H
