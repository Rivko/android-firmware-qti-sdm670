/*
 * Copyright (c) 2016-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */



#include <sys/time.h>
#include <vaapi_errors.h>
#include <vaapi_config.h>
#include <VAMUtilities.h>
#include <string>
#include <mutex>

uint64_t getTimeNow() {
    struct timespec res;
    clock_gettime(CLOCK_MONOTONIC, &res);
    return 1000000LL * static_cast<int64_t>(res.tv_sec) + res.tv_nsec / 1000;
}

std::mutex uuid_lock;
uint32_t uuid_count_to_reset = 0;
void CreateUUID(char *uuid_str, size_t uuid_str_size) {
    uint16_t mem[8];

    std::lock_guard<std::mutex> lg(uuid_lock);

    // change the seed after some numbers are generated
    if ((uuid_count_to_reset % 100) == 0) {
        srand(time(0));
        uuid_count_to_reset = 0;
    }
    uuid_count_to_reset++;

    for (size_t i = 0; i < sizeof(mem) / sizeof(uint16_t); i++)
        mem[i] = rand() % 0xFFFF;

    snprintf(uuid_str, uuid_str_size, "%04x%04x-%04x-%04x-%04x-%04x%04x%04x",
        mem[0], mem[1], mem[2], mem[3], mem[4], mem[5], mem[6], mem[7]);
}

bool IsValidUUID(const char *vaapi_id) {
    if (!vaapi_id) return false;
    if (strlen(vaapi_id) != VAAPI_UUID_LEN - 1) return false;
    for (int j = 0; j < 8; j++) {
        for (int i = 0; i < 4; i++, vaapi_id++) {
            if (!isxdigit(*vaapi_id)) return false;
        }
        if (j > 0 && j < 5) {
            if(*vaapi_id != '-') return false;
            vaapi_id++;
        }
    }

    return true;
}

///////////////////////////////////////////////////////
// Calibrition utility - start

int OCMatrixInverse3x3_1(float *mat, float *mat_inv) {
    float a[3][3];
    float *mat_inv1 = mat_inv;
    int i, j;
    float determinant = 0;

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            a[i][j] = mat[i * 3 + j];
        }
    }

    for (i = 0; i < 3; i++) {
        determinant += (a[0][i] * (a[1][(i + 1) % 3] * a[2][(i + 2) % 3] -
                        a[1][(i + 2) % 3] * a[2][(i + 1) % 3]));
    }

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            mat_inv1[i * 3 + j] = ((a[(i + 1) % 3][(j + 1) % 3] *
                                    a[(i + 2) % 3][(j + 2) % 3]) -
                                    (a[(i + 1) % 3][(j + 2) % 3] *
                                     a[(i + 2) % 3][(j + 1) % 3])) /
                                     determinant;
        }
    }

    return 0;
}

int getCalibratedParams(const vaapi_scene_calibration *caliInfo,
                        int64_t *params) {
    float sx2 = 0;
    float sxy = 0;
    float sx = 0;
    float sy2 = 0;
    float sy = 0;
    float sxz = 0;
    float syz = 0;
    float sz = 0;
    float fixed_height_ = 1;

    for (uint32_t i = 0; i < caliInfo->object_size; i++) {
        vaapi_scene_calibration_object object = caliInfo->objects[i];
        float height = object.pos.height * fixed_height_ /
                       object.physical_height;
        sx2 += object.pos.x * object.pos.x;
        sxy += object.pos.x * (object.pos.y + object.pos.height);
        sx += object.pos.x;
        sy2 += (object.pos.y + object.pos.height) *
               (object.pos.y + object.pos.height);
        sy += (object.pos.y + object.pos.height);
        sxz += object.pos.x * height;
        syz += (object.pos.y + object.pos.height) * height;
        sz += height;
    }

    float h_mat[9] = { sx2, sxy, sx, sxy, sy2, sy, sx, sy,
                       static_cast<float>(caliInfo->object_size) };
    float h_inv_mat[9];
    float p[3] = { sxz, syz, sz };
    OCMatrixInverse3x3_1(reinterpret_cast<float *>(h_mat),
                         reinterpret_cast<float *>(h_inv_mat));
    float a = 0;
    float b = 0;
    float c = 0;
    float * abc = reinterpret_cast<float *>(params);

    for (int i = 0; i < 3; i++) {
        a += h_inv_mat[i] * p[i];
        b += h_inv_mat[3 + i] * p[i];
        c += h_inv_mat[6 + i] * p[i];
    }

    abc[0] = a;
    abc[1] = b;
    abc[2] = c;
    return 0;
}
// Calibrition utility - end
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// JSON utilities

