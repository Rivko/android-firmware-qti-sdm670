//
// Created by gmk on 2016/12/5.
//

#ifndef HELLO_NI2_CMD_H
#define HELLO_NI2_CMD_H

#include <PS1080.h>
#include <OpenNI.h>
#include <vector>
#include <map>

#include <libusb.h>
#include <string>

using namespace openni;
using namespace std;

typedef bool (*cbfunc)(vector<string>& Command);


enum EPsProtocolOpCodes
{
    OPCODE_GET_VERSION = 0,
    OPCODE_KEEP_ALIVE = 1,
    OPCODE_GET_PARAM = 2,
    OPCODE_SET_PARAM = 3,
    OPCODE_GET_FIXED_PARAMS = 4,
    OPCODE_GET_MODE = 5,
    OPCODE_SET_MODE = 6,
    OPCODE_GET_LOG = 7,
    OPCODE_RESERVED_0 = 8,
    OPCODE_RESERVED_1 = 9,
    OPCODE_I2C_WRITE = 10,
    OPCODE_I2C_READ = 11,
    OPCODE_TAKE_SNAPSHOT = 12,
    OPCODE_INIT_FILE_UPLOAD = 13,
    OPCODE_WRITE_FILE_UPLOAD = 14,
    OPCODE_FINISH_FILE_UPLOAD = 15,
    OPCODE_DOWNLOAD_FILE = 16,
    OPCODE_DELETE_FILE = 17,
    OPCODE_GET_FLASH_MAP = 18,
    OPCODE_GET_FILE_LIST = 19,
    OPCODE_READ_AHB = 20,
    OPCODE_WRITE_AHB = 21,
    OPCODE_ALGORITM_PARAMS = 22,
    OPCODE_SET_FILE_ATTRIBUTES = 23,
    OPCODE_EXECUTE_FILE = 24,
    OPCODE_GET_CMOS_DATA = 25,
    OPCODE_SET_GMC_PARAMS = 26,
    OPCODE_GET_CPU_STATS = 27,
    OPCODE_BIST = 28,
    OPCODE_CALIBRATE_TEC = 29,
    OPCODE_GET_TEC_DATA = 30,
    OPCODE_CALIBRATE_EMITTER = 31,
    OPCODE_GET_EMITTER_DATA = 32,
    OPCODE_CALIBRATE_PROJECTOR_FAULT = 33,
    OPCODE_SET_CMOS_BLANKING = 34,
    OPCODE_GET_CMOS_BLANKING = 35,
    OPCODE_GET_CMOS_PRESETS = 36,
    OPCODE_GET_SERIAL_NUMBER = 37,
    OPCODE_GET_FAST_CONVERGENCE_TEC = 38,
    OPCODE_GET_PLATFORM_STRING = 39,
    OPCODE_GET_USB_CORE_TYPE = 40,
    OPCODE_SET_LED_STATE = 41,
    OPCODE_ENABLE_EMITTER = 42,
    OPCODE_ENABLE_FLOOD_LED = 43,
    OPCODE_GET_FLOOD_LED_STATUS = 44,
    OPCODE_GET_LASER_STATUS = 45,
    CMD_ACTIVITY_LED_SET = 76,
    CMD_ACTIVITY_LED_GET = 77,
    CMD_IR_EXPOSURE_GET = 78,
    CMD_IR_GAIN_GET = 79,
    CMD_ENABLE_LDP1 = 83,
    CMD_ENABLE_LDP2 = 84,
    CMD_ENABLE_EMITTER = 85,
    CMD_GAIN_SET = 87,
    CMD_EXP_SET = 96,
    CMD_GET_CAMERA_PARA = 97,
    OPCODE_KILL = 999,
};

enum LaserStatus
{
    LASER_OFF = 0,
    LASER_ON = 1,
    LASER_UNKNOWN = 0xff,
};

enum IrFloodLedStatus
{
    IR_LED_OFF = 0,
    IR_LED_ON = 1,
    IR_LED_UNKNOWN = 0xff,
};

