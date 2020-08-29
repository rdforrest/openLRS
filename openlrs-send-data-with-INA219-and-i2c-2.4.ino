/*
  Based on work by Tim Senecal.
  For Nano Every (Which has the advantage of having several serial ports in hardware)(I have also had problems with clones stopping working and requiring sketch to be reloaded.)
  Sends data over openLRS Passthru.
  To do this, open the openLRS configurator for TX , Set serial baudrate(bps) to 9600, data rate(bps) to 9600, Telemetry to "Yes" (This is Passthru)
  Connections to Hobbyking receiver module:
  Arduino TX (Pin TXD1) -> receiver  RX (Serial port pins, 4th pin down fom top - aerial on left)
  Arduino GND -> receiver GND (Serial port pins,1st pin down from top - aerial on left.)
  Voltage divider input to A2
  Other sensor input (e.g Depth) to A3
  Receiver +6v to Vin on arduino
  Arduino serial data default is, serial baud rate (9600) and 8 data bits, no parity,no stop,LSB and not inverted - the default
  Sept 2019-RDF - Voltage Divider. Move to  input from divider set by 4.7kohms and 15k ohms for 12 v battery. Better for  3.3 v input.
  (My original depth calibration depth (Using Tim's depth sensor detailed on SubPirates Forum)  = 3 * (depth - 84); // 84 to set zero)
  July 2020. Working on putting on voltage and current using an INA219 module.
  Aug 2020 This works well now. (Address is 64 (0x40)(From i2c scanner)

*/

#include <Wire.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219;

void setup() {

  // Open USB serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  uint32_t currentFrequency;

  Serial.println("Starting INA219 current monitor");

  // Initialize the INA219.
  // By default the initialization will use the largest range (32V, 2A).  However
  // you can call a setCalibration function to change this range (see comments).
  // To use a slightly lower 32V, 1A range (higher precision on amps):
  //ina219.setCalibration_32V_1A();
  // Or to use a lower 16V, 400mA range (higher precision on volts and amps):
  //ina219.setCalibration_16V_400mA();
  if (! ina219.begin()) {
    Serial.println("Failed to find INA219 chip");
    while (1) {
      delay(10);
    }

  }
  // Open second hardware serial port
  Serial1.begin(9600);
  Serial1.println("Wake-up Serial1 port!");
  delay(1000); // We wait 1 second

}

void loop() {

  // Get data from IN219
  float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  float loadvoltage = 0;
  float power_mW = 0;

  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  power_mW = ina219.getPower_mW();
  loadvoltage = busvoltage + (shuntvoltage / 1000);

  Serial.print("Bus Voltage:   "); Serial.print(busvoltage); Serial.println(" V");
  Serial.print("Shunt Voltage: "); Serial.print(shuntvoltage); Serial.println(" mV");
  Serial.print("Load Voltage:  "); Serial.print(loadvoltage); Serial.println(" V");
  Serial.print("Current:       "); Serial.print(current_mA); Serial.println(" mA");
  Serial.print("Power:         "); Serial.print(power_mW); Serial.println(" mW");
  Serial.println("");

  delay(2000);

  //Select and process variables to output (round off floating variables)
  int sensorVal = round(busvoltage);     // Round off bus voltage

  int sensorVal1 = abs(current_mA);     // This gives a very simple current draw figure, accuarate to about an amp.
  sensorVal1 = sensorVal1 / 1000;
  sensorVal1 = round(sensorVal1 + 0.5);

  // Read voltage sensor value on A2
  //int sensorVal = analogRead (A2);
  //sensorVal = sensorVal / 76; //  Fiddle factor (originally 67) is calibration value for voltage from earlier software by RDF
  String mysensorVal = String(sensorVal);

  // Read depth sensor value on A3
  //int sensorVal1 = analogRead (A3);
  // sensorVal1 = sensorVal1 / 10; //  Fiddle factor is calibration value
  String mysensorVal1 = String(sensorVal1);

  // print the voltage sensor value
  Serial.println(mysensorVal);
  Serial.print('\n'); // new line
  delay (200);

  // print the voltage sensor value to Serial
  Serial.println(mysensorVal1);
  Serial.print('\n'); // new line
  delay (200);

  Serial1.print(mysensorVal);
  delay (200);
  Serial1.print('\n'); // new line
  delay (200);

  // print the voltage sensor value to Serial1
  Serial1.print(mysensorVal1);
  delay (200);
  Serial1.print('\n'); // new line
  delay (200);

  //Test
  //Serial1.print("RDF");

  delay(10); // We wait 10mS to print the next value

}
