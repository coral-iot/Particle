#ifndef CORAL_H_
#define CORAL_H_

#include <string>
#include <vector>
#include <memory>
#include <new>
#include <memory>
#include <scoped_allocator>
#include <Arduino.h>

using namespace std;
typedef void (*func_ptr_void)(void);
typedef void (*func_ptr_int)(int);
typedef int (*func_ptr_return_int)(void);
typedef void (*func_ptr_color)(int, int, int);

/*
 * General base class for all devices
 */
class Device {    
  private:
  protected:
    func_ptr_void on_func; // function pointer to user-defined on function
    func_ptr_void off_func; // function pointer to user-defined off function
  public:
    string id; // Unique id for device
    string type; // type of device, such as "switch", "slider", "rgb", "sensor"
    int status; // on or off
    vector<string> states;
    // Constructor:
    Device(const string& d_id, const string& d_type, func_ptr_void d_on_func, func_ptr_void d_off_func);
    // Deconstructor:
    ~Device(){}
    // return status:
    inline int get_status() { return status; }
    // Sets user-defined on function:
    void set_on_func(func_ptr_void func_to_call) {
      on_func = func_to_call;
    }
    // Sets user-defined off function:
    void set_off_func(func_ptr_void func_to_call) {
      off_func = func_to_call;
    }
    // To be implemented by the device type:
    virtual void on() {
        status = 1;
        if (on_func)
            on_func();
    }
    virtual void off() {
      status = 0;
      if (off_func)
        off_func();
    }
};

/*
 * Class for all devices that maintain a basic on/off state
 * Examples: lights, doorlock
 */
class Switch: public Device {
  public:
    Switch(const string& d_id, const string& d_type, func_ptr_void d_on_func = 0, func_ptr_void d_off_func = 0) :
    Device(d_id, d_type, d_on_func, d_off_func) {};
    void on() {
        status = 1;
        if (on_func)
            on_func();
    }
    void off() {
        status = 0;
        if (off_func)
            off_func();
    }
};

/*
 * Class for all devices that maintain a basic on/off state and a sliding variable state
 * Examples: lights with dimmer, servo
 */
class Slider: public Device {
  private:
    int slider_val;
    func_ptr_int slider_func;
  public:
    Slider(const string& d_id, const string& d_type, func_ptr_void d_on_func = 0, func_ptr_void d_off_func = 0, func_ptr_int d_slider_func = 0) :
    Device(d_id, d_type, d_on_func, d_off_func), slider_func(d_slider_func) {};

    void set_slider_func(func_ptr_int func_to_call) {
      slider_func = func_to_call;
    }
    void update_slider(int val) {
        slider_val = val;
        if (slider_func)
            slider_func(val);
    }
    int value() {
      return slider_val;
    }
    void on() {
      status = 1;
      slider_val = 100;
      if (on_func)
        on_func();
    }
    void off() {
      status = 0;
      slider_val = 0;
      if (off_func)
        off_func();
    }
};

/*
 * Class for all devices that maintain a basic on/off state and a rgb values
 * Examples: lights
 */
class RGB_device: public Device {
  private:
    func_ptr_color color_func;
  public:
  
    RGB_device(const string& d_id, const string& d_type, func_ptr_void d_on_func = 0, func_ptr_void d_off_func = 0, func_ptr_color d_color_func = 0) :
    Device(d_id, d_type, d_on_func, d_off_func), color_func(d_color_func) {};
    
    int r;
    int g;
    int b;

    void set_color_func(func_ptr_color func_to_call) {
      color_func = func_to_call;
    }
    void red(int val) {
      r = val;
    }
    void blue(int val) {
      g = val;
    }
    void green(int val) {
      b = val;
    }
    void init_colors(int red, int green, int blue) {
      r = red;
      g = green;
      b = blue;
    }
    void set_colors(int red, int green, int blue) {
      init_colors(red,green,blue);
      if (color_func)
        color_func(r, g, b);
    }
    void on() {
      status = 1;
      r = 100;
      g = 100;
      b = 100;
      if (color_func)
        color_func(r,g,b);
    }
    void off() {
      status = 0;
      r = 0;
      g = 0;
      b = 0;
      if (color_func)
        color_func(0,0,0);
    }
};

/*
 * Class for all devices that are sensor related
 * Examples: temperature sensor
 */
class Sensor: public Device {
  private:
    func_ptr_return_int read_func;
  public:
    Sensor(const string& d_id, const string& d_type, func_ptr_void d_on_func = 0, func_ptr_void d_off_func = 0, func_ptr_return_int d_sense_func = 0) :
    Device(d_id, d_type, d_on_func, d_off_func), read_func(d_sense_func) {};
    
    // func_to_call should return an int value that can then be returned by read()
    void set_read_func(func_ptr_return_int func_to_call) {
      read_func = func_to_call;
    }
    int read() {
      if (read_func)
        return read_func();
      return -1;
    }
    void on() {
      status = 1;
      if (on_func)
        on_func();
    }
    void off() {
      status = 0;
      if (off_func)
        off_func();
    }
};

// Function prototypes:
void add_switch(const string& id, func_ptr_void d_on_func, func_ptr_void d_off_func);
void add_slider(const string& id, func_ptr_void d_on_func, func_ptr_void d_off_func, func_ptr_int d_slider_func);
void add_rgb(const string& id, func_ptr_void d_on_func, func_ptr_void d_off_func, func_ptr_color d_color_func);
void add_sensor(const string& id, func_ptr_void d_on_func, func_ptr_void d_off_func);
void make_device_list(String& list);
void coral_setup();
Device* get_device(string device_id);
int parse_cmd(String cmd);

#endif // #ifndef _CORAL_H_