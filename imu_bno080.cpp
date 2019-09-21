# include "main.h"

#ifdef BNO080_IMU

IMU_BNO080::IMU_BNO080(uint8_t deviceAddress)
{
	_deviceAddress = deviceAddress; // if provided, store the I2C address from user
	
	for(uint32_t i = 0; i < sizeof(sequenceNumber); i++) sequenceNumber[i] = 0;
	
	commandSequenceNumber = 0;
	_printDebug = false;
	rotationVector_Q1 = 14;
	accelerometer_Q1 = 8;
	linear_accelerometer_Q1 = 8;
	gyro_Q1 = 9;
	magnetometer_Q1 = 4;
}

IMU_BNO080::~IMU_BNO080()
{}


bool IMU_BNO080::lowLevelInit()
{
	I2C_InitTypeDef I2C_InitStructure;
	uint8_t gpioAltFunctionSel;
	
	if(IMU_I2C == I2C1)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
		gpioAltFunctionSel = GPIO_AF_I2C1;
	}
	else if(IMU_I2C == I2C2)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
		gpioAltFunctionSel = GPIO_AF_I2C2;
	}
	else if(IMU_I2C == I2C3)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C3, ENABLE);
		gpioAltFunctionSel = GPIO_AF_I2C3;
	}
	else
	{
		return false;
	}
	
	// configurates GPIO ports Reset Pin
	TM_GPIO_Init(
		BNO080_RESET_GPIO_PORT,
		BNO080_RESET_PIN,
		TM_GPIO_Mode_OUT,
		TM_GPIO_OType_PP,
		TM_GPIO_PuPd_UP,
		TM_GPIO_Speed_High
	);
	
	// configurates GPIO ports for I2C SCL and SDA
	TM_GPIO_InitAlternate(
		IMU_I2C_GPIO_PORT, 
		IMU_I2C_SCL_GPIO_PIN | IMU_I2C_SDA_GPIO_PIN,
		TM_GPIO_OType_PP,
		TM_GPIO_PuPd_NOPULL,
		TM_GPIO_Speed_Fast,
		gpioAltFunctionSel
	);
	
	I2C::structInit(&I2C_InitStructure);
	I2C_InitStructure.I2C_Ack = I2C_Ack_Disable;
	I2C_InitStructure.I2C_ClockSpeed = I2C_SPEED;
	
	I2C::init(IMU_I2C, &I2C_InitStructure);
	I2C::intConfig(IMU_I2C, I2C_IT_EVT | I2C_IT_BUF, DISABLE);
	I2C::cmd(IMU_I2C, ENABLE);
	
	return true;
}

// Attempt communication with the device
// Return true if we got a 'Polo' back from Marco
bool IMU_BNO080::begin(bool executeLowLevelInit)
{
	uint32_t result;

	if(executeLowLevelInit)
	{
		if(!lowLevelInit()) return false;
	}

	// execute hardware reset per low signal on rst pin
	hardReset();
	
	// Begin by resetting the IMU
//	if(!softReset()) return false;

	// Check communication with device
	shtpData[0] = SHTP_REPORT_PRODUCT_ID_REQUEST; //Request the product ID and reset info
	shtpData[1] = 0; //Reserved
	
	// Transmit packet on channel 2, 2 bytes
	result = sendPacket(CHANNEL_CONTROL, 2);
	if(result != EXIT_SUCCESS)
	{
		Debug::print("sendPacket() returns %s in begin()\n", RETURN_VALUES[result]);
		return false;
	}
	
	// Now we wait for response
	if(receivePacket())
	{
		if(shtpData[0] == SHTP_REPORT_PRODUCT_ID_RESPONSE)
		{
			return true;
		}
		else
		{
			Debug::print("receivePacket failed, data is not SHTP_REPORT_PRODUCT_ID_RESPONSE data: %i\n", shtpData[0]);
		}
	}
	else
	{
		Debug::print("receivePacket failed!\n");
	}

	return false; // Something went wrong
}

void IMU_BNO080::readImuData(ImuData* actualImuData)
{
	
}

void IMU_BNO080::hardReset()
{
	BNO080_RESET_GPIO_PORT->BSRR = GPIO_BSRR_BR_15;
	delay(10);
	BNO080_RESET_GPIO_PORT->BSRR = GPIO_BSRR_BS_15;
	delay(100);
	
	while(receivePacket());
	delay(100);
	while(receivePacket());
}

// Calling this function with nothing sets the debug port to Serial
// You can also call it with other streams like Serial1, SerialUSB, etc.
void IMU_BNO080::enableDebugging()
{
	_printDebug = true;
}

// Updates the latest variables if possible
// Returns false if new readings are not available
bool IMU_BNO080::dataAvailable()
{
	if(receivePacket())
	{
		// Check to see if this packet is a sensor reporting its data to us
		if(shtpHeader[2] == CHANNEL_REPORTS && shtpData[0] == SHTP_REPORT_BASE_TIMESTAMP)
		{
			parseInputReport(); // This will update the rawAccelX, etc variables depending on which feature report is found
			
			return true;
		}
	}
	
	return false;
}

