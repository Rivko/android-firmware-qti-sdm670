/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <iostream>

#include <framework/Resource.h>

std::ostream &operator <<(std::ostream &os, Resource<const char *, sametype> &r)
{
    const char * vp = r.get_value();
    if (vp)
    {
        os << vp;
    }
    else
    {
        os << "<null>";
    }
    return os;
}

template <typename T, template <class> class Rt>
std::ostream &operator <<(std::ostream &os, Resource<T, Rt> &r)
{
    Rt<T> vp = r.get_value();
    if (vp)
    {
        os << *vp;
    }
    else
    {
        os << "<null>";
    }
    return os;
}

template <typename Key, typename T, template <typename> class Rt, template <typename, typename, template <typename> class> class Rl>
std::ostream &operator<<( std::ostream &os, Rl<Key,T, Rt> &rl)
{
    os << "Type: " << rl.get_name() << "{" << std::endl;
    std::for_each(rl.begin(),
            rl.end(),
            [&os] (const typename Rl<Key, T, Rt>::vec_item_type &a) {
                if (a.second) {
                    os << std::string("\t") << a.first << ": " << *a.second << std::endl;
                }
            }
            );
    os << "}";
    return os;
}
