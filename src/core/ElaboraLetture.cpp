#ifndef ELABORA_LETTURE_C
#define ELABORA_LETTURE_C

#include "../../include/core/ElaboraLetture.hpp"
#include <algorithm>

ElaboraLetture::ElaboraLetture(DataQueue<letturaSensori>& queueInput,
                       DataQueue<letturaProcessata>& queueOutput,
                       std::atomic<bool>& running, int finestra)
                       : queueInput(queueInput), queueOutput(queueOutput), running_(running),finestra(finestra) {}

ElaboraLetture::~ElaboraLetture() {
    stop();
}

letturaProcessata ElaboraLetture::processaMisure(const std::vector<letturaSensori>& misure) {
    if (misure.size()) {
        letturaProcessata ret;
        auto maxStruct = std::max_element(
            misure.begin(),misure.end(),
            [](const letturaSensori& a, const letturaSensori& b) {
                return a.valore < b.valore;
            }
        );
        auto minStruct = std::min_element(
            misure.begin(),misure.end(),
            [](const letturaSensori& a, const letturaSensori& b) {
                return a.valore < b.valore;
            }
        );

        double sum = 0.0;
        for (const letturaSensori& item : misure) {
            sum += item.valore;
        }
        ret.media = sum/misure.size();
        ret.numeroLetture = misure.size();
        ret.max = maxStruct->valore;
        ret.min = minStruct->valore;
        ret.timestamp = misure.back().timestamp;
        ret.idSensore = maxStruct->idSensore;
        return ret;
    }
}

void ElaboraLetture::elaboraLoop() {
    while (running_) {
        auto lettura = queueInput.tryPop();
        if (lettura) {
            int sensorId = lettura->idSensore;
            buffers_[sensorId].push_back(*lettura);

            if (buffers_[sensorId].size() == finestra) {
                letturaProcessata result = processaMisure(buffers_[sensorId]);
                queueOutput.push(result);
                buffers_[sensorId].clear();
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void ElaboraLetture::start() {
    running_ = true;
    thread_ = std::make_unique<std::thread>(&ElaboraLetture::elaboraLoop, this);
}

void ElaboraLetture::stop() {
    running_ = false;
    if (thread_ && thread_->joinable()) {
        thread_ -> join();
    }
}

#endif