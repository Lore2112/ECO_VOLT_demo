/**
 * Elaborazione dati provenienti dalla queue di input (letture "raw" dai sensori)
 * Calcolo della media, max e min delle misure e immissione nella queue di output
 * come struct letturaProcessata
 */

#ifdef ELABORA_LETTURE
#define ELABORA_LETTURE

#include "types.hpp"
#include "DataQueue.hpp"
#include <atomic>
#include <thread>
#include <memory>
#include <map>
#include <vector>

class ElaboraLetture {
    private:
        DataQueue<letturaSensori>& queueInput;
        DataQueue<letturaProcessata>& queueOutput;
        std::atomic<bool>& running_;
        int finestra;
        std::unique_ptr<std::thread> thread_;
        std::map<int, std::vector<letturaSensori>> buffers_;

        void elaboraLoop();
        letturaProcessata processaMisure(const std::vector<letturaSensori>& misure);

    public: 
        ElaboraLetture(DataQueue<letturaSensori>& queueInput,
                       DataQueue<letturaProcessata>& queueOutput,
                       std::atomic<bool>& running, int finestra);
        ~ElaboraLetture();
        void start();
        void stop();
};

#endif