// This function pulls the data from the input report
// The input reports vary in length so this function stores the various 16-bit values as globals

// Unit responds with packet that contains the following:
// shtpHeader[0:3]: First, a 4 byte header
// shtpData[0:4]: Then a 5 byte timestamp of microsecond clicks since reading was taken
// shtpData[5 + 0]: Then a feature report ID (0x01 for Accel, 0x05 for Rotation Vector)
// shtpData[5 + 1]: Sequence number (See 6.5.18.2)
// shtpData[5 + 2]: Status
// shtpData[3]: Delay
// shtpData[4:5]: i/accel x/gyro x/etc
// shtpData[6:7]: j/accel y/gyro y/etc
// shtpData[8:9]: k/accel z/gyro z/etc
// shtpData[10:11]: real/gyro temp/etc
// shtpData[12:13]: Accuracy estimate
void IMU_BNO080::parseInputReport()
{
	// Calculate the number of data bytes in this packet
	int16_t dataLength = ((uint16_t)shtpHeader[1] << 8 | shtpHeader[0]);
	dataLength &= ~(1 << 15); // Clear the MSbit. This bit indicates if this package is a continuation of the last. 
	// Ignore it for now. TODO catch this as an error and exit

	dataLength -= 4; // Remove the header bytes from the data count
	
	uint8_t status = shtpData[5 + 2] & 0x03; //Get status bits
	uint16_t data1 = (uint16_t)shtpData[5 + 5] << 8 | shtpData[5 + 4];
	uint16_t data2 = (uint16_t)shtpData[5 + 7] << 8 | shtpData[5 + 6];
	uint16_t data3 = (uint16_t)shtpData[5 + 9] << 8 | shtpData[5 + 8];
	uint16_t data4 = 0;
	uint16_t data5 = 0;

	if(dataLength - 5 > 9)
	{
		data4 = (uint16_t)shtpData[5 + 11] << 8 | shtpData[5 + 10];
	}
	
	if(dataLength - 5 > 11)
	{
		data5 = (uint16_t)shtpData[5 + 13] << 8 | shtpData[5 + 12];
	}
	
	// Store these generic values to their proper global variable
	if(shtpData[5] == SENSOR_REPORTID_ACCELEROMETER)
	{
		accelAccuracy = status;
		rawAccelX = data1;
		rawAccelY = data2;
		rawAccelZ = data3;
	}
	else if(shtpData[5] == SENSOR_REPORTID_LINEAR_ACCELERATION)
	{
		accelLinAccuracy = status;
		rawLinAccelX = data1;
		rawLinAccelY = data2;
		rawLinAccelZ = data3;
	}
	else if(shtpData[5] == SENSOR_REPORTID_GYROSCOPE)
	{
		gyroAccuracy = status;
		rawGyroX = data1;
		rawGyroY = data2;
		rawGyroZ = data3;
	}
	else if(shtpData[5] == SENSOR_REPORTID_MAGNETIC_FIELD)
	{
		magAccuracy = status;
		rawMagX = data1;
		rawMagY = data2;
		rawMagZ = data3;
	}
	else if(shtpData[5] == SENSOR_REPORTID_ROTATION_VECTOR || shtpData[5] == SENSOR_REPORTID_GAME_ROTATION_VECTOR)
	{
		quatAccuracy = status;
		rawQuatI = data1;
		rawQuatJ = data2;
		rawQuatK = data3;
		rawQuatReal = data4;
		rawQuatRadianAccuracy = data5; // Only available on rotation vector, not game rot vector
	}
	else if(shtpData[5] == SENSOR_REPORTID_STEP_COUNTER)
	{
		stepCount = data3; // Bytes 8/9
	}
	else if(shtpData[5] == SENSOR_REPORTID_STABILITY_CLASSIFIER)
	{
		stabilityClassifier = shtpData[5 + 4]; // Byte 4 only
	}
	else if(shtpData[5] == SENSOR_REPORTID_PERSONAL_ACTIVITY_CLASSIFIER)
	{
		activityClassifier = shtpData[5 + 5]; // Most likely state
		
		// Load activity classification confidences into the array
		for(uint8_t x = 0 ; x < 9 ; x++) // Hardcoded to max of 9. TODO - bring in array size
		{
			_activityConfidences[x] = shtpData[5 + 6 + x]; // 5 bytes of timestamp, byte 6 is first confidence byte
		}
	}
	else
	{
		// This sensor report ID is unhandled.
		// See reference manual to add additional feature reports as needed
	}
	
	// TODO additional feature reports may be strung together. Parse them all.
}

// Return the rotation vector quaternion I
float IMU_BNO080::getQuatI()
{
	float quat = qToFloat(rawQuatI, rotationVector_Q1);
	
	return quat;
}

