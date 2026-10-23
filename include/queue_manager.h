
#ifndef QUEUE_MANAGER_H
#define QUEUE_MANAGER_H

#include "vehicle.h"

#include <cstddef>
#include <queue>
#include <string>
#include <unordered_map>

class QueueManager {
public:
    void enqueue(const std::string& roadId, const Vehicle& vehicle);
    bool dequeue(const std::string& roadId, Vehicle& vehicle);
    bool isEmpty(const std::string& roadId) const;
    std::size_t getSize(const std::string& roadId) const;

private:
    std::unordered_map<std::string, std::queue<Vehicle>> roadQueues_;
};

#endif
