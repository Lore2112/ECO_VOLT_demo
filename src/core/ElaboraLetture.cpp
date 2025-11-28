#ifndef ELABORA_LETTURE_C
#define ELABORA_LETTURE_C

#include "../../include/core/elaboraLetture.hpp"
#include <algorithm>
/**
 * \brief: costruttore della classe ElaboraLetture.
 * 
 * \param: DataQueue<letturaSensori>& queueInput: queue di input da processare
 * \param: DataQueue<letturaProcessata>& queueOutput: queue di output per dati processati
 * \param: std::atomic<bool>& running: bool thread safe per indicare the il thread è in run
 * \param: int finestra: numero di campionamenti per struct in uscita 
 */
ElaboraLetture::ElaboraLetture(DataQueue<letturaSensori>& queueInput,
                       DataQueue<letturaProcessata>& queueOutput,
                       std::atomic<bool>& running, int finestra)
                       : queueInput(queueInput), queueOutput(queueOutput), running_(running),finestra(finestra) {}

/**
 * \brief: distruttore della classe ElaboreLetture
 */
ElaboraLetture::~ElaboraLetture() {
    stop();
}

/**
 * \brief: Funzione che prende in input un vettore di strutture "letturaSensori" e ne calcola
 *         il massimo, il minimo, la media e con un timestamp e il numero di letture forma una
 *         struct "letturaProcessata".
 * \param: const std::vector<letturaSensori>& misure: vettore contenente le misure "raw" dai sensori
 * 
 * \return: letturaProcessata: contenente max, min, media, timestamp, idSensore e numero di letture
 */
letturaProcessata ElaboraLetture::processaMisure(const std::vector<letturaSensori>& misure) {
        letturaProcessata ret;
        /**
         * maxStruct punta alla struct di tipo letturaSensori che contiene la lettura più alta,
         * utilizzo della lambda per il confronto dei valori nelle struct
         */
        auto maxStruct = std::max_element(
            misure.begin(),misure.end(),
            [](const letturaSensori& a, const letturaSensori& b) {
                return a.valore < b.valore;
            }
        );
        
        /**
         * minStruct punta alla struct di tipo letturaSensori che contiene la lettura più bassa,
         * utilizzo della lambda per il confronto dei valori nelle struct
         */
        auto minStruct = std::min_element(
            misure.begin(),misure.end(),
            [](const letturaSensori& a, const letturaSensori& b) {
                return a.valore < b.valore;
            }
        );
        /**
         * calcolo media
         */
        double sum = 0.0;
        for (const letturaSensori& item : misure) {
            sum += item.valore;
        }

        /**
         * struct letturaProcessata ritornata
         */
        ret.media = sum/misure.size();
        ret.numeroLetture = misure.size();
        ret.max = maxStruct->valore;
        ret.min = minStruct->valore;
        ret.timestamp = misure.back().timestamp;
        ret.idSensore = maxStruct->idSensore;
        return ret;
    }
}

/**
 * \brief: funzione di loop che costruisce i buffer per ogni sensore, e quando raggiungono "finestra"
 *         elementi vengono processati e scritti sulla queue di output.
 */
void ElaboraLetture::elaboraLoop() {
    while (running_) {
        auto lettura = queueInput.tryPop();
        if (lettura) {
            int sensorId = lettura->idSensore;
            buffers_[sensorId].push_back(*lettura);

            if (buffers_[sensorId].size() == static_cast<size_t>(finestra)) {
                letturaProcessata result = processaMisure(buffers_[sensorId]);
                queueOutput.push(result);
                buffers_[sensorId].clear();
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

/**
 * \brief: funzione che crea il thread che esegue la funzione elaboraLoop.
 */
void ElaboraLetture::start() {
    running_ = true;
    thread_ = std::make_unique<std::thread>(&ElaboraLetture::elaboraLoop, this);
}

/**
 * \brief: funzione che ferma il thread in modo sicuro.
 */
void ElaboraLetture::stop() {
    running_ = false;
    if (thread_ && thread_->joinable()) {
        thread_ -> join();
    }
}

#endif