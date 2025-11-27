/*
*   types.hpp 
*   Definizione delle strutture di dati passate tra componenenti
*/
#ifndef TYPES_H
#define TYPES_H

#include <chrono>
#include <array>
/*
*   \brief: struct contenente i campionamenti dei sensori
*   \param: timestamp, data e ora campionamento
*   \param: valore, double contenente il campionamento
*/
struct letturaSensori {
    std::chrono::system_clock::time_point timestamp;
    double valore;
    int idSensore;

    letturaSensori() = default;
    letturaSensori(int id, double val) :
        idSensore(id), valore(val),
        timestamp(std::chrono::system_clock::now()) {}
};

/*
*   \brief: struct passata alla queue verso il database
*   \param: timestamp, data e ora campionamento
*   \param: media, max, min: media, massimo e minimo delle misure
*   \param: numeroLetture: numero di letture effettuate 
*/
struct letturaProcessata {
    std::chrono::system_clock::time_point timestamp;
    double media;
    double max;
    double min;
    int idSensore;
    int numeroLetture;

    letturaProcessata() = default;
    letturaProcessata(int idSensore, double min, double max, double media, 
                      std::chrono::system_clock::time_point timestamp,
                     int numeroLetture) :
                     idSensore(idSensore), min(min), max(max), media(media),
                     timestamp(timestamp), numeroLetture(numeroLetture) {}
};

#endif