// Return the rotation vector quaternion J
float IMU_BNO080::getQuatJ()
{
	float quat = qToFloat(rawQuatJ, rotationVector_Q1);
	
	return quat;
}

// Return the rotation vector quaternion K
float IMU_BNO080::getQuatK()
{
	float quat = qToFloat(rawQuatK, rotationVector_Q1);
	
	return quat;
}

// Return the rotation vector quaternion Real
float IMU_BNO080::getQuatReal()
{
	float quat = qToFloat(rawQuatReal, rotationVector_Q1);
	
	return quat;
}

// Return the rotation vector accuracy
float IMU_BNO080::getQuatRadianAccuracy()
{
	float quat = qToFloat(rawQuatRadianAccuracy, rotationVector_Q1);
	
	return quat;
}

// Return the acceleration component
uint8_t IMU_BNO080::getQuatAccuracy()
{
	return quatAccuracy;
}

// Return the acceleration component
float IMU_BNO080::getAccelX()
{
	float accel = qToFloat(rawAccelX, accelerometer_Q1);
	
	return accel;
}

// Return the acceleration component
float IMU_BNO080::getAccelY()
{
	float accel = qToFloat(rawAccelY, accelerometer_Q1);
	
	return accel;
}

// Return the acceleration component
float IMU_BNO080::getAccelZ()
{
	float accel = qToFloat(rawAccelZ, accelerometer_Q1);
	
	return accel;
}

// Return the acceleration component
uint8_t IMU_BNO080::getAccelAccuracy()
{
	return accelAccuracy;
}

// linear acceleration, i.e. minus gravity

// Return the acceleration component
float IMU_BNO080::getLinAccelX()
{
	float accel = qToFloat(rawLinAccelX, linear_accelerometer_Q1);
	
	return accel;
}

// Return the acceleration component
float IMU_BNO080::getLinAccelY()
{
	float accel = qToFloat(rawLinAccelY, linear_accelerometer_Q1);
	
	return accel;
}

// Return the acceleration component
float IMU_BNO080::getLinAccelZ()
{
	float accel = qToFloat(rawLinAccelZ, linear_accelerometer_Q1);
	
	return accel;
}

// Return the acceleration component
uint8_t IMU_BNO080::getLinAccelAccuracy()
{
	return accelLinAccuracy;
}

// Return the gyro component
float IMU_BNO080::getGyroX()
{
	float gyro = qToFloat(rawGyroX, gyro_Q1);
	
	return gyro;
}

// Return the gyro component
float IMU_BNO080::getGyroY()
{
	float gyro = qToFloat(rawGyroY, gyro_Q1);
	
	return gyro;
}

// Return the gyro component
float IMU_BNO080::getGyroZ()
{
	float gyro = qToFloat(rawGyroZ, gyro_Q1);
	
	return gyro;
}

// Return the gyro component
uint8_t IMU_BNO080::getGyroAccuracy()
{
	return gyroAccuracy;
}

// Return the magnetometer component
float IMU_BNO080::getMagX()
{
	float mag = qToFloat(rawMagX, magnetometer_Q1);
	
	return mag;
}

// Return the magnetometer component
float IMU_BNO080::getMagY()
{
	float mag = qToFloat(rawMagY, magnetometer_Q1);
	
	return mag;
}

// Return the magnetometer component
float IMU_BNO080::getMagZ()
{
	float mag = qToFloat(rawMagZ, magnetometer_Q1);
	
	return mag;
}

// Return the mag component
uint8_t IMU_BNO080::getMagAccuracy()
{
	return magAccuracy;
}

// Return the step count
uint16_t IMU_BNO080::getStepCount()
{
	return stepCount;
}

// Return the stability classifier
uint8_t IMU_BNO080::getStabilityClassifier()
{
	return stabilityClassifier;
}

// Return the activity classifier
uint8_t IMU_BNO080::getActivityClassifier()
{
	return activityClassifier;
}

// Given a record ID, read the Q1 value from the metaData record in the FRS (ya, it's complicated)
// Q1 is used for all sensor data calculations
int16_t IMU_BNO080::getQ1(uint16_t recordID)
{
  // Q1 is always the lower 16 bits of word 7
  uint16_t q = readFRSword(recordID, 7) & 0xFFFF; //Get word 7, lower 16 bits
  
	return q;
}

// Given a record ID, read the Q2 value from the metaData record in the FRS
// Q2 is used in sensor bias
int16_t IMU_BNO080::getQ2(uint16_t recordID)
{
  // Q2 is always the upper 16 bits of word 7
  uint16_t q = readFRSword(recordID, 7) >> 16; //Get word 7, upper 16 bits
  
	return q;
}

// Given a record ID, read the Q3 value from the metaData record in the FRS
// Q3 is used in sensor change sensitivity
int16_t IMU_BNO080::getQ3(uint16_t recordID)
{
  // Q3 is always the upper 16 bits of word 8
  uint16_t q = readFRSword(recordID, 8) >> 16; //Get word 8, upper 16 bits
  
	return q;
}

