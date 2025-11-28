#ifndef MAIN_CPP
#define MAIN_CPP

#include "core/ElaboraLetture.cpp"
#include "sensori/InterfacciaSensore.cpp"
#include "sensori/SensoreI2C.cpp"
#include <csignal>
#include <iostream>

std::atomic<bool> running(true);

void signalHandler(int signal)
{
    if (signal == SIGINT)
    {
        running = false;
    }
}

int main()
{
    // creazione queue
    DataQueue<letturaSensori> inputQueue;
    DataQueue<letturaProcessata> outputQueue;

    // signal Handler per stoppare il programma con ctrl+c
    signal(SIGINT, signalHandler);

    SensoreI2C SCD40(1, inputQueue, running, "/dev/i2c-1", 0x62);

    ElaboraLetture Elaboratore(inputQueue, outputQueue, running, 2);

    if (!SCD40.initialize())
    {
        std::cerr << "Sensori non inizializzti correttamente \n";
        return 1;
    }
    Elaboratore.start();
    SCD40.start();

    std::cout << "Running... press Ctrl+C per stop";
    while (running)
    {
        auto datiProcessati = outputQueue.tryPop();
        if (datiProcessati)
        {
            std::cout << "Sensore" << datiProcessati->idSensore << "\n";
            << "Media" << datiProcessati->media << "\n";
            << "Max" << datiProcessati->max << "\n";
            << "Min" << datiProcessati->min << "\n";
            << "N. letture" << datiProcessati->numeroLetture << "\n";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    std::cout << "Shutdown...";
    SCD40.stop();
    Elaboratore.stop();

    return 0;
}

#endif