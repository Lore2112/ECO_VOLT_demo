#ifdef SENSORE_SPI
#define SENSORE_SPI

#include "InterfacciaSensore.hpp"

class SensoreSPI : public InterfacciaSensore {
    private:
        int fdSPI;
        const char* deviceSPI;
        const uint32_t speedSPI;
        uint8_t modeSPI;
    protected:
        double readSensor() override;
    public:
        SensoreSPI(int idSensore, DataQueue<letturaSensori>& queue,
                std::atomic<bool>& running,
                const char* deviceSPI, const uint32_t speedSPI, uint8_t modeSPI);
        ~SensoreSPI();

        bool initialize();
}

#endif