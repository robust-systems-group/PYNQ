/***********************************************************************
 * FILENAME :        PowerMonitor.cpp
 *
 * DESCRIPTION :
 *       This is the definition of the PowerMonitor class. We use this class
 *       to communicate with the INA229 devices on the test carrier-board for
 *       the Minotaur chip.
 *
 * NOTES :
 *       n/a
 *
 * AUTHOR :    Tristen Nollman <tnollman@cs.stanford.edu>
 * START DATE :    08/2023
 ***********************************************************************/
#include "PowerMonitor.h"
#include "PowerMonitor.pb.h"
#include "PowerMonitorChannel.h"
#include <algorithm>
#include <arpa/inet.h>
#include <atomic>
#include <cassert>
#include <cstdint>
#include <ctime>
#include <iostream>
#include <iterator>
#include <netinet/in.h>
#include <stdint.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <vector>

static const std::string CHANNEL0 = "VDD_CORE";
static const std::string CHANNEL1 = "VDD_IO";
static const std::string CHANNEL2 = "VDD_RMACRO";
static const std::string CHANNEL3 = "VDD_RRAM";
static const std::string CHANNEL4 = "VDD_SMACRO";
static const std::string CHANNEL5 = "VDIO_CORE";
static const std::string CHANNEL6 = "VDIO_RMACRO";
static const std::string CHANNEL7 = "VDUH_RMACRO";

PowerMonitor::PowerMonitor() {
    dashboard_fd = -1;
    serv_fd = -1;
    app_fd = -1;
    channels.reserve(8);
}

PowerMonitor::~PowerMonitor() {
    // if sockets are open, close them
    if (dashboard_fd > -1) {
        close(dashboard_fd);
    }
    if (serv_fd > -1) {
        close(serv_fd);
    }
    if (app_fd > -1) {
        close(app_fd);
    }

    // if we have open PowerMonitorChannel's, call their destructor
    if (!channels.empty()) {
        for (PowerMonitorChannel* chan : channels) {
            chan->~PowerMonitorChannel();
        }
    }
}

void PowerMonitor::add_channels() {
    channels.push_back(new PowerMonitorChannel(CHANNEL0, 1, 0));
    channels.push_back(new PowerMonitorChannel(CHANNEL1, 1, 1));
    channels.push_back(new PowerMonitorChannel(CHANNEL2, 1, 2));
    channels.push_back(new PowerMonitorChannel(CHANNEL3, 1, 3));
    channels.push_back(new PowerMonitorChannel(CHANNEL4, 1, 4));
    channels.push_back(new PowerMonitorChannel(CHANNEL5, 1, 5));
    channels.push_back(new PowerMonitorChannel(CHANNEL6, 1, 6));
    channels.push_back(new PowerMonitorChannel(CHANNEL7, 1, 7));
}

void PowerMonitor::launch_worker(const char* remote, const char* path) {
    while (true) {
        // connect to PowerDataReceiver on control socket
        while (!connect_to_logging(remote)) {
            std::cerr << "[Error] failed to connect to logging service"
                      << std::endl;
            sleep(3);
        }
        // connect to controller via unix socket
        while (!connect_to_control(path)) {
            std::cerr << "[Error] Failed to connect to client over unix socket."
                      << std::endl;
            sleep(3);
        }
        // listen and forward app to monitor to PowerDataReceiver
        if (!listen_for_app()) {
            std::cerr << "[Error] listen for app failed." << std::endl;
        }
    }
}

bool PowerMonitor::listen_for_app() {
    // read app name from socket
    std::string app_name = read_app_name();
    if (app_name.empty()) {
        std::cerr << "[Error] Read empty string as name from unix socket."
                  << std::endl;
        return false;
    }

    // forward app name to PowerDataReceiver over port 5003
    if (!forward_app_name(app_name)) {
        std::cerr << "[Error] failed to forward app name" << std::endl;
        return false;
    }

    // wait for a stop signal and propogate it
    if (!wait_for_stop()) {
        std::cerr << "[Error] failed to read stop signal" << std::endl;
        return false;
    }

    // send any byte to make receiver stop
    if (!send_msg(dashboard_fd, "stop")) {
        std::cerr << "[Error] Failed to send stop to receiver." << std::endl;
        return false;
    }

    return true;
}

bool PowerMonitor::connect_to_control(const char* path) {
    struct sockaddr_un addr, client_addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    memcpy(&(addr.sun_path), path, strlen(path));

    serv_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (serv_fd < 0) {
        std::cerr << "[Error] Failed to get socket fd for CLI unix socket."
                  << std::endl;
        return false;
    }

    if (bind(serv_fd, (struct sockaddr*)&addr,
            sizeof(addr.sun_family) + strlen(addr.sun_path))
        < 0) {
        std::cerr << "[Error] failed to bind local socket" << std::endl;

        return false;
    }

    listen(serv_fd, 0);

    socklen_t clilen = sizeof(client_addr);
    app_fd = accept(serv_fd, (struct sockaddr*)&client_addr, &clilen);
    if (app_fd < 0) {
        std::cerr << "[Error] could not accept connection." << std::endl;
        return false;
    }
    return true;
}

