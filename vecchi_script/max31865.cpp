#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <cstring>
#include <cmath>

const char* SPI_DEVICE = "/dev/spidev0.0";
const uint32_t SPI_SPEED = 500000;

// MAX31865 registers
const uint8_t REG_CONFIG = 0x00;
const uint8_t REG_RTD_MSB = 0x01;

// Config values
const uint8_t CONFIG_BIAS = 0x80;
const uint8_t CONFIG_1SHOT = 0x20;
const uint8_t CONFIG_3WIRE = 0x10;
const uint8_t CONFIG_CLEAR_FAULT = 0x02;

// Hardware parameters
const double RTD_NOMINAL = 100.0;
const double REF_RESISTOR = 430.0;
const double RTD_A = 3.9083e-3;
const double RTD_B = -5.775e-7;

int fd;

void spi_write(uint8_t reg, uint8_t value) {
    uint8_t tx[2] = {(uint8_t)(reg | 0x80), value};
    uint8_t rx[2] = {0};
    struct spi_ioc_transfer tr = {};
    tr.tx_buf = (unsigned long)tx;
    tr.rx_buf = (unsigned long)rx;
    tr.len = 2;
    tr.speed_hz = SPI_SPEED;
    ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
}

uint16_t spi_read16(uint8_t reg) {
    uint8_t tx[3] = {(uint8_t)(reg & 0x7F), 0, 0};
    uint8_t rx[3] = {0};
    struct spi_ioc_transfer tr = {};
    tr.tx_buf = (unsigned long)tx;
    tr.rx_buf = (unsigned long)rx;
    tr.len = 3;
    tr.speed_hz = SPI_SPEED;
    ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    return (rx[1] << 8) | rx[2];
}

double calc_temperature(uint16_t raw) {
    raw >>= 1; // rimuovi bit di fault
    double resistance = (raw * REF_RESISTOR) / 32768.0;
    
    // formula resistenza
    double z = resistance / RTD_NOMINAL;
    double disc = RTD_A * RTD_A - 4.0 * RTD_B * (1.0 - z);
    if (disc < 0) return -999;
    
    double temp = (-RTD_A + sqrt(disc)) / (2.0 * RTD_B);
    return temp;
}

int main() {
    fd = open(SPI_DEVICE, O_RDWR);
    if (fd < 0) {
        std::cerr << "Failed to open " << SPI_DEVICE << "\n";
        return 1;
    }

    uint8_t mode = SPI_MODE_1;
    ioctl(fd, SPI_IOC_WR_MODE, &mode);
    ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &SPI_SPEED);

    while (true) {
        // rimuovi fault
        spi_write(REG_CONFIG, CONFIG_3WIRE | CONFIG_CLEAR_FAULT);
        usleep(10000);
        
        // rimuovi bias e aspetta conversione
        spi_write(REG_CONFIG, CONFIG_3WIRE | CONFIG_BIAS | CONFIG_1SHOT);
        usleep(70000);
        
        // leggi risultato
        uint16_t raw = spi_read16(REG_RTD_MSB);
        
        if (raw & 0x01) {
            std::cout << "Fault attiva\n";
        } else {
            double temp = calc_temperature(raw);
            double resistance = ((raw >> 1) * REF_RESISTOR) / 32768.0;
            std::cout << "Raw: " << (raw >> 1) 
                      << "  Resistenza: " << resistance << " Ω"
                      << "  Temperatura: " << temp << " °C\n";
        }
        
        spi_write(REG_CONFIG, 0x00);
        
        sleep(1); 
    }

    close(fd);
    return 0;
}
