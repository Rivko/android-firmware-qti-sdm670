/* Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <OperatorLookup.h>
#include <cutils/log.h>

const IINInfo OperatorLookup::IINInfoArray [] =
{
   /*--- operator = VzW ---*/
   {"VZW", "891480"},
   {"VZW", "891004"},
   {"VZW", "891005"},
   {"VZW", "891012"},
   /*--- operator = ATT ---*/
   {"ATT", "8901410"},
   {"ATT", "8901030"},
   {"ATT", "8901150"},
   {"ATT", "8901170"},
   {"ATT", "8901560"},
   {"ATT", "8901680"},
   {"ATT", "890138"},
   /*--- Do not modify the values of existing operators ---*/
   /*--- Add new Operator entry prefixed with "OEM_" below this line ---*/
};
string OperatorLookup::lookupOperator(const string& iccid_str) {
    // Traverse through operator map with the ICCID
    ALOGI(" OperatorLookup Enter : ICCID String Passed : %s", iccid_str.c_str());
    for (unsigned int i =0; i < (sizeof(IINInfoArray)/sizeof(IINInfoArray[0])); i++){
        if (iccid_str.compare(0, IINInfoArray[i].iin.length(),
            IINInfoArray[i].iin) == 0)  {
            string oper_str = IINInfoArray[i].name;
            ALOGI("OperatorLookup:  match found for iin [%s] and oper_str [%s]", IINInfoArray[i].iin.c_str(), oper_str.c_str());
            if ( (oper_str.compare("VZW") == 0) ||
                 (oper_str.compare("ATT") == 0) ||
                 (oper_str.compare("ROW") == 0) ||
                 (oper_str.compare(0,4,"OEM_") == 0) ) {

               return oper_str;
            } else {
               return ERROR_STR;
            }
        }
    }
    ALOGI("OperatorLookup: No Match Found for iccid: [%s] - Returning default operator: [%s]", iccid_str.c_str(), default_operator);
    return default_operator;
}
