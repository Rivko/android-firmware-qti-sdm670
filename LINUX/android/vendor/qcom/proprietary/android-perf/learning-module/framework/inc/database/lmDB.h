/******************************************************************************
  @file    lmDB.h
  @brief   Implementation of Learning Module Database

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
 ******************************************************************************/

#ifndef LMDB_H_
#define LMDB_H_


#include "internal/lmDBMacros.h"
#include "lmDBStruct.h"
#include "internal/lmDBDataTable.h"

/*************** TxtCol *******************/
enum lmdbTxtCol {
    TXTCOL_UNUSED,
    TXTCOL_APPNAME,
    TXTCOL_RSVNAME
};
static const char* lmdbTxtColNm [] = {
        LMDB_NULLSTRING,
        LMDB_STRINGIFY(APPNAME),
        LMDB_STRINGIFY(RSVNAME)
};
/******************************************/

/************** OrderByCol ****************/
enum lmdbOrderByCol {
    ORDERBYCOL_UNUSED,
    ORDERBYCOL_APPNAME,
    ORDERBYCOL_RSVNAME,
    ORDERBYCOL_INTCOL1
};
static const char* lmdbOrderByColNm [] = {
        LMDB_NULLSTRING,
        LMDB_STRINGIFY(APPNAME),
        LMDB_STRINGIFY(RSVNAME),
        LMDB_STRINGIFY(INTCOL1)
};
/******************************************/

/************** OrderByDir ****************/
enum lmdbOrderByDir {
    ORDERBYDIR_UNUSED,
    ORDERBYDIR_ASCENDING,
    ORDERBYDIR_DESCENDING
};
static const char* lmdbOrderByDirNm [] = {
        LMDB_NULLSTRING,
        LMDB_STRINGIFY(ASC),
        LMDB_STRINGIFY(DESC)
};
/******************************************/

enum lmdbError {
    LMDB_SQL_ERROR=-1,
    LMDB_NO_ERROR=0,
    LMDB_FILE_ERROR,
    LMDB_CONFIG_ERROR
};

class LMDBGlobal {
    LMDBGlobal();                                   //Not to be implemented
    LMDBGlobal(const LMDBGlobal& copy);             //Not to be implemented
    LMDBGlobal& operator=(const LMDBGlobal& copy);  //Not to be implemented
public:
    static int LMDBGlobalConfig();
    static int LMDBGlobalDeConfig();
};

template <class... T>
class LMDatabase : LMDBDataTable<T>... {
protected:
    using vecNm = typename std::vector<Name>;
public:
    template <class T1>
    using valType = typename LMDBDataTable<T1>::valType;
    template <class T1>
    using ptrType = typename LMDBDataTable<T1>::ptrType;
    template <class T1>
    using vecT = typename LMDBDataTable<T1>::vecT;
    template <class T1>
    using lstT = typename LMDBDataTable<T1>::lstT;


    template <class... N>
    LMDatabase(DBFlag, const Name, N&&...);
    ~LMDatabase();
    lmdbError LMDatabaseInit(); /*Note: should be called just after calling constructor*/
    template <class T1>
    vecT<T1> allocateDR(DRSize);
    template <class T1>
    int deallocateDR(vecT<T1>&);
    template <class T1>
    int getCountDR(LMDBDataRowKey&);
    template <class T1>
    vecT<T1> readAllocateDR(LMDBDataRowKey&, DRSize, lmdbOrderByCol=ORDERBYCOL_UNUSED, lmdbOrderByDir=ORDERBYDIR_UNUSED);
    template <class T1>
    int writeDeallocateDR(vecT<T1>&);
    template <class T1>
    int deleteDeallocateDR(vecT<T1>&);
    template <class T1>
    int deleteAllDR(LMDBDataRowKey&);
    template <class T1>
    vecNm getTxtColUniqList(lmdbTxtCol, lmdbOrderByCol=ORDERBYCOL_UNUSED, lmdbOrderByDir=ORDERBYDIR_UNUSED);
private:
    const DBFlag lmdb_flags;        //lmdb flags
    LMDB_SQL_DB_HDLR *lmdb_conn;    //lmdb connection hdlr
    const Name lmdb_path;           //lmdb path
    const Name lmdb_fname;          //lmdb filename

