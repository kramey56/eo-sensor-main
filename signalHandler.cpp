#include <signal.h>
#include <errno.h>
#include <iostream>

#include "signalHandler.h"

using namespace std;

bool SignalHandler::mbGotExitSignal = false;

SignalHandler::SignalHandler() {

}

SignalHandler::~SignalHandler() {

}

bool SignalHandler::gotExitSignal() {
    return mbGotExitSignal;
}

void SignalHandler::setExitSignal(bool _bExitSignal) {
    mbGotExitSignal = _bExitSignal;
}

void SignalHandler::exitSignalHandler(int _ignored) {
    mbGotExitSignal = true;
}

void SignalHandler::setupSignalHandlers() {
    if (signal((int) SIGINT, SignalHandler::exitSignalHandler) == SIG_ERR) {
        throw SignalException("Error setting up signal handlers");
    }
}