#ifndef SENSORE_I2C
#define SENSORE_I2C

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