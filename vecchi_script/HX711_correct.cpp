#include <pigpio.h>
#include <iostream>
#include <unistd.h>
#include <stdint.h>

#define DOUT 26   // GPIO pin per DOUT (BCM numbering)
#define SCK 19    // GPIO pin per SCK (BCM numbering)

const long OFFSET = -175650;
const float SCALE = (float)275000 / 234;

float getWeight(long raw) {
	return (raw - OFFSET) / SCALE;
}

long readHX711() {
    // Aspetta READY da HX711
    while (gpioRead(DOUT) == 1)
        gpioDelay(1); // 1 microsecondo 

    long value = 0;
	
    //scrivi 24 impulsi (vedi datasheet HX711)
    for (int i = 0; i < 24; i++) {
        gpioWrite(SCK, 1);
        gpioDelay(1);
        value = (value << 1) | gpioRead(DOUT);
        gpioWrite(SCK, 0);
        gpioDelay(1);
    }

    // Un inpulso extra per selezionare gain 128 e channel A
    gpioWrite(SCK, 1);
    gpioDelay(1);
    gpioWrite(SCK, 0);
    gpioDelay(1);

    // Converti in valore 24 bit con segno
    if (value & 0x800000)
        value |= ~0xFFFFFF;

    return value;
}

int main() {
    if (gpioInitialise() < 0) {
        std::cerr << "inizializzazione pigpio fallita" << std::endl;
        return 1;
    }

    gpioSetMode(DOUT, PI_INPUT);
    gpioSetMode(SCK, PI_OUTPUT);

    while (true) {
        long reading = readHX711();
	float weight = getWeight(reading);
	std::cout << "Raw: " << reading - OFFSET << "\t"; 
        std::cout << "Weight: " << weight << " g" << std::endl;
        usleep(500000); // 0.5 seconds
    }

    gpioTerminate();
    return 0;
}
