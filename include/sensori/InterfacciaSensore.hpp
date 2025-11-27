#ifndef SENSOR_INTERFACE_HPP
#define SENSOR_INTERFACE_HPP

#include "../core/types.hpp"
#include "../core/DataQueue.hpp"
#include <atomic>
#include <thread>
#include <memory>

class InterfacciaSensore {
    protected:
        int idSensore;
        /**
         * creazione queue thread safe di output per le letture dirette dei sensori
         */
        DataQueue<letturaSensori>& queueOutput;
        /**
         * bool thread safe che indica se il thread è in run
         */
        std::atomic<bool>& running_;
        /**
         * puntatore unico per il thread
         */
        std::unique_ptr<std::thread> thread_;

        virtual double readSensor() = 0;
        void sensorLoop();
    public:
        InterfacciaSensore(int idSensore, DataQueue<letturaSensori>& queue, std::atomic<bool>& running);
        virtual ~InterfacciaSensore();

        void start();
        void stop();
};

#endif