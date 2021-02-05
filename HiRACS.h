#ifndef HIRACS_H
#define HIRACS_H

#include <iostream>
#include <array>
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 

#include "Focuser.h"
#include "signalHandler.h"

using namespace std;

class HiRACS {
    private:
        int _masterSocket;
        fd_set _readfds;
        std::array<int, 5> _client_socket = {0};
        const int _max_clients = 5;
        Focuser _wfovFocuser;
        SignalHandler _signalHandler;
        char * prepareResponse(string);

    public:
        HiRACS();
        void getCommands();
};

#endif