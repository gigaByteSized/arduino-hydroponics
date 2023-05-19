#include <Arduino.h>
#include <DFRobot_EC.h>
#include <EEPROM.h>

// Pins for pH and EC sensors
#define PH_SENS_PIN A0
#define EC_SENS_PIN A1

// Pump relay module pins
#define WATER_PUMP_PIN 2
#define ACID_PUMP_PIN 3
#define NUTRIENT_PUMP_1_PIN 4
#define NUTRIENT_PUMP_2_PIN 5

// EC sensor global init
float ecVoltage, ecValue;

// Temperature
// TODO: In the absence of a temperature probe, change this value to the ambient room temperature (in celsius)!
float temperature = 25; // 25 = room temp

// Initialize EC EEPROM from DFRobot_EC headers
DFRobot_EC ec;

// Calibration formula is based on linear regression of voltage (x) against pH (y)
// Formula:
// y = xOffset(x) + yOffset
// Formula is basically in slope-intercept form:
// y = mx + b

float m = -1.92; //change this value to calibrate
float b = 11.55; //change this value to calibrate



// Sensor state thresholds

// pH
float pHDesiredMin = 5.5;
float pHDesiredMax = 6.0;
float pHFloor = 5.0;
float pHCeiling = 7.0;

// EC, in mS/cm
float ecLow = 1.4;
float ecHigh = 1.8;

// Pump timings (in ms)
int waterTiming = 300;
int acidTiming = 300;
int nutrientTiming = 900;

// Pump dispense buffer (in ms)
int pumpBuffer = 15000;
  
int samples = 10; // 10 samples per measurement cycle.
float adcResolution = 1024.0;

void setup() {
    pinMode(WATER_PUMP_PIN, OUTPUT);
    pinMode(ACID_PUMP_PIN, OUTPUT);
    pinMode(NUTRIENT_PUMP_1_PIN, OUTPUT);
    pinMode(NUTRIENT_PUMP_2_PIN, OUTPUT);

    Serial.begin(9600);
    ec.begin();
    delay(100);
}

float getpH(float pHVoltage) {
    return m * pHVoltage + b;
}

void offPumps(){
  digitalWrite(WATER_PUMP_PIN, LOW);
  digitalWrite(ACID_PUMP_PIN, LOW);
  digitalWrite(NUTRIENT_PUMP_1_PIN, LOW);
  digitalWrite(NUTRIENT_PUMP_2_PIN, LOW);
}

void controlWater() {
  digitalWrite(WATER_PUMP_PIN, HIGH);
  Serial.println("Turned on water pump! ");
  delay(waterTiming);
  Serial.println("Turned off water pump! ");
  digitalWrite(WATER_PUMP_PIN, LOW);
}

void controlAcid() {
  digitalWrite(ACID_PUMP_PIN, HIGH);
  Serial.println("Turned on acid pump! ");
  delay(acidTiming);
  Serial.println("Turned off acid pump! ");
  digitalWrite(ACID_PUMP_PIN, LOW);
}

void controlBase() {
  digitalWrite(NUTRIENT_PUMP_1_PIN, HIGH);
  digitalWrite(NUTRIENT_PUMP_2_PIN, HIGH);
  Serial.println("Turned on basic pumps! ");
  delay(nutrientTiming);
  Serial.println("Turned off basic pumps! ");
  digitalWrite(NUTRIENT_PUMP_1_PIN, LOW);
  digitalWrite(NUTRIENT_PUMP_2_PIN, LOW); 
}

void controlPumps(float pHValue, float ecValue) {
    // Turn off all pumps
    offPumps();

    checkEc:
    // EC-based pump control
    if (ecValue >= ecHigh) {
        // EC is high, dispense water
        Serial.println("EC value too high! Dispensing water! ");
        
        controlWater();
        
        delay(pumpBuffer); // Add 5-cycle buffer between dispenses
        goto checkPh;
    } else if (ecValue <= ecLow) {
        // EC is low, dispense water, basic solution1, and basic solution2
        Serial.println("EC value too low! Dispensing water, nutrient solution 1, and nutrient solution 2! ");
        
        controlWater();
        controlBase(); 
        
        delay(pumpBuffer); // Add 5-cycle buffer between dispenses
        goto checkPh;
    }
    
    checkPh:
    // pH-based pump control
    if (pHValue >= pHDesiredMin && pHValue <= pHDesiredMax) {
        // Desired pH range, no pumps need to be turned on
        return;
    } else if (pHValue <= pHFloor) {
        // pH below floor, dispense water, basic solution1, and basic solution2
        Serial.println("pH value too low! Dispensing water, nutrient solution 1, and nutrient solution 2! ");
        
        controlWater();
        controlBase();
        
        delay(pumpBuffer); // Add 5-cycle buffer between dispenses
        return;
    } else if (pHValue >= pHCeiling) {
        // pH above ceiling, dispense water and acidic solution
        Serial.println("pH value too low! Dispensing water and acidic solution! ");
                
        controlWater();        
        controlAcid();

        delay(pumpBuffer); // Add 5-cycle buffer between dispenses
        return;
    }
}

float readTemperature() {
  // Add code if temp sensor would be used.
  
  // Can't currently add a working implementation due to no way of connecting temperature probe to microcontroller.
}

void loop() {
    int pHMeasurements = 0;
    int ecMeasurements = 0;
    int temperatureMeasurements = 0;

    for (int i = 0; i < samples; i++) {
        pHMeasurements += analogRead(PH_SENS_PIN);
        ecMeasurements += analogRead(EC_SENS_PIN)/adcResolution*5000;
        
        // Call read temperature, uncomment if an implementation is added.
        // temperatureMeasurements += readTemperature();
        
        delay(10);
    }

    float pHVoltage = 5.0 / adcResolution * pHMeasurements / samples;
    float pHValue = getpH(pHVoltage);
    
    float ecVoltage = ecMeasurements / samples;

    // Uncomment if using a temperature probe
    // temperature = temperatureMeasurements / samples;
    float ecValue = ec.readEC(ecVoltage, temperature);

    // Hard-coded testing
// 
    
    controlPumps(pHValue, ecValue);

    Serial.print("pH = ");
    Serial.print(pHValue);
    Serial.print(", EC = ");
    Serial.println(ecValue);

    delay(3000);
}
