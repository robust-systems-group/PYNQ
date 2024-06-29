/***********************************************************************
 * FILENAME :        PowerMonitor.h
 *
 * DESCRIPTION :
 *       This is the header for the PowerMonitor Class, which defines
 *       the interface for the Minotaur power monitor test platform.
 *       We use this as a high-level driver to collect data from the
 *       INA229 devices on the carrier-board for the minotaur chip.
 *
 * NOTES :
 *       n/a
 *
 * AUTHOR :    Tristen Nollman <tnollman@cs.stanford.edu>
 * START DATE :    08/2023
 ***********************************************************************/
#ifndef POWERMONITOR_H
#define POWERMONITOR_H

#include "INA229.h"
#include "PowerMonitorChannel.h"
#include <atomic>
#include <cstdint>
#include <string>
#include <vector>

/* TODO:
 *      - Forward this process name to the DataLogger, along with a START
 *      and STOP message to signal it when to end logging and close the
 * connection to the PowerMonitor.
 *      - Tell CLI that we have forwarded the app name to DataLogger,
 *      and that we are waiting for the START signal
 */

/*
 * Class:  PowerMonitor
 * --------------------
 *   PowerMonitor manages communication between the labyrinth CLI (via a
 *   Unix socket), the dashboard/DataLogger (on remote machine via TCP socket)
 *   and the PowerMonitorChannel's on the minotaur board.
 *
 *   This class is used to communicate between the CLI on the FPGA and
 *   the remote dashboard/DataLogger. This class uses google protobuf's
 *   to send serialized data to the remote DataLogger.
 */
class PowerMonitor {

public:

  /*
   * Function:  PowerMonitor::PowerMonitor()
   * --------------------
   *   Constructor for PowerMonitor. Sets socket file descritors to -1
   *   so that we know they are not initialized, as well as reserves
   *   the required space for 8 PowerMonitorChannel's
   */
  PowerMonitor();

  /*
   * Function:  PowerMonitor::~PowerMonitor()
   * --------------------
   *   Destructor for PowerMonitor. Closes any open file descriptors
   *   for the two sockets, and calls the PowerMonitorChannel destructor on
   *   any open channels that this PowerMonitor owns.
   */
  ~PowerMonitor();

  /*
   * Function:  PowerMonitor::add_channels
   * --------------------
   *   Adds PowerMonitorChannel's to our channels vector.
   *   Each PowerMonitorChannel represents a different INA229 device.
   *   See PowerMonitorChannel for how that class works.
   *
   *   We allow PowerMonitorChannel to handle any errors with opening
   *   an ina229 device, so this function may silently fail if we fail
   *   to open one of the INA229 devices. The INA229 driver
   *   will log to cerr/perror when this happens.
   */
  void add_channels();


  /* function: PowerMonitor::read_app_name
   * ------------------------------
   * Reads the applciation of interest's name from the LabyrinthCli.
   * This call blocks until we receive the length of the incoming message,
   * and the message itself.
   *
   * Later, we should change this to use poll() so that it waits for
   * the LabyrinthCli to send an applciation of interest. This will still
   * block, but will be much cleaner, and easier to move to non-blocking if we
   * want.
   *
   * return:
   *      - on success: CliData protobuf message serialized as a string
   *      - on failure: empty string, sets this->status to PowerMonErr
   */
  std::string read_app_name();

  /*
   * Function:  PowerMonitor::send_msg
   * --------------------
   *  Sends a protobuf message (serialized as a string) to the
   *  socket specified by connection.
   *
   *  connection: Enum representing either the CLI connection, or
   *  the dashboard connection. Any other value will cause a failure.
   *
   *  s: the serialized protobuf message to send
   *
   *  returns: true on success, false on error with error message
   * printed to cerr
   */
  bool send_msg(int fd,
                const std::string &data);

  /* function: PowerMonitor::forward_app_name
   * ------------------------------
   * Forwards the application of interest message from the CLI to the remote
   * DataLogger so that it may log to the correct file.
   *
   * Params:
   *      - name: name of the application of interest as a serialized CliData
   * protobuf message
   *
   * return:
   *      - true on success, false on failure. Upon failure, sets status to
   *      PowerMonErr.
   */
  bool forward_app_name(const std::string &name);

  /*
   * Function:  PowerMonitor::serialize_stats
   * --------------------
   *  Calls PowerMonitorChannel::update_stats() on each of the
   * PowerMonitor's INA229 channels, and adds these new stats to a fresh
   * protobuf message. Once all channels have been updated, and their data
   * put into a protobuf message (PowerData_PowerChannelData and
   * PowerData), we then calculate a new timestamp for the protobuf
   * message, and finally serialize it to a string.
   *
   *  This function does not directly fail, but
   *
   *  s: the serialized protobuf message to send
   *
   *  returns: true on success, false on error with error message printed
   * to cerr
   */
  std::string serialize_stats();

  /* function: wait_for_stop()
   * ------------------------------
   *  Causes the PowerMonitor to wait for a stop signal (via protobuf
   * ControlData) from the LabyrinthCli. This function will return when a
   * stop has been received.
   *
   * This function is called by a secondary thread (the one not monitoring and
   * sending to the DataLogger). It sets an atomic flag when a stop signal is
   * received so that the monitoring thread knows to stop monitoring.
   */
  bool wait_for_stop();

  /* function: listen_for_app
   * ------------------------------
   * Listens for a connection to the local unix socket. Once a connection is 
   * established, this function will read the incoming app name from the unix 
   * socket, and then forward that to the PowerDataReceiver, and wait for a 
   * stop signal. Once a stop signal is received over the unix socket,
   * the powermonitor will forward it to the PowerDataReceiver to stop the 
   * app specific logging
   *
   *
   * Params:
   *      - remote: string representation of the PowerDataReceiver ip address
   *      - path: string pathnome of the unix socket
   */
  bool listen_for_app();
  void launch_worker(const char* remote, const char* path);

private:

  int dashboard_fd;
  int serv_fd; // cli unix socket
  int app_fd;
  std::vector<PowerMonitorChannel *> channels;

  /* function: PowerMonitor::send_bytes
   * ------------------------------
   * Writes a byte buffer to the filedescriptor fd. Will only write data_len
   * bytes to fd. Typically, fd is either a unix socket connected to the
   * LabyrinthCli, or a TCP socket connected to the remote DataLogger.
   *
   * Params:
   *      - fd: file-descriptor to write bytes to
   *      - data: buffer of bytes to write to fd
   *      - data_len: number of bytes in data to write
   *
   * return:
   *      - true on success, false otherwise
   */
  bool send_bytes(const int fd, const void *data, const size_t data_len);

  /*
   * Function:  PowerMonitor::read_n_bytes
   * --------------------
   *  Reads n bytes from the fpga_fd TCP Socket and places the data
   *  into buf. This function will attempt to read from the socket until it
   *  successfully reads n bytes or read() errors.
   *  On a failed read() call, this function will log the error to stderr
   *
   *  n: number of bytes to read from the socket.
   *  buf: buffer of at least size n bytes to place read data into.
   *
   *  return: true on successful read, false on error.
   */
  bool read_n_bytes(int fd, uint32_t n,
                    void *buf);

  /* function: connect_to_control
   * ------------------------------
   * Connected the powermonitor to a controller via the unix socket it listens
   * on. 
   *
   * Params:
   *      - path: pathname of the unix domain socket to open and listen on
   *
   * return:
   *      - true if connection successful, false otherwise
   */
    bool connect_to_control(const char* path);

    bool connect_to_logging(const char* remote);
};
#endif // POWERMONITOR_H