    lmdbError openLMdb();
    lmdbError closeLMdb();
    lmdbError LMDatabaseWriteCachedToDisk();
    template <class T1>
    lmdbError createLMdbTables();
    template <class T1>
    lmdbError writeCachedDRToDisk();
    void build_query_for_where_clause(LMDBDataRowKey&, Name&);
    size_t LMDB_memscpy(void*, size_t, const void*, size_t);
};


template <class... T>
template <class... N>
LMDatabase<T...>::LMDatabase(DBFlag flags, const Name db_name, N&&... name) : \
LMDBDataTable<T>(std::forward<N>(name))..., \
lmdb_flags(flags), \
lmdb_conn(NULL), \
lmdb_path(LMDB_ROOT_PATH), \
lmdb_fname(db_name) {
    LMDB_DEBUG_LIB("entering Ctor:Db=%s\n", lmdb_fname.c_str());
}

template <class... T>
LMDatabase<T...>::~LMDatabase() {
    LMDB_DEBUG_LIB("entering Dtor:Db=%s\n", lmdb_fname.c_str());
    LMDatabaseWriteCachedToDisk(); //flush cached data
    closeLMdb();
}

template <class... T>
lmdbError LMDatabase<T...>::LMDatabaseInit() {
    LMDB_DEBUG_LIB("entering fn:Db=%s\n", lmdb_fname.c_str());
    return openLMdb();
}

template <class... T>
lmdbError LMDatabase<T...>::openLMdb() {
    int sqlErr = LMDB_SQL_OK;
    Name lmdbFile(LMDB_INMEMORY_FILENAME);
    int sqlFlg = (LMDB_SQL_OPEN_RW | LMDB_SQL_OPEN_CR);
    if (!(lmdb_fname.empty())) {
        lmdbFile = lmdb_path + lmdb_fname;
    }
    const char* dbname = lmdbFile.c_str();
    if (LMDB_SQL_OK != (sqlErr = LMDB_SQL_OPEN_FN(dbname, &lmdb_conn, sqlFlg, NULL))) {
        QLOGE("ERROR:Db=%s sql open failed, err=%d\n", dbname, sqlErr);
        closeLMdb();
        return LMDB_SQL_ERROR;
    }
#ifdef LMDB_ENABLE_WAL_MODE
    /* WAL mode not needed for memory database */
    if (!(lmdb_fname.empty())) {
        LMDB_SQL_EXEC_STMT_CHK_OK(dbname, sqlErr, lmdb_conn, LMDB_SQL_PRAGMA_JOURNAL_MODE, NULL, NULL, NULL);
    }
#endif //LMDB_ENABLE_WAL_MODE
    sqlErr = 0;
    //((sqlErr |= createLMdbTables<T>()),...);
    int dummy[sizeof...(T)]={((sqlErr |= createLMdbTables<T>()),0)...};
    if (sqlErr)
        return LMDB_SQL_ERROR;

    return LMDB_NO_ERROR;
}


template <class... T>
template <class T1>
lmdbError LMDatabase<T...>::createLMdbTables() {
    using LMDBTbl = LMDBDataTable<T1>;
    int sqlErr;
    TblName tblNm = LMDBTbl::getTableName();
    if (lmdb_conn) {
        Name s = Q_C_TBL_HDR + tblNm;
        s += LMDB_STRINGIFY(Q_C_TBL_COLUMNS);
        LMDB_SQL_EXEC_STMT_CHK_OK_OR_RTN_ERR(tblNm.c_str(), sqlErr, lmdb_conn, s.c_str(), NULL, NULL, NULL);
    }
    return LMDB_NO_ERROR;
}


template <class... T>
lmdbError LMDatabase<T...>::closeLMdb() {
    int sqlErr;
    if (lmdb_conn) {
        Name lmdbFile(LMDB_INMEMORY_FILENAME);
        if (!(lmdb_fname.empty())) {
            lmdbFile = lmdb_path + lmdb_fname;
        }
        if (LMDB_SQL_OK != (sqlErr=LMDB_SQL_CLOSE_FN(lmdb_conn))) {
            QLOGE("ERROR:Db=%s sql close failed, err=%d\n", lmdbFile.c_str(), sqlErr);
            return LMDB_SQL_ERROR;
        }
    }
    return LMDB_NO_ERROR;
}


