#include <sys/time.h>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include <array>

#include "HiRACS.h"
#include "Focuser.h"
#include "signalHandler.h"
#include "CameraException.h"

#define PORT 8888

HiRACS::HiRACS() {
    int opt = true;
    const int max_clients = 5;
    struct sockaddr_in address;
    SignalHandler _signalHandler;

    // register signal handler for ctrl-c
    _signalHandler.setupSignalHandlers();
  
    // Create a socket to receive connections from HiRACS GUI
    if ((_masterSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        throw CameraException("Failed to create socket");
    }

    // Set socket to allow multiple connections
    if (setsockopt(_masterSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) <0) {
        throw CameraException("setsockopt failed");
    }

    // Set the socket details
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the port
    if (bind(_masterSocket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        throw CameraException("Bind failed");
   }

    std::cout << "HiRACS Command Processor listening on port " << PORT << std::endl;

    // Set the max number of pending connections to 3
    if (listen(_masterSocket, 3) < 0) {
        throw CameraException("Failure on listen");
        exit(EXIT_FAILURE);
    }

    // create instance of focuser
    Focuser _wfovFocuser(1);
}

char * HiRACS::prepareResponse(string resp) {
    char * newVal = new char[resp.size() + 1];
    std::copy(resp.begin(), resp.end(), newVal);
    newVal[resp.size()] = '\0';

    return newVal;
}

void HiRACS::getCommands() {
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int max_sd;
    int activity;
    int new_socket;
    char buffer[1025];
    string response;
    char * retval;

    std::cout << "Waiting for connection requests" << std::endl;

    // Wait for connections
    while(!_signalHandler.gotExitSignal()) {
        // clear the socket set
        FD_ZERO(&_readfds);

        // add master socket to set
        FD_SET(_masterSocket, &_readfds);
        max_sd = _masterSocket;

        // add child sockets to fd set
        for (int sd, i = 0; i < _max_clients; i++) {
            // socket descriptor
            sd = _client_socket[i];

            // add it to the set if it is valid
            if (sd > 0) {
                FD_SET(sd, &_readfds);
            }

            // highest fd needed later for select
            if (sd > max_sd) {
                max_sd = sd;
            }
        }

        // Wait for request. Will wait indefinitely.
        activity = select(max_sd + 1, &_readfds, NULL, NULL, NULL);
        if ((activity < 0) && (errno != EINTR)) {
            std::cout << "Select error" << std::endl;
        }

        // If interrupt was on master socket, we have an incoming connection request
        if (FD_ISSET(_masterSocket, &_readfds)) {
            if ((new_socket = accept(_masterSocket, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
                throw CameraException("Accept error on new connection");
            }

            // log socket number and client details
            std::cout << "New Connection, socket fd = " << new_socket << " ip = " 
                << inet_ntoa(address.sin_addr) << " port = " << ntohs(address.sin_port) << std::endl;

            // add new socket to fd set
            for (int i = 0; i < _max_clients; i++) {
                // if position is empty
                if (_client_socket[i] == 0) {
                    _client_socket[i] = new_socket;
                    // std::cout << "Added fd to set at " << i << std::endl;
                    break;
                }
            }
        }

        // If not on the master, it's an I/O operation on another socket
        for (int valread, sd, i = 0; i < _max_clients; i++) {
            sd = _client_socket[i];
            if (FD_ISSET(sd, &_readfds)) {
                // check for incoming message or closing
                if ((valread = read(sd, buffer, 1024)) == 0) {
                    // a disconnect
                    getpeername(sd, (struct sockaddr*)&address, (socklen_t *)&addrlen);
                    std::cout << "Host disconnected: ip = " << inet_ntoa(address.sin_addr) << " port = "
                        << ntohs(address.sin_port) << std::endl;
                    close(sd);
                    _client_socket[i] = 0;
                }
                else {
                    // process HiRACS command
                    buffer[valread] = '\0';
                    // std::cout << "Received: " << buffer << std::endl;
                    response = _wfovFocuser.lynxCmd(buffer);
                    retval = prepareResponse(response);
                    // std::cout << "Sending back: " << retval << std::endl;
                    if ((send(sd, retval, strlen(retval), 0)) < 0) {
                        std::cout << "Error on send" << std::endl;
                    }
                }
            }
        }
    }
    // should only get here if ^C is detected
    return;
}

int main( int argc, char **) {
    // create instance of HiRACS
    try {
        HiRACS hCam = HiRACS();
        // enter the command loop
        hCam.getCommands();
    }
    catch(CameraException e) {
        std::cout << "Exception while creating or using camera instance: " << e.what() << std::endl;
    }

   // terminated by signal
    std::cout << "HiRACS terminating" << std::endl;
    return 0;
}
