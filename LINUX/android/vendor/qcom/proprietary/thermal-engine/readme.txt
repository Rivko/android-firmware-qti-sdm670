thermal-engine
~~~~~~~~~~~~~~

This daemon monitors thermal/temperature sensor data and performs
actions based on a configuration file (default is /etc/thermal-engine.conf).

A sample configuration file is provided (thermal-engine.conf_sample).



Configuration file format:
~~~~~~~~~~~~~~~~~~~~~~~~~~
{debug}
sampling <default sampling rate in ms>

[<Algorithm instance label>] LABEL MUST BE UNIQUE MAX LEN 31 CHARACTERS
algo_type      <algorithm type for instance>  MUST BE FIRST FIELD IN USER
                                              DEFINED SECTION.
disable        <OPTIONAL field used to disable default target algorithm
                instances.


[<Algorithm instance label>]
algo_type      monitor
sensor         <sensor name>
sampling       <sampling rate in ms>
descending     <OPTIONAL threshold trigger direction is falling below value
                as opposed to default behavior rising above.>
thresholds     <threshold values in degree mC or mA> ...
thresholds_clr <temperature value to clear thresholds> ...
actions        <action to perform at threshold;
                multiple actions separated by '+'> ...
action_info    <additional info for action;
                multiple action_info separated by '+'> ...

[<Algorithm instance label>]
algo_type          pid
sampling           <sampling rate in ms>
sensor             <sensor name>
device             <device to be mitigated according to PID calculation>
set_point          <Target sensor point for PID algorithm>
set_point_clr      <Sensor point at which PID algorithm stops making
                    adjustments>
p_const            <PID P constant>
i_const            <PID I constant>
d_const            <PID D constant>
i_samples          <Number of integration samples for integration error
                    component>
dev_units_per_calc <Units of device adjustment per PID calculation outcome>
freq_scale         <Frequency scaling factor for DUAL PID>

[<Algorithm instance label>]
algo_type          ss
sampling           <sampling rate in ms>
sensor             <sensor name>
device             <device to be mitigated according to PID calculation>
set_point          <Target sensor point for PID algorithm>
set_point_clr      <Sensor point at which PID algorithm stops making
                    adjustments>
time_constant      <Multiplier of sampling period for holding off adjustments
                    when current and last error sample are equivalent>
device_max_limit   <OPTIONAL field to specify device mitigation max limit.
                    If it is defined, this instance mitigates device up to
                    device_max_limit value. It expects value in KHz for cpu
                    device and Hz for gpu device>
device_perf_floor   <OPTIONAL field to specify device performance mitgation
                    floor. If it is defined, this instance stops mitigating a
                    device to a level with a corresponding perf_lvl at or above
                    device_perf_floor.>

[<Algorithm instance label>]
algo_type          tb
sampling           <sampling rate in ms>
sensor             <sensor name>
device             <device to be mitigated according to token bucket calculation>
set_point          <Target sensor point for token bucket algorithm>
set_point_clr      <Sensor point at which token bucket algorithm stops making
                    adjustments>
time_constant      <Multiplier of sampling period for holding off adjustments
                    when current and last error sample are equivalent>
up_loop_gain       <Gain multiplier for token bucket reward calculations>
down_loop_gain     <Gain multiplier for token bucket penalty calculations>
auto_penalty       <Extra penalty added whenever a client is at or above its
                    set point. Should be in units of temperature/temp_scale_factor>
auto_reward        <Extra reward added whenever a client is below its set point.
                    Should be in units of temperature/temp_scale_factor>
temp_scale_factor  <Divider for temperature to get it to units of Degree Celsius from
                    the value returned by reading the sensor.>
freq_scale_factor  <Multiplier for device frequency readings. Default device frequency
                    unit is KHz, so set to 1 if device frequency is read in KHz,
                    1000 for Hz. This is necessary because GPU freq is often in Hz.>
quadratic_reward   <Boolean (zero false, nonzero true) to use a quadratic reward system
                    in the control loop>
quadratic_penalty  <Boolean (zero false, nonzero true) to use a quadratic penalty system
                    in the control loop>
use_timeout        <Boolean (zero false, nonzero true) to use a timeout when exiting the
                    active control zone for this client. This helps in smoothing handoff
                    between LMH and thermal sensors>
timeout            <If 'use_timeout' is true, then after a client's temp equals or falls
                    below its 'set_point_clr', the algorithm will wait 'timeout' number of
                    interrupts before turning off mitigation, feeding reward into the system
                    of value 'set_point' - 'set_point_clr'. This is useful in the case of LMH
                    since LMH may go to 0 briefly, and we don't want to have low temperatures
                    cause the bucket to spike. This value must be greater than 0.>
