# Console Driver

Console driver provides interfaces to stdout, stdin, stderr and stdlog. The 3 standard devices std(out/in/err) are linked with console device that will be linked with the console driver stars. But for stdlog, the device is linked when the driver calls logger attach routine. _Essentially this directory houses the drivers for consoles._

#### Console (L1 Driver)
- This devices connects to std(out/in/err)
- std-devices are used by libc stdio functions
- The underlying hardware is getting linked with the L2 Driver which is present under `con_serial_<family>` folder

#### Stdlog (L1 Driver)
- This devices connects to stdlog
- stdlog is a system standard logger provided by libsyslog
- By default syslog uses membuf to log system logs

## Build
- Maitri (cc build engine) requires a unique L2 driver folder which is represented with `con_serial_<family>`, where **family** is Family of Platform/Controller
- If in case the family is absent, a warning will be displayed during build, but the compilation will not fail.
