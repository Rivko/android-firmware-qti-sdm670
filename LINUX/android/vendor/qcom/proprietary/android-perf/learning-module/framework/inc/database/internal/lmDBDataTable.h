/******************************************************************************
  @file    lmDBDataTable.h
  @brief   Implementation of Learning Module Database

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
 ******************************************************************************/


#ifndef LMDB_DATATABLE_H_
#define LMDB_DATATABLE_H_

#include <vector>
#include <list>

#include "lmDBDebug.h"

template<class T>
class LMDBDataTable {
public:
    using valType = LMDBDataRow<T>;
    using ptrType = LMDBDataRow<T>*;
    using vecT = std::vector<ptrType>;
    using lstT = std::list<ptrType>;
protected:
    template <class N>
    LMDBDataTable(N&&);
    ~LMDBDataTable();
    vecT getRow(DRSize);
    int freeRow(vecT&);
    int cacheRow(vecT&);
    const TblName getTableName();
    lstT lmdb_cachedListPtr;
    Mtx lmdb_mtx;
private:
    LMDBDataTable(const LMDBDataTable& copy);               //Not to be implemented
    LMDBDataTable& operator=(const LMDBDataTable& copy);    //Not to be implemented

#ifdef LMDB_DATATABLE_DEBUG
    lstT lmdb_busyListPtr;
#endif //LMDB_DATATABLE_DEBUG
    const bool busyListDebug;
    const TblName tableName;
};


template<class T>
template<class N>
LMDBDataTable<T>::LMDBDataTable(N&& tname) : busyListDebug(BUSY_LIST_DEBUG), tableName(std::forward<N>(tname)) {
    LMDB_DEBUG_LIB_DETAILED("entering Ctor Dbt=%s\n", tableName.c_str());
}

template<class T>
LMDBDataTable<T>::~LMDBDataTable() {
    const char *tname = tableName.c_str();
    LMDB_DEBUG_LIB_DETAILED("entering Dtor Dbt=%s\n", tname);
    //may have to push cachelist to DB and then free both cachelist & busylist elements
    int cLstSz = lmdb_cachedListPtr.size(), bLstSz=0;

#ifdef LMDB_DATATABLE_DEBUG
    if (busyListDebug)
        bLstSz=lmdb_busyListPtr.size();
#endif //LMDB_DATATABLE_DEBUG

    if(bLstSz != 0 || cLstSz != 0) {
        QLOGE("ERROR:Dbt=%s blstSz=%d, clstSz=%d\n", tname, bLstSz, cLstSz);

        {
            std::lock_guard<Mtx> lock(lmdb_mtx);
            for(auto itr : lmdb_cachedListPtr)
            {
                ptrType rowPtr = itr;
                LMDB_DEBUG_LIB_DETAILED("Dbt=%s cLstrowPtr=%p\n", tname, rowPtr);
                if (rowPtr)
                    delete rowPtr;

            }
            lmdb_cachedListPtr.clear();

#ifdef LMDB_DATATABLE_DEBUG
            if(busyListDebug) {
                for(auto itr : lmdb_busyListPtr)
                {
                    ptrType rowPtr = itr;
                    LMDB_DEBUG_LIB_DETAILED("Dbt=%s bLstRowPtr=%p\n", tname, rowPtr);
                    if (rowPtr)
                        delete rowPtr;

                }
                lmdb_busyListPtr.clear();
            }
#endif //LMDB_DATATABLE_DEBUG
        } //unlock
    }
}


template<class T>
const TblName LMDBDataTable<T>::getTableName() {
    return tableName;
}

template<class T>
typename LMDBDataTable<T>::vecT LMDBDataTable<T>::getRow(DRSize reqCount) {
    vecT node;
    const char *tname = tableName.c_str();
    LMDB_DEBUG_LIB_DETAILED("Dbt=%s reqCnt=%zu\n", tname, reqCount);
    if (reqCount <= 0) {
        return node;
    }

    if (reqCount > RESTRICT_MAX_ALLOCATE_DR_ROW_REQCOUNT) {
        reqCount = RESTRICT_MAX_ALLOCATE_DR_ROW_REQCOUNT;
        QLOGW("Dbt=%s Restrict reqCnt to %zu\n", tname, reqCount);
    }

    for(DRSize i = 0; i < reqCount; i++) {
        ptrType rowPtr = new valType();
        if (rowPtr) {
            LMDB_DEBUG_LIB_DETAILED("Dbt=%s rowPtr=%p, szOfRow=%zu szOfT=%zu\n", tname, rowPtr, sizeof(valType), sizeof(T));
            node.emplace_back(rowPtr);

#ifdef LMDB_DATATABLE_DEBUG
            if (busyListDebug) {
                {
                    std::lock_guard<Mtx> lock(lmdb_mtx);
                    lmdb_busyListPtr.emplace_back(rowPtr);
                } //unlock
            }
#endif //LMDB_DATATABLE_DEBUG
        } else {
            QLOGE("ERROR:Dbt=%s new operator failure, allocated=%zu\n", tname, i);
            return node;
        }
    }
    return node;
}


template<class T>
int LMDBDataTable<T>::freeRow(vecT& node) {
    const char *tname = tableName.c_str();
    int nodeSize = node.size();
    LMDB_DEBUG_LIB_DETAILED("Dbt=%s reqCnt=%d\n", tname, nodeSize);
    if (!nodeSize)
        return 0;

    for(int i = 0; i < nodeSize; i++) {
        ptrType rowPtr = node[i];
        LMDB_DEBUG_LIB_DETAILED("Dbt=%s itr=%d rowPtr=%p\n", tname, i, rowPtr);
        if (rowPtr)
            delete rowPtr;

#ifdef LMDB_DATATABLE_DEBUG
        if (busyListDebug && rowPtr) {
            {
                std::lock_guard<Mtx> lock(lmdb_mtx);
                lmdb_busyListPtr.remove(rowPtr);
            } //unlock
        }
#endif //LMDB_DATATABLE_DEBUG
    }
    node.clear();
    return nodeSize;
}



template<class T>
int LMDBDataTable<T>::cacheRow(vecT& node) {
    const char *tname = tableName.c_str();
    int nodeSize = node.size();
    LMDB_DEBUG_LIB_DETAILED("entering fn:Dbt=%s reqCnt=%d\n", tname, nodeSize);
    if (!nodeSize)
        return 0;

    for(int i = 0; i < nodeSize; i++) {
        ptrType rowPtr = node[i];
        LMDB_DEBUG_LIB_DETAILED("Dbt=%s itr=%d rowPtr=%p\n", tname, i, rowPtr);
        if (rowPtr) {
            {
                std::lock_guard<Mtx> lock(lmdb_mtx);
                lmdb_cachedListPtr.emplace_back(rowPtr);
#ifdef LMDB_DATATABLE_DEBUG
                if (busyListDebug) {
                    lmdb_busyListPtr.remove(rowPtr);
                }
#endif //LMDB_DATATABLE_DEBUG
            } //unlock
        }
    }
    node.clear();
    return nodeSize;
}


#endif /* LMDB_DATATABLE_H_ */