unified_rail       <If 'unified_rail' is true for any configuration instance of the algorithm,
                    the unified rail logic of token bucket is applied>



[<virtual sensor name>]
algo_type          virtual
trip_sensor        <sensor name>
set_point          <when above this temperature value, virtual sensor starts polling mode>
set_point_clr      <when below this temperature value, virtual sensor stops polling mode>
sensors            <array of sensors to calculate weighted temperature sum>
weights            <array of weighted number>
math               <0 is default, to use the weights. "math" is optional for default. 1 for
                    minimum of sensors, 2 for max of sensors>
sampling           <default sampling rate>

* Optional 'debug' flag determines if debug logging is enabled.
* 'thresholds'/'thresholds_clr'/'actions'/'action_info' accepts a list of
  space separated values for up to 8 thresholds.

'actions' field
---------------
* 'none' ;
- ACTION - Do nothing
- ACTION_INFO - ignored

* 'report'
- ACTION - Report threshold crossing to UI
- ACTION_INFO - ignored
- NOTES - The threshold crossing information is sent across an
          abstract local socket "THERMALD_UI" in newline-separated
          string format.  Report action runs upon clearing or
	  triggering a level, slightly different from the other
	  actions which are run upon reaching the level, e.g.
	  clearing level n+1 or triggering level n.

	  Parameters are sent in the following order.
          * sensorname - Name of sensor reporting
          * temperature - Current temperature
          * current_threshold_level - current threshold level triggered or cleared
          * is_trigger - "true" on level trigger, "false" on level clearing

	  Example Android code to listen to this notification is
          provided in the documentation folder.

* 'cpu'
- ACTION - CPU frequency scaling
- ACTION_INFO - Max CPU frequency in KHz

* 'cpuN'
- ACTION - CPU frequency scaling where N is the specific CPU core [0..MAX CORES]
- ACTION_INFO - Max CPU frequency in KHz

* 'clusterN'
- ACTION - CLUSTER frequency scaling where N is the specific CLUSTER ID
- ACTION_INFO - Max CLUSTER frequency in KHz

* 'hotplug_N'
- ACTION -  Hotplug a specific CPU core [1..MAX CORES]
- ACTION_INFO - 0 for online a core, or 1 to offline it.

* 'lcd'
- ACTION - LCD brightness throttling
- ACTION_INFO - 0-255 value for max LCD brightness

* 'modem'
- ACTION - Request throttling of modem functionality
- ACTION_INFO - 0-3 throttling level for modem mitigation

* 'fusion'
- ACTION - Request throttling of fusion modem functionality
- ACTION_INFO - 0-3 throttling level for fusion modem mitigation

* 'battery'
- ACTION - Battery charging current throttling
- ACTION_INFO - 0-3 throttling level for battery charging current

* 'gpu'
- ACTION - GPU frequency scaling
- ACTION_INFO - Max GPU frequency in Hz

* 'wlan'
- ACTION - WLAN throttling
- ACTION_INFO - 0-4 throttling level for WLAN mitigation

* 'shutdown'
- ACTION - Shutdown target
- ACTION_INFO - Shutdown delay in ms

* 'vdd_restriction'
- ACTION - Request voltage restriction of all vdd rails on SoC
- ACTION_INFO - 1 for request for vdd_restriction,
                0 for release vdd_restriction

* 'camera'
- ACTION - camera fps throttling and camera shutdown mitigation
- ACTION_INFO - 0-3 throttling level for camera fps mitigation,
                10 level for camera app shutdown

* 'camcorder'
- ACTION - camcorder fps throttling and camcorder shutdown mitigation
- ACTION_INFO - 0-3 throttling level for camcorder fps mitigation,
                10 level for camcorder app shutdown

* 'mdp'
- ACTION - Request throttling of MDP CX voting
- ACTION_INFO - 0-3 throttling level for MDP mitigation

* 'venus'
- ACTION - Request throttling of VENUS CX voting
- ACTION_INFO - 0-3 throttling level for VENUS mitigation

* 'modem_cx'
- ACTION - Request throttling of modem CX voting
- ACTION_INFO - 0-3 throttling level for MODEM CX mitigation

'device' field
---------------
* 'cpu'
- DEVICE - Dynamic CPU frequency scaling

* 'cpuN'
- DEVICE - Dynamic CPU frequency scaling where N is the specific CPU core [0..MAX CORES]

* 'clusterN'
- DEVICE - Dynamic CLUSTER frequency scaling where N is the specific CLUSTER ID

* 'gpu'
- DEVICE - Dynamic GPU frequency scaling

Example 1:
----------
sampling         1000

