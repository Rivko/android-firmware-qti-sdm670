/******************************************************************************
 *   @file    ReferenceAction.h
 *   @brief   Reference Derived Action Class
 *
 *   DESCRIPTION
 *      Reference action class containing only an output Database container.
 *
 *---------------------------------------------------------------------------
 *    Copyright (c) 2017 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *---------------------------------------------------------------------------
 *******************************************************************************/
#ifndef REFERENCEACTION_H
#define REFERENCEACTION_H

#include "Action.h"
#include "lmDB.h"

/* Output Database Row structure */
struct ReferenceOutput
{
    double timeBetweenLaunches;
};

/* Output Database class */
class ReferenceOutputDb : LMDatabase<ReferenceOutput> {
public:
    static ReferenceOutputDb* getInstance();

private:
    //ctor, copy ctor, assignment overloading
    ReferenceOutputDb(unsigned long int flags, const std::string dbName,
            const std::string tableName) :
        LMDatabase<ReferenceOutput>(flags, dbName, tableName) {
    }
    ReferenceOutputDb(ReferenceOutputDb const& copy);               //Not to be implemented
    ReferenceOutputDb& operator=(ReferenceOutputDb const& copy);    //Not to be implemented
    ~ReferenceOutputDb() {
        delete mDbInstance;
        mDbInstance = nullptr;
    }

    static ReferenceOutputDb* mDbInstance;
};

/* Actual Action Class */
class ReferenceAction : public Action {
private:
    ReferenceAction() :
        mReferenceActionOutputDb(NULL) {
        /* Get instance of singleton DB and keep pointer */
        mReferenceActionOutputDb = ReferenceOutputDb::getInstance();
    }

    ReferenceAction(ReferenceAction const&);
    ReferenceAction& operator=(ReferenceAction const&);
    ~ReferenceAction() {}

    ReferenceOutputDb* mReferenceActionOutputDb;
public:
    void* getActionOutputDb() {
        return mReferenceActionOutputDb;
    }

    static ReferenceAction& getInstance() {
/*
 * Meyer's singleton implementation
 */
        static ReferenceAction refActionInstance;
        return refActionInstance;
    }
    void applyAction(const AsyncTriggerPayload& payload);
};

#endif /* REFERENCEACTION_H */
