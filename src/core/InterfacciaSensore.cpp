#ifndef INTERFACCIA_SENSORE_C
#define INTERFACCIA_SENSORE_C

#include "../../include/sensori/InterfacciaSensore.hpp"

InterfacciaSensore::InterfacciaSensore( int idSensore, 
    DataQueue<letturaSensori>& queue, std::atomic<bool>& running) 
    : idSensore(idSensore), queueOutput(queue), running_(running) {}

InterfacciaSensore::~InterfacciaSensore() {
    stop();
}

void InterfacciaSensore::sensorLoop() {
    while(running_) {
        double lettura = readSensor();
        letturaSensori structLettura(idSensore, lettura);
        queueOutput.push(structLettura);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void InterfacciaSensore::start() {
    running_ = true;
    thread_ = std::make_unique<std::thread>(&InterfacciaSensore::sensorLoop, this);
}

void InterfacciaSensore::stop() {
    running_ = false;
    if (thread_ && thread_->joinable()) {
        thread_ -> join();
    }
}

#endif