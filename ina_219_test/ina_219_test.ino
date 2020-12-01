#include <Adafruit_INA219.h>
#include <Wire.h>

const int INA_addr_0 = 0x40;
const int INA_addr_1 = 0x41;
const int INA_addr_2 = 0x44;
const int INA_addr_3 = 0x45;

Adafruit_INA219 *INAs_sensors[4];
float USBs_data[4][4] = {0.0}

// {shuntvoltage, busvoltage, current, loadvoltage}

// Prototype of functions
void Read_INA219_Values(void);
void Serial_Send(void);

String serialInput = "";

void setup() {

    Serial.begin(9600);
    Serial.println("Measuring voltage and current with INA219 ...");

    INAs_sensors[0] = new Adafruit_INA219(INA_addr_0);
    INAs_sensors[1] = new Adafruit_INA219(INA_addr_1);
    INAs_sensors[2] = new Adafruit_INA219(INA_addr_2);
    INAs_sensors[3] = new Adafruit_INA219(INA_addr_3);

    INAs_sensors[0]->begin();
    INAs_sensors[1]->begin();
    INAs_sensors[2]->begin();
    INAs_sensors[3]->begin();
}

void loop() {
    read_ina219_values();
    serial_send();
    delay(2000);
}

void read_ina219_values(void) {

    for (int i = 0; i < 4; i++) {
        USBs_data[i][0] = INAs_sensors[i]->getShuntVoltage_mV();
        USBs_data[i][1] = INAs_sensors[i]->getBusVoltage_V();
        USBs_data[i][2] = abs(INAs_sensors[i]->getCurrent_mA());
        USBs_data[i][3] = USBs_data[i][1] + (USBs_data[i][0] / 1000)
    }
}

void serial_send(void) {
    char array_ini = '[';
    char array_end = ']';
    String json_array = "";

    json_array += array_ini;

    for (int i = 0; i < 4; i++) {
        char obj_ini = '{';
        char obj_end = '}';

        String usb_id = "\"usb_id\":"
        String shuntvoltage = "\"shunt_voltage\":";
        String busvoltage = "\"bus_voltage\":";
        String loadvoltage = "\"load_voltage\":";
        String current_mA = "\"current_mA\":";

        String json_obj = "";

        json_obj += obj_ini;
        json_obj += usb_id + i + ',';
        json_obj += shuntvoltage + USBs_data[i][0] + ',';
        json_obj += busvoltage + USBs_data[i][1] + ',';
        json_obj += current_mA + USBs_data[i][2] + ',';
        json_obj += loadvoltage + USBs_data[i][3];
        json_obj += obj_end;
        json_array += json_obj + ',';
    }

    json_array += array_end;
    Serial.println(json_array);
    Serial.println("");
    
}