// Given a record ID, read the resolution value from the metaData record in the FRS for a given sensor
float IMU_BNO080::getResolution(uint16_t recordID)
{
  // The resolution Q value are 'the same as those used in the sensor's input report'
  // This should be Q1.
  int16_t Q = getQ1(recordID);
  
  // Resolution is always word 2
  uint32_t value = readFRSword(recordID, 2); //Get word 2

  float resolution = qToFloat(value, Q);

  return resolution;
}

// Given a record ID, read the range value from the metaData record in the FRS for a given sensor
float IMU_BNO080::getRange(uint16_t recordID)
{
  // The resolution Q value are 'the same as those used in the sensor's input report'
  // This should be Q1.
  int16_t Q = getQ1(recordID);
  
  // Range is always word 1
  uint32_t value = readFRSword(recordID, 1); // Get word 1

  float range = qToFloat(value, Q);

  return range;
}

// Given a record ID and a word number, look up the word data
// Helpful for pulling out a Q value, range, etc.
// Use readFRSdata for pulling out multi-word objects for a sensor (Vendor data for example)
uint32_t IMU_BNO080::readFRSword(uint16_t recordID, uint8_t wordNumber)
{
	// Get word number, just one word in length from FRS
	if(readFRSdata(recordID, wordNumber, 1) == true) return metaData[0]; // Return this one word

	return 0; // Error
}

// Ask the sensor for data from the Flash Record System
// See 6.3.6 page 40, FRS Read Request
void IMU_BNO080::frsReadRequest(uint16_t recordID, uint16_t readOffset, uint16_t blockSize)
{
	uint32_t result;
	
  shtpData[0] = SHTP_REPORT_FRS_READ_REQUEST; // FRS Read Request
  shtpData[1] = 0; // Reserved
  shtpData[2] = (readOffset >> 0) & 0xFF; // Read Offset LSB
  shtpData[3] = (readOffset >> 8) & 0xFF; // Read Offset MSB
  shtpData[4] = (recordID >> 0) & 0xFF; // FRS Type LSB
  shtpData[5] = (recordID >> 8) & 0xFF; // FRS Type MSB
  shtpData[6] = (blockSize >> 0) & 0xFF; // Block size LSB
  shtpData[7] = (blockSize >> 8) & 0xFF; // Block size MSB

  // Transmit packet on channel 2, 8 bytes
	result = sendPacket(CHANNEL_CONTROL, 8);
	if(result != EXIT_SUCCESS) Debug::print("sendPacket() returns %s in in frsReadRequest()\n", RETURN_VALUES[result]);
}

// Given a sensor or record ID, and a given start/stop bytes, read the data from the Flash Record System (FRS) for this sensor
// Returns true if metaData array is loaded successfully
// Returns false if failure
bool IMU_BNO080::readFRSdata(uint16_t recordID, uint8_t startLocation, uint8_t wordsToRead)
{
  uint8_t spot = 0;

  // First we send a Flash Record System (FRS) request
  frsReadRequest(recordID, startLocation, wordsToRead); //From startLocation of record, read a # of words

  // Read bytes until FRS reports that the read is complete
  while(1)
  {
    // Now we wait for response
    while(1)
    {
			uint8_t counter = 0;
			
			while(!receivePacket())
			{
				if(counter++ > 100) return false; // Give up
				delay(1);
			}

			// We have the packet, inspect it for the right contents
			// See page 40. Report ID should be 0xF3 and the FRS types should match the thing we requested
			if(shtpData[0] == SHTP_REPORT_FRS_READ_RESPONSE)
			if(((uint16_t)shtpData[13] << 8 | shtpData[12]) == recordID) break; // This packet is one we are looking for
    }

    uint8_t dataLength = shtpData[1] >> 4;
    uint8_t frsStatus = shtpData[1] & 0x0F;

    uint32_t data0 = (uint32_t)shtpData[7] << 24 | (uint32_t)shtpData[6] << 16 | (uint32_t)shtpData[5] << 8 | (uint32_t)shtpData[4];
    uint32_t data1 = (uint32_t)shtpData[11] << 24 | (uint32_t)shtpData[10] << 16 | (uint32_t)shtpData[9] << 8 | (uint32_t)shtpData[8];

    // Record these words to the metaData array
    if(dataLength > 0)
		{
			metaData[spot++] = data0;
		}
		
		if(dataLength > 1)
		{
			metaData[spot++] = data1;
		}
		
		if(spot >= MAX_METADATA_SIZE)
		{
			if(_printDebug) Debug::print("metaData array over run. Returning.\n");
			
			return true; // We have run out of space in our array. Bail.
		}
		
		if(frsStatus == 3 || frsStatus == 6 || frsStatus == 7)
		{
			return true; // FRS status is read completed! We're done!
    }
  }
}

