# grid-radio-control
## NOTE: raspberrypi04 has damagaed pins 17/27 and it uses 23/24. This was done manually
Code to use the Raspberry Pi as a reconfigurable antenna and RF swith controller, the OS installed is 2019-04-08-raspbian-stretch-full.

This code can be used without having to physically ssh into the Raspberry pi, an example of this would be the following code (sends socket command to socket port *PORT* and code *CODE* to that port):

`sshpass -p kapilrocks ssh -oStrictHostKeyChecking=no  -X pi@raspberrypi1 "python /home/pi/grid-antenna-control/python/client_cli.py localhost *PORT* *CODE*" > /dev/null 2>&1 &`

## `rpi_control.sh`

This automates the control of raspberry pis:

      ./rpi_test.sh [-a] [-r <raspberryXX>] -p <808Y> -m <0-5>

        -l | -list : list all available RPis
        -r | --rpi : rx grid node
        -p | --port : port [8080 - RALA, 8081 - RFSwitch]
        -m | --mode : mode [0-5 for RALA, 1-4 for RFswitch]
        -a | --all : if this is given, the selected mode will
                       be sent to the selected port of all RPis
        -h | --help : to display this help

        HINT: Either control one Raspberry pi at a time or all of them.
        If configuring all, make sure no one else is running a experiment

examples:

      # reset all RPis to use the conventional antennas
      ./rpi_control.sh -a --port 8081 --mode 2

      # set RPi 1 to use RALA in directional mode 2
      ./rpi_control.sh -r raspberrypi01 -p 8081 -m 1  # Set Rfswitch to use RALA as desired antenna
      ./rpi_control.sh -r raspberrypi01 -p 8080 -m 2  # Set RALA to mode 2

#### How to use this in python

The following would toggle automatically the states of the antenna, could be use concurrently with dragon radio or integrated on the code itself:

      """ This code randomly selects the RALA state"""
      import os
      import random
      from time import sleep

      rpi = "raspberrypi01"
      interval = 0.5 # Amount of seconds to sleep

      myCmd = './rpi_control.sh --rpi '+rpi+' --port 8081 --mode 1' # Select RALA on the RFSwitch
      os.system(myCmd)

      # Randomly change state for RALA (0 - omni, 1-4 directional)
      while True:
          mode = random.randint(0,4) # Random between 0 and 4 both included

          # Send the command
          myCmd = './rpi_control.sh --rpi '+rpi+' --port 8080 --mode '+str(mode)
          os.system(myCmd)

          # Wait interval amount of seconds
          sleep(interval)

      
## Server side

* `rala_control.cc`: This file is the Alford Loop controller.
* `sp4t_control.cc`: This file is the SP4T controller.

To compile each one of these, run the following (wiringPi needs to be installed):

* `g++ rala_control.cc -o rala_control -l wiringPi`
* `g++ sp4t_control.cc -o sp4t_control -l wiringPi`

The controllers are set up so that on boot up they are ran as a cron job (explained at the end). If something doesn't seem to be working, first thing to do is check the the controllers are running: `ps aux | grep "grid"`. This should return two results, each one corresponding to each one of the controllers. The next step is to check hardware connections, see that all pins are the proper ones and if none of this is successful, it's most likely that the antenna or RFswitch are either not connected properly or damaged.

## Client code

The Raspberry has two socket servers listening, one for the RFswitch and another one for the PRA. To control those, one needs to talk to the sockets. The following is a description of multiple versions available.

### C++ version

* `client_cli.c`: This file is a simple client socket, shows how to send a single command to the rala and sp4t server sockets using the command line interface (CLI) and then closes the socket connection. Run this to compile `g++ client_cli.c -o client_cli`. To use it: `./client_cli dest_IP port_num state`

### Python version

* `client_cli.py`: simple client, will have to be run every time we want to make a change, recommended if we just need to set up a particular state for an antenna or the RF Switch and nothing else. To run it: `python3 client_cli.py pihostname 808X state`.
* `client_cli_continuous.py`: same functionality, but it's a streaming socket so that you can keep sending states at any pace needed. Recommended if we want to interact with the radios as we are running them.

## Notes on the server sockets

### `RALA_CONTROL.CC` runs on port 8080

Unless the user changes the pins on the code, the Alford loop has to be connected to the Raspberry Pi 3 Model B as follows:

RALA | Raspberry Pi
------ | ------
1 | GPIO26
2 | GPIO20
3 | GPIO19
4 | GPIO16
GND | GND

When this controller is ran, the Omnidirectional mode (0) is enabled by default. A server socket is created on port 8080 and it will listen for any connection. The available states that can be sent to this server are:

| Code          | State           |
| ------------- |:---------------:|
| 0             | Omnidirectional |
| 1             | Direction 1     |
| 2             | Direction 2     |  
| 3             | Direction 3     |  
| 4             | Direction 4     |  
| 5             | Off mode        |  


## `SP4T_SWITCH.CC` runs on port 8081

Unless the user changes the pins on the code, the SP4T has to be connected to the Raspberry Pi 3 Model B as follows:

SP4T | Raspberry Pi
------ | ------
A      | GPIO17
B      | GPIO27
Vcc    | 3V3
GND    | GND

When this controller is run, the RF port 1 (1) is enabled by default. A server socket is created on port 8081 and it will listen for any connection. The available states that can be sent to this server are:

| Code          | State           | State           |
| ------------- |:---------------:|:---------------:|
| 1             | RF PORT 1       | RALA            |
| 2             | RF PORT 2       | Omnidirectional |
| 3             | RF PORT 3       | DYSE            |
| 4             | RF PORT 4       | RF Matrix       |  


## How to setup a Raspberry Pi from an existing one

1. Insert on a USB adaptor the empty (formated ext4) micro sd card of same size (32GB samsung evo select, green and white).

2. Use the copy sd card tool on the raspberry (only available on the GUI mode, which all raspberry's have  available).

3. Wait till it is done and insert the card on the new raspberry.

4. Change the hostname to match the grid node it is connected to and done (g.e. raspberrypi04)

## How to setup a new Raspberry Pi from scratch

1. Clone the repository on ~/ == /home/pi

      `cd
      git clone https://github.com/dwsl/grid-antenna-control.git`

2. Set a cron job to start the controllers on startup

        % Make sure the startup script can be executed
        chmod +x /home/pi/grid-antenna-control/startup.sh
        crontab -e
        % Add the following to the crontab list
        @reboot /home/pi/grid-antenna-control/startup.sh

3. Compile the cpp code:

      `cd ~/grid-antenna-control/cpp
      g++ rala_control.cc -o rala_control -l wiringPi
      g++ sp4t_control.cc -o sp4t_control -l wiringPi`

4. Change the hostname of the pi, so that you can ssh in it. Also enable SHH

      `sudo raspi-config # Change the keyboard and locale too`

5. Reboot the system and test everything works

      `sudo shutdown -r`

6. Check the controllers are running:

      `ps aux | grep "grid" # Should see both controllers`

7. Connect all the hardware and see if the alford loop actually changes states when told to do so. For this use either the client_cli on C or on python, it doesn't make any difference.

Some errors may occur, the most common ones are:

* Depending on how Raspbian is installed, wiringPi might not be available by default and it need to be installed.
* Make sure the hostname is consistent with the rest of Raspberries on the grid and the radio that the Pi is connected to.

# TODO

* Script to mass handle all Raspberry pi's

## PINOUT FOR REFERENCE

![Pinout](http://wiki.sunfounder.cc/images/9/95/Pi3_gpio.png)
