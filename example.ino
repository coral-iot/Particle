// This #include statement was automatically added by the Particle IDE.
#include <SparkJson.h>
#include "device_hub.h"
#include <neopixel.h>
#include <SparkJson.h>
#include <string>
#include <vector>
#include <stdexcept>

// Used for neopixel pins:
#define PIN            6
#define NUMPIXELS      24

using namespace std;

int led0 = D0;
int led1 = D1;

Servo servo;
int servo_pos = 0;

void setup() {
    Serial.begin(9600);
    pinMode(led0, OUTPUT); // used for test pint
    pinMode(led1, OUTPUT); // used for test pint

  	add_switch("0", &turn_on_light1, &turn_off_light1);
    add_slider("1", &turn_on_servo1, &turn_off_servo1, &servo_control);
    
    iota_setup();
    
}

void loop() { }

//Turns any type of device on
void turn_on_light1() {
    digitalWrite(led0,HIGH);
    return;
}
//Turns any type of device off
void turn_off_light1() {
    digitalWrite(led0,LOW);
    return;
}

//Turns any type of device on
void turn_on_servo1() {
    return;
}
//Turns any type of device off
void turn_off_servo1() {
    servo.detach();
    return;
}

// NOTES: make sure to return void
void servo_control(int i) {
    Serial.println(i);
   // Set the servo
   servo.attach(A5);
   servo.write(i);
   delay(1000);
   return;
}