template <class... T>
lmdbError LMDatabase<T...>::LMDatabaseWriteCachedToDisk() {
    LMDB_DEBUG_LIB("entering fn:Db=%s\n", lmdb_fname.c_str());
    int sqlErr = 0;
    //((sqlErr |= writeCachedDRToDisk<T>()),...);
    int dummy[sizeof...(T)]={((sqlErr |= writeCachedDRToDisk<T>()),0)...};
    if (sqlErr)
        return LMDB_SQL_ERROR;

    return LMDB_NO_ERROR;
}


#ifdef LMDB_DEBUG_SQL_SHADOW_WR
#undef LMDB_DEBUG_SQL_DEBUG
#define LMDB_DEBUG_SQL_DEBUG(x, ...) LMDB_DEBUG_SQL_NODEBUG(x, __VA_ARGS__)
#else
#endif //LMDB_DEBUG_SQL_SHADOW_WR
template <class... T>
template <class T1>
lmdbError LMDatabase<T...>::writeCachedDRToDisk() {
    using LMDBTbl = LMDBDataTable<T1>;
    if (lmdb_conn) {
        int sqlErr,sqlErr1;
        TblName tblNm = LMDBTbl::getTableName();
        const char* tname = tblNm.c_str();
        LMDB_SQL_STMT *stmt_hdl, *stmt_hdl_insert = NULL, *stmt_hdl_update = NULL;

        Name s = Q_I_VAL_HDR + tblNm;
        s += LMDB_STRINGIFY(Q_I_VAL_FLD);
        LMDB_SQL_PREPARE_STMT_CHK_OK_RTN_ERR(tname, sqlErr, lmdb_conn, s.c_str(), s.length(), &stmt_hdl_insert, NULL);

        s = Q_U_VAL_HDR + tblNm + LMDB_SPACE;
        s += LMDB_STRINGIFY(Q_U_VAL_FLD);
        LMDB_SQL_PREPARE_STMT_CHK_OK_RTN_ERR(tname, sqlErr, lmdb_conn, s.c_str(), s.length(), &stmt_hdl_update, NULL);

        LMDB_SQL_EXEC_TXN_CHK_OK(tname, sqlErr, lmdb_conn, LMDB_SQL_BEGIN_IMM_TXN_STMT, NULL,NULL,NULL);

        vecT<T1> v;
        bool iterate = false;
        do {
            {
                std::lock_guard<Mtx> lock(LMDBTbl::lmdb_mtx);
                if ((iterate = !(LMDBTbl::lmdb_cachedListPtr.empty()))) {
                    v.emplace_back(LMDBTbl::lmdb_cachedListPtr.front());
                    LMDBTbl::lmdb_cachedListPtr.pop_front();
                } else
                    break;

            } //unlock

            ptrType<T1> rowPtr = v.back();
            auto stHdrPtr = &(rowPtr->dataRowHeader);
            auto stKeyPtr = &(rowPtr->dataRowKey);
            auto stDataPtr = &(rowPtr->dataRowPayload);
            if (stHdrPtr->rID) {
                stmt_hdl = stmt_hdl_update;
            } else {
                stmt_hdl = stmt_hdl_insert;
            }

            int colBindIndex = COL1_BIND_INDEX;
            for(int i = 0; i < KEY_TYPE_INT_RSVD; i++, colBindIndex++) {
                if (stKeyPtr->keys[i] != LMDB_INTKEY_DEFVAL) {
                    LMDB_SQL_STEP_PSTMT_CHK_OK(tname, sqlErr, bind_int,  stmt_hdl, colBindIndex, (stKeyPtr->keys[i]));
                }
            }

            if (!(stKeyPtr->appNm.empty())) {
                LMDB_SQL_STEP_PSTMT_CHK_OK(tname, sqlErr, bind_text, stmt_hdl, colBindIndex, stKeyPtr->appNm.c_str(), stKeyPtr->appNm.length(), LMDB_SQL_SMC_NOFREE);
            }
            colBindIndex++;

            if (!(stKeyPtr->rsvdNm.empty())) {
                LMDB_SQL_STEP_PSTMT_CHK_OK(tname, sqlErr, bind_text, stmt_hdl, colBindIndex, stKeyPtr->rsvdNm.c_str(), stKeyPtr->rsvdNm.length(), LMDB_SQL_SMC_NOFREE);
            }
            colBindIndex++;

            LMDB_SQL_STEP_PSTMT_CHK_OK(tname, sqlErr, bind_blob, stmt_hdl, colBindIndex, stDataPtr, sizeof(T1), LMDB_SQL_SMC_NOFREE);
            colBindIndex++;

            if (stHdrPtr->rID) {
                LMDB_SQL_STEP_PSTMT_CHK_OK(tname, sqlErr, bind_int,  stmt_hdl, colBindIndex, (stHdrPtr->rID));
            }
            colBindIndex++;

            LMDB_SQL_STEP_PSTMT_CHK_DONE(tname, sqlErr, step, stmt_hdl);
            LMDB_SQL_STEP_PSTMT_CHK_OK(tname, sqlErr,  reset, stmt_hdl);
        } while (iterate);
        LMDBTbl::freeRow(v);

        LMDB_SQL_EXEC_TXN_CHK_OK(tname, sqlErr, lmdb_conn, LMDB_SQL_COMMIT_TXN_STMT, NULL,NULL,NULL);

        LMDB_SQL_STEP_PSTMT_CHK_OK(tname, sqlErr1, finalize, stmt_hdl_insert);
        LMDB_SQL_STEP_PSTMT_CHK_OK(tname, sqlErr, finalize, stmt_hdl_update);
        if ((LMDB_SQL_OK != sqlErr1) || (LMDB_SQL_OK != sqlErr)) {
            return LMDB_SQL_ERROR;
        }
    }
    return LMDB_NO_ERROR;
}
#ifdef LMDB_DEBUG_SQL_SHADOW_WR
#undef LMDB_DEBUG_SQL_DEBUG
#define LMDB_DEBUG_SQL_DEBUG(x, ...) LMDB_DEBUG_SQL(x, __VA_ARGS__)
#else
#endif //LMDB_DEBUG_SQL_SHADOW_WR



