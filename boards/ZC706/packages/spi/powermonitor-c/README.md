# Powermonitor

Powermonitor is the program for reading power data from Minotaur's INA229 devices
on the carrier board. This was moved to C++ once we migrated out INA229 driver from
python to C. 

This application also includes classes for streaming the collected data over a remote 
connection via the PowerDataStreamer class. We use this to collect logs and display 
powerdata to our dashboard on our remote build servers.

## Usage

To use powermonitor, you need to ssh into the minotaur system (one of our FPGAs). Once
you are logged in, you can invoke the powermonitor from the LabyrinthCLI:

'''bash
LabyrinthCLI> powermonitor
'''

The default execution will prompt you for an app name to send to the remote receiver,
which will write all incoming data to a log file named after that app. For example,
if you want to monitor 'generic-test-app', you would enter that into the prompt, and
the remote receiver will log the data to a file named 'generic-test-app.log'.

If you want to skip the prompt, you can provide the app name directly from the CLI:

'''bash
LabyrinthCLI> powermonitor -a <app-name>
''' 

This bypasses the prompt, and directly forwards <app-name> to the receiver. 

By default, powermonitor will only send data over a connection for TIMEOUT seconds, where
TIMEOUT is defined in src/main.cpp. The default TIMEOUT is 5 minutes (300 seconds). You can 
end a logging session early simply by ctrl-C out of the powermonitor.
