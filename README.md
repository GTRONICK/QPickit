# QPickit
QT based Pickit2 programmer tool

**NOTE**: You must install pk2cmd on your system first!: (The following links are given as reference only)

ArchLinux: https://aur.archlinux.org/packages/pk2cmd-plus  
Ubuntu: https://www.microchip.com/forums/m795386.aspx or https://nathandumont.com/blog/setting-up-pk2cmd-on-ubuntu  
Other distros: https://curuxa.org/en/Pk2cmd_manual_installation  

![QPickit2](https://sites.google.com/site/gtronick/QPickit.png)

Simple and quick pk2cmd front end. This is a GUI for the pk2cmd command line tool for programming PIC microcontrollers, 
using the Pickit2 programmer and debugger Hardware

**How to build**

You must have qt, qmake and make installed in your system.
run:  
`qmake QPickit.pro`  
`make all -j4`  
`make clean`  