template <class... T>
template <class T1>
typename LMDatabase<T...>::template vecT<T1> LMDatabase<T...>::allocateDR(DRSize reqCount) {
    using LMDBTbl = LMDBDataTable<T1>;
    TblName tblNm = LMDBTbl::getTableName();
    LMDB_DEBUG_LIB("entering fn:Dbt=%s reqCnt=%lu\n", tblNm.c_str(), reqCount);
    return LMDBTbl::getRow(reqCount);
}

template <class... T>
template <class T1>
int LMDatabase<T...>::deallocateDR(vecT<T1>& v) {
    using LMDBTbl = LMDBDataTable<T1>;
    TblName tblNm = LMDBTbl::getTableName();
    LMDB_DEBUG_LIB("entering fn:Dbt=%s vSz=%zu\n", tblNm.c_str(),v.size());
    return LMDBTbl::freeRow(v);
}

#ifdef LMDB_DEBUG_SQL_SHADOW_RD
#undef LMDB_DEBUG_SQL_DEBUG
#define LMDB_DEBUG_SQL_DEBUG(x, ...) LMDB_DEBUG_SQL_NODEBUG(x, __VA_ARGS__)
#else
#endif //LMDB_DEBUG_SQL_SHADOW_RD
template <class... T>
template <class T1>
int LMDatabase<T...>::getCountDR(LMDBDataRowKey& k) {
    using LMDBTbl = LMDBDataTable<T1>;
    TblName tblNm = LMDBTbl::getTableName();
    const char *tname = tblNm.c_str();
    LMDB_DEBUG_LIB("entering fn:Dbt=%s\n", tname);

    int Count=0;
    if (lmdb_conn) {
        int sqlErr;
        LMDB_SQL_STMT *stmt_hdl = NULL;
        Name s = Q_SC_VAL_HDR + tblNm + LMDB_SPACE;
        build_query_for_where_clause(k, s);

        LMDB_SQL_PREPARE_STMT_CHK_OK(tname, sqlErr, lmdb_conn, s.c_str(), s.length(), &stmt_hdl, NULL);
        if (LMDB_SQL_OK != sqlErr) {
            return Count;
        }

        /*while (1)*/ {
            const unsigned char *cUCPtr;
            LMDB_SQL_STEP_PSTMT_CHK_ROW(tname, sqlErr, step, stmt_hdl);
            if (LMDB_SQL_ROW == sqlErr) {
                LMDB_SQL_STEP_COLFN_NOCHECK(tname, Count, column_int, stmt_hdl, COL0_SEL_INDEX);
            } /*else { //LMDB_SQL_ROW?
                LMDB_SQL_STEP_PSTMT_CHK_OK(tname, sqlErr,  reset, stmt_hdl);
                break;
            }*/
        }
        LMDB_SQL_STEP_PSTMT_CHK_OK(tname, sqlErr, finalize, stmt_hdl);
    }
    return Count;
}