std::string getStrFromMetadataFrame(const vaapi_metadata_frame *pM) {
    if (pM == nullptr) {
        return "";
    }

    std::stringstream ss;
    ss << "{ \"timestamp\":" << pM->pts / 1000;

    if (pM->object_num > 0) {
        ss << "," << " \"objects\":[";
    }

    for (uint32_t i = 0; i < pM->object_num; i++) {
        vaapi_object &obj = pM->objects[i];

        if (i > 0) {
            ss << ",";
        }

        ss << " { \"id\":" << obj.id<< ",";
        ss << " \"display_name\":\"" << obj.display_name << "\",";
        ss << " \"type\":" << static_cast<int>(obj.type) << ",";
        ss << " \"confidence\":" << static_cast<int>(obj.confidence) <<",";
        ss << " \"position\":{\"x\":" << obj.pos.x << ",";
        ss << " \"y\":" << obj.pos.y << ",";
        ss << " \"width\":" << obj.pos.width << ",";
        ss << " \"height\":" << obj.pos.height << "}";

        bool isReserveNeeded = false;
        for (uint32_t j = 0; j < VAAPI_RESERVE_ITEM; j++) {
            isReserveNeeded |= obj.reserve[j];
        }
        if (isReserveNeeded) {
            ss << ", \"reserve\":[" << obj.reserve[0];
            for (int j = 1; j < VAAPI_RESERVE_ITEM; j++) {
                ss << ", " << obj.reserve[j];
            }
            ss << "]";
        }

        isReserveNeeded = false;
        for (uint32_t j = 0; j < VAAPI_RESERVE_ITEM; j++) {
            isReserveNeeded |= strlen(obj.reserve_str[j]);
        }
        if (isReserveNeeded) {
            ss << ", \"reserve_str\":[\"" << obj.reserve_str[0] << "\"";
            for (uint32_t j = 1; j < VAAPI_RESERVE_ITEM; j++) {
                ss << ", \"" << obj.reserve_str[j] << "\"";
            }
            ss << "]";
        }

        ss << " }";
    }

    if (pM->object_num > 0) {
        ss << " ]";
    }
    ss << " }";

    return ss.str();
}

std::string getStrFromEvent(const vaapi_event *pE) {
    if (pE == nullptr) {
        return "";
    }

    std::stringstream ss;
    ss << "{";
    ss << " \"id\":\"" << pE->id << "\",";
    ss << " \"type\":" << pE->type << ",";
    ss << " \"timestamp\":" << pE->pts << ",";
    ss << " \"rule_id\":\"" << pE->rule_id << "\",";
    ss << " \"objects\":[";
    ss << " { \"id\":" << pE->obj.id << ",";
    ss << " \"display_name\":\"" << pE->obj.display_name << "\",";
    ss << " \"type\":" << pE->obj.type << ",";
    ss << " \"confidence\":" << static_cast<int>(pE->obj.confidence) <<",";
    ss << " \"position\":{ \"x\":" << pE->obj.pos.x << ",";
    ss << " \"y\":" << pE->obj.pos.y << ",";
    ss << " \"width\":" << pE->obj.pos.width << ",";
    ss << " \"height\":" << pE->obj.pos.height << "}";

    bool isReserveNeeded = false;
    for (uint32_t j = 0; j < VAAPI_RESERVE_ITEM; j++) {
        isReserveNeeded |= pE->obj.reserve[j];
    }
    if (isReserveNeeded) {
        ss << ", \"reserve\":[";
        for (uint32_t i = 0; i < VAAPI_RESERVE_ITEM; i++) {
            ss << pE->obj.reserve[i];
            if (i != VAAPI_RESERVE_ITEM - 1) {
                ss << ", ";
            }
        }
        ss << "]";
    }

    isReserveNeeded = false;
    for (uint32_t j = 0; j < VAAPI_RESERVE_ITEM; j++) {
        isReserveNeeded |= strlen(pE->obj.reserve_str[j]);
    }
    if (isReserveNeeded) {
        ss << ", \"reserve_str\":[";
        for (uint32_t i = 0; i < VAAPI_RESERVE_ITEM; i++) {
            bool isPrintable = true;
            for (uint32_t j = 0; j < VAAPI_NAME_LEN &&
                                 pE->obj.reserve_str[i][j] != '\0'; j++) {
                if (isprint(pE->obj.reserve_str[i][j]) == false) {
                    isPrintable = false;
                }
            }

            if (isPrintable) {
                ss << " \"" << pE->obj.reserve_str[i];
            } else {
                ss << " \"[UN-PRINTABLE]";
            }

            if (i != VAAPI_RESERVE_ITEM - 1) {
                ss << "\", ";
            }
        }
        ss << "\"]";
    }
    ss << " } ]";

    isReserveNeeded = false;
    for (uint32_t j = 0; j < VAAPI_RESERVE_ITEM; j++) {
        isReserveNeeded |= pE->reserve[j];
    }
    if (isReserveNeeded) {
        ss << ", \"reserve\":[";
        for (uint32_t i = 0; i < VAAPI_RESERVE_ITEM; i++) {
            ss << pE->reserve[i];
            if (i != VAAPI_RESERVE_ITEM - 1) {
                ss << ", ";
            }
        }
        ss << "]";
    }

    isReserveNeeded = false;
    for (uint32_t j = 0; j < VAAPI_RESERVE_ITEM; j++) {
        isReserveNeeded |= strlen(pE->reserve_str[j]);
    }
    if (isReserveNeeded) {
        ss << ", \"reserve_str\":[";
        for (uint32_t i = 0; i < VAAPI_RESERVE_ITEM; i++) {
            bool isPrintable = true;
            for (uint32_t j = 0; j < VAAPI_NAME_LEN &&
                                 pE->reserve_str[i][j] != '\0'; j++) {
                if (isprint(static_cast<int>(pE->reserve_str[i][j])) == false) {
                    isPrintable = false;
                }
            }

            if (isPrintable) {
                ss << " \"" << pE->reserve_str[i];
            } else {
                ss << " \"[UN-PRINTABLE]";
            }

            if (i != VAAPI_RESERVE_ITEM - 1) {
                ss << "\", ";
            }
        }
        ss << "\"]";
    }
    ss << " }";

    return ss.str();
}

