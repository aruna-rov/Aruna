# Control

The sensor for the closed looped system can be (almost) any MPU controller. Please see [esp-mpu-driver repo](https://github.com/natanaeljr/esp32-MPU-driver) for more information.

## Com

com interface with the control unit.

* Port: `3`
* Priority: `1`

com package layout:

1 byte | 1 byte | 1 byte | 1 byte | 2 bytes
--- | --- | --- | --- | --- 
from port | to port | command | flags | data
`0xXX` | `0x03` | `0xXX` | flag A or B | `0xXXXX` 


### Avaliable commands

#### Get sensor data
get yaw, roll, pitch, x, y, z data from the MPU sensor.

*request*
* command: `0x01`
* flags: A, what sensor data you want (see flags section)
* data: none

*response*

One package gets transmitted for each valid requested flag
* command: `0x01`
* flags: what sensor information is in the data section
* data: data of sensor

#### Set target speed/rotation
Set the wanted speed or rotation using a closed loop system.
The system will do its best to match the target.
Please note that damping rules must be set in order for it to function.

*request*
* command: `0x02`
* flags: A, axis to manipulate
* data: speed (mm/s) or rotation (max rotation is 0xFFFF)

*response*
* nothing...

#### Set speed directly
Use this to set the speed of the control drivers directly (open loop)
damping will have no effect
#### get damping
*request*
* command: `0x10`
* flags: B, what damping to get?

### Flags
Flags can be A or B
#### Flag A: axis
below are the avaliable flags. if a bit is `1` its means that that mode is enabled.
direction can only be read and indicates if the ROV is capable of backward and forward of left and right acceleration (depending on the axis)
``` 
    CONTROL_X = (1 << 0),
    CONTROL_Y = (1 << 1),
    CONTROL_Z = (1 << 2),
    CONTROL_ROLL = (1 << 3),
    CONTROL_YAW = (1 << 4),
    CONTROL_PITCH = (1 << 5),
    CONTROL_DIRECTION_PLUS = (1 << 6),
    CONTROL_DIRECTION_MIN = (1 << 7)
```

#### Flag B: damping