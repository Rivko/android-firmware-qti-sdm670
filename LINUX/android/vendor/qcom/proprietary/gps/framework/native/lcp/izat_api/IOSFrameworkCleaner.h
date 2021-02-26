/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  IOSFrameworkCleaner

  Copyright  (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#ifndef __IZAT_MANAGER_IOSFRAMEWORKCLEANER_H__
#define __IZAT_MANAGER_IOSFRAMEWORKCLEANER_H__

namespace izat_manager {

/**
 * @brief IOSFrameworkCleaner interface
 * @details IOSFrameworkCleaner interface;
 *          IOSFrameworkCleaner provides an interface to clean-up
 *          OS dependent classes for memory etc. Acts as placeholder for adding
 *          clean-up methods for OS dependent code.
 */
class IOSFrameworkCleaner {

public:

    /**
     * @brief Clean up resources allocated for OSNP
     * @details Clean up resources allocated for OSNP
     */
    virtual void cleanOSNP () = 0;

    /**
     * @brief Destructor
     * @details Destructor
     */
    virtual ~IOSFrameworkCleaner () {}
};

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_IOSFRAMEWORKCLEANER_H__
