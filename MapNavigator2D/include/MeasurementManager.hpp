#pragma once
#include <vector>
#include "../include/point.hpp"

class MeasurementManager {
public:
    MeasurementManager() = default;
    ~MeasurementManager() = default;

    void addPoint(float x_norm, float y_norm, float mapWidth, float mapHeight);
    bool removePointAt(float x_px, float y_px, float mapWidth, float mapHeight);

    const std::vector<MeasurementPoint>& getPoints() const { return measurementPoints; }
    float getTotalDistance() const { return totalMeasuredDistance; }
    void setTotalDistance(float d) { totalMeasuredDistance = d; }

    void clear() { measurementPoints.clear(); totalMeasuredDistance = 0.0f; }

private:
    std::vector<MeasurementPoint> measurementPoints;
    float totalMeasuredDistance = 0.0f;
    float pointRadius = 6.0f;
};
