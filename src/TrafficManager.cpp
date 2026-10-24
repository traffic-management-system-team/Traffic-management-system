#include "TrafficManager.h"

TrafficManager::TrafficManager(
    TrafficSignal& trafficSignal,
    QueueManager& manager
)
    : signal(trafficSignal),
      queueManager(manager) {
}

bool TrafficManager::processNextVehicle(
    const std::string& roadId,
    Vehicle& processedVehicle
) {
    if (!signal.canMove()) {
        return false;
    }

    if (queueManager.isEmpty(roadId)) {
        return false;
    }

    return queueManager.dequeue(roadId, processedVehicle);
}

void TrafficManager::resetSignal() {
    signal.reset();
}