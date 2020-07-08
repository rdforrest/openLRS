/*
  Based on work by Tim Senecal.
  For Nano Every. (I have had problems with clones stopping working and requiring sketch to be reloaded.)
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
  (My original deoth calibration depth (Using Tim's depth sensor detailed on SubPirates Forum)  = 3 * (depth - 84); // 84 to set zero)
*/

void setup() {

  // Open USB serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("Wake-up-you Serial usb port!");
  delay(1000); // We wait 1 second

  // Open second hardware serial port
  Serial1.begin(9600);

  Serial1.println("Wake-up Serial1 port!");
  delay(1000); // We wait 1 second

}
void loop() {

  // Read voltage sensor value on A2
  int sensorVal = analogRead (A2);
  sensorVal = sensorVal / 76; //  Fiddle factor (originally 67) is calibration value for voltage from earlier software by RDF
  String mysensorVal = String(sensorVal);

  // Read depth sensor value on A3
  int sensorVal1 = analogRead (A3);
  sensorVal1 = sensorVal1/10; //  Fiddle factor is calibration value
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