// Send command to reset IC
// Read all advertisement packets from sensor
// The sensor has been seen to reset twice if we attempt too much too quickly.
// This seems to work reliably.
bool IMU_BNO080::softReset()
{
	uint32_t result;
	shtpData[0] = 1; //Reset
	
	// Attempt to start communication with sensor
	// Transmit packet on channel 1, 1 byte
	result = sendPacket(CHANNEL_EXECUTABLE, 1);
	if(result != EXIT_SUCCESS)
	{
		Debug::print("sendPacket() returns %s in in softReset()\n", RETURN_VALUES[result]);
		return false;
	}
	Debug::print("softReset() 1\n");
	// Read all incoming data and flush it
//	delay(100);
	while(receivePacket());
	Debug::print("softReset() 2\n");
	delay(100);
	while(receivePacket());
	
	return true;
}


// Get the reason for the last reset
// 1 = POR, 2 = Internal reset, 3 = Watchdog, 4 = External reset, 5 = Other
uint8_t IMU_BNO080::resetReason()
{
	uint32_t result;
	shtpData[0] = SHTP_REPORT_PRODUCT_ID_REQUEST; // Request the product ID and reset info
	shtpData[1] = 0; // Reserved
	
	// Transmit packet on channel 2, 2 bytes
	result = sendPacket(CHANNEL_CONTROL, 2);
	if(result != EXIT_SUCCESS)
	{
		Debug::print("sendPacket() returns %s in in resetReason()\n", RETURN_VALUES[result]);
		return false;
	}
	
	// Now we wait for response
	if(receivePacket())
	{
		if(shtpData[0] == SHTP_REPORT_PRODUCT_ID_RESPONSE) return shtpData[1];
	}
	
	return 0;
}

// Given a register value and a Q point, convert to float
// See https://en.wikipedia.org/wiki/Q_(number_format)
float IMU_BNO080::qToFloat(int16_t fixedPointValue, uint8_t qPoint)
{
  float qFloat = (float)fixedPointValue;
  qFloat *= pow(2.0f, qPoint * -1);
  
	return qFloat;
}

// Sends the packet to enable the rotation vector
void IMU_BNO080::enableRotationVector(uint16_t timeBetweenReports)
{
	setFeatureCommand(SENSOR_REPORTID_ROTATION_VECTOR, timeBetweenReports);
}


// Sends the packet to enable the rotation vector
void IMU_BNO080::enableGameRotationVector(uint16_t timeBetweenReports)
{
	setFeatureCommand(SENSOR_REPORTID_GAME_ROTATION_VECTOR, timeBetweenReports);
}

// Sends the packet to enable the accelerometer
void IMU_BNO080::enableAccelerometer(uint16_t timeBetweenReports)
{
	setFeatureCommand(SENSOR_REPORTID_ACCELEROMETER, timeBetweenReports);
}

// Sends the packet to enable the accelerometer
void IMU_BNO080::enableLinearAccelerometer(uint16_t timeBetweenReports)
{
	setFeatureCommand(SENSOR_REPORTID_LINEAR_ACCELERATION, timeBetweenReports);
}

// Sends the packet to enable the gyro
void IMU_BNO080::enableGyro(uint16_t timeBetweenReports)
{
	setFeatureCommand(SENSOR_REPORTID_GYROSCOPE, timeBetweenReports);
}

// Sends the packet to enable the magnetometer
void IMU_BNO080::enableMagnetometer(uint16_t timeBetweenReports)
{
	setFeatureCommand(SENSOR_REPORTID_MAGNETIC_FIELD, timeBetweenReports);
}

// Sends the packet to enable the step counter
void IMU_BNO080::enableStepCounter(uint16_t timeBetweenReports)
{
	setFeatureCommand(SENSOR_REPORTID_STEP_COUNTER, timeBetweenReports);
}

// Sends the packet to enable the Stability Classifier
void IMU_BNO080::enableStabilityClassifier(uint16_t timeBetweenReports)
{
	setFeatureCommand(SENSOR_REPORTID_STABILITY_CLASSIFIER, timeBetweenReports);
}

// Sends the packet to enable the various activity classifiers
void IMU_BNO080::enableActivityClassifier(uint16_t timeBetweenReports, uint32_t activitiesToEnable, uint8_t (&activityConfidences)[9])
{
	_activityConfidences = activityConfidences; //Store pointer to array	

	setFeatureCommand(SENSOR_REPORTID_PERSONAL_ACTIVITY_CLASSIFIER, timeBetweenReports, activitiesToEnable);
}

// Sends the commands to begin calibration of the accelerometer
void IMU_BNO080::calibrateAccelerometer()
{
	sendCalibrateCommand(CALIBRATE_ACCEL);
}

// Sends the commands to begin calibration of the gyro
void IMU_BNO080::calibrateGyro()
{
	sendCalibrateCommand(CALIBRATE_GYRO);
}

