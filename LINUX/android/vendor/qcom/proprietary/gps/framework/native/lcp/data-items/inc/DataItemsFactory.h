/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  DataItemsFactory

  Copyright  (c) 2015-2018 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#ifndef __IZAT_MANAGER_DATAITEMSFACTORY_H__
#define __IZAT_MANAGER_DATAITEMSFACTORY_H__

#include <DataItemId.h>
#include <IDataItemCore.h>
#include <IDataItemSerializer.h>

namespace izat_manager
{
using loc_core::IDataItemCore;

// DataItemsFactory Factory
/**
 * @brief DataItemsFactory class
 * @details DataItemsFactory class
 *
 */
class DataItemsFactory {

private:
    /**
     * @brief Template function to create/clone a Data item
     * @details Template function to create/clone a Data item and return the
     * object of template type requested which is a base object (dataitem or
     * serializer) of the concrete data item.
     *
     * @param diId Data item id to be created.
     * @param di   Data item to be cloned. This will be passed in case we need
     * to clone the data item based on this, else it will be NULL.
     * @return Pointer to a heap instance of the template type.
     */
    template<typename T>
    static T* createDataItem(DataItemId diId, IDataItemCore * di);

public:
    /**
     * @brief Create new data item (on heap)
     * @details Create new data item (on heap)
     *
     * @param id Data item id to be created
     * @return Pointer to a heap instance of IDataItemCore.
     */
    static IDataItemCore* createNewDataItem(DataItemId id);

    /**
     * @brief Clone a new data item (on heap)
     * @details Clone a new data item (on heap) and returns the
     * serializer object interface of the data item.
     *
     * @param di Data item to be cloned
     * @return Pointer to a heap instance of IDataItemSerialization.
     */
    static IDataItemSerialization * cloneDataItem(IDataItemCore * di);
};

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_DATAITEMSFACTORY_H__
