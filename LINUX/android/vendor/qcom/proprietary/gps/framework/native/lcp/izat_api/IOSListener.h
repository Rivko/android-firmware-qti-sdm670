#ifndef __IZAT_MANAGER_IOSLISTENER_H__
#define __IZAT_MANAGER_IOSLISTENER_H__

#include "IzatTypes.h"
#include "IIzatListener.h"

namespace izat_manager {

/**
 * @brief IOSListener interface
 * @details IOSListener interface;
 *          A OS dependent entity designated as the listener of locations
 *         meta-data and related updates from Izat subsystem
 */
class IOSListener
    : public IIzatListener {

public:

    /**
     * @brief Gets bit masks of streams that this listener listens to
     * @details Gets bit masks of streams that this
     *          listener listens to
     * @return listener bit mask
     */
    virtual IzatListenerMask listensTo () const = 0;

    /**
     * @brief Destructor
     * @details Destructor
     */
    virtual ~IOSListener () {}
};

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_IOSLISTENER_H__
