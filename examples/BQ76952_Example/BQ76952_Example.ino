/*
* Description :   Custom software for the OpenBMS project by Martin Jäger, Lead Developer & Founder | Libre Solar
* Author      :   James Fotherby
* Date        :   27/11/2024
* License     :   MIT
* This code is published as open source software. Feel free to share/modify.
*
*
*  - This example runs on the ESP32-C3 on the OpenBMS hardware. Current sense resistor = 300uR 
*  - Configures the BQ76952 and all of its parameters to be suitable for a 16S LiFePO4 315Ah home battery system powering a 5000W Vitron inverter 
*/
//d:\BMS\Arduino BMS\BQ76952\src\BQ76952.cpp d:\BMS\Arduino BMS\BQ76952\src\BQ76952.h
#include <Wire.h>
#include <BQ76952.h>

#define LED_GREEN_PIN       0
#define LED_RED_PIN         1
#define ALERT_PIN           2
#define BUTTON_LOW_PIN      3

#define I2C_SDA_PIN         8
#define I2C_SCL_PIN         9                                       // Pulling to ground and reseting device enters it into BOOT mode

BQ76952 bms;

void setup() {
  Serial.begin(115200);
  
  // Note: On Arduino Uno, I2C pins are fixed to A4 (SDA) and A5 (SCL). 
  // The pin arguments are ignored by our library on Uno.
  bms.begin(I2C_SDA_PIN, I2C_SCL_PIN);

  // Disable Arduino Uno internal pull-ups (A4/A5) to prevent 5V back-feeding into the 3.3V rail.
  // The board's own 4.7k pull-up resistors will pull the lines to 3.3V safely.
  #if defined(__AVR_ATmega328P__) || defined(__AVR__)
    digitalWrite(SDA, LOW);
    digitalWrite(SCL, LOW);
  #endif

  // Check if BQ76952 / BQ76942 is responding on the I2C bus
  delay(100);
  Wire.beginTransmission(0x08); // Default 7-bit I2C address for BQ76952/BQ76942
  byte error = Wire.endTransmission();
  
  if (error == 0) {
    Serial.println(F("[SUCCESS] I2C connection to BQ76952/BQ76942 is working!"));
  } else {
    Serial.println(F("[ERROR] Cannot find BQ76952/BQ76942. Please check your wiring and press the K1 WAKE button."));
    while (1) {
      delay(1000);
    }
  }

  bms.reset();
  delay(100); 

// Configure the BQ76952
bms.setConnectedCells(16);          // Configure for 16-series cells
bms.writeByteToMemory(FET_Options, 0x1D);
bms.writeByteToMemory(DA_Configuration, 0x06);

bms.writeByteToMemory(Enabled_Protections_A, 0b10101100);
bms.writeByteToMemory(CUV_Threshold, 60);
bms.writeByteToMemory(COV_Threshold, 67);
bms.writeByteToMemory(OCD1_Threshold, 25);
bms.writeByteToMemory(OCD1_Delay, 127);
bms.writeByteToMemory(SCD_Threshold, SCD_80);
bms.writeByteToMemory(SCD_Delay, 2);

bms.writeIntToMemory(Mfg_Status_Init, 0x0050);

bms.setFET(ALL, ON);
}

void loop()
{
    delay(1000);

    Serial.println();
    Serial.println("========== BQ76952 16S Battery ==========");

    // Read Cell Voltages
    for (uint8_t cell = 1; cell <= 16; cell++)
    {
        uint16_t mv = bms.getCellVoltage(cell);

        Serial.print("Cell ");
        if (cell < 10) Serial.print("0");
        Serial.print(cell);
        Serial.print(" : ");
        Serial.print(mv / 1000.0, 3); 
        Serial.println(" V");
    }

    // Stack Voltage
    float stackVoltage = bms.getCellVoltage(17) * 0.01;

    // Pack Voltage
    float packVoltage = bms.getCellVoltage(18) * 0.01;

    // Current
    float current = bms.getCurrent() * 0.01;

    Serial.println("----------------------------------------");

    Serial.print("Stack Voltage : ");
    Serial.print(stackVoltage, 2);
    Serial.println(" V");

    Serial.print("Pack Voltage  : ");
    Serial.print(packVoltage, 2);
    Serial.println(" V");

    Serial.print("Current       : ");
    Serial.print(current, 2);
    Serial.println(" A");

    Serial.println("========================================");
}





