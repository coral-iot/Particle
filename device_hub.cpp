// Add implemetation files here
#include "device_hub.h"
#include <Arduino.h>
#include "SparkJson/SparkJson.h"
#include <string>

vector<Device*> device_list; // List of all current devices
//String device_list_str;

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

void add_sensor(const string& id, func_ptr_void d_on_func = 0, func_ptr_void d_off_func = 0){
    Device* new_sensor = new Sensor(id, "3", d_on_func, d_off_func);
    device_list.push_back(new_sensor);
}

void make_device_list(String& list){
    StaticJsonBuffer<1024> jsonBuffer;

    JsonObject& root = jsonBuffer.createObject();
    JsonArray& devicesArr = root.createNestedArray("devices");

    for (Device* cur_device : device_list) {
        JsonObject& device_json_ob = jsonBuffer.createObject();
        device_json_ob["id"] = (cur_device->id).c_str(); // Add device id
        device_json_ob["t"] = (cur_device->type).c_str();  // Add device type
        
        // Add device status:
        if (cur_device->status){
            device_json_ob["e"] = "1";
        }
        else {
            device_json_ob["e"] = "0";
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


//Use this later for if-else statements
string_code hash_token(String inString) {
    if (inString == "switch") return eSwitch;
    if (inString == "slider") return eSlider;
    if (inString == "rgb") return eRGB;
    if (inString == "sensor") return eSensor;
    if (inString == "on") return eOn;
    if (inString == "off") return eOff;
    return eOff;
    
}

//Parsing JSON command passed by server, only one exec command allowed
int parse_cmd(String cmd) {
    Serial.println(cmd);
    // Parse json object
    char buf[512];
    cmd.toCharArray(buf, 512);
    StaticJsonBuffer<512> jsonBuffer;
    JsonObject& json = jsonBuffer.parseObject(buf);

    //NOTE: Only use on/off to start/stop any device completely
    
    /*
    { "arg": "{\"id\": \"light1\", \"exec\": \"on\" } }  
    */
    const char* tmp_type = json["t"];
    char* type = const_cast<char*>(tmp_type);
    
    const char* tmp_exec = json["e"];
    char* exec = const_cast<char*>(tmp_exec);
    
    Device* device = NULL;
    for (Device* cur_device : device_list) {
        if (cur_device->id == json["id"]){
            device = cur_device;
        }
    }
    
    // Check if call is a generic on/off call:
    if (strcmp(exec, "1") == 0) {
        device->on();
        return 1;
    } else if (strcmp(exec, "0") == 0) {
        device->off();
        return 2;
    }
    
    /* If call is NOT generic on/off: */
    
    if (strcmp(type, "1") == 0) {  // SLIDER:
        Slider* slider = (Slider *) device;
        if (strcmp(exec, "2") == 0) {
            const char* tmp_val = json["v"];
            char* val_str = const_cast<char*>(tmp_val);
            int value = atoi(val_str);
            slider->update_slider(value);
            return 3;
        }
    } else if (strcmp(type, "2") == 0) {  // RGB DEVICE:
        RGB_device* rgb_device = (RGB_device *) device;
        if (exec == "2") {
            rgb_device->set_colors((int) json["r"], (int) json["g"], (int) json["b"]);
            return 4;
        }
    } else if (strcmp(type, "3") == 0) {  // SENSOR:
        Sensor* sensor = (Sensor *) device;
        if (exec == "2") {
            sensor->read();
            return 5;
        }
    }
    return -1;
}