#include "Sensor.h"


void Sensor::Setup()
{
    // Wire.begin();

    //KX134 I2C Setup
    // Serial.begin(115200);
    // Serial.println("Welcome.");

    // //Wait for the Serial monitor to be opened.
    // while (!Serial)
    // delay(50);
    // if (!kxAccel.begin())
    // {
    // Serial.println("Could not communicate with the the KX13X. Freezing.");
    // // while (1)
    // //     ;
    // }
    // Serial.println("Ready.");

    // if (kxAccel.softwareReset())
    // {
    //     Serial.println("Reset.");

    //     // Give some time for the accelerometer to reset.
    //     // It needs two, but give it five for good measure.
    //     delay(5);

    //     // Many settings for KX13X can only be
    //     // applied when the accelerometer is powered down.
    //     // However there are many that can be changed "on-the-fly"
    //     // check datasheet for more info, or the comments in the
    //     // "...regs.h" file which specify which can be changed when.
    //     kxAccel.enableAccel(false);

    //     kxAccel.setRange(SFE_KX132_RANGE16G); // 16g Range
    //     // kxAccel.setRange(SFE_KX134_RANGE16G);         // 16g for the KX134

    //     kxAccel.enableDataEngine(); // Enables the bit that indicates data is ready.
    //     // kxAccel.setOutputDataRate(); // Default is 50Hz
    //     kxAccel.enableAccel();
    // }
    //KX134 SPI SETUP

    // Get the chip select pin ready.
    pinMode(m_KxAccelPin, OUTPUT);
    digitalWrite(m_KxAccelPin, HIGH);

    SPI.begin();

    Serial.begin(115200);
    Serial.println("Welcome.");

    // Wait for the Serial monitor to be opened.
    while (!Serial) { delay(50); }

    if (!m_KxAccel.begin(m_KxAccelPin))
    {
        Serial.println("Could not communicate with the the KX13X. Freezing.");
    }

    Serial.println("Ready.");

    // Reset the chip so that old settings don't apply to new setups.
    if (m_KxAccel.softwareReset())
    {
	    Serial.println("Reset.");
	}

    // Give some time for the accelerometer to reset.
    // It needs two, but give it five for good measure.
    delay(5);

    // Many settings for KX13X can only be
    // applied when the accelerometer is powered down.
    // However there are many that can be changed "on-the-fly"
    // check datasheet for more info, or the comments in the
    // "...regs.h" file which specify which can be changed when.
	m_KxAccel.enableAccel(false);

	m_KxAccel.setRange(SFE_KX132_RANGE16G); // 16g Range
    // kxAccel.setRange(SFE_KX134_RANGE16G);         // 16g for the KX134

	m_KxAccel.enableDataEngine(); // Enables the bit that indicates data is ready.
    // kxAccel.setOutputDataRate(); // Default is 50Hz
    m_KxAccel.enableAccel();
    //END KX134 SETUP

    //START OF BMP280 SETUP
    Serial.println(F("BMP280 test"));
    unsigned status;
    
    status = m_Bmp.begin();
    if (!status) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                        "try a different address!"));
    Serial.print("SensorID was: 0x"); Serial.println(m_Bmp.sensorID(),16);
    Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
    Serial.print("        ID of 0x60 represents a BME 280.\n");
    Serial.print("        ID of 0x61 represents a BME 680.\n");
    }
    /* Default settings from datasheet. */
    m_Bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                    Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                    Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                    Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                    Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
    //END OF BMP280 SETUP

    //START OF MAX31855 SETUP

    Serial.println("MAX31855 test");
    // wait for MAX chip to stabilize
    delay(500);
    Serial.print("Initializing sensor...");
    if (!m_Thermocouple.begin()) {
    Serial.println("ERROR.");
    while (1) delay(10);
    }

    // OPTIONAL: Can configure fault checks as desired (default is ALL)
    // Multiple checks can be logically OR'd together.
    //// thermocouple.setFaultChecks(MAX31855_FAULT_OPEN | MAX31855_FAULT_SHORT_VCC);  // short to GND fault is ignored

    Serial.println("DONE.");
    //END OF MAX31855 SETUP


	if (m_Bmi.begin() < 0) { Serial.println("Bmi failed to intialized"); }

}

void Sensor::ReadAcceleration(){
    // Check if data is ready.
    if (m_KxAccel.dataReady())
    {
        m_KxAccel.getAccelData(&m_SD.m_AccelerometerData);
        Serial.print("X: ");
        Serial.print(m_SD.m_AccelerometerData.xData, 4);
        Serial.print(" Y: ");
        Serial.print(m_SD.m_AccelerometerData.yData, 4);
        Serial.print(" Z: ");
        Serial.print(m_SD.m_AccelerometerData.zData, 4);
        Serial.println();
    }
    //delay(20); // Delay should be 1/ODR (Output Data Rate), default is 1/50ODR
}

void Sensor::ReadBarometer(){

    //m_SD.m_Temperature = m_Bmp.readTemperature();
    m_SD.m_BarometerVal = m_Bmp.readPressure();
    m_SD.m_RelativeAltitude = m_Bmp.readAltitude(1013.25); /* Adjusted to local forecast! */

    //Adafruit_BMP280 bmp(BMP_CS); // hardware SPI
    //Adafruit_BMP280 bmp(BMP_CS, BMP_MOSI, BMP_MISO,  BMP_SCK);
    //Serial.print(F("Temperature = "));
    //Serial.print(m_SD.m_Temperature);
    //Serial.println(" *C");

    Serial.print(F("Pressure = "));
    Serial.print( m_SD.m_BarometerVal);
    Serial.println(" Pa");

    Serial.print(F("Approx altitude = "));
    Serial.print( m_SD.m_RelativeAltitude); /* Adjusted to local forecast! */
    Serial.println(" m");

    Serial.println();
    //delay(2000);

}

void Sensor::ReadThermocouple(){

    // Example creating a thermocouple instance with hardware SPI
    // on a given CS pin.
    //#define MAXCS   10
    //Adafruit_MAX31855 thermocouple(MAXCS);

    // Example creating a thermocouple instance with hardware SPI
    // on SPI1 using specified CS pin.
    //#define MAXCS   10
    //Adafruit_MAX31855 thermocouple(MAXCS, SPI1);

    // basic readout test, just print the current temp
    Serial.print("Internal Temp = ");
    Serial.println(m_Thermocouple.readInternal());

    double c = m_Thermocouple.readCelsius();
    if (isnan(c))
	{
        Serial.println("Thermocouple fault(s) detected!");
        uint8_t e = m_Thermocouple.readError();
        if (e & MAX31855_FAULT_OPEN) Serial.println("FAULT: Thermocouple is open - no connections.");
        if (e & MAX31855_FAULT_SHORT_GND) Serial.println("FAULT: Thermocouple is short-circuited to GND.");
        if (e & MAX31855_FAULT_SHORT_VCC) Serial.println("FAULT: Thermocouple is short-circuited to VCC.");
    }
	else
	{
        Serial.print("C = ");
        Serial.println(c);
    }
    //Serial.print("F = ");
    //Serial.println(thermocouple.readFahrenheit());

    //delay(1000);
}

void Sensor::ReadSensorData()
{
    ReadAcceleration();

    ReadBarometer();

    ReadThermocouple();

    delay(2000);
}

SensorData Sensor::GetData() { return m_SD; }