#include "cmd.h"
#include "util.h"
#include <iostream>
#include <fstream>


bool atoi2(const char* str, int* pOut)
{
    int output = 0;
    int base = 10;
    int start = 0;

    if (strlen(str) > 1 && str[0] == '0' && str[1] == 'x')
    {
        start = 2;
        base = 16;
    }

    for (size_t i = start; i < strlen(str); i++)
    {
        output *= base;
        if (str[i] >= '0' && str[i] <= '9')
            output += str[i] - '0';
        else if (base == 16 && str[i] >= 'a' && str[i] <= 'f')
            output += 10 + str[i] - 'a';
        else if (base == 16 && str[i] >= 'A' && str[i] <= 'F')
            output += 10 + str[i] - 'A';
        else
            return false;
    }
    *pOut = output;
    return true;
}

cmd::cmd(void)
{
    m_vid = 0x2bc5;
    m_pid = 0x0401;
    seq_num = 0x00;
    status_IrFlood = IR_LED_UNKNOWN;
    status_Laser = LASER_UNKNOWN;
    status_LDP = LDP_UNKNOWN;
    handle = NULL;
}


cmd::~cmd(void)
{
    device.close();
}

int cmd::init(void)
{
    DeviceInfo dInfo;
    uint32_t n = 0;

    int rc = OpenNI::initialize();
    if (rc != STATUS_OK)
    {
        LOGE("Initialize failed\n%s\n", OpenNI::getExtendedError());
        return rc;
    }

    LOGE("%s LINUX enter\n",__func__);

	openni::Array<DeviceInfo> deviceList;
	char uri[16];

    OpenNI::enumerateDevices(&deviceList);

	dInfo = deviceList[0];

	rc = device.open(dInfo.getUri());
	m_astrDevicePaths = dInfo.getUri();

	//cout << "Uri: " << dInfo.getUri() << endl;

	if (rc != STATUS_OK)
	{
		LOGE("Couldn't open device\n%s\n", OpenNI::getExtendedError());
		return rc;
	}
	dInfo = device.getDeviceInfo();
	LOGE("vid %04x, pid %04x\n", dInfo.getUsbVendorId(), dInfo.getUsbProductId());
	m_vid = dInfo.getUsbVendorId();
	m_pid = dInfo.getUsbProductId();

	int nRetVal = libusb_init(NULL);

	if(nRetVal !=0)
	    return -1;

	int cnt = libusb_get_device_list(NULL, &devs);
	if(cnt<0)
	{
		libusb_free_device_list(devs,1);
		LOGE("failed to list deivce\n");
		return -1;
	}


	int i=0;
	int found = 0;
	struct libusb_device_descriptor desc;

	while ((dev = devs[i++]) != NULL)
	{

	    int res = libusb_get_device_descriptor(dev, &desc);
	    if (res < 0)
	    {
			LOGE("failed to get device descriptor");
			libusb_free_device_list(devs, 1);
			return -1;
	    }

	    int vid = desc.idVendor;
	    int pid = desc.idProduct;
	    int bus = libusb_get_bus_number(dev);
	    int addr = libusb_get_device_address(dev);

	    LOGE(uri,"%04x/%04x@%d/%d", vid, pid, bus, addr);
	    //LOGE("(%04x/%04x@%d/%d)\n", vid, pid, bus, addr);

	    if (0 == strncmp(uri, m_astrDevicePaths, 14))
	    {
	   		found = 1;
	  		break;
	    }
	}

	rc = libusb_open(dev, &handle);

    return rc;
}

void cmd::RegisterCB(string cmd, cbfunc func, const string& strHelp)
{
    for (unsigned int i = 0; i < cmd.size(); i++)
        cmd[i] = (char)tolower(cmd[i]);
    cbs[cmd] = func;
    helps[cmd] = strHelp;
}