// Sends the commands to begin calibration of the magnetometer
void IMU_BNO080::calibrateMagnetometer()
{
	sendCalibrateCommand(CALIBRATE_MAG);
}

// Sends the commands to begin calibration of the planar accelerometer
void IMU_BNO080::calibratePlanarAccelerometer()
{
	sendCalibrateCommand(CALIBRATE_PLANAR_ACCEL);
}

// See 2.2 of the Calibration Procedure document 1000-4044
void IMU_BNO080::calibrateAll()
{
	sendCalibrateCommand(CALIBRATE_ACCEL_GYRO_MAG);
}

void IMU_BNO080::endCalibration()
{
	sendCalibrateCommand(CALIBRATE_STOP); // Disables all calibrations
}

// Given a sensor's report ID, this tells the BNO080 to begin reporting the values
void IMU_BNO080::setFeatureCommand(uint8_t reportID, uint16_t timeBetweenReports)
{
	setFeatureCommand(reportID, timeBetweenReports, 0); //No specific config
}

// Given a sensor's report ID, this tells the BNO080 to begin reporting the values
// Also sets the specific config word. Useful for personal activity classifier
void IMU_BNO080::setFeatureCommand(uint8_t reportID, uint16_t timeBetweenReports, uint32_t specificConfig)
{
	uint32_t result;
	uint32_t microsBetweenReports = (uint32_t)timeBetweenReports * 1000L;

	shtpData[0] = SHTP_REPORT_SET_FEATURE_COMMAND; // Set feature command. Reference page 55
	shtpData[1] = reportID; // Feature Report ID. 0x01 = Accelerometer, 0x05 = Rotation vector
	shtpData[2] = 0; // Feature flags
	shtpData[3] = 0; // Change sensitivity (LSB)
	shtpData[4] = 0; // Change sensitivity (MSB)
	shtpData[5] = (microsBetweenReports >> 0) & 0xFF; // Report interval (LSB) in microseconds. 0x7A120 = 500ms
	shtpData[6] = (microsBetweenReports >> 8) & 0xFF; // Report interval
	shtpData[7] = (microsBetweenReports >> 16) & 0xFF; // Report interval
	shtpData[8] = (microsBetweenReports >> 24) & 0xFF; // Report interval (MSB)
	shtpData[9] = 0; // Batch Interval (LSB)
	shtpData[10] = 0; // Batch Interval
	shtpData[11] = 0; // Batch Interval
	shtpData[12] = 0; // Batch Interval (MSB)
	shtpData[13] = (specificConfig >> 0) & 0xFF; // Sensor-specific config (LSB)
	shtpData[14] = (specificConfig >> 8) & 0xFF; // Sensor-specific config
	shtpData[15] = (specificConfig >> 16) & 0xFF; // Sensor-specific config
	shtpData[16] = (specificConfig >> 24) & 0xFF; // Sensor-specific config (MSB)

	// Transmit packet on channel 2, 17 bytes
	result = sendPacket(CHANNEL_CONTROL, 17);
	if(result != EXIT_SUCCESS) Debug::print("sendPacket() returns %s in in setFeatureCommand()\n", RETURN_VALUES[result]);
}

// Tell the sensor to do a command
// See 6.3.8 page 41, Command request
// The caller is expected to set P0 through P8 prior to calling
void IMU_BNO080::sendCommand(uint8_t command)
{
	uint32_t result;
	shtpData[0] = SHTP_REPORT_COMMAND_REQUEST; // Command Request
	shtpData[1] = commandSequenceNumber++; // Increments automatically each function call
	shtpData[2] = command; // Command

	//Caller must set these
	/*shtpData[3] = 0; //P0
	shtpData[4] = 0; //P1
	shtpData[5] = 0; //P2
	shtpData[6] = 0;
	shtpData[7] = 0;
	shtpData[8] = 0;
	shtpData[9] = 0;
	shtpData[10] = 0;
	shtpData[11] = 0;*/

	// Transmit packet on channel 2, 12 bytes
	result = sendPacket(CHANNEL_CONTROL, 12);
	if(result != EXIT_SUCCESS) Debug::print("sendPacket() returns %s in in sendCommand()\n", RETURN_VALUES[result]);
}

