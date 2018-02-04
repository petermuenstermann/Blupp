# Blupp
Controlling BiOrb aquarium illumination by ESP8266 board

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
The IR receiver is located in a dome on the top of the module. It is a three pin integrated receiver module like the [Vishay TSOP4438](https://www.vishay.com/docs/82490/tsop321.pdf).

To control the module, we have two choices:
1. we use an external IR sender module which is driven by our Wemos wifi module
1. we open the housing and inject the IR control codes directly at the pins of the IR receiver

I chose the second way, because I did not have a IR sender module and I wanted to put the Wemos-Module inside the housing. Be shure to loose any warranty for the LED module.  
The housing conststs of two plastic parts which are carefully sticked together by transparent glue. To seperate them, I needed a small, sharp screw driver, a lot of patience and some brute force.

![the CP board looks like:](https://github.com/petermuenstermann/Blupp/blob/master/images/BiOrb_MCR_board.jpg "BiOrb MCR LED PC board")  
the position of the three pins of the IR receiver are marked with a green box.

A brief analysis of the PC board showed, tha the module is controlled by a Microchip PIC16F1704 microcontroller.  
The input power supply (12VAC) is rectified and regulated down to 5V.  
__Unfortunately__, the power of the LVR is to weak to drive both the PIC and my Wemos module.  
To get rid of this, there are (at least) three options:
1. using another wifi module, like the ESP1, with lower power requirements. Be aware, that most of the modules run with 3.3V and that you will need an additional voltage regulator (or a more advanced module with integrated power management)
1. replacing the linear voltage regulator by a more powerful version
1. supplying external power to the wifi module.

As I didn't want to spend more money, so I took one of my external micro usb power supplies for external powering.
To be more flexible during the development, I soldered two cables at the ground and the signal pin of the IR receiver
and fed them outside the housing of the LED module.

## analyzing the IR remote protocol
One step towards emulating the IR remote control is to extract the protocol used by the sender/receiver.
_normally_, this is a 38kHz modulated light pattern, composed of sequences of pulses (mark) and pauses (space).  
The IR receiver already demodulates the 38KHz pulses, so that we olly see the mark (open collector low) and space (open collector high) signals at the output.

The analysis can easily be done with the the "dump" demo of the IRLib2 library. You can use the Wemos module and feed the signal of the IR receiver over a voltage divider (inputs are not 5V tolerant) to one of the inputs.

It shows, that the remote uses a NEC type protocol with 9ms mark and 4.5ms space for the head, the followed by 32 bits for the code (MSB).  
A high bit is 1.7ms mark and 0.55ms space, a low bit 0.55 mark and 0.55 space.

To send the code, we don't need the IRLib2 any more. The code can by synthesized by the simple code snippet

    void HighLow(uint16_t highTime, uint16_t lowTime) {
      digitalWrite(IR_PIN, HIGH);
      delayMicroseconds(highTime);
      digitalWrite(IR_PIN, LOW);
      delayMicroseconds(lowTime);
    }

    void SendCode(uint32_t code) {
      uint32_t mask = 0x80000000;
    
      // Header
      HighLow(9000, 4500);
  
      while (mask) {
        if ( code & mask )  HighLow(550, 1700);
                       else HighLow(550, 550);
        mask >>= 1;
      }
    }
    
The Signal normally is 'low', whereas the output of the IR Rreceiver is normally 'high' (which means open-collector open).   
We add a open collector/drain transistor to the control output of the Wemos module, which gives a logical OR with the IR receiver and bypasses the problem with the 5V signals on the BiOrb PC board. So we can use the remote control and teh Wemos module simultaneously.

## Wifi setup
The wifi Setup is mainly taken from the _ESPWifiMulti_ examples for the Wemos module.
The connection status is coded over the color of the BiOrb Lamp.

## getting the network time
The code is taken from the _NTP-TZ-DST_ example program.

If the network time is received, the program iterates thru all the "lamp events" until the present daytime.

## web server with remote control
I found the very wonderful demo _FSBrowser_ based on the _ESP8266WebServer_ library 




