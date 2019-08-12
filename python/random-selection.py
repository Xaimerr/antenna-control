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
