#pragma once

#include <Wire.h>
#include <SPI.h>
#include <Arduino.h>
#include <array>
#include <algorithm>

#include <BMI088.h>
#include <SparkFun_KX13X.h> // Click here to get the library: http://librarymanager/All#SparkFun_KX13X
#include <Adafruit_BMP280.h>
#include <Adafruit_MAX31855.h>

#include "States/States.h"

// data chunk format
// |----------------------|-----------|-----------|
// |      data            |   size    | data type |
// |----------------------|-----------|-----------|
// | timestamp            | 04 bytes  | u32       |
// | rocket state         | 01 byte   | u8        |
// | acceleration x axis  | 02 bytes  | float16   |
// | acceleration y axis  | 02 bytes  | float16   |
// | acceleration z axis  | 02 bytes  | float16   |
// | gyroscope x axis     | 02 bytes  | float16   |
// | gyroscope y axis     | 02 bytes  | float16   |
// | gyroscope z axis     | 02 bytes  | float16   |
// | relative altitude    | 02 bytes  | float16   |
// | pressure             | 04 bytes  | float32   |
// | thermocouple temp    | 02 bytes  | float16   |
// |----------------------|-----------|-----------|
// | total                | 25 bytes  |           |
// |----------------------|-----------|-----------|

struct SensorData
{
	FlightState m_State;
	// Struct for the accelerometer's data
	outputData m_AccelerometerData;

	struct Gyro
	{
		float x;
		float y;
		float z;
	} m_Gyro;

	float m_RelativeAltitude{0};
	// pressure
	float m_BarometerVal{0};

	float m_Temperature{0};



	/* Sensor - needs to store data from the following sensors:
	 - accelerometer -kx134 - 8bit, 16bit https://www.mouser.ca/ProductDetail/Kionix/KX134-1211?qs=BJlw7L4Cy79%2FET%2FI6G7icQ%3D%3D

	 - scrapped - gyroscope 6axis includes accelerometer -bmi088
	 - magnetometer -NA
	 - find one which does all 3 of above sensors
	 - barometer -bmp280 8bit https://www.bosch-sensortec.com/products/environmental-sensors/pressure-sensors/bmp280/#software
	 - thermocouple - MAX31855 https://learn.adafruit.com/thermocouple/downloads
	 - fram - adafruit i2c fram

	 - flight state
	*/

	/*
	bmp280 :   Struct BMP280_data { float Temperature , float  Pressure, float Altitude}

	fram: returns an 8-bit integer when reading a memory address

	MAX31855: float for the temperature

	kx134: Struct accelerometer_data {float X, float Y, float Z}
	*/

};

class Sensor
{
public:
	/*
	 * Thermocouple : int8_t MaxClk, int8_t MaxCS, int8_t MaxDO
	 * Bmi : uint8_t AccelAddr, uint8_t GyroAddr
	 */
	Sensor(std::array<int8_t, 3> ThermocouplePin, std::array<uint8_t, 2> BmiPin, uint8_t KxAccelPin) :
		m_Thermocouple(Adafruit_MAX31855(ThermocouplePin[0], ThermocouplePin[1], ThermocouplePin[2])),
		m_Bmi(Bmi088(Wire, BmiPin[0], BmiPin[1])),
		m_KxAccelPin(KxAccelPin){}

	template<typename T> requires std::is_arithmetic_v<T>
	static T Average(T New, T Old, float NewPercentage)
	{
		NewPercentage = std::clamp(NewPercentage, 0.f, 1.f);
		return NewPercentage * New + (1.f - NewPercentage) * Old;
	}

	void Setup();

	void ReadAcceleration();

	void ReadBarometer();

	void ReadThermocouple();

	void ReadSensorData();

	SensorData GetData();

private:

	SensorData m_SD;

	Adafruit_MAX31855 m_Thermocouple;
	Bmi088 m_Bmi;
	SparkFun_KX132 m_KxAccel;
	// SparkFun_KX134 kxAccel; // For the KX134, uncomment this and comment line above

	// I2C
	Adafruit_BMP280 m_Bmp;

	const uint8_t m_KxAccelPin;

};






