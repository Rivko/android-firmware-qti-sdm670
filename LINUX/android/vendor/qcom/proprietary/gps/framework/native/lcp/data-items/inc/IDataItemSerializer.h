/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  IDataItemSerialization

Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef __IZAT_MANAGER_IDATAITEMSERIALIZATION_H__
#define __IZAT_MANAGER_IDATAITEMSERIALIZATION_H__

#include <comdef.h>
#include <string>

namespace qc_loc_fw {
    class OutPostcard;
    class InPostcard;
}

namespace izat_manager {

using namespace ::qc_loc_fw;
using namespace std;

/**
 * @brief IDataItemSerialization interface.
 * @details IDataItemSerialization interface.
 *
 * @param out Pointer to an instance of OutPostcard.
 * @return Zero for success or non zero for failure.
 */
class IDataItemSerialization {
public:
    /**
     * @brief Serialize.
     * @details Serialize.
     *
     * @param out Pointer to an instance of OutPostcard.
     * @return Zero for success or non zero for failure.
     */
    virtual int32 serialize (OutPostcard * out) = 0;
    /**
     * @brief Deserialize.
     * @details Deserialize.
     *
     * @param in Pointer to an instance of InPostcard.
     * @return Zero for success or non zero for failure.
     */
    virtual int32 deSerialize (InPostcard * in) = 0;

    virtual const string & getCardName () const = 0;

    virtual bool hasCardName () const = 0;

    /**
     * @brief Destructor.
     * @details Destructor.
     */
    virtual ~IDataItemSerialization () {}
};

} //namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_IDATAITEMSERIALIZATION_H__