void cmd::RegisterMnemonic(string strMnemonic, string strCommand)
{
    for (unsigned int i = 0; i < strCommand.size(); i++)
        strCommand[i] = (char)tolower(strCommand[i]);
    for (unsigned int i = 0; i < strMnemonic.size(); i++)
        strMnemonic[i] = (char)tolower(strMnemonic[i]);

    if (cbs.find(strCommand) != cbs.end())
    {
        mnemonics[strMnemonic] = cbs[strCommand];
    }
}


int cmd::init_header(void *buf, uint16_t cmd, uint16_t data_len)
{
    //protocol_header *pheader = (protocol_header *)buf;
    pheader = (protocol_header *)buf;
    pheader->magic = CMD_HEADER_MAGIC;
    pheader->size =	data_len / 2;
    pheader->opcode = cmd;
    pheader->id = seq_num;
    seq_num++;

    return 0;
}

int cmd::send(void *cmd_req, uint16_t req_len, void *cmd_resp, uint16_t *resp_len)
{
    uint32_t actual_len;
    int rc;

    rc = libusb_control_transfer(handle,
		                         LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR,
                                 0x00,
                                 0x0000,
                                 0x0000,
                                 (unsigned char *)cmd_req,
                                 req_len,
                                 1000);

    do
    {
        actual_len = libusb_control_transfer(handle,
             LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_VENDOR,
                0x00,0x0000,0x0000,(unsigned char *)cmd_resp,0x200,1000);
    }while((actual_len == 0)||(actual_len == 0x200));

    *resp_len = actual_len;

    return rc;
}

#if 1
int cmd::SendldpCmd(uint16_t cmd, void *cmdbuf, uint16_t cmd_len, void *replybuf, uint16_t reply_len)
{
    int res;
    unsigned int actual_len;
    //uint8_t obuf[0x400];
    //uint8_t ibuf[0x200];


    cam_hdr *chdr = (cam_hdr*)obuf;
    cam_hdr *rhdr = (cam_hdr*)ibuf;
    if (handle == 0) return -1;
    if (cmd_len & 1 || cmd_len > (0x400 - sizeof(*chdr))) {
        return -1;
    }

    chdr->magic[0] = 0x47;
    chdr->magic[1] = 0x4d;
    chdr->cmd = cmd;
    chdr->tag = m_camTag;
    chdr->len = cmd_len / 2;


    //copy the cmdbuf
    memcpy(obuf + sizeof(*chdr), cmdbuf, cmd_len);

    res = libusb_control_transfer(handle,
			LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR,
			0x00,
			0x0000,
			0x0000,
			(unsigned char *)obuf,
			cmd_len + sizeof(*chdr),
			1000);

    if (res < 0)
    {
        LOGE("send_cmd: Output control transfer failed (%d)\n", res);
        return res;
    }

    res = libusb_control_transfer(handle,
			LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR,
			0x00,
			0x0000,
			0x0000,
			(unsigned char *)obuf,
			cmd_len + sizeof(*chdr),
			1000);

    if (actual_len < (int)sizeof(*rhdr)) {
        LOGE("send_cmd: Input control transfer failed (%d)\n", res);
        return res;
    }
    actual_len -= sizeof(*rhdr);

#if 1
    if (rhdr->magic[0] != 0x52 || rhdr->magic[1] != 0x42) {
        LOGE("send_cmd: Bad magic %02x %02x\n", rhdr->magic[0], rhdr->magic[1]);
        return -1;
    }
    if (rhdr->cmd != chdr->cmd) {
        LOGE("send_cmd: Bad cmd %02x != %02x\n", rhdr->cmd, chdr->cmd);
        return -1;
    }
    if (rhdr->tag != chdr->tag) {
        LOGE("send_cmd: Bad tag %04x != %04x\n", rhdr->tag, chdr->tag);
        return -1;
    }
    if (rhdr->len != (actual_len / 2)) {
        LOGE("send_cmd: Bad len %04x != %04x\n", rhdr->len, (int)(actual_len / 2));
        return -1;
    }

    if (actual_len > reply_len) {
        LOGE("send_cmd: Data buffer is %d bytes long, but got %d bytes\n", reply_len, actual_len);
        memcpy(replybuf, ibuf + sizeof(*rhdr), reply_len);
    }
    else {
        memcpy(replybuf, ibuf + sizeof(*rhdr), actual_len);
    }
#endif

    m_camTag++;

    return actual_len;

}
#endif

