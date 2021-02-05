#include <stdio.h>
#include <map>
#include "Focuser.h"

using std::string;
using std::cout;
using std::endl;
using std::stoi;
using std::stof;

Focuser::Focuser(int id, int mhws, float hwss, int sss, int mss, int lss): _maxHwSteps(mhws), _hwStepSize(hwss), 
                                                                           _smallStepSize(sss), _mediumStepSize(mss),
                                                                           _largeStepSize(lss)
{
    const std::string LYNX_IP = "192.168.1.50";
    const unsigned short LYNX_PORT = 9760;
    _focuserSocket = 0;
    struct sockaddr_in serv_addr;

    _header = "";
    if (id == 1) {
        _header = "F1";
    }
    else if (id == 2) {
        _header = "F2";
    }

    if ((_focuserSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        throw FocuserException("Socket creation error");
    }

    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(LYNX_PORT); 

    if(inet_pton(AF_INET, LYNX_IP.c_str(), &serv_addr.sin_addr)<=0)  
    { 
        throw FocuserException("Invalid address/Address not supported"); 
    }

    if (connect(_focuserSocket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        throw FocuserException("Connection Failed"); 
    } 

    // Get status to initialize status data structure
    _getStatus(); 
}

Focuser::~Focuser() {
    close(_focuserSocket);
}

string Focuser::lynxCmd(string cmd) {
    char buffer[1024] = {0};

    string msg = "<" + _header + cmd + ">";
    int n = msg.length();
    char command[n + 1];
    strcpy(command, msg.c_str());
    
    int rc = send(_focuserSocket, command, strlen(command), 0);
    if (rc < 0) {
        throw FocuserException("Error on socket send()");
    }
    
    rc = read(_focuserSocket, buffer, 1024);
    if (rc < 0) {
        throw FocuserException("Error on socket read()");
    }

    string s(buffer);

    return  s;
}

void Focuser::_getStatus() {
    const string msg = "GETSTATUS";

    string resp = lynxCmd(msg);
    
    string::size_type key_pos = 0;
    string::size_type key_end;
    string::size_type val_pos;
    string::size_type val_end;

    while ((key_end = resp.find(" =", key_pos)) != string::npos) {
        if ((val_pos = resp.find_first_not_of("= ", key_end)) == string::npos)
            break;

        val_end = resp.find('\n', val_pos);
        status.emplace(resp.substr(key_pos, key_end - key_pos), resp.substr(val_pos, val_end - val_pos));

        key_pos = val_end;
        if (key_pos != string::npos)
            ++key_pos;
    }

    /* std::map<string, string>::iterator it = status.begin();
 
	// Iterate over the map using c++11 range based for loop
	 for (std::pair<string, string> element : status) {
		// Accessing KEY from element
		string key = element.first;
		// Accessing VALUE from element.
		string value = element.second;
		cout << key << " :: " << value << endl;
	} */
}

int Focuser::getPosition() {
    return stoi(status["Curr Pos"]);
}

int Focuser::getTarget() {
    return stoi(status["Targ Pos"]);
}

float Focuser::getTemp() {
    return stof(status["Temp(C)"]);
}

bool Focuser::isHomed() {
    return (status["IsHomed"] == "1");
}

bool Focuser::isMoving() {
    _getStatus();
    return (status["IsMoving"] == "1");
}

bool Focuser::hasTempProbe() {
    return (status["TmpProbe"] == "1");
}

int Focuser::getMaxHwSteps() {
    return _maxHwSteps;
}

float Focuser::getHwStepSize() {
    return _hwStepSize;
}

vector<int> Focuser::getStepSizes() {
    vector<int> steps = {_smallStepSize, _mediumStepSize, _largeStepSize};
    return steps;
}

/* int main( int argc, char **) {
    try {
        Focuser myFocuser = Focuser(1);
        int pos = myFocuser.getPosition();
        cout << "Position: " << pos << endl;

        cout << "isMoving: ";
        if (myFocuser.isMoving()) {
            cout << "True";
        }
        else 
            cout << "False";
        cout << endl;
    }
    catch (FocuserException& e) {
        cout << e.what() << endl;
        exit;
    }
} */