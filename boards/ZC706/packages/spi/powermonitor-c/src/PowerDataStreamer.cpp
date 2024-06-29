#include "PowerDataStreamer.h"
#include <arpa/inet.h>
#include <chrono>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <thread> // used for sleep

PowerDataStreamer::PowerDataStreamer(PowerMonitor* pmon, uint32_t port) {
    this->pmon = pmon;
    int serverfd, newsockfd;
    struct sockaddr_in addr;
    int opt = 1;
    socklen_t addrlen = sizeof(addr);

    // create socket fd
    if ((serverfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Failed to aquire socket fd" << std::endl;
        exit(EXIT_FAILURE);
    }

    // attach socket to port 5002
    if (setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
            sizeof(opt))) {
        std::cerr << "Failed to set options for socket." << std::endl;
        exit(EXIT_FAILURE);
    }
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(serverfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Bind failed." << std::endl;
        exit(EXIT_FAILURE);
    }

    // listen for connections
    if (listen(serverfd, 5) < 0) {
        std::cerr << "call to listen() failed." << std::endl;
        exit(EXIT_FAILURE);
    }

    // accept connection
    if ((newsockfd = accept(serverfd, (struct sockaddr*)&addr, &addrlen)) < 0) {
        std::cerr << "Failed to accept incoming connection." << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "Connected!" << std::endl;
    // successfully connected
    clisockfd = newsockfd;
    stopped.store(false);
}

PowerDataStreamer::~PowerDataStreamer() { close(clisockfd); }

bool PowerDataStreamer::send_all(void* data, uint32_t len) {

    // send msg
    auto to_send = len;
    while (to_send > 0) {
        auto sent = send(clisockfd, data + (len - to_send), to_send, 0);
        if (sent < 0) {
            char* error = strerror(errno);
            printf("%s\n", error);
            std::cerr << "[Error] Failed to send message" << std::endl;
            // return false;
            continue;
        }
        to_send -= sent;
    }
    return true;
}

void PowerDataStreamer::stream_data(const struct CLIFlags& options) {
    const auto limit = options.timeout;

    const auto start = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed(0);

    while (elapsed < limit) {
        std::string data = pmon->serialize_stats();
        uint32_t msgSize = data.length();
        uint32_t net_size = htonl(msgSize);
        // send message size first
        if (!send_all((void*)&net_size, sizeof(uint32_t))) {
            // client disconnected
            break;
        }
        // send serialized proto
        if (!send_all((void*)data.data(), msgSize)) {
            // client disconnected
            break;
        }
        
        // sleep for read_freq ms before we update out timer and start the next
        // iteration
        std::this_thread::sleep_for(options.read_freq);

        //  update timer
        const auto curr = std::chrono::steady_clock::now();
        elapsed = curr - start;
    }

    std::cout << "Timer expired, done sending power data." << std::endl;
}

void PowerDataStreamer::stop() { stopped.store(true); }
