import socket, sys, time

# Error check input arguments, if none asume localhost testing
if len(sys.argv) == 1:
    destinationIP = "localhost"
    port = 8080
    state = '0' # By default send omni configuration
elif len(sys.argv) == 4:
    destinationIP = sys.argv[1]
    port = int(sys.argv[2])
    state = sys.argv[3]
else:
    print("USAGE: 'python3 file.py destinationIP port message'")
    print("ERROR not enough input arguments.")
    exit()
# Send the socket command
while True:
    try:
        clientsocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        clientsocket.connect((destinationIP, port))
        clientsocket.send(state.encode())
        state = input("Desired state? [0 -4]: ") 
    except KeyboardInterrupt:
        print("\nAdios")
        exit()

