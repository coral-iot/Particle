## Particle Library for Coral
A C++ package for managing multiple wifi-enabled devices on a single Photon.

### Introduction
The purpose of this library is to make it quick and easy to add and manage multiple devices on your Photon. 
With minimal setup, you will be able to change the state of any device hooked up to your Photon with Coral or custom HTTP requests.
For example, you could change the RGB value of multiple lights at the same time, turn a Servo motor that unlocks a door, receive sensor data from a thermometer, and more with a single request.

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
The String at the beginning of add_switch() and add_slider() is an arbitrary device id. We recommend just labelling each device incrementally starting from 0 as we did above. Identifiers should be 1 or 2 characters maximum.

That's it! Now we are ready to control our light switch and servo motor with Coral.


### Device Classes
#### Switch: 
The switch class is built for simple on/off functionality. 
##### add_switch(const string, void(*on_function)(void), void(*off_function)(void))


#### Slider
The slider class can turn on/off and do something based off of an integer value.

##### add_slider(const string, void(*on_function)(void), void(*off_function)(void), void(*slide_function)(int))

#### RGB
The RGB class can turn on/off and do something based off of 3 integer values.

##### add_rgb(const string, void(*on_function)(void), void(*off_function)(void), void(*rgb_function)(int, int, int))

### Sensor
The Sensor class can turn on/off and return sensor data values.

##### add_sensor(const string, void(*on_function)(void), void(*off_function)(void))

TODO


### Making Requests
If you are using Coral, you don't need to worry about anything in this section - we will cover all of this for you!

Unfortunately, Particle has a 63 character limit on the argument length of Particle.function(), so the request that we send send to the Photon needs to be encoded.

Each request contains a series of comma separated Keys and Values. An example of a Key would be "id", which stands for the unique identifier of a device on your Photon. The Value of "id" would be the same 1-2 character String id that you used during the device setup. Most of the Keys are abbreviated so that we don't quickly run out the 63 character limit. All of the abbreviations are in the table below.

Key | Key Abbreviation | Values | Respective Value Abbreviations
--- | --- | --- | --- |
1-2 Character Identifier |  id | "1-2 character string" | N/A
Type of Device |  t | switch, slider, rgb, sensor | 0, 1, 2, 3
Function execution | e | on, off, special function (slider values, sensor reading, etc.) | on, off, 2
Special function input | v | Some integer value | N/A
Red | r | 0-255 | N/A
Green| g | 0-255 | N/A
Blue | b | 0-255 | N/A

The "special function" refers to a third function that might be used by a class (such as a slider) that takes in some input value. This may seem confusing at first, so lets do an example.
If we send the input `{"id,l1,type,0,exec,on"}`, then the device with the id "l1" and of type 0 (a light) will be told to turn on. 

Another example: If we send the input `{"id,s1,type,1,exec,2,v,50"}`, then the device with the id "s1" and of type 1 (a slider) will be told to execute its "special function" with an input value of 50. Notice that each key and value is comma separated and the order of key-value pairs doesn't matter.

### Developers/Contributors
Feel free to contribute and add more device classes.