string cmd::get_sn_number(void)
{
    int ret;
    uint16_t data_len;
    uint16_t resp_len;
    char sn[15];

    ret = open();
    if (ret < 0)
        return "";

    data_len = 0;
    ret = init_header(req_buf, OPCODE_GET_SERIAL_NUMBER, data_len);
    if (ret)
    {
        cout << "init header of get sn number failed" << endl;
        return "";
    }

    ret = send(req_buf, CMD_HEADER_LEN + data_len, resp_buf, &resp_len);

    if(ret<0)
	{
		cout << "send cmd  get serial number failed" << endl;
		return "";
	}

    strncpy(sn, (char *)(resp_buf + 10), sizeof(sn));
    LOGE("sn : %s\n", sn);

    return sn;
}


int cmd::get_version(void)
{
    Status rc;

    char strPlatformString[XN_DEVICE_MAX_STRING_LENGTH];
    char strVersion[20];

    int size = sizeof(strPlatformString);
    rc = device.getProperty(XN_MODULE_PROPERTY_SENSOR_PLATFORM_STRING, strPlatformString, &size);
    if (rc != openni::STATUS_OK)
    {
        LOGE("Error: %s\n", openni::OpenNI::getExtendedError());
        return rc;
    }

    if (strPlatformString[0] != '\0')
    {
        strncpy(strVersion, strPlatformString + 2, sizeof(strVersion));
        LOGE("%s\n", strVersion);
    }

    return rc;

}

OBCameraParams cmd::get_cmos_params(void)
{
    int ret;
    uint16_t data_len;
    uint16_t resp_len;
    uint32_t *p = NULL;
    OBCameraParams param[1];

    memset(param, 0, sizeof(OBCameraParams));

    ret = open();
    if (ret < 0)
        return param[0];

    data_len = 0;
    ret = init_header(req_buf, CMD_GET_CAMERA_PARA, data_len);
    if (ret)
    {
        cout << "init header of get_cmos_params failed" << endl;
        return param[0];
    }

    ret = send(req_buf, CMD_HEADER_LEN + data_len, resp_buf, &resp_len);
    if (ret)
        cout << "send cmd get_cmos_params failed" << endl;

    memcpy((unsigned char *)param, resp_buf + 10, sizeof(OBCameraParams));

    if (param[0].is_m == IsMirroredTrue)
    {
        LOGE("Mirrored : yes\n");
    }
    else if (param[0].is_m == IsMirroredFalse)
    {
        LOGE("Mirrored : no\n");
    }
    else
    {
        LOGE("Unkonwn status of mirror\n");
    }

    LOGE("[IR Camera Intrinsic]\n %.3f %.3f %.3f %.3f\n",
           param[0].l_intr_p[0], param[0].l_intr_p[1],
           param[0].l_intr_p[2], param[0].l_intr_p[3]);
    LOGE("RGB Camera Intrinsic]\n %.3f %.3f %.3f %.3f\n",
           param[0].r_intr_p[0], param[0].r_intr_p[1],
           param[0].r_intr_p[2], param[0].r_intr_p[3]);
    LOGE("[Rotate Matrix]\n");
    LOGE(" %.3f %.3f %.3f\n %.3f %.3f %.3f\n %.3f %.3f %.3f\n",
           param[0].r2l_r[0], param[0].r2l_r[1], param[0].r2l_r[2],
           param[0].r2l_r[3], param[0].r2l_r[4], param[0].r2l_r[5],
           param[0].r2l_r[6], param[0].r2l_r[7], param[0].r2l_r[8]);
    LOGE("[Translate Matrix]\n %.3f %.3f %.3f\n",
           param[0].r2l_t[0], param[0].r2l_t[1], param[0].r2l_t[2]);
    LOGE("[IR Camera Distorted Params ]\n %f %f %f %f %f\n",
           param[0].l_k[0], param[0].l_k[1], param[0].l_k[2],
           param[0].l_k[3], param[0].l_k[4]);
    LOGE("[RGB Camera Distorted Params]\n %f %f %f %f %f\n",
           param[0].r_k[0], param[0].r_k[1], param[0].r_k[2],
           param[0].r_k[3], param[0].r_k[4]);

    return param[0];
}

