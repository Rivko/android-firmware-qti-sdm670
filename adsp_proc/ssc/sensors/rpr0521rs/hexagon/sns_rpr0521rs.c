//==============================================================================
// MODULE        : sns_rpr0521rs.c
// FUNCTION      : Driver source for Ambient Lihgt Sensor and Proximity Sensor 
//               : (3in1 sensor)
// AUTHOR        : Masafumi Seike
// MAKING DATE   : Oct/30/2017
// MODIFIED      : Ryosuke Yamamoto
// MODIFIED DATE : Dec/4/2018
// REMARKS       :
// COPYRIGHT     : Copyright (C) 2017 - 2018 - ROHM CO.,LTD.
//               : Redistribution and use in source and binary forms, with or
//               : without modification, are permitted provided that the
//               : following conditions are met:
//               : 1. Redistributions of source code must retain the above
//               : copyright notice, this list of conditions and the following
//               : disclaimer.
//               : 2. Redistributions in binary form must reproduce the above
//               : copyright notice, this list of conditions and the following
//               : disclaimer in the documentation and/or other materials
//               : provided with the distribution.
//               : 3. Neither the name of ROHM CO.,LTD. nor the names of its
//               : contributors may be used to endorse or promote products
//               : derived from this software without specific prior written
//               : permission.
//               : THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
//               : CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
//               : INCLUDING , BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
//               : MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//               : DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
//               : BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//               : CONTRIBUTORS EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
//               : BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
//               : SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//               : INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
//               : WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//               : NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//               : OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
//               : SUCH DAMAGE.
//==============================================================================
#include "sns_rpr0521rs_sensor.h"
#include "sns_register.h"
#include "sns_rpr0521rs_sensor_instance.h"

sns_rc sns_register_rpr0521rs(sns_register_cb const *register_api);

//==============================================================================
// NAME       : sns_register_rpr0521rs
// FUNCTION   : register sensor function and sensor instance
// REMARKS    :
//==============================================================================
sns_rc sns_register_rpr0521rs(sns_register_cb const *register_api)
{
    // Register ALS Sensor.
    register_api->init_sensor(sizeof(RPR0521RS_STATE), &rpr0521rs_als_sensor_api, &rpr0521rs_sensor_instance_api);

    // Register PS Sensor.
    register_api->init_sensor(sizeof(RPR0521RS_STATE), &rpr0521rs_ps_sensor_api, &rpr0521rs_sensor_instance_api);

    return (SNS_RC_SUCCESS);
}
