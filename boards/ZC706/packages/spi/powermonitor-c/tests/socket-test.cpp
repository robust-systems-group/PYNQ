/***********************************************************************
 * FILENAME :        socket-test.cpp
 *
 * DESCRIPTION :
 *      Used for testing the socket communication between the powermonitor and
 *      the DataLogger without the need to actually run on the FPGA and have
 *      access to the INA229 devices. Instead, our PowerMonitorChannel class
 *      uses dummy values rather than read from a spi device. This allows us
 *      to test the socket communication of the PowerMonitor on our local
 *      environment.
 *
 * NOTES :
 *
 *
 * AUTHOR :    Tristen Nollman <tnollman@cs.stanford.edu>
 * START DATE :    08/2023
 ***********************************************************************/
#include "INA229.h"
#include "PowerDataStreamer.h"
#include "PowerMonitor.h"
#include "PowerMonitor.pb.h"
#include "PowerMonitorChannel.h"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <ctime>
#include <google/protobuf/text_format.h>
#include <google/protobuf/util/time_util.h>
#include <iostream>
#include <iterator>
#include <netinet/in.h>
#include <stdint.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <thread>
#include <time.h>
#include <unistd.h>
#include <vector>

#define DATA_PORT 5002
#define DEFAULT_TIMEOUT 300
#define DEFAULT_POLL 0

PowerMonitorChannel::PowerMonitorChannel(
    const std::string& channel, uint8_t bus, uint8_t cs) {
    this->channel_name = channel;
    this->device = nullptr;
}

PowerMonitorChannel::~PowerMonitorChannel() { }

bool PowerMonitorChannel::update_stats() { return true; }

double PowerMonitorChannel::get_vbus() { return 1.0; }

double PowerMonitorChannel::get_vshunt() { return 2.0; }

double PowerMonitorChannel::get_current() { return 3.0; }

double PowerMonitorChannel::get_power() { return 4.0; }

std::string PowerMonitorChannel::get_name() { return this->channel_name; }

std::string get_user_input() {
    std::string s;
    getline(std::cin, s);
    std::cout << "\n";
    return s;
}

void print_help() {
    std::cout << "Usage: powermonitor [OPTIONS]" << std::endl;
    std::cout 
        << "OPTIONS:\n -a <app-name>: supply an app name for the remote log. "
            "This bypasses the prompt.\n"
            "-t <seconds>: set the timeout limit. This determines how long the"
            "powermonitor may run before stopping its stream.\n"
            "-f <milliseconds>: set the read frequency in milliseconds. This 
            "controls how much time we wait between each read of the INA devices." 
        << std::endl;
}

void print_welcome() {
    std::cout << "Welcome to the Minotaur Powermonitor.\n" << std::endl;
    std::cout
        << "Powermonitor works by taking an application name from the user "
           "(you), and \nforwarding the app name to a remote receiver. The "
           "receiver then creates a \nlog file for the application, and logs "
           "all "
           "incoming power data to that file.\nWhen you wish to stop power "
           "monitoring for that app, you simply press enter \nhere, which will "
           "send a stop signal to the receiver, and ready the receiver \nand "
           "powermonitor for monitoring another app.\n"
        << std::endl;
}

void forward_app_name(PowerDataStreamer& streamer, std::string app) {
    std::cout << "Sending app name...";

    // Create protobuf message from app name
    ControlData control_data;
    control_data.set_app_name(app);
    control_data.set_start(true);
    std::string tmp;
    control_data.SerializeToString(&tmp);

    // calculate message size
    uint32_t msg_size = tmp.length();
    uint32_t net_size = htonl(msg_size);

    // send msg size
    if (!streamer.send_all((void*)&net_size, sizeof(uint32_t))) {
        exit(EXIT_FAILURE);
    }

    // send msg
    if (!streamer.send_all((void*)tmp.data(), tmp.length())) {
        exit(EXIT_FAILURE);
    }

    std::cout << "Done!" << std::endl;
}

void parse_flags(int argc, char* argv[], struct CLIFlags* options) {
    if (argc < 3) {
        std::cerr << "[Error] Invalid use. Try 'powermonitor -h' for help." << std::endl;
        exit(EXIT_FAILURE);
    }
    
    // check for help flag
    if (argv[1] == "-h") {
        print_help();
        exit(EXIT_SUCCESS);
    }

    // for each arg after process name
    for (int i = 1; i < argc; i++) {
        char* flag = argv[i];
        // check which, if any, of the options the flag represents
        switch flag {
            case "-a":
                options->app_name = argv[i + 1];
                break;
            case "-t":
                // convert char* to integer
                options->timeout = std::chrono::seconds(stoi(argv[i + 1]));
                break;
            case "-f":
                options->read_freq = std::chrono::milliseconds(stoi(argv[i + 1]));
                break;
            default:
                break;
        }
    }
}

struct CLIFlags {
    std::string app_name;
    std::chrono::seconds timeout;
    std::chrono::milliseconds read_freq;
};

int main(int argc, char* argv[]) {
    struct CLIFlags options;
    bool prompt = true;
    // set defaults, will be overwritten if any flags were supplied
    options.timeout = std::chrono::seconds(DEFAULT_TIMEOUT);
    options.read_freq = std::chrono::milliseconds(DEFAULT_POLL);
    
    // check for cli flags
    if (argc > 1) {
        prompt = false;
        parse_flags(argc, argv, &options);
    } else {
        print_welcome();
    }

    // init power channels
    PowerMonitor pmon;
    pmon.add_channels();

    // listen for connection from receiver
    PowerDataStreamer pmon_streamer(&pmon, DATA_PORT);

    if (prompt) {
        std::cout << "Please enter an app name for the remote log file: ";
        options.app_name = get_user_input();
    }
    forward_app_name(pmon_streamer, options.app_name);

    // while client is connected, stream data
    pmon_streamer.stream_data(options);
    return 0;
}