// This tells the BNO080 to begin calibrating
// See page 50 of reference manual and the 1000-4044 calibration doc
void IMU_BNO080::sendCalibrateCommand(uint8_t thingToCalibrate)
{
	/*shtpData[3] = 0; //P0 - Accel Cal Enable
	shtpData[4] = 0; //P1 - Gyro Cal Enable
	shtpData[5] = 0; //P2 - Mag Cal Enable
	shtpData[6] = 0; //P3 - Subcommand 0x00
	shtpData[7] = 0; //P4 - Planar Accel Cal Enable
	shtpData[8] = 0; //P5 - Reserved
	shtpData[9] = 0; //P6 - Reserved
	shtpData[10] = 0; //P7 - Reserved
	shtpData[11] = 0; //P8 - Reserved*/
	
	// Clear this section of the shtpData array
	for(uint8_t x = 3 ; x < 12 ; x++) shtpData[x] = 0;
	
	if(thingToCalibrate == CALIBRATE_ACCEL) shtpData[3] = 1;
	else if(thingToCalibrate == CALIBRATE_GYRO) shtpData[4] = 1;
	else if(thingToCalibrate == CALIBRATE_MAG) shtpData[5] = 1;
	else if(thingToCalibrate == CALIBRATE_PLANAR_ACCEL) shtpData[7] = 1;
	else if(thingToCalibrate == CALIBRATE_ACCEL_GYRO_MAG)
	{
		shtpData[3] = 1;
		shtpData[4] = 1;
		shtpData[5] = 1;
	}
	else if(thingToCalibrate == CALIBRATE_STOP) ; // Do nothing, bytes are set to zero

	// Using this shtpData packet, send a command
	sendCommand(COMMAND_ME_CALIBRATE);
}

// This tells the BNO080 to save the Dynamic Calibration Data (DCD) to flash
// See page 49 of reference manual and the 1000-4044 calibration doc
void IMU_BNO080::saveCalibration()
{
	/*shtpData[3] = 0; //P0 - Reserved
	shtpData[4] = 0; //P1 - Reserved
	shtpData[5] = 0; //P2 - Reserved
	shtpData[6] = 0; //P3 - Reserved
	shtpData[7] = 0; //P4 - Reserved
	shtpData[8] = 0; //P5 - Reserved
	shtpData[9] = 0; //P6 - Reserved
	shtpData[10] = 0; //P7 - Reserved
	shtpData[11] = 0; //P8 - Reserved*/
	
	// Clear this section of the shtpData array
	for(uint8_t x = 3 ; x < 12 ; x++) shtpData[x] = 0;

	// Using this shtpData packet, send a command
	sendCommand(COMMAND_DCD); //Save DCD command
}

// Check to see if there is any new data available
// Read the contents of the incoming packet into the shtpData array
bool IMU_BNO080::receivePacket()
{
	uint8_t packetLSB;
	uint8_t packetMSB;
	uint32_t result;
	
	result = readHeader(shtpHeader);
	if(result != EXIT_SUCCESS)
	{
		Debug::print("readHeader() returns %s in receivePacket()\n", RETURN_VALUES[result]);
		begin(false);
		return false;
	}
	
	packetLSB = shtpHeader[0];
	packetMSB = shtpHeader[1];
	
	// Calculate the number of data bytes in this packet
	int16_t dataLength = ((uint16_t)packetMSB << 8 | packetLSB);
	dataLength &= ~(1 << 15); //Clear the MSbit.
	// This bit indicates if this package is a continuation of the last. Ignore it for now.
	// TODO catch this as an error and exit
	if(dataLength == 0) return false; // Packet is empty, All done
	
	dataLength -= 4; // Remove the header bytes from the data count

	result = getData(dataLength);
	if(result != EXIT_SUCCESS)
	{
		Debug::print("getData() returns %s in receivePacket()\n", RETURN_VALUES[result]);
		begin(false);
		return false;
	}

	return true;
}

// Sends multiple requests to sensor until all data bytes are received from sensor
// The shtpData buffer has max capacity of MAX_PACKET_SIZE. Any bytes over this amount will be lost.
// Arduino I2C read limit is 32 bytes. Header is 4 bytes, so max data we can read per interation is 28 bytes
int IMU_BNO080::getData(uint16_t bytesRemaining)
{
	uint16_t dataSpot = 0; // Start at the beginning of shtpData array
	uint8_t tmp;
	

//	// Setup a series of chunked 32 byte reads
//	while(bytesRemaining > 0)
//	{
//		uint16_t numberOfBytesToRead = bytesRemaining;
//		if(numberOfBytesToRead > (I2C_BUFFER_LENGTH - 4)) numberOfBytesToRead = (I2C_BUFFER_LENGTH - 4);

//		_i2cPort->requestFrom((uint8_t)_deviceAddress, (uint8_t)(numberOfBytesToRead + 4));
//		if(waitForI2C() == false) return 0; // Error

//		// The first four bytes are header bytes and are throw away
//		_i2cPort->read();
//		_i2cPort->read();
//		_i2cPort->read();
//		_i2cPort->read();

//		for(uint8_t x = 0 ; x < numberOfBytesToRead ; x++)
//		{
//			uint8_t incoming = _i2cPort->read();
//			if(dataSpot < MAX_PACKET_SIZE)
//			{
//				shtpData[dataSpot++] = incoming; // Store data into the shtpData array
//			}
//			else
//			{
//				// Do nothing with the data
//			}
//		}

//		bytesRemaining -= numberOfBytesToRead;
//	}
//	return true; // Done!
	if(bytesRemaining > 0)
	{
		if(!I2C::start(IMU_I2C, _deviceAddress, I2C_Direction_Receiver)) return I2C_START_ERROR;
		if(!I2C::readAck(IMU_I2C, &tmp)) return I2C_READ_ACK_ERROR;
		if(!I2C::readAck(IMU_I2C, &tmp)) return I2C_READ_ACK_ERROR;
		if(!I2C::readAck(IMU_I2C, &tmp)) return I2C_READ_ACK_ERROR;
		if(!I2C::readAck(IMU_I2C, &tmp)) return I2C_READ_ACK_ERROR;
		
		for(uint32_t index = 0; index < bytesRemaining; index++)
		{
			if(index < (bytesRemaining - 1))
			{
				if(!I2C::readAck(IMU_I2C, &tmp)) return I2C_READ_ACK_ERROR;
			}
			else
			{
				if(!I2C::readNack(IMU_I2C, &tmp)) return I2C_READ_NACK_ERROR;
			}
			
			if(dataSpot < MAX_PACKET_SIZE)
			{
				shtpData[dataSpot++] = tmp; // Store data into the shtpData array
			}
			else
			{
				// Do nothing with the data
			}
		}
	}
	
	return EXIT_SUCCESS;
}