unsigned short cmd::read_i2c(vector<string>& Command, XnControlProcessingData& I2C)
{
    if (Command.size() != 4)
    {
        cout << "Usage: " << Command[0] << " " << Command[1] << " <cmos> <register>" << endl;
        return true;
    }

    int nRegister;
    if (!atoi2(Command[3].c_str(), &nRegister))
    {
        LOGE("Don't understand %s as a register\n", Command[3].c_str());
        return true;
    }
    I2C.nRegister = (unsigned short)nRegister;

    int nParam = 0;

    int command;
    if (!atoi2(Command[2].c_str(), &command))
    {
        cout << "cmos must be 0/1" << endl;
        return true;
    }

    if (command == 1)
        nParam = XN_MODULE_PROPERTY_DEPTH_CONTROL;
    else if (command == 0)
        nParam = XN_MODULE_PROPERTY_IMAGE_CONTROL;
    else
    {
        cout << "cmos must be 0/1" << endl;
        return true;
    }

    if (device.getProperty(nParam, &I2C) != openni::STATUS_OK)
    {
        cout << "GetParam failed!" << endl;
        return true;
    }

    cout << "I2C(" << command << ")[0x" << hex << I2C.nRegister << "] = 0x" << hex << I2C.nValue << endl;

    return I2C.nValue;
    //return true;
}

bool cmd::write_i2c(vector<string>& Command, XnControlProcessingData& I2C)
{
    if (Command.size() != 5)
    {
        cout << "Usage: " << Command[0] << " " << Command[1] << " <cmos> <register> <value>" << endl;
        return true;
    }

    int nRegister, nValue;
    if (!atoi2(Command[3].c_str(), &nRegister))
    {
        LOGE("Don't understand %s as a register\n", Command[3].c_str());
        return true;
    }
    if (!atoi2(Command[4].c_str(), &nValue))
    {
        LOGE("Don't understand %s as a value\n", Command[4].c_str());
        return true;
    }
    I2C.nRegister = (unsigned short)nRegister;
    I2C.nValue = (unsigned short)nValue;

    int nParam = 0;

    int command;
    if (!atoi2(Command[2].c_str(), &command))
    {
        LOGE("cmos should be 0 (depth) or 1 (image)\n");
        return true;
    }

    if (command == 1)
        nParam = XN_MODULE_PROPERTY_DEPTH_CONTROL;
    else if (command == 0)
        nParam = XN_MODULE_PROPERTY_IMAGE_CONTROL;
    else
    {
        cout << "cmos must be 0/1" << endl;
        return true;
    }

    openni::Status rc = device.setProperty(nParam, I2C);
    if (rc != openni::STATUS_OK)
    {
        LOGE("%s\n", openni::OpenNI::getExtendedError());
    }

    return true;
}