template <class... T>
template <class T1>
typename LMDatabase<T...>::template vecT<T1> LMDatabase<T...>::readAllocateDR(LMDBDataRowKey& k, DRSize reqCount, lmdbOrderByCol orderByCol, lmdbOrderByDir orderByDir) {
    using LMDBTbl = LMDBDataTable<T1>;
    TblName tblNm = LMDBTbl::getTableName();
    const char *tname = tblNm.c_str();
    LMDB_DEBUG_LIB("entering fn:Dbt=%s reqCnt=%lu\n", tname, reqCount);

    vecT<T1> node;
    if (lmdb_conn) {
        int sqlErr;
        if (reqCount > RESTRICT_MAX_READALLOC_REQCOUNT ) {
            reqCount = RESTRICT_MAX_READALLOC_REQCOUNT;
            QLOGW("Dbt=%s Restrict reqCnt to %zu\n", tname, reqCount);
        } else if (reqCount == 0) {
            reqCount = RESTRICT_MAX_READALLOC_REQCOUNT;
        }

        LMDB_SQL_STMT *stmt_hdl = NULL;
        Name s = Q_S_VAL_HDR + tblNm + LMDB_SPACE;
        build_query_for_where_clause(k, s);
        if (orderByCol != ORDERBYCOL_UNUSED) {
            Name s1 = ORDER_BY_HDR;
            s1 += lmdbOrderByColNm[orderByCol];
            s1 += LMDB_SPACE;
            s1 += lmdbOrderByDirNm[orderByDir];
            s += s1;
        }

        LMDB_SQL_PREPARE_STMT_CHK_OK(tname, sqlErr, lmdb_conn, s.c_str(), s.length(), &stmt_hdl, NULL);
        if (LMDB_SQL_OK != sqlErr) {
            return node;
        }

        while (reqCount-- > 0) {
            const void *cVPtr;
            const unsigned char *cUCPtr;
            LMDB_SQL_STEP_PSTMT_CHK_ROW(tname, sqlErr, step, stmt_hdl);
            if (LMDB_SQL_ROW == sqlErr) {
                vecT<T1> tempNode = LMDBTbl::getRow(1);
                if (!tempNode.size())
                    break;

                auto nodePtr= tempNode.back();
                node.emplace_back(nodePtr);
                tempNode.clear();

                auto stHdrPtr = &(nodePtr->dataRowHeader);
                auto stKeyPtr = &(nodePtr->dataRowKey);
                auto stDataPtr = &(nodePtr->dataRowPayload);

                int colSelectIndex = COL0_SEL_INDEX;
                for(int i = 0; i < KEY_TYPE_INT_RSVD; i++, colSelectIndex++) {
                    /*check for NULL value*/
                    LMDB_SQL_STEP_COLFN_NOCHECK(tname, cUCPtr, column_text, stmt_hdl, colSelectIndex);
                    if (cUCPtr) {
                        LMDB_SQL_STEP_COLFN_NOCHECK(tname, stKeyPtr->keys[i], column_int, stmt_hdl, colSelectIndex);
                    }
                }
                LMDB_SQL_STEP_COLFN_NOCHECK(tname, cUCPtr, column_text, stmt_hdl, colSelectIndex);
                colSelectIndex++;
                stKeyPtr->appNm = reinterpret_cast<const char*>(cUCPtr);

                LMDB_SQL_STEP_COLFN_NOCHECK(tname, cUCPtr, column_text, stmt_hdl, colSelectIndex);
                colSelectIndex++;
                if (cUCPtr != NULL) {
                    stKeyPtr->rsvdNm = reinterpret_cast<const char*>(cUCPtr);
                }

                LMDB_SQL_STEP_COLFN_NOCHECK(tname, cVPtr, column_blob, stmt_hdl, colSelectIndex);
                colSelectIndex++;
                LMDB_memscpy(stDataPtr, sizeof(*stDataPtr), cVPtr, sizeof(*stDataPtr));

                LMDB_SQL_STEP_COLFN_NOCHECK(tname, stHdrPtr->rID, column_int64, stmt_hdl, colSelectIndex);
                colSelectIndex++;
            } else { //LMDB_SQL_DONE?
                LMDB_SQL_STEP_PSTMT_CHK_OK(tname, sqlErr,  reset, stmt_hdl);
                break;
            }
        }

        LMDB_SQL_STEP_PSTMT_CHK_OK(tname, sqlErr, finalize, stmt_hdl);
    }
    return node;
}
#ifdef LMDB_DEBUG_SQL_SHADOW_RD
#undef LMDB_DEBUG_SQL_DEBUG
#define LMDB_DEBUG_SQL_DEBUG(x, ...) LMDB_DEBUG_SQL(x, __VA_ARGS__)
#else
#endif //LMDB_DEBUG_SQL_SHADOW_RD