[PMIC_THERM_MON]
algo_type        monitor
sensor           PMIC_THERM
sampling         5000
thresholds       40200       45000   50000
thresholds_clr   38000       43000   48000
actions          cpu+report  cpu     cpu
action_info      1188000+0   368640  245760

Description:
1) Default sampling rate of 1 second.
For sensor 'PMIC_THERM', sample at 5 second rate (overrides default):
2) Threshold level 1 triggered at 40.2 deg C; clears if temperature drops
   to/below 38 deg C.  When triggered, adjust maximum allowed CPU to
   1188000 KHz, which in this example is already the current maximum
   frequency, and results in no action.
   Also generate a report, action_info value 0 is ignored.
3) Threshold level 2 triggered at 45 deg C; clears if temperature drops
   to/below 43 deg C.  When triggered, adjust maximum allowed CPU to
   368640 KHz.
4) When threshold level 2 cleared at 43 deg C; adjust CPU back to maximum
   frequency 1188000 KHz.
5) When threshold level 1 cleared at 38 deg C; generate a report for level 1
   clearing, all mitigation is reset.

Example 2:
----------
debug
sampling         2000

[PMIC_THERM_MONITOR]
algo_type        monitor
sensor           PMIC_THERM
sampling         5000
thresholds       40200         45000   50000
thresholds_clr   38000         43000   48000
actions          cpu+report    cpu     report+shutdown
action_info      768000+0      368640  0+6000

Description:
1) Debug logging output is enabled.
2) Default sampling rate of 2 second.
For sensor 'PMIC_THERM', sample at 5 second rate (overrides default):
3) Threshold level 1 triggered at 40.2 deg C; clears if temperature drops
   to/below 38 deg C.  When triggered, adjust maximum allowed CPU to 768000 KHz,
   and generate a report (action_info value 0 is ignored).
4) Threshold level 2 triggered at 45 deg C; clears if temperature drops
   to/below 43 deg C.  When triggered, adjust maximum allowed CPU to 368640 KHz.
5) Threshold level 3 triggered at 50 deg C; clears if temperature drops
   to/below 48 deg C. When triggered, generate a report and shutdown the
   target after a delay of 6 seconds.

Example 3:
----------
debug
sampling         2000

[bcl_monitor]
algo_type        monitor
descending
sensor           bcl
sampling         1000
thresholds       100      0
thresholds_clr   500      100
actions          report   report
action_info      0        0

Description:
1) Debug logging output is enabled.
2) Default sampling rate of 2 second.
For battery current limit 'bcl', sample at 1 second rate (overrides default):
3) Threshold level 1 triggered if ibat is at (imax - 100mA); clears if ibat drops
   to/below (imax - 500mA).  When triggered, generate a report (action_info value 0 is ignored).
4) Threshold level 2 triggered if ibat is at imax; clears if ibat drops
   to/below (imax - 100mA).  When triggered, generate a report (action_info value 0 is ignored).

Example 4:
----------
debug

[TEST_PID]
algo_type          pid
sensor             tsens_tz_sensor0
device             cpu
sampling           1000
set_point          85000
set_point_clr      65000
p_const            1.0
i_const            1.0
d_const            1.0
i_samples          10
dev_units_per_calc 10000

Description:
1) Debug logging output is enabled.
2) PID instance labeled 'TEST_PID'.
3) Sensor used is tsens_tz_sensor0.
4) device to mitigate is CPU.
5) 1 second sampling rate.
5) set_point is the threshold for PID mitigation and set pont of PID algorithm.
6) set_point_clr is the threshold at which to stop PID mitigation calculation.
7) p_const, i_const, and d_const are constants used in PID equation.
8) i_samples integration error sample count to be used in PID equation.
9) dev_units_per_calc 10000kHz(kHz unit becase this is CPU device) is multiplied
   with PID calculation outcome to determine adjustment on the cpu device.

Example 5:
---------
debug

[virtual-sensor-0]
algo_type          virtual
trip_sensor        tsens_tz_sensor8
set_point          35000
set_point_clr      30000
sensors            tsens_tz_sensor1 tsens_tz_sensor5
weights            40 60
sampling           250

[Test-PID]
algo_type          pid
sensor             virtual-sensor-0
device             cpu1
sampling           250
set_point          55000
set_point_clr      50000
p_const            1.25
i_const            0.8
d_const            0.5
i_samples          10
dev_units_per_calc 5000

Description:
1) PID instance 'Test-PID' is running based on virtual-sensor-0 reading.
2) virtual-sensor-0 needs to be defined manually by users as [<virtual sensor name>]
3) trip_sensor is used to determine when virtual sensor polling mode will start.
4) trip_sensor has to be a regular sensor, cannot be another virtual sensor.
5) set_point is the threshold for trip sensor. When above this value, trip
   sensor will transition from interrupt mode to polling mode. The polling rate
   is based on the sampling number defined in [<virtual sensor name>] instance.
