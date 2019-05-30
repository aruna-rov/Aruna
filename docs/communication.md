# RS485
below is the connection circuit for rs485. baud rate of is 5000000.
### RS485 connection circuit schematic:
```
         VCC ---------------+                               +-------------- VCC
                            |                               |
                    +-------x-------+               +-------x-------+
         RXD <------| RO            |               |             RO|-----> RXD
                    |              A|---------------|A              |
         TXD ------>| DI  MAX485    |               |    MAX485   DI|<----- TXD
ESP32               |              B|---------------|B              |           SERIAL ADAPTER SIDE
         RTS --+--->| DE            |               |             DE|---+
               |    |            GND|--+---------+--|GND            |   |
               +----| /RE           |  |         |  |            /RE|---+-- RTS
                    +---------------+  |         |  +---------------+
                                       |         |            
         GND --------------------------+         +------------------------- GND
```

### pinout

PIN | ESP	| RS485
--- | ---	| ---
RXD	| 22	| RO
TXD	| 23	| DI
RTS	| 18	| DE/RE
GRND| GND	| GND
v5	| -		| VCC