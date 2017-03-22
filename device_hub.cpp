// Add implemetation files here
#include "device_hub.h"
#include <Arduino.h>
#include "SparkJson/SparkJson.h"
#include <string>
#include <map>


vector<Device*> device_list; // List of all current devices
String device_list_str;

Device::Device (const string& d_id, const string& d_type, func_ptr_void d_on_func = 0, func_ptr_void d_off_func = 0) : 
  on_func(d_on_func), off_func(d_off_func), id(d_id), type(d_type), status(0) { }

void add_switch(const string& id, func_ptr_void d_on_func = 0, func_ptr_void d_off_func = 0){
    Device* new_switch = new Switch(id, "0", d_on_func, d_off_func);
    device_list.push_back(new_switch);
}

void add_slider(const string& id, func_ptr_void d_on_func = 0, func_ptr_void d_off_func = 0, func_ptr_int d_slider_func = 0){
    Device* new_slider = new Slider(id, "1", d_on_func, d_off_func, d_slider_func);
    device_list.push_back(new_slider);
}

void add_rgb(const string& id, func_ptr_void d_on_func = 0, func_ptr_void d_off_func = 0, func_ptr_color d_color_func = 0){
    Device* new_rgb = new RGB_device(id, "2", d_on_func, d_off_func, d_color_func);
    device_list.push_back(new_rgb);
}

void add_sensor(const string& id, func_ptr_void d_on_func = 0, func_ptr_void d_off_func = 0, func_ptr_return_int d_sense_func = 0){
    Device* new_sensor = new Sensor(id, "3", d_on_func, d_off_func, d_sense_func);
    device_list.push_back(new_sensor);
}

void iota_setup() {
  	Particle.variable("device_list", &device_list_str, STRING);
  	Particle.function("change_state", parse_cmd);
	make_device_list(device_list_str);
}
/*
 * Generates a String encoding of the device list that can be
 * accessed by POST requests via Particle.function().
 *
 * As of 3/1/17: Maximum String length is 622 bytes
 *
 * Currently does not update after initialization
 */
void make_device_list(String& list){
    StaticJsonBuffer<1024> jsonBuffer;

    JsonObject& root = jsonBuffer.createObject();
    JsonArray& devicesArr = root.createNestedArray("devices");

    for (Device* cur_device : device_list) {
        JsonObject& device_json_ob = jsonBuffer.createObject();
        device_json_ob["id"] = (cur_device->id).c_str(); // Add device id
        device_json_ob["t"] = (cur_device->type).c_str();  // Add device type
        
        // Add device status:
        if (cur_device->status) {
            device_json_ob["e"] = "1";
        }
        else {
            device_json_ob["e"] = "0";
        }
        // Add device value if applicable
        if (device_json_ob["t"] == "3") { //Device type 3 is a Sensor
            Sensor* sensor = (Sensor *) cur_device;
            device_json_ob["v"] = (*sensor).read(); //Stores int sensor value
        }
        devicesArr.add(device_json_ob);
    }

    char buffer[1024];
    root.printTo(buffer, sizeof(buffer));
    list = buffer;
}

Device* get_device(string device_id){
    for(Device* cur_device : device_list) {
        if(cur_device->id == device_id){
            return cur_device;
        }
    }
    return 0;
}

/*
 * Parses string-encoded command passed by server.
 * There must be an even number of tokens passed in due to the
 * key-value pair format and tokenization.
 * 
 * EXAMPLE INPUT: {"id,light1,type,1,exec,on,v,50"}  
 * Types:
 * 0: switch (not necessary to input)
 * 1: slider
 * 2: rgb
 * 3: sensor
 * Keys:
 * id: Id string of a device
 * t: The type of a device
 * e: The executable being triggered
 *    Values: on, off, "2" (value/sensor input)
 * v: The value for a command (only for execs that required a value input)
 * r,g,b: RGB values paired with integers
 *
 */
int parse_cmd(String cmd) {
    Serial.println(cmd);
    
    //Initializing variables to process cmd input into a dictionary
    std::map<string, string> dict;
    char* dup = strdup(cmd.c_str());
    char* token = strtok(dup, ",");
    
    //Add tokens 2 at a time to the map. e.g. dict["type"] = "switch"
    while (token != NULL) {
       //Process next two char* tokens and storing as strings in the dict[key]=value
       string key(token);
       token = strtok(NULL, ","); 
       string value(token);
       dict[key] = value;
       token = strtok(NULL, ",");
    }
    free(dup);
    
    /* Process Tokens */
    
     Device* device = get_device(dict["id"]);
    
    // Check if call is a generic on/off call:
    if (dict["e"] == "on") {
        device->on();
        return 1;
    } else if (dict["e"] == "off") {
        device->off();
        return 2;
    }
    // If call is NOT generic on/off:
    if (dict["t"] == "1") {  // SLIDER:
        Slider* slider = (Slider *) device;
        if (dict["e"] == "2") {
            int value = atoi(dict["v"].c_str()); 
            slider->update_slider(value);
            return 3;
        }
    } else if (dict["t"] == "2") {  // RGB DEVICE:
        RGB_device* rgb_device = (RGB_device *) device;
        if (dict["e"] == "2") {
            //Converting r,g,b strings to ints
            int r = atoi(dict["r"].c_str()); 
            int g = atoi(dict["g"].c_str()); 
            int b = atoi(dict["b"].c_str());
            rgb_device->set_colors(r,g,b);
            return 4;
        }
    } else if (dict["t"] == "3") {  // SENSOR:
        Sensor* sensor = (Sensor *) device;
        if (dict["e"] == "2") {
            return sensor->read(); //Return sensor value
        }
    }
    return -1;
}