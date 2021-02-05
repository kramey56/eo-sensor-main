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
    // Wrapper for the entire camera system. As new components are added, they will be
    // instantiated here.
    private:
        int _masterSocket;
        fd_set _readfds;
        std::array<int, 5> _client_socket = {0};
        const int _max_clients = 5;
        Focuser _wfovFocuser;                       // Wide field-of-view focuser. A separate controller will be added for narrow field
        SignalHandler _signalHandler;
        char * prepareResponse(string);

    public:
        HiRACS();
        void getCommands();
};

#endif
