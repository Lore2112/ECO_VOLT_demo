/*
*   Data queue thread safe dove mettere dati in attesa che vengano processati dai thread
*/

#ifndef DATAQUEUE_H
#define DATAQUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>

template <typename T>
class DataQueue {
    private:
        std::queue<T> queue_;
        mutable std::mutex mtx_;
        std::condition_variable cv_;

    public:
        void push(T item) {
            {
                std::lock_guard<std::mutex> lock(mtx_);
                queue_.push(std::move(item));
            }
            cv_.notify_one();
        }

        //elimina l'ultimo elemento se esiste
        //ritorna l'elemento eliminato
        std::optional<T> tryPop() {
            std::lock_guard<std::mutex> lock(mtx_);
            if (queue_.empty())
                return std::nullopt;
            T item = std::move(queue_.front());
            queue_.pop();
            return item;
        }

        bool empty() const {
            std::lock_guard<std::mutex> lock(mtx_);
            return queue_.empty();
        }

};

#endif