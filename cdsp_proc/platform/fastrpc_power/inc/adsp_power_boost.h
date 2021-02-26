#ifndef ADSP_POWER_BOOST_H
#define ADSP_POWER_BOOST_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * enable all the hardware resources for compute
 * @retval, 0 on success
 */
int adsp_power_boost_on(void);

/**
 * disable all the hardware resources for compute
 */
void adsp_power_boost_off(void);

#ifdef __cplusplus
}
#endif
#endif //ADSP_POWER_BOOST_H