int cmd::ldp_set(bool ldp_status)
{
    int ret;
    uint16_t data_len;
    uint16_t resp_len;

    uint32_t a;
    uint32_t *b = (uint32_t *)0x50005000;
    uint32_t r, _d;
    uint32_t v;
    uint8_t buf1[10];
    uint8_t buf2[10];

    _d = ldp_status & 0x01;

    ret = open();
    if (ret < 0)
        return false;

    a = (uint32_t)(b + 6);
    if (a % 4 != 0)
    {
        LOGE("a is illegal\n");
        return -1;
    }

    *(uint32_t*)buf1 = a;

    data_len = 4;
    init_header(obuf,CMD_ENABLE_LDP1,data_len);
    memcpy(obuf + sizeof(*pheader), buf1, data_len);

    ret = send(obuf, data_len + sizeof(*pheader), ibuf, &resp_len);

    v = *(uint32_t*)&buf2[2];

    r = (v & (~(1 << 7))) | ((_d << 7));

    *(uint32_t*)buf1 = (uint32_t)(b + 6);
    *(uint32_t*)&buf1[4] = r;
    data_len = 8;

    init_header(obuf,CMD_ENABLE_LDP2,data_len);
    memcpy(obuf + sizeof(*pheader), buf1, data_len);

    ret = send(obuf, data_len + sizeof(*pheader), ibuf, &resp_len);


    *(uint32_t*)buf1 = (uint32_t)(b + 7);
    *(uint32_t*)&buf1[4] = ~r;

    init_header(obuf,CMD_ENABLE_LDP2,data_len);
    memcpy(obuf + sizeof(*pheader), buf1, data_len);

    ret = send(obuf, data_len + sizeof(*pheader), ibuf, &resp_len);

    switch (ldp_status)
    {
        case 0x00:
            status_LDP= LDP_OFF;
            break;
        default:
            status_LDP = LDP_ON;
            break;

    }

    return ret;
}

int cmd::ldp_get(enum LDPStatus &status)
{
    switch (status)
    {
        case LDP_OFF:
            cout << "LDP is off." << endl;
            break;
        case LDP_ON:
            cout << "LDP is on." << endl;
            break;
        default:
            cout << "LDP is unknown." << endl;
            break;
    }

    return status;

}

int cmd::emitter_set(bool emitter_status)
{
    int ret;

    uint16_t data_len;
    uint16_t resp_len;

    ret = open();
    if (ret < 0)
        return false;

    data_len = 2;

    ret = init_header(req_buf, CMD_ENABLE_EMITTER, data_len);
    if (ret)
    {
        cout << "init header of Emitter failed" << endl;
        return ret;
    }

    if (emitter_status == true)
    {
        req_buf[8] = 0x01;
        req_buf[9] = 0x00;
    }
    if (emitter_status == false)
    {
        req_buf[8] = 0x00;
        req_buf[9] = 0x00;
    }

    ret = send(req_buf, CMD_HEADER_LEN + data_len, resp_buf, &resp_len);

    if (ret < 0)
    {
        cout << "send Emitter failed" << endl;
    }

    return ret;
}

int cmd::emitter_get(enum LaserStatus &status)
{
    int ret;
    uint16_t data_len;
    uint16_t resp_len;

    ret = open();
    if (ret < 0)
        return false;

    data_len = 0;

    ret = init_header(req_buf, OPCODE_GET_LASER_STATUS, data_len);
    if (ret)
    {
        cout << "init header of EmitterGet failed" << endl;
        return ret;
    }
    ret = send(req_buf, CMD_HEADER_LEN + data_len, resp_buf, &resp_len);
    if (ret >= 0)
    {
        switch (req_buf[8])
        {
            case 0x00:
                status = LASER_OFF;
                cout << "emitter is off." << endl;
                break;
            case 0x01:
                status = LASER_ON;
                cout << "emitter is on." << endl;
                break;
            default:
                status = LASER_UNKNOWN;
                cout << "emitter is unknown." << endl;
                break;
        }
    }
    else
    {
        cout << "send EmitterGet failed" << endl;
    }

    return ret;
}

int cmd::ir_flood_set(bool status)
{
    int ret;
    uint16_t data_len;
    uint16_t resp_len;

    ret = open();
    if (ret < 0)
        return false;

    data_len = 2;

    ret = init_header(req_buf, OPCODE_ENABLE_FLOOD_LED, data_len);
    if (status == true)
    {
        req_buf[8] = 0x01;
        req_buf[9] = 0x00;
    }
    if (status == false)
    {
        req_buf[8] = 0x00;
        req_buf[9] = 0x00;
    }

    ret = send(req_buf, CMD_HEADER_LEN + data_len, resp_buf, &resp_len);
    if (ret != 0)
    {
        cout << "send IrFlood failed" << endl;
    }

    return ret;
}

