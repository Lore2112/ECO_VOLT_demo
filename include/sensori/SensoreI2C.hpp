/**
 * Classe figlia di "InterfacciaSensore" per sensori che comunicano in I2C
 */

#ifndef SENSORE_I2C_H
#define SENSORE_I2C_H

#include "InterfacciaSensore.hpp"

class SensoreI2C : public InterfacciaSensore {
    private:
        int fdI2c;
        const char* device;
        int i2cAddress;
    protected:
        double readSensor() override;
    public:
        SensoreI2C(int idSensore, DataQueue<letturaSensori>& queue,
                    std::atomic<bool>& running, const char* device, int i2cAddress);
        ~SensoreI2C();

        bool initialize();
};

#endif