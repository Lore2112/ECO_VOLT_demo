#ifndef SENSORE_SERIALE
#define SENSORE_SERIALE

#include "InterfacciaSensore.hpp"

class SensoreSeriale : public InterfacciaSensore {
    private:
        int gpioData;
        int gpioClk;
    protected:
        double readSensor() override;
    public:
        SensoreSeriale(int idSensore, DataQueue<letturaSensori>& queue,
                       std::atomic<bool>& running, int gpioData, int gpioClk);
        ~SensoreSeriale();

        bool initialize();
};

#endif