int cmd::ir_flood_get(enum IrFloodLedStatus &status)
{
    int ret;
    uint16_t data_len;
    uint16_t resp_len;

    ret = open();
    if (ret < 0)
        return false;

    data_len = 0;

    ret = init_header(req_buf, OPCODE_GET_FLOOD_LED_STATUS, data_len);
    if (ret)
    {
        cout << "init header of EmitterGet failed" << endl;
        return ret;
    }

    ret = send(req_buf, CMD_HEADER_LEN + data_len, resp_buf, &resp_len);


    if (ret >= 0)
    {
        switch (req_buf[8])
        {
            case 0x00:
                status = IR_LED_OFF;
                cout << "Ir Flood Led is off." << endl;
                break;
            case 0x01:
                status = IR_LED_ON;
                cout << "Ir Flood Led is on." << endl;
                break;
            default:
                status = IR_LED_UNKNOWN;
                cout << "Ir Flood Led is unknown." << endl;
                break;
        }
    }
    else
    {
        cout << "send IrFloodGet failed" << endl;
    }

    return ret;
}

int cmd::ir_exposure_set(uint32_t val)
{
    int ret;
    uint16_t data_len;
    uint16_t resp_len;

    ret = open();
    if (ret < 0)
        return false;

    data_len = 2;
    ret = init_header(req_buf, CMD_EXP_SET, data_len);
    if (ret)
    {
        cout << "init header of set_exp failed" << endl;
        return ret;
    }

    req_buf[8] = val;
    req_buf[9] = (val & 0xff00) >> 8;
    ret = send(req_buf, CMD_HEADER_LEN + data_len, resp_buf, &resp_len);
    if (ret<0)
		cout << "send cmd set_exp failed" << endl;
    return ret;

}

int cmd::ir_exposure_get(uint32_t &val)
{
    int ret;
    uint16_t data_len;
    uint16_t resp_len;

    LOGE("ir_exposure_get val=0x%x\n", val);

    ret = open();
    if (ret < 0)
        return false;

    data_len = 0;
    ret = init_header(req_buf, CMD_IR_EXPOSURE_GET, data_len);
    if (ret)
    {
        cout << "init header of ir_exposure_get failed" << endl;
        return ret;
    }
    ret = send(req_buf, CMD_HEADER_LEN + data_len, resp_buf, &resp_len);

    if (ret >= 0)
    {
        val = resp_buf[10] | resp_buf[11] << 8;
    }
    else
    {
        cout << "send ir_exposure_get failed" << endl;
    }

    return ret;
}

#if 0
bool cmd::ir_gain_get(void)
{
    vector<string> cmd_r;
    char *argv_r[4] = {};
    int i;
    XnControlProcessingData I2C;

    argv_r[0] = "i2c";
    argv_r[1] = "read";
    argv_r[2] = "1";
    argv_r[3] = "0x35";
    for (i = 0; i < 4; i++)
    {
        cmd_r.push_back(argv_r[i]);
    }

    m_I2CReg = read_i2c(cmd_r, I2C);

    return true;
}

bool cmd::ir_gain_set(const char* val)
{
    vector<string> cmd_r;
    const char *argv_r[5] = {};
    int i;

    XnControlProcessingData I2C;

    argv_r[0] = "i2c";
    argv_r[1] = "write";
    argv_r[2] = "1";
    argv_r[3] = "0x35";
    argv_r[4] = val;

    for (i = 0; i < 5; i++)
    {
        cmd_r.push_back(argv_r[i]);
    }

    write_i2c(cmd_r, I2C);

    return true;
}
#endif

