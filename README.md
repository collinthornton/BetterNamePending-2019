# Mercury Robotics Competition - 2019

This repository is a collection of client-side, server-side, and microcontroller (Arduino, Nucleo, and/or Raspberry Pi) code for the [2019 Mercury Robotics Challenge](https://github.com/osu-mercury-robotics). 

> __Update:__ We won the **Best Design** award out of teams from North Carolina, Kansas, Oklahoma, and Columbia.

The Mercury Robotics Challenge is an annual, intercollegiate robot competition held at Oklahoma State University in Stillwater, Oklahoma. The competition is centered around remote robotics, with a key focus on WLAN connectivity. There are typically 3-5 sections to the competition, encompassing problems such as grasping, manuevering in rough terrain, obstacle detection/avoidance, and more. While the handbook encourages autonomous behavior with score bonuses, remote operation is allowed. If a driver is utilized, he/she must connect to the robot through the Internet and must control the robot from a distant location.

> *__Note to developers:__ Please give meaningful titles to commits and update the corresponding Readme file with a short description and/or API where applicable.* 


## Client Side
This code implements a web-based GUI for control of the robot. It provides real-time video feed, intuitive control options, and a CLI option for advanced input.


## Microcontroller
All software in this folder runs on embedded processors. It is organized by platform: 

* Arduino
* Nucleo

The Nucleo F746ZG was chosen to be implemented on the final robot due to superior processing power, memory, and community support as compared to other microprocessors. As it is compatible with ST-LINK and Mbed. We utilized Arduino UNOs to validate algorithms and simplify the testing procedure. 

All embedded software was developed in C++. We utilized Visual Studio Code, extended with PlatformIO, as a lightweight IDE.

### Nucleo
Houses all code to be ran on the embedded Nucleo. Utilizes object oriented code to abstract the relationship between low-level and high-level software. Algorithms are defined and _implemented_ in .h files to simplify the compilation processes. 

More documentation on individual algorithms can be found in the [folder](https://github.com/Collin-Thornton/BetterNamePending-2019/tree/master/Microcontroller/Nucleo/include).


## Server Side
We utilized a remote server to handle communication between the robot and driver PC to avoid dependence on port-forwarding. This code sets up a message forwarding platform such that the robot and PC can communicate.The general protocol for connection is outlined below.

1. Startup LobbyServer.java on remote server
2. Power up robot. It will automatically attempt to connect to the server.
   - _On Success:_ Green LEDs will illuminate next to the WiFi antenna
   - _On Failure:_ Red LEDs will illuminate next to the WiFi antenna and an continuous tone will be played
3. Connect to GUI on driver PC. 
4. Control robot 
