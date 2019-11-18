#!/usr/bin/python
""" This python code reads the GPIO state of any RPi and based on that 
returns the state of the antenna and the RFswitch

Xaime Rivas Rey 9/30/19"""
import wiringpi
wiringpi.wiringPiSetupGpio() # Set GPIO numbering scheme

# Define rala pins and states
rala_pins = [26, 20, 19, 16]
rala_states = [[1,1,1,1], # Omnidirectional
	       [1,0,0,0], # Direction 1
               [0,1,0,0], # Direction 2
               [0,0,1,0], # Direction 3
               [0,0,0,1], # Direction 4
               [0,0,0,0]] # Off]

# Define RFSwitch pins and states
rfswitch_pins = [17,27]
rfswitch_states = [[0,0], # RFPort 1
                   [1,0], # RFPort 2
                   [0,1], # RFPort 3
                   [1,1]] # RFPort 4

# Get state for RALA
gpio = [wiringpi.digitalRead(pin) for pin in rala_pins]
for i in range(len(rala_states)):
	if rala_states[i] == gpio:
		print("RALA state: "+str(i))
		break

# Get state for RFSwitch
gpio = [wiringpi.digitalRead(pin) for pin in rfswitch_pins]
for i in range(len(rfswitch_states)):
	if rfswitch_states[i] == gpio:
		print("RFSwitch state: "+str(i+1))
		break
