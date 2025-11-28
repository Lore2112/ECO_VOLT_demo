#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <cmath>

#include "../../include/sensori/SensoreI2C.hpp"

SensoreI2C::SensoreI2C (int idSensore, DataQueue<letturaSensori>& queue,
                    std::atomic<bool>& running, const char* device, int i2cAddress)
                    : InterfacciaSensore(idSensore, queue, running),
                    fdI2c(-1), device(device), i2cAddress(i2cAddress) {}

SensoreI2C::~SensoreI2C() {
    if(fdI2c >= 0) {
        close(fdI2c);
    }
}

bool SensoreI2C::initialize() {
    fdI2c = open(device, O_RDWR);

    if (fdI2c < 0) {
		std::cerr << "Failed to open i2c bus. \n";
		return false;
	}
	
	if (ioctl(fdI2c, I2C_SLAVE, i2cAddress) < 0) {
		std::cerr << "Failed to connect to device. \n";
		return false;
	}
    // 1. Send "Stop Periodic Measurement" (0x3F86) first.
    // This handles the case where the program crashed previously 
    // and left the sensor running.
    uint8_t stop_cmd[2] = {0x3F, 0x86};
    write(fdI2c, stop_cmd, 2); 
    usleep(500000); // Wait 500ms for it to stop

    // 2. Send "Start Periodic Measurement" (0x21B1)
    uint8_t start_cmd[2] = {0x21, 0xB1};
    if (write(fdI2c, start_cmd, 2) != 2) {
        std::cerr << "Failed to send start command during init: " << "\n";
        return false;
    }

    // 3. Wait for the first measurement to be ready (approx 5 seconds)
    std::cout << "SCD4x initialized. Warming up (5s)...\n";
    sleep(5); 

    return true;
}

/**
 * Funzione specifica per SCD40, sensore di umidità, temperatura e C02
 */
double SensoreI2C::readSensor() {
    
    // send "read measurement" command 0xEC05
	uint8_t read_cmd[2] = {0xEC, 0x05};
    if (write(fdI2c, read_cmd, 2) != 2) {
        std::cerr << "Failed to send read command\n";
        return NAN;
    }
	usleep(2000);
		
	uint8_t data[9]; //from datasheet, data is 9 bytes long
	int n = read(fdI2c, data, 9);
	if (n != 9) {
		std::cerr << "failed to read data. \n";
		return NAN;
	}

    //parsing	
	uint16_t co2_raw = (data[0] << 8) | data[1];

    return static_cast<double>(co2_raw);
}
