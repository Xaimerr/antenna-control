#!/bin/bash
config_all=0 #
list_all=0

PASSWD=***********
RASPBERRY_LIST=(
    "raspberrypi01"
    "raspberrypi03"
    "raspberrypi04"
    "raspberrypi05"
    "raspberrypi06"
    "raspberrypi07"
    "raspberrypi08"
    "raspberrypi09"
    "raspberrypi10"
    "raspberrypi11"
    "raspberrypi12"
    "raspberrypi13"
    "raspberrypi14"
    "raspberrypi15"
    "raspberrypi16"
    "raspberrypi17"
    "raspberrypi18"
    "raspberrypi19"
)

usage()
{
	echo "INVALID ARGUMENTS..."
    echo ""
    echo "USAGE: ./rpi_test.sh [-a] [-r <raspberryXX>] -p <808Y> -m <0-5>"
    echo ""
    echo "-l | --list : list all available RPis"
    echo "-s | -states : list the modes of all RPis"
    echo "-r | --rpi : rx grid node"
    echo "-p | --port : port [8080 - RALA, 8081 - RFSwitch]"
    echo "-m | --mode : mode [0-5 for RALA, 1-4 for RFswitch]"
    echo "-a | --all : if this is given, the selected mode will "
    echo "               be sent to the selected port of all RPis"
    echo "-h | --help : to display this help"
    echo ""
    echo "HINT: Either control one Raspberry pi at a time or all of them."
    echo "If configuring all, make sure no one else is running a experiment"
    echo ""
    echo "Examples:"
    echo ""
    echo "  # reset all RPis to use the conventional antennas"
    echo "  ./rpi_control.sh -a --port 8081 --mode 2"
    echo ""
    echo "  # set RPi 1 to use RALA in directional mode 2"
    echo "  ./rpi_control.sh -r raspberrypi01 -p 8081 -m 1  # Set Rfswitch to use RALA as desired antenna"
    echo "  ./rpi_control.sh -r raspberrypi01 -p 8080 -m 2  # Set RALA to mode 2"
    echo ""
}

# While there is an input argument check to see which case it is
while (( $# )); do
  case $1 in
<<<<<<< HEAD
		-l|--list) # List all modes
			list_all=1
			break
			;;
		-s|--states) # List all states
			states=1
			break
			;;
=======
	-l|--list) # List all modes
		list_all=1
		break
		;;
	-s|--states) # List all states
		states=1
		break
		;;
>>>>>>> 81c67f66edcb21af56342f1b74ec66d083da90c6
        -r|--rpi) # Raspberry to be configured
			rpis+=($2)
			shift 2
			;;
        -p|--port) # Desired destination port
            ports+=($2)
            shift 2
            ;;
        -m|--mode) # Mode we want to send
            modes+=($2)
            shift 2
            ;;
        -a|--all)  # Configure all RPis
			config_all=1
			shift 1
			;;
        -h|--help) # Print the help
            usage
            exit 0
			;;
		*) 		   # Other cases
			usage
            exit 0
            ;;
  esac
done


# If the user wants to see all available raspberry's
if [ $list_all -eq 1 ]
then 
	echo ""
	echo "The Raspberry Pis available are the following:"
	echo "================================================"
	echo ""
	# Parse output of gridcli to get the numbers and display raspberrypiXX
	for name in $(gridcli -l | sed 1,3d)
	do
		echo "raspberrypi${name//[!0-9]/}"
	done
	exit 0
fi

# If the user wants to see the modes for all raspberry's
<<<<<<< HEAD
if [ $list_all -eq 1 ]
=======
if [ $states -eq 1 ]
>>>>>>> 81c67f66edcb21af56342f1b74ec66d083da90c6
then 
	echo ""
	echo "The Raspberry Pis are configured as follows:"
	echo "================================================"
	echo ""
<<<<<<< HEAD
	for i in $(seq 0 $(( ${#rpis[@]}-1 )))
	do
		sshpass -p "$PASSWD" ssh -oStrictHostKeyChecking=no pi@${rpis[i]}".local" "python /home/pi/grid-antenna-control/python/get_state.py"
=======
	for i in $(seq 0 $(( ${#RASPBERRY_LIST[@]}-1 )))
	do
		echo ""
		echo "${RASPBERRY_LIST[i]}"
		sshpass -p "$PASSWD" ssh -oStrictHostKeyChecking=no pi@${RASPBERRY_LIST[i]}".local" "python /home/pi/grid-antenna-control/python/get_state.py"
>>>>>>> 81c67f66edcb21af56342f1b74ec66d083da90c6
	done
	exit 0
fi

# If the user wants to set all raspberry's to a certain state
if [ $config_all -eq 1 ]
then # Check there is a port and mode given
	if [ "${#ports[@]}" -eq 0 ]
	then
		echo "Add a port and a mode"
		exit 0
	fi

	if [ "${#modes[@]}" -eq 0 ]
	then
		echo "Add a mode"
		exit 0
	fi
	
	# Set all Raspberry Pis to desired state
	for rpi in ${RASPBERRY_LIST[@]}
	do
		echo "Configuring "$rpi" PORT "$ports" to MODE "$modes
		sshpass -p "$PASSWD" ssh -oStrictHostKeyChecking=no pi@$rpi".local" "python /home/pi/grid-antenna-control/python/client_cli.py localhost "$ports" "$modes
	done
	echo "DONE"
    exit 0
fi

if [ "${#rpis[@]}" -eq 0 ]
then
	usage
	exit 0
fi

if [ "${#ports[@]}" -eq 0 ]
then
	usage
	exit 0
fi

if [ "${#modes[@]}" -eq 0 ]
then
	usage
	exit 0
fi


# Tell the user a list of RPis to be configured
echo "RPis to be configured:"
for rpi in ${rpis[@]}
do
    echo $rpi
done
echo ""

#Configure the Raspberry Pis and show as y
echo "Starting configuration..."
for i in $(seq 0 $(( ${#rpis[@]}-1 )))
do
	echo "Configuring "${rpis[i]}" PORT "${ports[$i]}" to MODE "${modes[$i]}
	sshpass -p "$PASSWD" ssh -oStrictHostKeyChecking=no pi@${rpis[i]}".local" "python /home/pi/grid-antenna-control/python/client_cli.py localhost "${ports[$i]}" "${modes[$i]} > /dev/null 2>&1
done
echo "DONE"
exit 0
