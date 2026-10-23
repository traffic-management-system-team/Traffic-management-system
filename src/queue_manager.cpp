
#include "queue_manager.h"

void QueueManager::enqueue(
    const std::string& roadId,
    const Vehicle& vehicle
) {
    roadQueues_[roadId].push(vehicle);
}

bool QueueManager::dequeue(
    const std::string& roadId,
    Vehicle& vehicle
) {
    auto it = roadQueues_.find(roadId);

    if (it == roadQueues_.end() || it->second.empty()) {
        return false;
    }

    vehicle = it->second.front();
    it->second.pop();

    return true;
}

bool QueueManager::isEmpty(const std::string& roadId) const {
    auto it = roadQueues_.find(roadId);

    return it == roadQueues_.end() || it->second.empty();
}

std::size_t QueueManager::getSize(const std::string& roadId) const {
    auto it = roadQueues_.find(roadId);

    if (it == roadQueues_.end()) {
        return 0;
    }

    return it->second.size();
}
