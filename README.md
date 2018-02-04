# Blupp
Controlling BiOrb aquarium illumination by ESP8285 board

I bought a BiOrb LIFE Aquarium for my wife, recently.
It was shipped with a nice multi-color LED illumination (MCR LED) with remote control, but without any automatic day/night cycle.
In my and the fisch eyes, this is cleary a lack of functionality.

I had a ["Wemos D1 mini pro"](https://wiki.wemos.cc/products:d1:d1_mini_pro) left from a former project, 
so I used this to upgrade the illumination module with the following functionality:
* connect to different access points (with fix IP-Address)
* span an access-point if none of the networks is present (the time based illumination is disabled)
* get network time and change illumination according to programmed schedule
* run a web server for a  web remote control

## hardware setup
The MCR LED module is controlled by a small, credit card size IR remote control.
The IR receiver is located in a dome on the top of the module. It is a three pin integrated receiver module like the [Vishay TSOP34138](https://www.vishay.com/docs/82490/tsop321.pdf).

To control the module, we have two choices:
1. we place a sender module outside of the aquarium lamp which is driven by our Wemos module
1. we open the housing and inject the control codes directly on the pins of the IR module

I chose the second way, because I did not have a IR sender module and I wanted to put the Wemos-Module inside the housing. Be shure to loose any warranty for the LED module.  
The housing conststs of two plastic parts which are carefully sticked together by transparent glue. To seperate them, I needed a small, sharp screw driver, a lot of patience and some brute force.

![Biorb MCR LED PC board](https://github.com/petermuenstermann/Blupp/images/BiOrb_MCR_board.jpg "Beispielbild")

A brief analysis of the PC board showed, tha the module is controlled by a Microchip PIC16F1704 microcontroller.  
The input power supply (12VAC) is rectified and regulated down to 5V.  
__Unfortunately__, the power of the LVR is to weak to drive both the PIC and my Wemos module.

There are (at least) three options:
1. using another wifi module, like the ESP1, with lower power requirements
1. replacing the linear voltage regulator by a more powerful version
1. supplying external power to the wifi module.

I didn't want to spend mor money, so I took one of my external micro usb power supplies for external powering.




