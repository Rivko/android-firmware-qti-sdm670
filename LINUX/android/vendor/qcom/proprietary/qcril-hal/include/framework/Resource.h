/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef FRAMEWORK_RESOURCE_H_INCLUDED
#define FRAMEWORK_RESOURCE_H_INCLUDED

#include <memory>
#include <utility>
#include <vector>
#include <algorithm>
#include <mutex>
#include <shared_mutex>
#include <map>

template <typename Key,
        typename T,
        template <typename> class Rt = std::shared_ptr,
        template <typename ...> class Container = std::vector>
class ResourceList;

template <typename T, template <class> class Rt>
class Resource
{
    private:
        Rt<T> ref;
        size_t idx;

    public:
        Resource(Rt<T> ref, size_t idx):
            ref(std::move(ref)),
            idx(idx)
        {
            std::cout << "Resource" << ref << std::endl;
        }
        Rt<T> get_value() {
            return ref;
        }
        void set_index(size_t idx)
        {
            this->idx = idx;
        }
        size_t get_index()
        {
            return this->idx;
        }
};

template <typename T>
using ptrtype = T*;
template <typename T>
using sametype = T;

// std::ostream &operator <<(std::ostream &os, Resource<const char *, sametype> &r)
// {
//     const char * vp = r.get_value();
//     if (vp)
//     {
//         os << vp;
//     }
//     else
//     {
//         os << "<null>";
//     }
//     return os;
// }
// 
// template <typename T, template <class> class Rt>
// std::ostream &operator <<(std::ostream &os, Resource<T, Rt> &r)
// {
//     Rt<T> vp = r.get_value();
//     if (vp)
//     {
//         os << *vp;
//     }
//     else
//     {
//         os << "<null>";
//     }
//     return os;
// }

template
<typename Key,
    typename T,
    template <typename> class Rt,
    template <typename ...> class Container>
class ResourceList
{
    public:
        using resref        = Resource<T, Rt>;
        using resrefp       = std::shared_ptr<resref>;
        using cont_item_type = std::pair<Key, resrefp>;
        using cont_type   = Container<cont_item_type>;
        using iterator      = typename cont_type::iterator;

        mutable std::shared_timed_mutex the_mutex;
    private:
        const std::string rs_type_name;
        cont_type the_resources;
    public:
        ResourceList(const std::string name):
            rs_type_name(name)
        {
        }
        const std::string &get_name() { return rs_type_name; }

        iterator begin()
        {
            std::shared_lock<std::shared_timed_mutex> lock(the_mutex);
            return the_resources.begin();
        }
        iterator end()
        {
            std::shared_lock<std::shared_timed_mutex> lock(the_mutex);
            return the_resources.end();
        }

        resrefp operator[] (size_t idx)
        {
            return the_resources[idx].second;
        }

        const resrefp findOrAddResource(const Key k, Rt<T> r)
        {
            std::unique_lock<std::shared_timed_mutex> lock(the_mutex);
            auto it = find_if(the_resources.begin(),
                    the_resources.end(),
                    [k] (std::pair<Key, resrefp> pair) {
                        return pair.first == k;
                    }
                    );
            if (it == the_resources.end()) {
                resrefp ref = std::make_shared<resref>(r, it - the_resources.begin());
                it = the_resources.emplace(it, k, ref);
                return it->second;
            } else {
                return it->second;
            }
        }
};

template
<typename Key,
    typename T,
    template <typename> class Rt>
class ResourceList<Key, T, Rt, std::map>
{
    public:
        using resref        = Resource<T, Rt>;
        using resrefp       = std::shared_ptr<resref>;
        using cont_item_type = std::pair<Key, resrefp>;
        using cont_type   = std::map<Key, resrefp>;
        using iterator      = typename cont_type::iterator;

        mutable std::shared_timed_mutex the_mutex;
    private:
        const std::string rs_type_name;
        cont_type the_resources;
    public:
        ResourceList(const std::string name):
            rs_type_name(name)
        {
        }
        const std::string &get_name() { return rs_type_name; }

        iterator begin()
        {
            std::shared_lock<std::shared_timed_mutex> lock(the_mutex);
            return the_resources.begin();
        }
        iterator end()
        {
            std::shared_lock<std::shared_timed_mutex> lock(the_mutex);
            return the_resources.end();
        }


        resrefp operator[] (Key key)
        {
            return the_resources[key];
        }

        const resrefp findOrAddResource(const Key k, Rt<T> r)
        {
            std::unique_lock<std::shared_timed_mutex> lock(the_mutex);
            auto res = the_resources[k];
            typename cont_type::iterator it;
            if (!res)
            {
                res = new resrefp(new resref(r));
                it = the_resources.emplace(k, r);
            }
            return res;
        }
};

// template <typename Key, typename T, template <typename> class Rt, template <typename, typename, template <typename> class> class Rl>
// std::ostream &operator<<( std::ostream &os, Rl<Key,T, Rt> &rl)
// {
//     os << "Type: " << rl.get_name() << "{" << std::endl;
//     std::for_each(rl.begin(),
//             rl.end(),
//             [&os] (const typename Rl<Key, T, Rt>::cont_item_type &a) {
//                 if (a.second) {
//                     os << std::string("\t") << a.first << ": " << *a.second << std::endl;
//                 }
//             }
//             );
//     os << "}";
//     return os;
// }

#endif