int cmd::ir_gain_set(uint32_t val)
{
    int ret;
    uint16_t data_len;
    uint16_t resp_len;

    ret = open();
    if (ret < 0)
        return ret;

    data_len = 2;
    ret = init_header(req_buf, CMD_GAIN_SET, data_len);
    if (ret)
    {
        cout << "init header of gain_set failed" << endl;
        return ret;
    }

    req_buf[8] = val;
    req_buf[9] = (val & 0xff00) >> 8;
    ret = send(req_buf, CMD_HEADER_LEN + data_len, resp_buf, &resp_len);
    if (ret<0)
        cout << "send cmd gain_set failed" << endl;

    return ret;
}

int cmd::open()
{
    int res = 0;
    int found = 0;
    int cnt = 0;
    int i = 0;

    res = libusb_init(NULL); // TODO: check res

    cnt = libusb_get_device_list(NULL, &devs);
    if (cnt < 0)
    {
        libusb_free_device_list(devs, 1);
        return -1;
    }

    struct libusb_device_descriptor desc;

    while ((dev = devs[i++]) != NULL)
    {

        res = libusb_get_device_descriptor(dev, &desc);
        if (res < 0)
        {
            printf("failed to get device descriptor");
            libusb_free_device_list(devs, 1);
            return -1;
        }

        printf("VID %04x:PID %04x (bus %d, device %d)\n", desc.idVendor, desc.idProduct,
               libusb_get_bus_number(dev), libusb_get_device_address(dev));

        if ( (desc.idVendor == 0x2bc5) && ( (desc.idProduct>>8)==0x04 ) )
        {
            m_camTag = 0;
            found = 1;
            printf("find device successfully\r\n");
            break;
        }
    }

    if (found == 0)
    {
        printf("find device failed\r\n");
        return -1;
    }

    res = libusb_open(dev, &handle);
    if (res != 0)
    {
        printf("open device failed\r\n");
        return -1;
    }

    return 0;
}

int cmd::send_usb(uint16_t c, void *buf0, uint16_t l1, void *buf1, uint16_t l2)
{
    unsigned int al = 0;

    int res;
    uint8_t ob[0x400];
    uint8_t ib[0x200];
    cam_hdr *chdr = (cam_hdr*)ob;
    cam_hdr *rhdr = (cam_hdr*)ib;

    if (handle == 0) return -1;

    if (l1 & 1 || l1 > (0x400 - sizeof(*chdr))) {
        return -1;
    }

    /*chdr->magic[0] = 0x47;
    chdr->magic[1] = 0x4d;
    chdr->c = c;
    chdr->t = m_camTag;
    chdr->l = l1 / 2;*/

    chdr->magic[0] = 0x47;
    chdr->magic[1] = 0x4d;
    chdr->cmd = c;
    chdr->tag = m_camTag;
    chdr->len = 11 / 2;

    //copy the cmdbuf
    memcpy(ob + sizeof(*chdr), buf0, l1);
    res = libusb_control_transfer(handle,
                                  LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR,
                                  0x00,
                                  0x0000,
                                  0x0000,
                                  (unsigned char *)ob,
                                  l1 + sizeof(*chdr),
                                  1000);

    if (res < 0)
    {
        printf("send failed (%d)\n", res);
        return res;
    }

    do
    {
        al = libusb_control_transfer(handle, LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_VENDOR, 0x00, 0x0000, 0x0000, (unsigned char *)ib, 0x200, 1000);
    } while ((al == 0) || (al == 0x200));

    if (al < (int)sizeof(*rhdr)) {
        return res;
    }
    al -= sizeof(*rhdr);

    if (rhdr->magic[0] != 0x52 || rhdr->magic[1] != 0x42) {
        printf("-1\n");
        return -1;
    }
    if (rhdr->cmd != chdr->cmd) {
        printf("-2\n");
        return -2;
    }
    if (rhdr->tag != chdr->tag) {
        printf("-3\n");
        return -3;
    }
    if (rhdr->len != (al / 2)) {
        printf("-4\n");
        return -4;
    }

    if (al > l2) {
        memcpy(buf1, ib + sizeof(*rhdr), l2);
    }
    else {
        memcpy(buf1, ib + sizeof(*rhdr), al);
    }

    m_camTag++;

    return al;
}
