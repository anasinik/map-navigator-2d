#include "../include/MeasurementManager.hpp"
#include <cmath>

void MeasurementManager::addPoint(float xNorm, float yNorm, float mapWidth, float mapHeight)
{
    MeasurementPoint newPoint{ xNorm, yNorm };

    if (!measurementPoints.empty()) {
        MeasurementPoint prev = measurementPoints.back();

        float dx = (newPoint.xNorm - prev.xNorm) * mapWidth;
        float dy = (newPoint.yNorm - prev.yNorm) * mapHeight;

        totalMeasuredDistance += sqrtf(dx * dx + dy * dy);
    }

    measurementPoints.push_back(newPoint);
}

bool MeasurementManager::removePointAt(float x_px, float y_px, float mapWidth, float mapHeight)
{
    y_px = mapHeight - y_px;

    for (size_t i = 0; i < measurementPoints.size(); ++i) {
        float px = measurementPoints[i].xNorm * mapWidth;
        float py = measurementPoints[i].yNorm * mapHeight;

        float dx = x_px - px;
        float dy = y_px - py;

        if (sqrtf(dx * dx + dy * dy) <= pointRadius) {

            if (i > 0) {
                float dxPrev = (measurementPoints[i].xNorm - measurementPoints[i - 1].xNorm) * mapWidth;
                float dyPrev = (measurementPoints[i].yNorm - measurementPoints[i - 1].yNorm) * mapHeight;
                totalMeasuredDistance -= sqrtf(dxPrev * dxPrev + dyPrev * dyPrev);
            }

            if (i < measurementPoints.size() - 1) {
                float dxNext = (measurementPoints[i + 1].xNorm - measurementPoints[i].xNorm) * mapWidth;
                float dyNext = (measurementPoints[i + 1].yNorm - measurementPoints[i].yNorm) * mapHeight;
                totalMeasuredDistance -= sqrtf(dxNext * dxNext + dyNext * dyNext);

                if (i > 0) {
                    float dxReconnect = (measurementPoints[i + 1].xNorm - measurementPoints[i - 1].xNorm) * mapWidth;
                    float dyReconnect = (measurementPoints[i + 1].yNorm - measurementPoints[i - 1].yNorm) * mapHeight;
                    totalMeasuredDistance += sqrtf(dxReconnect * dxReconnect + dyReconnect * dyReconnect);
                }
            }

            measurementPoints.erase(measurementPoints.begin() + i);
            return true;
        }
    }

    return false;
}
