ofxRPiCameraVideoGrabber
========================

STATUS: PERPETUAL DEVELOPMENT

DESCRIPTION:   
openFrameworks addon to control the Raspberry Pi Camera Module. This does _not_ provide still camera functionality.


REQUIREMENTS:
openFrameworks .8 or higher [Setup Guide](http://openframeworks.cc/setup/raspberrypi/)   
Developed with GPU memory set at 256, overclock to medium but 128/default should work as well   
Desktop Mode (X11 enabled) may work but untested

USAGE:   
Clone into your openFrameworks/addons folder
Either copy one of the examples into /myApps or add ofxRPiCameraVideoGrabber to the addons.make file in your project

LED Toggling requires gpio program provided via wiringPi   
https://projects.drogon.net/raspberry-pi/wiringpi/the-gpio-utility/


The addon works in a few different modes:

TEXTURE MODE:   
Allows the use of:
 - Shaders
 - Pixel access
 - Overlays, etc
 
 
NON-TEXTURE MODE (or direct-to screen)   
In non-texture mode the camera is rendered directly to the screen. It typically looks a bit faster/cleaner but no other drawing operations can happen.


RECORDING:   
Recording is available in both texture and non-texture modes


EXAMPLES:   

example-non-texture   
Camera turns on and is rendered full screen inside app   
Press the "e" key to toggle through built in filters


example-texture-mode  
Camera turns on and renders to a texture that is drawn at full screen and a scaled version   
Press the "e" key to toggle through built in filters   


example-shaders   
Basic shader usage with texture-mode  
Press the "e" key to toggle through built in filters 
Press the "s" key to toggle shader   

example-pixels:   
Demonstrates how to get pixels (needed for OpenCv operations). 


THANKS:
Thanks to @tjormola for sharing his demos and exploration - especially in regards to recording
https://github.com/tjormola/rpi-openmax-demos

and thanks to @linuxstb for helping get started with the camera and OpenMax
https://github.com/linuxstb/pidvbip



 





 
