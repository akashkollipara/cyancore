# Driver
_This folder houses all the driver directories_

Cyancore driver framework requires the platform to provide a memory segment in ram to store driver table. This table consists of all driver entries which are enabled by user during compiler time. These drivers should ideally be started in the startup routine. Driver framework provides other apis which will let user start driver with the help of name of driver. _Refer to driver.c for more info_

## Build
- There are 2 types of driver, essential and user-config driver.
- Essential drivers are those which is absolute must to enable minimal functionality of the platform to execute basic code, usually system programs.
- User Config driver are those which can be enabled and can be called in the user programs.
