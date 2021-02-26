/* ===================================================================
**
** Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: Template_Impl.hpp
** DESC: Generic template class to implement randomization of
**       various data types
** ================================================================ */

#include <random>
#include <vector>

#ifndef TEMPLATE_IMPL_HPP
#define TEMPLATE_IMPL_HPP

namespace sensors_stability {

    /*
     * Return randomized sampling rate or stream duration between min and max
     *
     * @param min as const T reference
     * @param max as const T reference
     * @param seed as const size_t reference
     * @return randomized value between min and max of type T
     */
    template<typename T>
    T get_random_min_max(const T& min, const T& max, const size_t& seed_val,
                                        const int& rand_cnt)
    {
        std::mt19937 mt;
        mt.seed(seed_val);
        int error_value = -1;

        // check if rate is float
        if (std::is_floating_point<T>::value) {
            // returns randomized value between min and max
            std::uniform_real_distribution<T> dist(min, max);

            float rand_float = 0.0f;

            for(int i = 0; i < rand_cnt; ++i) {
                rand_float = dist(mt);
            }
            return rand_float;
        }
        if(std::is_integral<T>::value) { // if rate is integer
            // returns randomized value between min and max
            std::uniform_int_distribution<T> dist(min, max);

            int rand_int = 0;

            for(int i = 0; i < rand_cnt; ++i) {
                rand_int = dist(mt);
            }
            return rand_int;
        }
        return error_value;
    } // end get_random_min_max
//----------------------------------------------------------------------------

    /*
     * Return randomized sensor or odr rate
     *
     * @param V as const std::Vector reference
     * @param seed as const size_t reference
     * @return randomized sensor or odr rate of type V
     */
    template<typename V>
    V get_random_sensor_or_odr(const std::vector<V>& v, const size_t& seed_val,
                                                        const int& rand_cnt)
    {
        //std::vector<V> const& v
        //std::cout << v.size() - 1 << std::endl;
        int last_element_idx = v.size() - 1;
        int random = get_random_min_max(0, last_element_idx, seed_val, rand_cnt);

        return(v[random]);

    } // end get_random_sensor_or_odr
//---------------------------------------------------------------------------
} // end namespace sensors_stability
///////////////////////////////////////////////////////////////////////////////

#endif /* TEMPLATE_IMPL_HPP */
