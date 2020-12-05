from time import sleep
import json
import serial

from serial.tools import list_ports
from prometheus_client import start_http_server
from prometheus_client import Gauge

INTERVAL = 2
SERIAL_BAUD_RATE = 9600
SERVER_PORT = 9999

device = list_ports.comports()[0].device
serial_buffer = serial.Serial(device, SERIAL_BAUD_RATE)

shunt_voltage = Gauge("shunt_voltage_millivolts",
                      "Measure of the voltage passing through the Shunt resistor. (Millivolts)",
                      labelnames=['board', 'usb_port'])

bus_voltage = Gauge("bus_voltage_volts",
                    "Measure of the voltage passing through the bus. (Volts)",
                    labelnames=['board', 'usb_port'])

shunt_current = Gauge("shunt_current_milliamps",
                      "Measure of the current passing through the Shunt resistor. (Millivolts)",
                      labelnames=['board', 'usb_port'])

load_voltage = Gauge("load_voltage_volts",
                     "Measure of the total load voltage. (Volts)",
                     labelnames=['board', 'usb_port'])

bus_power = Gauge("bus_power_watts",
                  "Measure of the power consumption of USB port. (Watts)",
                  labelnames=['board', 'usb_port'])

def read_loop():
    while True:
        serial_buffer.flushInput()
        
        if serial_buffer.inWaiting():
            try:
                serial_data = serial_buffer.readline().decode('utf-8').strip()
                measures = json.loads(serial_data)
            except:
                continue

            for measure in measures:
                label_values = {
                    "board": str(device),
                    "usb_port": measure['usb_id']
                }

                shunt_voltage.labels(**label_values).set(measure['shunt_voltage'])
                bus_voltage.labels(**label_values).set(measure['bus_voltage'])
                shunt_current.labels(**label_values).set(measure['current_mA'])
                load_voltage.labels(**label_values).set(measure['load_voltage'])
                bus_power.labels(**label_values).set(measure['bus_power'])

        sleep(INTERVAL)

if __name__ == '__main__':
    try:
        start_http_server(SERVER_PORT)
        read_loop()

    except KeyboardInterrupt:
        print(" Interrupted!")
        exit(0)