template <class... T>
template <class T1>
int LMDatabase<T...>::writeDeallocateDR(vecT<T1>& v) {
    using LMDBTbl = LMDBDataTable<T1>;
    TblName tblNm = LMDBTbl::getTableName();
    LMDB_DEBUG_LIB("entering fn:Dbt=%s vSz=%zu\n", tblNm.c_str(),v.size());

    int rval;
    if((rval=LMDBTbl::cacheRow(v))) {
        writeCachedDRToDisk<T1>();
    }
    return rval;
}

#ifdef LMDB_DEBUG_SQL_SHADOW_DEL
#undef LMDB_DEBUG_SQL_DEBUG
#define LMDB_DEBUG_SQL_DEBUG(x, ...) LMDB_DEBUG_SQL_NODEBUG(x, __VA_ARGS__)
#else
#endif //LMDB_DEBUG_SQL_SHADOW_DEL
template <class... T>
template <class T1>
int LMDatabase<T...>::deleteDeallocateDR(vecT<T1>& v) {
    using LMDBTbl = LMDBDataTable<T1>;
    TblName tblNm = LMDBTbl::getTableName();
    const char *tname = tblNm.c_str();
    LMDB_DEBUG_LIB("entering fn:Dbt=%s\n", tname);

    int Count=0;
    if (lmdb_conn) {
        int sqlErr, vSize;
        LMDB_SQL_STMT *stmt_hdl = NULL;

        Name s = Q_DR_VAL_HDR + tblNm + LMDB_SPACE + LMDB_STRINGIFY(Q_DR_VAL_CLAUSE);

        LMDB_SQL_PREPARE_STMT_CHK_OK(tname, sqlErr, lmdb_conn, s.c_str(), s.length(), &stmt_hdl, NULL);
        if (LMDB_SQL_OK != sqlErr) {
            return Count;
        }

        LMDB_SQL_EXEC_TXN_CHK_OK(tname, sqlErr, lmdb_conn, LMDB_SQL_BEGIN_IMM_TXN_STMT, NULL,NULL,NULL);
        vSize = v.size();
        for(int i = 0; i < vSize; i++)
        {
            auto stHdrPtr = &(v[i]->dataRowHeader);
            if (stHdrPtr->rID) {
                LMDB_SQL_STEP_PSTMT_CHK_OK(tname, sqlErr, bind_int,  stmt_hdl, COL1_BIND_INDEX, (stHdrPtr->rID));
                LMDB_SQL_STEP_PSTMT_CHK_DONE(tname, sqlErr, step, stmt_hdl);
                LMDB_SQL_STEP_PSTMT_CHK_OK(tname, sqlErr,  reset, stmt_hdl);
            }
        }
        LMDB_SQL_EXEC_TXN_CHK_OK(tname, sqlErr, lmdb_conn, LMDB_SQL_COMMIT_TXN_STMT, NULL,NULL,NULL);

        LMDB_SQL_STEP_PSTMT_CHK_OK(tname, sqlErr, finalize, stmt_hdl);
        if (LMDB_SQL_OK != sqlErr) {
            return 0;
        }
    }
    return LMDBTbl::freeRow(v);
}


