#include "PublicTools.h"
#include <cmath>


double distance(double x1, double y1, double x2, double y2) {
    return std::hypot(x2 - x1, y2 - y1);
}
double pointToSegmentDistance(double ax, double ay, double bx, double by, double cx, double cy) {
    double abx = bx - ax;
    double aby = by - ay;
    double acx = cx - ax;
    double acy = cy - ay;
    double bcx = cx - bx;
    double bcy = cy - by;

    // 计算投影长度t
    double t = (acx * abx + acy * aby) / (abx * abx + aby * aby);

    // 如果垂足落在线段AB上
    if (t >= 0 && t <= 1) {
        // 计算垂足坐标
        double px = ax + t * abx;
        double py = ay + t * aby;
        // 返回点C到垂足的距离
        return distance(cx, cy, px, py);
    }

    // 否则返回点C到A或B的距离中较小的一个
    return std::fmin(distance(ax, ay, cx, cy), distance(bx, by, cx, cy));
}