std::string getStrFromRule(const vaapi_rule *pR) {
    if (pR == nullptr) {
        return "";
    }

    std::stringstream ss;
    ss << "{ \"atomic_rules\":";
    ss << "[{";
    ss << " \"id\":\"" << pR->id << "\"";
    ss << ", \"name\":\"" << pR->name << "\"";
    ss << ", \"event_type\":" << pR->type;
    ss << ", \"sensitivity\":" << (uint32_t)pR->sensitivity;
    ss << ", \"min_size\":" << (uint32_t)pR->minimum_size;
    ss << ", \"scene_type\":" << pR->scene_type;

    ss << ", \"reserve\":[";
    for (uint32_t i = 0; i < VAAPI_RESERVE_ITEM; i++) {
        ss << pR->reserve[i];
        if (i != VAAPI_RESERVE_ITEM - 1) {
            ss << ", ";
        }
    }
    ss << "]";

    ss << ", \"reserve_str\":[";
    for (uint32_t i = 0; i < VAAPI_RESERVE_ITEM; i++) {
        bool isPrintable = true;
        for (uint32_t j = 0; j < VAAPI_NAME_LEN &&
                             pR->reserve_str[i][j] != '\0'; j++) {
            if (isprint(pR->reserve_str[i][j]) == false) {
                isPrintable = false;
            }
        }

        if (isPrintable) {
            ss << " \"" << pR->reserve_str[i] << "\"";
        } else {
            ss << " \"[UN-PRINTABLE]\"";
        }

        if (i != VAAPI_RESERVE_ITEM - 1) {
            ss << ", ";
        }
    }
    ss << "]";

    if (pR->zone_size) {
        ss << ", \"zones\":[";
        for (uint32_t i = 0; i < pR->zone_size; i++) {
            ss << "\"" << pR->zones[i].id << "\"";

            if (i != pR->zone_size - 1) {
                ss << ", ";
            }
        }
        ss << "]";
    }

    ss << "}]";

    if (pR->zone_size) {
        ss << ", \"zones\":[";
        for (uint32_t i = 0; i < pR->zone_size; i++) {
            ss << "{";
            ss << "\"id\":\"" << pR->zones[i].id << "\",";
            ss << "\"type\":" << pR->zones[i].zone_type << ",";
            ss << "\"points\":[";
            for (uint32_t j = 0; j < pR->zones[i].point_size; j++) {
                ss << "{";
                ss << "\"x\":" << pR->zones[i].points[j].x << ",";
                ss << "\"y\":" << pR->zones[i].points[j].y << "}";
                if (j != pR->zones[i].point_size - 1) {
                    ss << ", ";
                }
            }
            ss << "]";
            ss << "}";
            if (i != pR->zone_size - 1) {
                ss << ", ";
            }
        }
        ss << "]";
    }

    ss << "}";

    return ss.str();
}

std::string getStrFromEnrollInfo(const vaapi_enrollment_info *pEnroll)
{
    if (pEnroll == nullptr) {
        return "";
    }

    std::stringstream ss;
    ss << "{";
    ss << " \"feature_id\":\"" << pEnroll->id << "\"";
    ss << ", \"image_id\":\"" << pEnroll->img_id << "\"";
    ss << ", \"display_name\":\"" << pEnroll->display_name << "\"";
    ss << ", \"object_type\":" << (uint32_t)pEnroll->type;
    ss << ", \"image_format\":" << (uint32_t)pEnroll->img_format;
    for (int i = 0; i < 3; i++) {
        ss << ", \"plane" << i+1 <<"_width\":" << pEnroll->img_width[i];
        ss << ", \"plane" << i+1 <<"_height\":" << pEnroll->img_height[i];
        ss << ", \"plane" << i+1 <<"_stride\":" << pEnroll->img_pitch[i];
    }
    ss << "}";

    return ss.str();
}

// JSON utilities
///////////////////////////////////////////////////////
