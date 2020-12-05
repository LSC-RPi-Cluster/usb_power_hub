#include <Wire.h>
#include <INA219.h>

#define DELAY_MS 2000
#define BAUD_RATE 9600

const int INA_addr_0 = 0x40;
const int INA_addr_1 = 0x41;
const int INA_addr_2 = 0x44;
const int INA_addr_3 = 0x45;

INA219 *INAs_sensors[4];
float USBs_data[4][5];

// Functions prototypes
void read_ina219_values(void);
void serial_send(void);

String serialInput = "";

void setup() {
    Serial.begin(9600);

    INAs_sensors[0] = new INA219(INA_addr_0);
    INAs_sensors[1] = new INA219(INA_addr_1);
    INAs_sensors[2] = new INA219(INA_addr_2);
    INAs_sensors[3] = new INA219(INA_addr_3);

    INAs_sensors[0]->begin();
    INAs_sensors[1]->begin();
    INAs_sensors[2]->begin();
    INAs_sensors[3]->begin();
}

void loop() {
    read_ina219_values();
    serial_send();
    delay(DELAY_MS);
}

void read_ina219_values(void) {
    for (int i = 0; i < 4; i++) {
        USBs_data[i][0] = INAs_sensors[i]->shuntVoltage() * 1000;      // milivolts
        USBs_data[i][1] = INAs_sensors[i]->busVoltage();               // volts 
        USBs_data[i][2] = abs(INAs_sensors[i]->shuntCurrent() * 1000); // miliamps
        USBs_data[i][3] = USBs_data[i][1] + (USBs_data[i][0] / 1000);  // volts
        USBs_data[i][4] = INAs_sensors[i]->busPower();                 // watts
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

        String usb_id = "\"usb_id\":";
        String shuntvoltage = "\"shunt_voltage\":";
        String busvoltage = "\"bus_voltage\":";
        String loadvoltage = "\"load_voltage\":";
        String current_mA = "\"current_mA\":";
        String buspower = "\"bus_power\":";

        String json_obj = "";

        json_obj += obj_ini;
        json_obj += usb_id + i + ',';
        json_obj += shuntvoltage + USBs_data[i][0] + ',';
        json_obj += busvoltage + USBs_data[i][1] + ',';
        json_obj += current_mA + USBs_data[i][2] + ',';
        json_obj += loadvoltage + USBs_data[i][3]+ ',';
        json_obj += buspower + USBs_data[i][4];
        json_obj += obj_end;
        json_array += json_obj + ',';
    }

    json_array.remove(json_array.length()-1);

    json_array += array_end;
    Serial.println(json_array);
    
}