6) set_point_clr is the threshold for trip sensor. When below this value, trip
   sensor will stop polling mode and wait for next threshold event.
7) sensors define an array of regular sensors that virtual sensor requires to calculate
   the weighted temperature.
8) weights define an array of weighted value that corresponding to the array of sensors.
9) set_point of virtual sensor has to be less than the set_point for pid so that pid
   will get notification when set_point is reached. Otherwise, pid will not be able to
   know the temperature if virtual sensor doesn't start polling mode.
10)If trip sensor does not support changing from interrupt mode to polling mode,
   then 9) can be ignored. Virtual sensor will simply keep reading temperature
   every sampling period.

Example 6:
----------
debug

[bcm_monitor]
algo_type        monitor
sensor           bcm
sampling         1000
thresholds       70000    90000
thresholds_clr   69000    89000
actions          cpu      cpu
action_info      768000   384000

Description:
1) Debug logging output is enabled.
2) Thermal-engine configures a threshold of 70000 mPercent for level 1.
   Threshold level 1 triggered if 70% of rated current reaches.
   When triggered, adjust maximum allowed CPU to 768000 KHz,
3) Thermal-engine configures a threshold of 90000 mPercent for level 2.
   Threshold level 2 triggered if 90% of rated current reaches.
   When triggered, adjust maximum allowed CPU to 384000 KHz.
4) The bcm supports only two level of mitigation.
5) Only thresholds 40000, 50000, 60000, 70000, 80000 and 90000 is valid.

Example 7:
----------

debug

[TB-CPU4]
algo_type            tb
sampling             10
sensor               tsens_tz_sensor13
device               cluster1
set_point            85000
set_point_clr        50000
time_constant        1
up_loop_gain         2
down_loop_gain       3
auto_penalty         1.0
auto_reward          0.0
temp_scale_factor    1000
freq_scale_factor    1
quadratic_reward     1
quadratic_penalty    1
use_timeout          0
timeout              1

Description:
1) debug indicates that debug logging output is enabled.
2) We are configuring CPU4 to be controlled by token bucket. This instance
   will monitor tsens_tz_sensor13 and control the frequency on cluster1,
   since algo_type is tb, sensor is tsens_tz_sensor13 and device is cluster1.
3) Temperature readings and subsequent frequency mitigation will occur every
   10ms since sampling is 10.
4) Temperature readings and subsequent frequency mitigation will begin when
   tsens_tz_sensor13 reaches 85000mC and will continue every 10ms until
   tsens_tz_sensor13 reaches 50000mC, since set_point is 85000 and
   set_point_clr is 50000.
5) time_constant is currently unused but is reserved for future use.
6) up_loop_gain set to 2 means that for every degree that tsens_tz_sensor13
   is below its set_point, the reward will increase by a factor of 2.
7) down_loop_gain set to 3 means that for every degree that tsens_tz_sensor13
   overshoots its set_point, the penalty will increase by a factor of 3.
8) auto_penalty of 1.0 means that the number of degrees of overshoot used
   to calculate penalty will always be incremented by 1.0. This means that at
   85C, the temperature overshoot will be 1C, 86C->2C, etc.
9) auto_reward of 0.0 means that the number of degrees of undershoot used to
   calculate the reward will not be augmented. This means that at 80C, the
   temperature undershoot will be 5C, 81C->4C, etc.
10) temp_scale_factor of 1000 means that the temperature readings from
   tsens_tz_sensor13 are received in mC and not C. This used to vary between
   tsens sensors and other types of sensors in the thermal-engine such as gen
   sensors.
11) freq_scale_factor of 1 means that the frequency readings from device1 are
   recieved in kHz. This varies between devices, for instance the GPU frequency
   readings on 8994 are received in Hz, and therefore the freq_scale_factor
   for the 8994 GPU is 1000.
12) quadratic_reward of 1 means to square the amount of reward. Whereas
   reward would normally be (undershoot * up_loop_gain), if this value is
   nonzero then reward is calculated as (undershoot * up_loop_gain)^2.
13) quadratic_penalty of 1 means to square the amount of penalty. Whereas
   penalty would normally be (overshoot * down_loop_gain), if this value is
   nonzero then penalty is calculated as (overshoot * down_loop_gain)^2.
14) use_timeout of 0 means that when the readings on tsens_tz_sensor13 reach
   their set_point_clr, the algorithm should immediately stop polling and
   stop mitigating. If this value were nonzero, then the algorithm would wait
   for timeout number of sequential interrupts (i.e. timeout * sampling ms)
   in which the reading from tsens_tz_sensor13 was below set_point_clr before
   ceasing to poll and mitigate.
