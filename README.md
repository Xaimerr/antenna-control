# grid-radio-control
This repository has the code neccesary to use the Raspberry Pi as a reconfigurable antenna controller and the RF swith to decide what each radio is connected to (DYSE, conventional antenna, reconfigurable antenna or switch matrix):

This code can be ran without having to physically ssh into the Raspberry pi, one could ran the following:

`sshpass -p kapilrocks ssh -X pi@raspberrypi1 "python /home/pi/grid-antenna-control/python/client_cli.py localhost 8081 RFPORT" > /dev/null 2>&1 &`

## CPP Code

* `rala_control.cc`: This file is the Alford Loop controller.
* `sp4t_control.cc`: This file is the SP4T controller.
* `client_cli.c`: This file is a simple client socket, shows how to send a command to the rala and sp4t server sockets using the command line interface (CLI).

To compile each one of these, run the following commands:

* `g++ rala_control.cc -o rala_control -l wiringPi`, if this doesn't work, the wiringPi libraries are most likely not installed.
* `g++ sp4t_control.cc -o sp4t_control -l wiringPi`
* `g++ client_cli.c -o client_cli`

The controllers are setup so that on boot up they are ran as a cron job. If something doesn't seem to be working, first thing to do is check the the controllers are running: `ps aux | grep "grid"`. This should return two results, each one corresponding to each one of the controllers. The next step is to check hardware connections, see that all pins are the proper ones and if none of this is successful, it's most likely that the antenna or RFswitch are damaged (wouldn't be the first time unfortunately).

## Python code

For sending commands, since the radio used for SC2 has a python interface, I've also developed a client in python, the actual controllers I just left on CPP because, if it works... why change it:

* `client_cli.py`: simple client, will have to be run every time we want to make a change, recommended if we just need to set up a particular state for an antenna or the RF Switch and nothing else. To run it: `python3 client_cli.py pihostname 808X state`.

## `RALA_CONTROL.CC`

Unless the user changes the pins on the code, the Alford loop has to be connected to the Raspberry Pi 3 Model B as follows(see figure at the bottom):

RALA | Raspberry Pi
------ | ------
1 | GPIO26
2 | GPIO20
3 | GPIO19
4 | GPIO16
GND | GND

When this controller is run, the Omnidirectional mode (0) is enabled by default. A server socket is created on port 8080 and it will listen for any connection. The available states that can be sent to this server are:

| Code          | State           |
| ------------- |:---------------:|
| 0             | Omnidirectional |
| 1             | Direction 1     |
| 2             | Direction 2     |  
| 3             | Direction 3     |  
| 4             | Direction 4     |  
| 5             | Off mode        |  

To turn the RALA off, you should terminate the program on the terminal pressing Ctrl + C. If you run this process on the background and then kill it the RALA will stay on whatever state it was last.

## `SP4T_SWITCH.CC`

Unless the user changes the pins on the code, the SP4T has to be connected to the Raspberry Pi 3 Model B as follows:

SP4T | Raspberry Pi
------ | ------
A | GPIO17
B | GPIO27
Vcc | 3V3
GND | GND

When this controller is run, the RF port 1 (1) is enabled by default. A server socket is created on port 8081 and it will listen for any connection. The available states that can be sent to this server are:

| Code          | State           | State           |
| ------------- |:---------------:|:---------------:|
| 1             | RF PORT 1       | RALA            |
| 2             | RF PORT 2       | Omnidirectional |
| 3             | RF PORT 3       | DYSE            |
| 4             | RF PORT 4       | RF Switch       |  

## `CLIENT_CLI.C`

Example usage: `./client_cli dest_IP port_num state`

## How to setup a new Raspberry Pi

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

* ~~Add the changes from server.cc to rala and sp4t controllers.~~
* ~~Add python controller so that it can easily be integrated with SC2 Radio~~
* ~~Allow python code to take input from command window, as the client_cli.c code does.~~
* Integrate this with radio, so that we have another knob (antenna state)
