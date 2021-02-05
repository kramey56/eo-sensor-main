# eo_sensor
Prototype for Stratolyte camera control code.

## Purpose
The HiRACS camera was a planned, high-resolution camera to be mounted on World View's Stratolite balloons. The code
in this repository is the start of the daemon running on the payload computer to control the camera and provide
telemetry back to ground-based operators.

## Structure
The code is currently divided into two major components:
  1. HiRACS
  2. Focuser

### HiRACS
HiRACS receives commands from the ground and routes them to the proper subsystem. It opens up a internet port as a 
listener. When a new connection is requested, it spawns off a new port to handle that connection. The HiRACS code
checks for errors in creating connections, opening ports, receiving data, and closing connections when no longer
needed.

HiRACS also receives responses from camera subsystems and sends them back to the controller.

### Focuser
The Focuser code receives relevent commands from the HiRACS daemon. It formats them into the command syntax required
by the focusing unit and then sends them to it to a pre-defined internet address and port. It also gets the response
from the focusing unit, formats it approriately, and sends it back to the daemon.
