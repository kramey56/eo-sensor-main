#ifndef __CAMERAEXCEPTION_H__
#define __CAMERAEXCEPTION_H__

#include <stdexcept>

using std::runtime_error;

class CameraException : public runtime_error {
    public:
        CameraException(const std::string& _message)
            : std::runtime_error(_message) {

            }
};

#endif