enum LDPStatus
{
    LDP_OFF = 0,
    LDP_ON = 1,
    LDP_UNKNOWN = 0xff,
};


typedef struct {
    uint16_t magic;
    uint16_t size;
    uint16_t opcode;
    uint16_t id;
} protocol_header;

typedef struct OBCameraParams
{
    float l_intr_p[4];//[fx,fy,cx,cy]
    float r_intr_p[4];//[fx,fy,cx,cy]
    float r2l_r[9];//[r00,r01,r02;r10,r11,r12;r20,r21,r22]
    float r2l_t[3];//[t1,t2,t3]
    float l_k[5];//[k1,k2,k3,p1,p2]
    float r_k[5];//[k1,k2,k3,p1,p2]
    int is_m;
}OBCameraParams;

#if 1
typedef struct _cam_hdr{
    uint8_t     magic[2];
    uint16_t    len;
    uint16_t    cmd;
    uint16_t    tag;
} cam_hdr;
#endif


#define CMD_HEADER_MAGIC	(0x4d47)
#define CMD_HEADER_LEN		(0x08)

bool atoi2(const char* str, int* pOut);

class cmd
{
public:
    cmd(void);
    ~cmd(void);
public:
    int init(void);
    int get_version(void);
    string get_sn_number(void);
    OBCameraParams get_cmos_params(void);
    //int get_cmos_data(uint32_t offset, uint16_t size);
    // the tec data and the temparature of function doesn't apply
    //float get_temparature(void);
    //float get_tec_data(void);
    unsigned short read_i2c(vector<string>& Command, XnControlProcessingData& I2C);
    bool write_i2c(vector<string>& Command, XnControlProcessingData& I2C);
    int ir_flood_set(bool status);
    int ir_flood_get(enum IrFloodLedStatus &status);
    int ir_exposure_set(uint32_t val);
    int ir_exposure_get(uint32_t &val);
    //bool ir_gain_set(const char* val);
    //bool ir_gain_get(void);
    int emitter_set(bool emitter_status);
    int emitter_get(enum LaserStatus &status);
    void RegisterCB(string cmd, cbfunc func, const string& strHelp);
    void RegisterMnemonic(string strMnemonic, string strCommand);
    int ldp_set(bool ldp_status);
    int ldp_get(enum LDPStatus &status);
    int ir_gain_set(uint32_t val);
    int SendldpCmd(uint16_t cmd, void *cmdbuf, uint16_t cmd_len, void *replybuf, uint16_t reply_len);

    Device device;
    unsigned short m_I2CReg;

    map<string, cbfunc> cbs;
    map<string, cbfunc> mnemonics;
    map<string, string>  helps;
    protocol_header *pheader;

    IrFloodLedStatus status_IrFlood;
    LaserStatus status_Laser;
    LDPStatus status_LDP;

private:
    //XnUInt16 seq_num;
    //XnUInt8	req_buf[512];
    //XnUInt8	resp_buf[512];
    uint16_t seq_num;
    uint8_t	req_buf[512];
    uint8_t	resp_buf[512];

    uint8_t obuf[0x400];
    uint8_t ibuf[0x200];

    uint8_t buf1[10];
    uint8_t buf2[10];

    int m_vid;
    int m_pid;
    uint16_t m_camTag;

#ifdef WIN32
    XN_USB_DEV_HANDLE m_hUSBDevice;
	const XnUSBConnectionString* m_astrDevicePaths;
#else
    libusb_device **devs;
    libusb_device *dev;
    libusb_device_handle *handle;
    const char* m_astrDevicePaths;
#endif

private:

    int init_header(void *buf, uint16_t cmd, uint16_t data_len);
    int send(void *cmd_req, uint16_t req_len, void *cmd_resp, uint16_t *resp_len);
    int open();
    int send_usb(uint16_t c, void *buf0, uint16_t l1, void *buf1, uint16_t l2);
};



#define IsMirroredTrue		1
#define IsMirroredFalse		2


#endif //HELLO_NI2_CMD_H
