/*
 * Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#pragma once
#include <functional>
#include <string>


/* forward declaration of qmi implementation class */
class ssc_qmi_connection;

enum ssc_error_type
{
    SSC_CONNECTION_RESET
};

/**
 * type alias for ssc connection event callback
 */
using ssc_event_cb = std::function<void(const uint8_t *data, size_t size)>;

using ssc_error_cb = std::function<void(ssc_error_type error)>;

using ssc_resp_cb = std::function<void(const uint32_t error_value)>;

/**
 * @brief This class represents a live connection to SSC.
 *
 * All communication to sensors core happens using an instance
 * of this class. Mupliple instances of this class are allowed
 * and can work concurrently in a multi-threaded environment.
 *
 */
class ssc_connection
{
public:
    /**
     * @brief creates a new connection to SSC and registers a
     *        callback funtion for events.
     *
     * @param event_cb register this function as a callback. this
     *                 will be called when an event from ssc is
     *                 received over this connection.
     */
    ssc_connection(ssc_event_cb event_cb);

    /**
     * @brief closes connection to ssc, destroys this object,
     *        de-registers the callback and releases any resources
     *        used by the connection.
     */
    ~ssc_connection();

    /**
     * @brief send an encoded request message to ssc
     *
     * @param pb_req_message_encoded string populated with protobuf
     *                               encoded client request message.
     */
    void send_request(const std::string& pb_req_message_encoded);

    void register_error_cb(ssc_error_cb cb);

    void register_resp_cb(ssc_resp_cb cb);

    void set_unsuspendable_channel();

    /**
     * @brief report indication with out using seperate worker and
     * report in qmi call back itself.
     */
    void use_report_indication();

private:
    std::unique_ptr<ssc_qmi_connection> _qmi_conn;
};
