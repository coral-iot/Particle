## Particle Library for Coral
A C++ package for managing multiple wifi-enabled devices on a single Photon.

### Introduction
The purpose of this library is to make it quick and easy to add and manage multiple devices on your Photon. 
With minimal setup, you will be able to change the state of any device hooked up to your Photon with Coral or custom HTTP requests.
For example, you could change the RGB value of multiple lights at the same time, turn a Servo motor that unlocks a door, receive sensor data from a thermometer, and more with a single line of JSON! 

Additionally, this package is built for integration with the Coral web platform.

### Setup
```C++
#include "coral.h"
```
To use the library, simply download and include the package. If you are using Particle Build, go to 'Libraries->Community Libraries'. Search "coral", and add the library to your project.

### How to Use
#### Adding a device
To setup your first device, you first want to look at our list of device classes and choose which one suits your project the best. This library is built on a bunch of device classes with generic functionality For example, the 'Switch' class has the ability to turn_on() and turn_off(). So if you are building something which only needs to be turned on and off, you would first create your own turn_on() and turn_off() functions:

```C++
//Turns on the light that I hooked up to my Photon
void turn_on_light() {
    digitalWrite(D1, HIGH);
    return;
}
//Turns off the light that I hooked up to my Photon
void turn_off_light() {
    digitalWrite(D1, LOW);
    return;
}

```
Next, you can pass the function pointers to the add_switch() function from the library in the Photon's setup() loop:

```C++
void setup() {
    /**
    Any of your own setup code goes here.
    **/
    add_switch("0", &turn_on_light1, &turn_off_light1);
    a few lines of code
    coral_setup();
}
```
Now your device has been added! Lastly, you can see we wrote coral_setup() at the end up the setup() method. coral_setup() sets up all of your devices once they have been added and connects them to the Internet via Particle's built-in functions.

It is essential for coral_setup() to be called at the very end of your setup() function.

#### Adding more devices

Let's say we want to control a Servo motor on the same Particle. We first need to look at the available devices in the library. The 'Slider' class should work well for Servo motor, because all we need to do with a Servo is change an integer value which corresponds to some angle. Again, we define our own functions for controlling the servo:
```C++
//Turn on the servo (not actually useful in this example)
void turn_on_servo() {
    return;
}
//Turn off the servo
void turn_off_servo() {
    servo.detach();
    return;
}

void servo_control(int i) {
   servo.attach(A5);
   servo.write(i);
   return;
}
```
Now that we have the functions defined for our Servo, we pass them in to the our add_slider() function:

```C++
void setup() {
    /**
    Any of your own setup code goes here.
    **/
    add_switch("0", &turn_on_light1, &turn_off_light1);
    add_slider("1", &turn_on_servo, &turn_off_servo, &servo_control);
    
    coral_setup();
}
```
The String at the beginning of add_switch() and add_slider() is an arbitrary device id. We recommend just labelling each device incrementally starting from 0 as we did above.

That's it! Now we are ready to control our light switch and servo motor with Coral or HTTP.


### Device Classes




### Making Requests
If you are using Coral, you don't need to worry about anything in this section - we will cover all of this for you!



### Developers/Contributors


