#include "TrafficManager.h"

TrafficManager::TrafficManager(
    TrafficSignal& trafficSignal,
    QueueManager& manager,
    const std::string& approach
)
    : signal(trafficSignal),
      queueManager(manager),
      approachId(approach) {
}
bool TrafficManager::processNextVehicle(
    const std::string& roadId,
    Vehicle& processedVehicle
) { 
    if (roadId != approachId) {
        return false;
    }
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