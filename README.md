Hello and welcome to assignment 3!

Question 1 is related to the Dining Philosopher's problem excercise: Airport Runway Allocation System Scenario Description.

THE SCENARIO: In a busy airport, there are a limited number of runways (3 runways) and a single control tower.
Each airplaine needs to acquire
  -Runway access for either takeoff or landing.
  -Clearance from the conrol tower.

There are 3 phases that need to be follow also. 
1. Deadlock Prone Phase.
2. Deadlock-Free Phase.
3. Starvation Prevention Phase.

There are three files associated to solve this question.

The first file ./Q1 basically covers the deadlock-prone phase. The output will present an airplane (#1) acquiring a runway, another airplane (#2) acuires another runway, and another airplane (#3) acquiring the last runway.
It will be observed that the control tower is locked by Airplane #5 which will be trying to request a runway which is not available. The deadlock detection function will detect and output if a deadlock has occurred. 

This creates a deadlock since it has met the four necessary conditions:
1. Mutual Exclusion: Runways and the control tower are mutually exlusive resources.
2. Hold and Wait: Each airplane holds a resource (the runway) while waiting for another (the control tower).
3. No Preemption: The runway and control tower cannot be forcibly taken from a thread.
4. Circular Wait: Multiple threads form a circular chain of dependencies.

The second file ./Q1_2 has a airplane deadlock free function which basically tries to prevent the deadlock from occuring. Taking a look at the deadlock conditions above it basically prevents the circular wait to occur by making
sure that a consistent order of controller requests first then runway second ensures that the circular dependency cannot form. It also makes sure that the airlpane threads do not hold the runway resource while waiting for the control
tower to respond. 

The output will demonstrate that more threads make progress but will still be held. This means that starvation is occuring and it is a problem for this case. It is related to the Dining Philosophers problem where the longer waiting
threads will be ignored since the new airplanes will always get top priority over the airplanes that have been waiting for a long time. 

To fix the starvation problem ./Q1_3 has a startvation free function which basically gets rid of the the starvation problem. A fair queue methods has been implemented to ensure that each airplane will be given the opportunity to gain access
to the control tower and to have access to the runway. A first in first out is basically occuring in this scenario. The output will show a consistent progress for the airplanes and the above mentioned problems have been removed.

Question 2 is task where I am supposed to create a chat server and the corresponding client programs. The server will act as a message relay between all the connected clients. Each client should be capable of sending messages to
the server, and the server should broadcase the messages to all other connected clients. The server can handle multiple client connections simultanously in a synchronous manner.

The program has to follow the requirements:
1. Server:
   -Accept multiple client connections simultaneously using multithreading.
   -Receive messages from clients and broadcast them to all connected clients (except the server).
   -Handle client disconnections gracefully.
2. Client;
   -Connect to the server.
   -Allow the user to send messages from other clients via the server.

Overview of the server code ./Q2_server: The code listens for incoming client connections. Secondly, it spawns a thread for each client to handle communication independently, 
broadcasts messages from one client to all others, and manages disconnections. The broadcast function sends the message to all clients except the sender. 
The mutex lock makes sure that the clients array is not modified by multiple threads simultaneously. The handle client thread function allows the server to continuously listen for messages from the client using recv. 
If the return from recv is zero then the client has been disconnected. If a client has been disconnected then it is removed from the client array. In the main function a detailed breakdown follows:
	1. Socket Creation: Create a TCP socket (AF_INET for IPV 4, SOCK_STREAM for TCP). If it fails to create a socket then prompt the user.
	2. Binding: Associates the server socket with a specified address (INADDR_ANY) and port (PORT).
	3. Listening: Places the server into listening mode, allowing it to accept up to MAX_CLIENTS simultaneous connections.
	4. Accepting Connections: Wait for any incoming client connections. Return a new socket descriptor for the new client.
	5. Spawn Threads: Each client will have their own thread .
Broadcasting: The last step is to call the broadcast function to send the message to all the other clients.

Overview of the client code ./Q2_client: The client code establishes a connection to the server, creates a separate thread to listen for messages from the server, 
allows the user to type messages and send them to the server, and finally disconnects them when the user enters exit. The receive function is used to listen for messages. 
The recv reads data from the server and stores it in the buffer. Once the server disconnects or an error was found, the client closes the socket and exits. The main function for the client is a lot more simple than the server, it basically has the simple socket and address that creates a TCP socket for communication with the server. The same address setup is followed in the client as the server. The connecting to the server if statement is to attempt a connection with the server at the address and port. If it fails to connect then it terminates itself. A separate thread runs the receive function, this allows the main thread to handle user input while receiving messages from the server. The fgets() function is used in the main thread to read an input from the user. 

The outputs for the 2 clients connecting to the server and sending their inputs have been displayed below the code. One clients said "wuzzup guys" and the other clients says "Hi friends!". 

