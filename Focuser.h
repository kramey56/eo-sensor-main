#ifndef FOCUSER_H
#define FOCUSER_H

#include <map>
#include <vector>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 

using std::map;
using std::vector;
using std::string;

class FocuserException: public std::exception {
    private:
        string msg;

    public:
        FocuserException(string m="Exception in Focuser"):msg(m)
        {

        }

        const string what() {return msg;}
};

class Focuser {
    private:
        map<string, string> status;
        string _header;
        int _focuserSocket;
        const int _maxHwSteps;
        const float _hwStepSize;
        const int _smallStepSize;
        const int _mediumStepSize;
        const int _largeStepSize;
        void _getStatus();

    public:
        enum Speed : int {HIGH=0, LOW=1};

        Focuser(int id=1, int mhws=112000, float hwss=8e-5, int sss=13, int mss=130, int lss=1300);
        ~Focuser();
        void home();
        void center();
        void moveTo(int);
        void moveIn(Speed);
        void moveOut(Speed);
        void halt();
        bool autoFocus();
        int getMaxHwSteps();
        float getHwStepSize();
        vector<int> getStepSizes();
        string lynxCmd(string);

        int getPosition();
        bool isHomed();
        bool isMoving();
        int getTarget();
        bool hasTempProbe();
        float getTemp();
};

#endif