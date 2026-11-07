#ifndef QUEUE_MANAGER_H
#define QUEUE_MANAGER_H

#include "vehicle.h"

#include <cstddef>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

class QueueManager {
public:
    // SAD-compatible API
    bool addVehicle(const std::string& roadId, const Vehicle& vehicle);
    bool dequeueVehicle(const std::string& roadId, Vehicle& vehicle);
    std::size_t getQueueLength(const std::string& roadId) const;

    // Existing API retained for backward compatibility
    void enqueue(const std::string& roadId, const Vehicle& vehicle);
    bool dequeue(const std::string& roadId, Vehicle& vehicle);
    bool isEmpty(const std::string& roadId) const;
    std::size_t getSize(const std::string& roadId) const;

    // Returns an independent copy of the queue in FIFO order.
    std::vector<Vehicle> snapshot(const std::string& roadId) const;

private:
    std::unordered_map<std::string, std::queue<Vehicle>> roadQueues_;
};

#endif
