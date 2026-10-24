#ifndef TRAFFIC_MANAGER_H
#define TRAFFIC_MANAGER_H

#include "TrafficSignal.h"
#include "queue_manager.h"
#include "vehicle.h"

#include <string>

class TrafficManager {
private:
    TrafficSignal& signal;
    QueueManager& queueManager;
    std::string approachId;
public:
   TrafficManager(
    TrafficSignal& trafficSignal,
    QueueManager& manager,
    const std::string& approach
);

    bool processNextVehicle(
        const std::string& roadId,
        Vehicle& processedVehicle
    );

    void resetSignal();
};

#endif