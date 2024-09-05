

// Sockets allow programs to communicate over a network, enabling data 
// exchange between different devices or processes.
// Client-server model
// Sockets support various network protocols, primarily TCP/IP for reliable data transfer.

// we need to add network capabilities to app, allowing it to send and receive data over the internet or local networks.
// server should listen and accept client connections.
// enable handling multiple client connections simultaneously.

////---- initListenSocket:
// Create a socket using socket()
// Set socket options if needed
// Bind the socket to the specified IP and port using bind()
// Start listening for connections with listen()
// Handle any errors that occur during these steps

// Close the listening socket using close() or shutdown()
// Reset any relevant member variables
// Getter methods
// Error handling