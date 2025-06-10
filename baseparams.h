#ifndef BASEPARAMS_H
#define BASEPARAMS_H


class BaseParams
{
public:
    BaseParams(bool startInit) {
        if (not startInit) {
            return;
        }

        axisMin[0] = 0.0f;
        axisStep[0] = 40.0f;
        axisCount[0] = 25;

        axisMin[1] = -500.0f;
        axisStep[1] = 40.0f;
        axisCount[1] = 25;

        axisMin[2] = 2640.0f;
        axisStep[2] = 40.0f;
        axisCount[2] = 25;

    }

    // 数据规模信息
    // 0 x轴
    // 1 y轴
    // 2 z轴
    float axisMin[3];
    float axisStep[3];
    int axisCount[3];
};

#endif // BASEPARAMS_H
