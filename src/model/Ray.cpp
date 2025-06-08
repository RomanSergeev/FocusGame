#include "Ray.h"

bool Ray::intersects(const AABB& boundingBox, float& distance) const {
    float tMin = 0.0f;
    float tMax = std::numeric_limits<float>::max();

    for (int i = 0; i < 3; ++i) {
        if (glm::abs(direction[i]) < 1e-8f) {
            // Ray is parallel to slab. No hit if origin not within slab.
            if (origin[i] < boundingBox.min[i] || origin[i] > boundingBox.max[i])
                return false;
        } else {
            float ood = 1.0f / direction[i];
            float t1 = (boundingBox.min[i] - origin[i]) * ood;
            float t2 = (boundingBox.max[i] - origin[i]) * ood;

            if (t1 > t2) std::swap(t1, t2);
            tMin = glm::max(tMin, t1);
            tMax = glm::min(tMax, t2);
            if (tMin > tMax)
                return false;
        }
    }

    distance = tMin;
    return true;
}