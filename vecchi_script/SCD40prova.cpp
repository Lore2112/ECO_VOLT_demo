#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <cmath>

int main() {
	const char *device = "/dev/i2c-1";
	int address = 0x62;
	
	int file = open(device, O_RDWR);
	if (file < 0) {
		std::cerr << "Failed to open i2c bus. \n";
		return 1;
	}
	
	if (ioctl(file, I2C_SLAVE, address) < 0) {
		std::cerr << "Failed to connect to device. \n";
		return 1;
	}
	
	//measurement
	
	//according to datasheet, command 0x21B1 starts periodic measurement
	uint8_t start_cmd[2] = {0x21, 0xB1};
	write(file, start_cmd, 2);
	std::cout << "starting measurements...\n";
	
	sleep(5); //sensor needs 5 seconds before reading
	
	while (true) {
		// send "read measurement" command 0xEC05
		uint8_t read_cmd[2] = {0xEC, 0x05};
		write (file, read_cmd, 2);
		usleep(500000); //500ms for data ready
		
		uint8_t data[9]; //from datasheet, data is 9 bytes long
		int n = read(file, data, 9);
		if (n != 9) {
			std::cerr << "failed to read data. \n";
			sleep(5);
			continue;
		}
		
		//parsing
		
		uint16_t co2_raw = (data[0] << 8) | data[1];
		uint16_t t_raw = (data[3] << 8) | data[4];
		uint16_t rh_raw = (data[6] << 8) | data[7];
		
		//convert using manifacturers formulae
		
		float co2 = co2_raw; //in ppm
		float temperature = -45.0f + 175.0f * ((float)t_raw / 65535.0f);
		float humidity = 100.0f * ((float)rh_raw / 65535.0f);
		
		std::cout 	<< "CO2: " << co2 << " ppm,"
					<< "Temp: " << temperature << " °C, "
					<< "Humidity: " << humidity << " %\n";
		
		sleep(5); 
	}
	close(file);
	return 0;
}