template <class... T>
template <class T1>
int LMDatabase<T...>::deleteAllDR(LMDBDataRowKey& k) {
    using LMDBTbl = LMDBDataTable<T1>;
    TblName tblNm = LMDBTbl::getTableName();
    const char *tname = tblNm.c_str();
    LMDB_DEBUG_LIB("entering fn:Dbt=%s\n", tname);
    int Count=0;
    if (lmdb_conn) {
        int sqlErr;
        LMDB_SQL_STMT *stmt_hdl = NULL;

        Name s = Q_D_VAL_HDR + tblNm + LMDB_SPACE;
        build_query_for_where_clause(k, s);

        LMDB_SQL_PREPARE_STMT_CHK_OK(tname, sqlErr, lmdb_conn, s.c_str(), s.length(), &stmt_hdl, NULL);
        if (LMDB_SQL_OK != sqlErr) {
            return Count;
        }

        LMDB_SQL_STEP_PSTMT_CHK_DONE(tname, sqlErr, step, stmt_hdl);
        if (LMDB_SQL_DONE == sqlErr) {
            Count = LMDB_SQL_ROW_MOD_FN(lmdb_conn);
        }
        LMDB_SQL_STEP_PSTMT_CHK_OK(tname, sqlErr, finalize, stmt_hdl);
    }
    return Count;
}
#ifdef LMDB_DEBUG_SQL_SHADOW_DEL
#undef LMDB_DEBUG_SQL_DEBUG
#define LMDB_DEBUG_SQL_DEBUG(x, ...) LMDB_DEBUG_SQL(x, __VA_ARGS__)
#else
#endif //LMDB_DEBUG_SQL_SHADOW_DEL



#ifdef LMDB_DEBUG_SQL_SHADOW_COL_LIST
#undef LMDB_DEBUG_SQL_DEBUG
#define LMDB_DEBUG_SQL_DEBUG(x, ...) LMDB_DEBUG_SQL_NODEBUG(x, __VA_ARGS__)
#else
#endif //LMDB_DEBUG_SQL_SHADOW_COL_LIST
template <class... T>
template <class T1>
typename LMDatabase<T...>::vecNm LMDatabase<T...>::getTxtColUniqList(lmdbTxtCol col, lmdbOrderByCol orderByCol, lmdbOrderByDir orderByDir) {
    using LMDBTbl = LMDBDataTable<T1>;
    TblName tblNm = LMDBTbl::getTableName();
    const char *tname = tblNm.c_str();
    Name colNm = lmdbTxtColNm[col];
    LMDB_DEBUG_LIB("entering fn:Dbt=%s colNm=%s\n", tname, colNm.c_str());
    using LMDBTbl = LMDBDataTable<T1>;

    vecNm node;
    if (lmdb_conn) {
        int sqlErr;
        DRSize Count=0;
        LMDB_SQL_STMT *stmt_hdl = NULL;
        Name s = Q_SC_DA_VAL_HDR + colNm + ") FROM " + tblNm;

        LMDB_SQL_PREPARE_STMT_CHK_OK(tname, sqlErr, lmdb_conn, s.c_str(), s.length(), &stmt_hdl, NULL);
        if (LMDB_SQL_OK != sqlErr) {
            return node;
        }

        /*while (1)*/ {
            const unsigned char *cUCPtr;
            LMDB_SQL_STEP_PSTMT_CHK_ROW(tname, sqlErr, step, stmt_hdl);
            if (LMDB_SQL_ROW == sqlErr) {
                LMDB_SQL_STEP_COLFN_NOCHECK(tname, Count, column_int, stmt_hdl, COL0_SEL_INDEX);
            } /*else { //LMDB_SQL_DONE?
                LMDB_SQL_STEP_PSTMT_CHK_OK(tname, sqlErr,  reset, stmt_hdl);
                break;
            }*/
        }

        LMDB_SQL_STEP_PSTMT_CHK_OK(tname, sqlErr, finalize, stmt_hdl);
        if (Count == 0) {
            return node;
        } else if (Count > RESTRICT_MAX_TXTCOL_UNIQUELIST_REQCOUNT ) {
            Count = RESTRICT_MAX_TXTCOL_UNIQUELIST_REQCOUNT;
            QLOGW("Dbt=%s Restrict Cnt to %zu\n", tname, Count);
        }


        s = Q_S_DA_VAL_HDR + colNm + " FROM " + tblNm + LMDB_SPACE;
        if (orderByCol != ORDERBYCOL_UNUSED) {
            Name s1 = ORDER_BY_HDR;
            s1 += lmdbOrderByColNm[orderByCol];
            s1 += LMDB_SPACE;
            s1 += lmdbOrderByDirNm[orderByDir];
            s += s1;
        }

        LMDB_SQL_PREPARE_STMT_CHK_OK(tname, sqlErr, lmdb_conn, s.c_str(), s.length(), &stmt_hdl, NULL);
        if (LMDB_SQL_OK != sqlErr) {
            return node;
        }

        node.reserve(Count);

        for(DRSize i = 0; i < Count; i++) {
            const unsigned char *cUCPtr;
            LMDB_SQL_STEP_PSTMT_CHK_ROW(tname, sqlErr, step, stmt_hdl);
            if (LMDB_SQL_ROW == sqlErr) {
                //node.emplace_back(NULL);
                LMDB_SQL_STEP_COLFN_NOCHECK(tname, cUCPtr, column_text, stmt_hdl, COL0_SEL_INDEX);
                node.emplace_back(reinterpret_cast<const char*>(cUCPtr));
            } else { //LMDB_SQL_DONE?
                LMDB_SQL_STEP_PSTMT_CHK_OK(tname, sqlErr,  reset, stmt_hdl);
                break;
            }
        }

        LMDB_SQL_STEP_PSTMT_CHK_OK(tname, sqlErr, finalize, stmt_hdl);
    }
    return node;
}
#ifdef LMDB_DEBUG_SQL_SHADOW_COL_LIST
#undef LMDB_DEBUG_SQL_DEBUG
#define LMDB_DEBUG_SQL_DEBUG(x, ...) LMDB_DEBUG_SQL(x, __VA_ARGS__)
#else
#endif //LMDB_DEBUG_SQL_SHADOW_COL_LIST