bool PowerMonitor::read_n_bytes(int fd, uint32_t n, void* buf) {
    int num_read = 0;
    int res;
    while (num_read < n) {
        res = read(fd, buf + num_read, n - num_read);
        if (res == 0) {
            std::cerr << "[Warning] Got 0 bytes from read" << std::endl;
            continue;
        } else if (res < 0) {
            std::cerr << "[Error] reading " << n << " bytes. Got " << res
                      << std::endl;
            return false;
        }
        num_read += res;
    }
    return true;
}

std::string PowerMonitor::read_app_name() {
    std::string s;
    uint32_t name_len;
    // get length of name packet
    if (!read_n_bytes(app_fd, sizeof(name_len), &name_len)) {
        std::cerr
            << "[Error] Failed reading app name length from labyrinth cli."
            << std::endl;
        return "";
    }
    name_len = ntohl(name_len);
    // make sure they didnt send a huge length value which could overflow
    // the stack here
    assert(name_len < 1024);
    char buf[name_len + 1]; // room for null terminator to be added
    if (!read_n_bytes(app_fd, name_len, &buf)) {
        std::cerr << "[Error] Failed reading app name from Labyrinth CLI"
                  << std::endl;
        return "";
    }
    // append null terminator
    buf[name_len] = '\0';

    // turn raw string bytes into cpp string
    s = (char*)buf;
    return s;
}

bool PowerMonitor::send_bytes(
    const int fd, const void* data, const size_t data_len) {
    // send msg
    auto len = data_len;
    auto to_send = len;
    while (to_send > 0) {
        auto sent = send(fd, data + (len - to_send), to_send, 0);
        if (sent < 0) {
            char* error = strerror(errno);
            printf("%s\n", error);
            std::cerr << "[Error] Failed to send message" << std::endl;
            return false;
        }
        to_send -= sent;
    }
    return true;
}

bool PowerMonitor::send_msg(int fd, const std::string& data) {
    return send_bytes(fd, (void*)data.data(), data.length());
}

bool PowerMonitor::connect_to_logging(const char* remote) {
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(5003);
    // get socket fd
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "[Error] Failed to get socket fd." << std::endl;
        return false;
    }

    // connect to receiver
    if (bind(sockfd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "[Error] failed to bind to receiver control" << std::endl;
        return false;
    }

    listen(sockfd, 5);

    struct sockaddr_in cli_addr;
    socklen_t clilen = sizeof(cli_addr);
    dashboard_fd = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen);
    if (dashboard_fd < 0) {
        std::cerr << "[Error] failed to connect to remote." << std::endl;
        close(dashboard_fd);
        close(sockfd);
        return false;
    }
}

bool PowerMonitor::forward_app_name(const std::string& name) {

    // send message
    if (!send_msg(dashboard_fd, name)) {
        std::cerr << "[Error] Failed to forward app name to DataLogger. Could "
                     "not send name."
                  << std::endl;
        return false;
    }

    return true;
}

std::string PowerMonitor::serialize_stats() {
    PowerData power_data;
    for (PowerMonitorChannel* chan : channels) {
        PowerData_PowerChannelData* power_channel_data
            = power_data.add_channels();
        if (!chan->update_stats()) {
            std::cerr << "Couldnt update channel stats." << std::endl;
        }
        power_channel_data->set_name(chan->get_name());
        power_channel_data->set_bus_voltage(chan->get_vbus());
        power_channel_data->set_shunt_voltage(chan->get_vshunt());
        power_channel_data->set_current(chan->get_current());
        power_channel_data->set_power(chan->get_power());
    }

    // populate message timestamp
    auto ts = new google::protobuf::Timestamp {};
    struct timespec curr_ts;
    memset(&curr_ts, 0, sizeof(struct timespec));
    int res = clock_gettime(CLOCK_REALTIME, &curr_ts);
    if (res) {
        std::cerr << "Failed to get time." << std::endl;
    }
    ts->set_seconds(curr_ts.tv_sec);
    ts->set_nanos(curr_ts.tv_nsec);
    power_data.set_allocated_timestamp(ts);

    std::string s;
    power_data.SerializeToString(&s);
    return s;
}

bool PowerMonitor::wait_for_stop() {
    uint32_t len;
    ControlData msg;

    if (!read_n_bytes(app_fd, sizeof(uint32_t), &len)) {
        std::cerr << "Failed to read message len from CLI." << std::endl;
        return false;
    }
    len = ntohl(len);

    char buf[len + 1];
    if (!read_n_bytes(app_fd, len, (void*)&buf)) {
        std::cerr << "Failed to read message from CLI." << std::endl;
        return false;
    }
    buf[len] = '\0';

    std::string tmp = (char*)buf;
    msg.ParseFromString(tmp);

    // check for stop signal
    // if (!msg.has_stop_monitoring()) {
    //    std::cerr << "Received message from CLI, but doesnt have "
    //                 "stop_monitoring signal"
    //              << std::endl;
    //    return false;
    //}
    // if (!msg.stop_monitoring()) {
    //    std::cerr << "Received message with stop_monitoring signal, but the "
    //                 "signal was false."
    //              << std::endl;
    //    return false;
    //}

    return true;
}
