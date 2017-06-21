const float V_IN = 3.3; // AD Converter Reference Voltage in Volt
                        // Unlike the Uno, the MKR1000 is a 3.3V system
const int MAX_SENSOR_VALUE = 1023; // AD Converter max value (10 bit) 
const float SENSOR_RATE = 10.e-3; // LM35 has 10mV/degree C
const int SENSOR_PIN = A0; // select the input pin

int sensorValue = 0; // value coming from the sensor
float voltage = 0.0; // voltage computed from sensor data
float temperature = 0.0; // temperature computed from voltage

void setup() {
  Serial.begin(9600);
}

void loop() {
  // read the value from the sensor:
  sensorValue = analogRead(SENSOR_PIN);
  Serial.print("Sensor Value: ");
  Serial.print(sensorValue);
  
  voltage = sensorValue*V_IN/MAX_SENSOR_VALUE;
  Serial.print("  Voltage: ");
  Serial.print(voltage);
  Serial.print("V  ");
  
  temperature = voltage/SENSOR_RATE;
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print("C");
  
  Serial.println();
}
