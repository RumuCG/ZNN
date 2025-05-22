#ifndef VERTEXDATA_H
#define VERTEXDATA_H
#include <QVector3D>
#include <QColor>
struct VertexData {
    QVector3D position;
    QVector3D color;

    // 默认颜色是白色
    VertexData(float x = 0.0f, float y = 0.0f, float z = 0.0f) {
        position = { x, y, z };
        color = { 1.0f, 1.0f, 1.0f };
    }
    void setColor(float a, float mn, float mx) {
        // 根据速度的最大值和最小值将该节点的速度值归一化到[0.0, 1.0]
        double t = (a - mn) / (mx - mn);
        if (t < 0.0) t = 0.0;
        if (t > 1.0) t = 1.0;

        // 使用 HSL 颜色进行过度
        // 计算色相(240.0为蓝 360.0为红)
        double hue = 240.0 + 120.0 * t;

        QColor tmp = QColor::fromHslF(hue / 360.0, 1.0, 0.5);
        color = { static_cast<float>(tmp.redF()), static_cast<float>(tmp.greenF()), static_cast<float>(tmp.blueF()) };
    }
};
#endif // VERTEXDATA_H
