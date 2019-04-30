# Control

## Acceleration and gyroscope
The sensor for the closed looped system can be (almost) any MPU controller. Please see [esp-mpu-driver repo](https://github.com/natanaeljr/esp32-MPU-driver) for more information.

The sensor is not required. The control module will try to find an attached MPU controller and print "MPU functionality disabled." if the controller is not working propely. The following functionality is then disabled:
* Closed loop control (damping, keep velocity or degree etc.)
* get/set velocity or degree

See the com command "activate MPU" and "get MPU status" for more information.

**MPU pinout:**

PIN 	| ESP	| MPU
--- 	| ---	| ---
I²C SDA	| 26	| SDA
I²C CLK	| 25	| CLK
INTR	| 15	| INR
GRND	| GND	| GND
v3.3	| -		| VCC

## Drivers
The control module can have multiple drivers at the same time for diffrent axis.

### L293D
The L293D is a low cost motor driver. Any motors (5v-30v) can be attached.
The control module does not check if the L293D is attached, so it wil always asume it is on.

**L293D pinout:**

PIN		| ESP	| L293D
---		| ---	| ---
IN1		| 13	| 1
IN2		| 12	| 7
IN3		| 14	| 10
IN4		| 27	| 15
motorV (4.5-36)	| -		| 8
interV 5v	| -		| 16
GRND	| GND	| 4, 5, 13 or 12	|

## Com

com interface with the control unit.

* Port: `3`
* Priority: `1`

**com axis package layout:**

1 byte | 1 byte | 1 byte | 1 byte | 2 bytes
--- | --- | --- | --- | --- 
from port | to port | command | axis | data
`0x00` - `0xFF` | `0x03` | `0x00` - `0xFF` | `0x01` - `0x7F`  | `0x0000` - `0xFFFF` 

*all packages use the axis package layout unless stated otherwise*

**com simple package layout:**

1 byte | 1 byte | 1 byte | 1 byte
--- | --- | --- | ---  
from port | to port | command | data
`0x00` - `0xFF` | `0x03` | `0x00` - `0xFF` | `0x00` - `0xFF` 



### axis mask
below are the avaliable axis. if a bit is `1` its means that that mode is enabled.
if direction bit is `0` the axis will go plus (forward for X axis).
``` 
    CONTROL_AXIS_MASK_X = (1 << 0),
    CONTROL_AXIS_MASK_Y = (1 << 1),
    CONTROL_AXIS_MASK_Z = (1 << 2),
    CONTROL_AXIS_MASK_ROLL = (1 << 3),
    CONTROL_AXIS_MASK_YAW = (1 << 4),
    CONTROL_AXIS_MASK_PITCH = (1 << 5),
    CONTROL_DIRECTION = (1 << 6),
```

### Avaliable commands

#### Set speed
Set the speed of the motors directly

*request*
* command: `0x01`
* axis: `0x01` - `0x7F` multiple axis; this applies to and in what direction they need to go (`0x40` is backward)
* data: speed (`0xFFFF` is maximum)

*response*
* none...

this command applies to all axis.
#### Get speed
get the speed of the motors.

*request*
* command: `0x02`
* axis: `0x01` - `0x3F` multiple axis; the requested axis you want the speed of. Direction is not used.
* data: none

*response*
* command: `0x02`
* axis: `0x01` - `0x20` single axis; the axis that the data applies to
* data: `0x0` - `0xFFFF` speed of the motor

for each requested axis a indifitial response is send.

#### Set velocity
Set the wanted speed or rotation using a closed loop system.
The system will do its best to match the target.
Please note that damping rules must be set in order for it to function.

*request*
* command: `0x03`
* axis: `0x01` - `0x7F` multiple axis to manipulate
* data: speed (mm/s) (max is `0xFFFF`)

*response*
* nothing...

#### Get velocity
Get the current velocity (mm/s) of a axis.

*request*
* command: `0x04`
* axis: `0x01` - `0x3F` multiple axis the get the velocity from.
* data: none

*response*
* command: `0x04`
* axis: `0x01` - `0x20` single axis at a time
* data:  max `0xFFFF ` speed of given axis in mm/s

foreach axis in request a indivitual response is send.

#### Set degree
Set the degree of a axis. Please note that only roll, yaw and pitch are supported

*request*
* command: `0x05`
* axis: `0x08` - `0x38` multiple target axis
* data: degree to go to. (`0` - `0xFFFF`); `0xFFFF` represents 360 degrees. And `0x0000` is 0 degrees. Use everything in between.

*response*
* none...

The ROV will rotate in the direction that is the shortest to the target degree for that axis.

#### Get degree
Get the degree of an axis. Note that only roll, yaw and pitch are supported.

*request*
* command: `0x06`
* axis: `0x08` - `0x38` multiple desired axis
* data: none

*response*
* command: `0x06`
* axis: `0x08` - `0x20` axis that corresponds with the data.
* data: `0x00` - `0xFFFF` degree from 0 to 360 degrees.

For each valid axis in the request a new response is send.

#### Get direction
get the direction of a given axis.

*request*
* command: `0x07`
* axis: `0x01` - `0x3F` mulpiple axis to get the direction from
* data: none

*response*
* command: `0x07`
* axis: `0x01` - `0x20` single axis
* data: `0` if direction plus, `1` if direction min

foreach axis in request a seperate response will be send.

#### Set damping
set damping rules

*request*
* command: `0x10`
* axis: `0x01` - `0x3F` desired multiple axis
* data: `0` - `0x02` damping configuration

*response*
* none...

*data damping configuration:*
```
	CONTROL_DAMP_DISABLE = 0,
	CONTROL_DAMP_KEEP_VELOCITY = 0x01,
	CONTROL_DAMP_KEEP_DEGREE = 0x02,
```
* `CONTROL_DAMP_DISABLE` will disable all damping for that axis
* `CONTROL_DAMP_KEEP_VELOCITY` ROV will try to keep the current velocity
* `CONTROL_DAMP_KEEP_DEGREE` ROV will try to keep the current degree (only row, yaw and pitch)

#### Get damping
get the damping rules for a given axis

*request*
* command: `0x11`
* axis: `0x01` - `0x3F` desired multiple axis
* data: none

*response*
* command: `0x11`
* axis: `0x01` - `0x20` one axis at a time
* data: `0` - `0x02` damping configuration

foreach axis in request a seperate response will be send.

#### Set running state
Set the state of the control tasks.

**This command uses the simple com package**

*request*
* command: `0x20`
* data: `0x01` if set to on, `0` to turn off.

*response*
* none...

#### Get running state
get the state of the control tasks.

**This command uses the simple com package**

*request*
* command: `0x21`
* data: none

*response*
* command: `0x21`
* data: if running return `0x01`, else `0`

#### Test sensors
Test the MPU sensor for closedloop feedback

**This command uses the simple com package**

*request*
* command: `0x22`
* data: none

*response*
* command: `0x22`
* data: if test succeeded `0x01`, else `0`

#### Calibrate sensors
Calibrate the sensors (set 0 point). takes about 400ms. So stand still and level at that time.

**This command uses the simple com package**

*request*
* command: `0x23`
* data: none

*response*
* none...

#### Get supported axis
Get a axis mask of all the axis that are supported by this ROV.

*request*
* command: `0x24`
* axis: none
* data: none

*response*
* command: `0x24`
* axis: `0x00` - `0x3F` axis mask of supported axis
* data: none

#### Activate MPU
Try to activate the MPU if it was disabled on boot.

**This command uses the simple com package**

*request*
* command: `0x27`
* data: none

*response*
* none...

#### Get MPU status
See if the MPU is enabled and working.

**This command uses the simple com package**

*request*
* command: `0x28`
* data: none

*response*
* command: `0x28`
* data: `0x01` of on, `0` if off.
