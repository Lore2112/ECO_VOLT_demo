#ifndef INTERFACCIA_SENSORE_C
#define INTERFACCIA_SENSORE_C

#include "../../include/sensori/InterfacciaSensore.hpp"
#include <cmath>

/**
 * \brief: costruttore per la classe InterfacciaSensore.
 * 
 * \param: int idSensore: identificatore del sensore
 * \param: DataQueue<letturaSensori>& queue: queue di output le letture dei sensori
 * \param: std::atomic<bool>& running: bool thread safe che indica se il thread è in run
 */
InterfacciaSensore::InterfacciaSensore( int idSensore, 
    DataQueue<letturaSensori>& queue, std::atomic<bool>& running) 
    : idSensore(idSensore), queueOutput(queue), running_(running) {}

/**
 * \brief: distruttore per la classe InterfacciaSensore.
 */
InterfacciaSensore::~InterfacciaSensore() {
    stop();
}

/**
 * \brief: funzione di loop che legge il sensore e, se la lettura è valida, scrive
 *         sulla queue di uscita.
 */
void InterfacciaSensore::sensorLoop() {
    while(running_) {
        double lettura = readSensor();
        if(!std::isnan(lettura)){
            letturaSensori structLettura(idSensore, lettura);
            queueOutput.push(structLettura);
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

/**
 * \brief: funzione che inizia il thread che esegue la funzione sensorLoop.
 */
void InterfacciaSensore::start() {
    running_ = true;
    thread_ = std::make_unique<std::thread>(&InterfacciaSensore::sensorLoop, this);
}

/**
 * \brief: funzione che ferma il thread in modo sicuro.
 */
void InterfacciaSensore::stop() {
    running_ = false;
    if (thread_ && thread_->joinable()) {
        thread_ -> join();
    }
}

#endif