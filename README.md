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




