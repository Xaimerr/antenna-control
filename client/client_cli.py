import socket, sys

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
try:
    clientsocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    clientsocket.connect((destinationIP, port))
    clientsocket.send(state.encode())
except:
    print("USAGE: 'python3 file.py destinationIP port message'")
    print("Unexpected error:", sys.exc_info()[0])   
    #raise # Run if you want mroe details about error
    
