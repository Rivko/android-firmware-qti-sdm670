/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "utils.h"
#include "board.h"

#define SYS_SOC_ID  "/sys/devices/soc0/soc_id"
#define SYS_PLATFROM_SUBTYPE_ID "/sys/devices/soc0/platform_subtype_id"
#define SYS_PLATFROM_VERSION "/sys/devices/soc0/platform_version"

/* chip information */
static soc_info_t soc_list[] = {
    {"MDM9200", 57},
    {"MDM9600", 58},
    {"MSM8260", 70},
    {"MSM8660", 71},
    {"APQ8060", 86},
    {"MSM8960", 87},
    {"MSM7225A", 88},
    {"MSM7625A", 89},
    {"MSM7227A", 90},
    {"MSM7627A", 91},
    {"ESM7227A", 92},
    {"ESM7225A", 96},
    {"ESM7627A", 97},
    {"MSM7225AA", 98},
    {"MSM7625AA", 99},
    {"ESM7225AA", 100},
    {"MSM7227AA", 101},
    {"MSM7627AA", 102},
    {"ESM7227AA", 103},
    {"APQ8064", 109},
    {"MSM8930", 116},
    {"MSM8630", 117},
    {"MSM8230", 118},
    {"APQ8030", 119},
    {"MSM8627", 120},
    {"MSM8227", 121},
    {"MSM8660A", 122},
    {"MSM8260A", 123},
    {"APQ8060A", 124},
    {"MSM8974", 126},
    {"MSM8225", 127},
    {"MSM8625", 129},
    {"MPQ8064", 130},
    {"MSM7225AB", 131},
    {"MSM7625AB", 132},
    {"ESM7225AB", 133},
    {"MDM9625", 134},
    {"MSM7125A", 135},
    {"MSM7127A", 136},
    {"MSM8125A", 137},
    {"MSM8960AB", 138},
    {"APQ8060AB", 139},
    {"MSM8260AB", 140},
    {"MSM8660AB", 141},
    {"MSM8930AA", 142},
    {"MSM8630AA", 143},
    {"MSM8230AA", 144},
    {"MSM8626", 145},
    {"MSM8610", 147},
    {"MDM9225", 149},
    {"MDM9225M", 150},
    {"MDM9625M", 152},
    {"APQ8064AB", 153},
    {"MSM8930AB", 154},
    {"MSM8630AB", 155},
    {"MSM8230AB", 156},
    {"APQ8030AB", 157},
    {"MSM8226", 158},
    {"MSM8826", 159},
    {"APQ8030AA", 160},
    {"MSM8110", 161},
    {"MSM8210", 162},
    {"MSM8810", 163},
    {"MSM8212", 164},
    {"MSM8612", 165},
    {"MSM8812", 166},
    {"MSM8125", 167},
    {"MDM9310", 171},
    {"APQ8064AA", 172},         /* aka V2 SLOW_PRIME */
    {"APQ8084", 178},
    {"MSM8130", 179},
    {"MSM8130AA", 180},
    {"MSM8130AB", 181},
    {"MSM8627AA", 182},
    {"MSM8227AA", 183},
    {"APQ8074", 184},
    {"MSM8274", 185},
    {"MSM8674", 186},
    {"MDM9635", 187},
    {"FSM9900", 188},
    {"FSM9905", 189},
    {"FSM9955", 190},
    {"FSM9950", 191},
    {"FSM9915", 192},
    {"FSM9910", 193},
    {"MSM8974AC", 194},
    {"MSM8126", 198},
    {"APQ8026", 199},
    {"MSM8926 ", 200},
    {"MSM8326", 205},
    {"MSM8916", 206},
    {"MSM8994", 207},
    {"APQ8074AA", 208},
    {"APQ8074AB", 209},
    {"APQ8074AC", 210},
    {"MSM8274AA", 211},
    {"MSM8274AB", 212},
    {"MSM8274AC", 213},
    {"MSM8674AA", 214},
    {"MSM8674AB", 215},
    {"MSM8674AC", 216},
    {"MSM8974AA", 217},
    {"MSM8974AB", 218},
    {"APQ8028", 219},
    {"MSM8128", 220},
    {"MSM8228", 221},
    {"MSM8528", 222},
    {"MSM8628", 223},
    {"MSM8928", 224},
    {"MSM8510", 225},
    {"MSM8512", 226},
    {"MSM8936", 233},
    {"MDM9640", 234},
    {"MSM8939", 239},
    {"APQ8036", 240},
    {"APQ8039", 241},
    {"MSM8236", 242},
    {"MSM8636", 243},
    {"MSM8909", 245},
    {"MSM8996", 246},
    {"APQ8016", 247},
    {"MSM8216", 248},
    {"MSM8116", 249},
    {"MSM8616", 250},
    {"MSM8992", 251},
    {"APQ8092", 252},
    {"APQ8094", 253},
    {"FSM9008", 254},
    {"FSM9010", 255},
    {"FSM9016", 256},
    {"FSM9055", 257},
    {"MSM8209", 258},
    {"MSM8208", 259},
    {"MDM9209", 260},
    {"MDM9309", 261},
    {"MDM9609", 262},
    {"MSM8239", 263},
    {"APQ8009", 265},
    {"MSM8952", 264},
    {"MSM8956", 266},
    {"MSM8929", 268},
    {"MSM8629", 269},
    {"MSM8229", 270},
    {"APQ8029", 271},
    {"APQ8056", 274},
    {"MSM8609", 275},
    {"FSM9916", 276},
    {"APQ8076", 277},
    {"MSM8976", 278},
    {"APQ8052", 289},
    {"APQ8096", 291},
    {"MSM8998", 292},
    {"MSM8953", 293},
    {"MSM8937", 294},
    {"APQ8037", 295},
    {"MSM8909W", 300},
    {"APQ8009W", 301},
    {"MSM8996L", 302},
    {"MSM8917", 303},
    {"APQ8053", 304},
    {"MSM8996SG", 305},
    {"APQ8017", 307},
    {"MSM8217", 308},
    {"MSM8617", 309},
    {"MSM8996AU", 310},
    {"APQ8096AU", 311},
    {"APQ8096SG", 312},
    {"MSM8940", 313},
    {"SDM660", 317},
    {"SDM630", 318},
    {"APQ8998", 319},
    {"MSM8920", 320},
};

int get_os_type(char *buf, int size){

    if(buf == NULL)
        return -1;

#ifdef ANDROID_WEARABLES
    strlcpy(buf, OS_ANDROID_WEAR, size);
#else
    strlcpy(buf, OS_ANDROID, size);
#endif

   return 0;
}

int get_chip_name(char *buf, int size) {

    int soc_id = 0, i;

    if(buf == NULL)
        return -1;

    if(!read_file(SYS_SOC_ID, buf, sizeof(buf))) {
        soc_id = atoi(buf);
    }

    ALOGI("soc id:%d", soc_id);

    for(i = 0; i < sizeof(soc_list) / sizeof(soc_info_t); i++) {
        if(soc_list[i].soc_id == soc_id) {
            strlcpy(buf, soc_list[i].name, size);
            return 0;
        }
    }

    return -1;
}

int get_subtype_id(char *buf, int size) {
    if(buf == NULL)
        return -1;

    if(read_file(SYS_PLATFROM_SUBTYPE_ID, buf, size))
        return -1;

    return 0;
}

int get_platform_version(char *buf, int size) {
    if(buf == NULL)
        return -1;

    if(read_file(SYS_PLATFROM_VERSION, buf, size))
        return -1;

    return 0;
}
