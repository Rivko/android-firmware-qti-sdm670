/*==============================================================================
 *
 *                           EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the module.
 *   Notice that changes are listed in reverse chronological order.
 *
 * When         Who     Version  What
 * ----------   ---     -------  -----------------------------------------------
 * 02-14-2018   QC      1.8.0    Restricted handling of flush request such that the only driver
 *                               activity is to send a flush complete event
 * 08-03-2017   SEN     1.7.0    Threshold values support in Registry(For both Temp and Humidity) 
 * 07-27-2017   QC      1.6.0    Update temp and humidity diff to 0.5
 * 07-21-2017   SEN     1.5.0    Bug fixes and full registry support
 * 07-12-2017   SEN     1.4.0    Island mode, periodic timer, registry support
 * 06-26-2017   SEN     1.3.1    Add revisioning file
 * 06-19-2017   SEN     1.3.0    Support communication test
 * 06-16-2017   QC      -        Update SHTW2 driver to OpenSSC 5.0.4
 * 05-05-2017   SEN     1.2.1    Update physical sensor config envent
 * 03-05-2017   SEN     1.2.0    Change threshold values
 * 03-05-2017   SEN     -        FIX humidity signal in SHTW2 driver
 * 19-04-2017   QC      -        Change driver from streaming to on-change mode
 * 02-15-2017   SEN     1.0.0    Initial driver implementation for OpenSSC 5.0.0
 * 
 *==============================================================================
 */
#define SHTW2_DRIVER_VERSION 0x00010008    // major[31:16].minor[15:0]