// Given the data packet, send the header then the data
// Returns false if sensor does not ACK
// TODO - Arduino has a max 32 byte send. Break sending into multi packets if needed.
uint32_t IMU_BNO080::sendPacket(uint8_t channelNumber, uint8_t dataLength)
{
	uint8_t packetLength = dataLength + 4; // Add four bytes for the header

	uint32_t index;
	uint8_t header[4];
	
	header[0] = packetLength & 0xFF;
	header[1] = packetLength >> 8;
	header[2] = channelNumber;
	header[3] = sequenceNumber[channelNumber]++;
	
	if(!I2C::start(IMU_I2C, _deviceAddress, I2C_Direction_Transmitter)) return I2C_START_ERROR;
	
	for(index = 0; index < 4; index++)
	{
		if(!I2C::write(IMU_I2C, header[index])) return I2C_WRITE_ERROR;
	}
	
	for(index = 0; index < dataLength; index++)
	{
		if(!I2C::write(IMU_I2C, shtpData[index])) return I2C_WRITE_ERROR;
	}
	
	if(!I2C::stop(IMU_I2C)) return I2C_STOP_ERROR;
	
	return EXIT_SUCCESS;
}

// Pretty prints the contents of the current shtp header and data packets
void IMU_BNO080::printPacket()
{
	if(_printDebug)
	{
		uint16_t packetLength = (uint16_t)shtpHeader[1] << 8 | shtpHeader[0];

		// Print the four byte header
		Debug::print("Header:");
		for(uint8_t x = 0 ; x < 4 ; x++)
		{
			Debug::print(" ");
			if(shtpHeader[x] < 0x10) Debug::print("0");
			Debug::print("%x", shtpHeader[x]);
		}

		uint8_t printLength = packetLength - 4;
		if(printLength > 40) printLength = 40; // Artificial limit. We don't want the phone book.

		Debug::print(" Body:");
		for(uint8_t x = 0 ; x < printLength ; x++)
		{
			Debug::print(" ");
			if(shtpData[x] < 0x10) Debug::print("0");
			Debug::print("%x", shtpData[x]);
		}

		if (packetLength & 1 << 15)
		{
			Debug::print(" [Continued packet] \n");
			packetLength &= ~(1 << 15);
		}

		Debug::print(" Length:");
		Debug::print("%i", packetLength);

		Debug::print(" Channel:");
		if(shtpHeader[2] == 0) Debug::print("Command");
		else if (shtpHeader[2] == 1) Debug::print("Executable");
		else if (shtpHeader[2] == 2) Debug::print("Control");
		else if (shtpHeader[2] == 3) Debug::print("Sensor-report");
		else if (shtpHeader[2] == 4) Debug::print("Wake-report");
		else if (shtpHeader[2] == 5) Debug::print("Gyro-vector");
		else Debug::print("%i", shtpHeader[2]);
		
		Debug::print("\n");
	}
}

// low level i2c read method
uint32_t IMU_BNO080::readHeader(uint8_t* header)
{
	if(!I2C::start(IMU_I2C, _deviceAddress, I2C_Direction_Receiver)) return I2C_START_ERROR;
	if(!I2C::readAck(IMU_I2C, header)) return I2C_READ_ACK_ERROR;
	if(!I2C::readAck(IMU_I2C, header + 1)) return I2C_READ_ACK_ERROR;
	if(!I2C::readAck(IMU_I2C, header + 2)) return I2C_READ_ACK_ERROR;
	if(!I2C::readNack(IMU_I2C, header + 3)) return I2C_READ_NACK_ERROR;
	
	return EXIT_SUCCESS;
}

#endif