template <class... T>
inline size_t LMDatabase<T...>::LMDB_memscpy(void *dstPtr, size_t dstSz, const void *srcPtr, size_t srcSz) {
    size_t minSz = (dstSz < srcSz) ? dstSz : srcSz;
    memcpy(dstPtr, srcPtr, minSz);
    return minSz;
}

template <class... T>
void LMDatabase<T...>::build_query_for_where_clause(LMDBDataRowKey& k, Name& s) {
    bool appNmReq = !(k.appNm.empty()), rsvdNmReq = !(k.rsvdNm.empty()), whrClsReq = false;
    Name separator, qPrefix = "WHERE (";

    if (appNmReq) {
        if (!whrClsReq) {
            s += qPrefix; whrClsReq = true;
        }
        s += LMDB_STRINGIFY(APPNAME);
        separator = LMDB_COMMA;
    }

    if (rsvdNmReq) {
        if (!whrClsReq) {
            s += qPrefix; whrClsReq = true;
        }
        s += separator;
        s += LMDB_STRINGIFY(RSVNAME);
        separator = LMDB_COMMA;
    }

    for(int i = 0; i < KEY_TYPE_INT_RSVD; i++) {
        if (!(k.keys[i] == LMDB_INTKEY_DEFVAL)) {
            if (!whrClsReq) {
                s += qPrefix; whrClsReq = true;
            }
            s += separator;
            s += LMDB_STRINGIFY(INTCOL1);
            separator = LMDB_COMMA;
        }
    }

    if (whrClsReq) {
        s += ") = (";
    }
    separator = LMDB_NULLSTRING;

    if (appNmReq) {
        s += "'" + k.appNm + "'";
        separator = LMDB_COMMA;
    }

    if (rsvdNmReq) {
        s += separator;
        s += "'" + k.rsvdNm + "'";
        separator = LMDB_COMMA;
    }

    for(int i = 0; i < KEY_TYPE_INT_RSVD; i++) {
        if (!(k.keys[i] == LMDB_INTKEY_DEFVAL)) {
            s += separator;
            s += std::to_string((lmdbIntKey) k.keys[i]);
            separator = LMDB_COMMA;
        }
    }

    if (whrClsReq) {
        s += ") ";
    }
}

#endif /* LMDB_H_ */
