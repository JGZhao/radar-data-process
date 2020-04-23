#include "global_functions.h"
#include "global_definitions.h"

/*
 * 功能: 循环相加, augend + addend, 结果在[floor, floor + range) 之间
 */
int circularlyAddInt(int augend, int addend, int floor, int range)
{
    if (range <= 0)
    {
        return augend;
    }
    int tmp = augend + addend - floor;
    while (tmp < 0)
    {
        tmp += range;
    }
    tmp = tmp % range;
    return floor + tmp;
}

double circularlyAddDouble(double augend, double addend, double floor, double range)
{
    if (range <= 0)
    {
        return augend;
    }
    double tmp = augend + addend - floor;
    while (tmp < 0)
    {
        tmp += range;
    }
    tmp = fmod(tmp, range);
    return floor + tmp;
}

/*
 * 功能: 判断是否在一个区间内, input 是否在[floor, ceil) 之间, 其中floor <= ceil, 且 min <= floor, ceil <= max
 */
bool isBetweenFloorAndCeilInt(int input, int floor, int ceil, int min, int max)
{
    if (min > max)
    {
        return false;
    }

    if (floor > max || floor < min || ceil > max || ceil < min)
    {
        return false;
    }

    if (floor <= ceil)
    {
        if (input >= floor && input < ceil)
        {
            return true;
        }
    }
    else
    {
        if ((input >= floor && input <= max) || (input >= min && input <= ceil))
        {
            return true;
        }
    }
    return false;
}

bool isBetweenFloorAndCeilDouble(double input, double floor, double ceil, double min, double max)
{
    if (min > max)
    {
        return false;
    }

    if (floor > max || floor < min || ceil > max || ceil < min)
    {
        return false;
    }

    if (floor <= ceil)
    {
        if (input >= floor && input < ceil)
        {
            return true;
        }
    }
    else
    {
        if ((input >= floor && input <= max) || (input >= min && input < ceil))
        {
            return true;
        }
    }
    return false;
}

/*
 * 功能: 判断是否在一个区间内, input 是否在[floor, ceil) 之间, 其中floor <= ceil, 且 -180 <= floor, ceil <= 180
 */
bool isBetweenFloorAndCeilDegPlusMinus180(double input, double floor_deg, double ceil_deg)
{
    // 左闭右闭, [-180, 180]
    if (floor_deg > 180.0 || floor_deg < -180.0 || ceil_deg > 180.0 || ceil_deg < -180.0)
    {
        return false;
    }

    if (floor_deg <= ceil_deg)
    {
        if (input >= floor_deg && input < ceil_deg)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        if ((input >= floor_deg && input <= 180.0) || (input >= -180.0 && input < ceil_deg))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}

void convertRangeAzimToLonLat(double _lon1, double _lat1, double _range_2to1, double _azim_2to1, double& _lon2, double& _lat2)
{
    /*
     * 功能: 距离方位转换经纬度
     *     输入：
     *        目标1的经纬度（度）
     *        目标2相对目标1的距离（m）
     *        目标2相对目标1的方位角（度）
     *     输出：
     *        目标2的经纬度（度）
     */

    double spherical_azim2_to_1 = _range_2to1 / EARTH_RADIUS;
    double tmp_lat2, tmp_lon2;

    //角度转弧度
    _lat1 = _lat1 * DEG2RAD;
    _azim_2to1 = _azim_2to1 * DEG2RAD;

    tmp_lat2 = sin(_lat1) * cos(spherical_azim2_to_1) + cos(_lat1) * sin(spherical_azim2_to_1) * cos(_azim_2to1);
    tmp_lat2 = acos(tmp_lat2);
    tmp_lon2 = asin(sin(spherical_azim2_to_1) * sin(_azim_2to1) / sin(tmp_lat2));

    tmp_lon2 = tmp_lon2 * RAD2DEG;
    _lon2 = _lon1 + tmp_lon2;

    tmp_lat2 = tmp_lat2 * RAD2DEG;
    _lat2 = 90 - tmp_lat2;
}

void convertLonLatToRangeAzim(double _lon1, double _lat1, double _lon2, double _lat2, double& _range_2to1, double& _azim_2to1)
{
    /*
     * 功能: 经纬度转换距离方位
     *     输入：
     *        目标1的经纬度（度）
     *        目标2的经纬度（度）
     *     输出：
     *        目标2相对目标1的距离（m）和方位（度）
     */

    double lon1_rad = _lon1 * DEG2RAD;
    double lon2_rad = _lon2 * DEG2RAD;
    double lat1_rad = _lat1 * DEG2RAD;
    double lat2_rad = _lat2 * DEG2RAD;
    double lon_rad_diff_2to1 = lon2_rad - lon1_rad;
    double lat_rad_diff_2to1 = lat2_rad - lat1_rad;

    // 转换距离
    _range_2to1 = 2 * asin(sqrt(pow(sin(lat_rad_diff_2to1 / 2.0), 2) + cos(lat1_rad) * cos(lat2_rad) * pow(sin(lon_rad_diff_2to1 / 2.0), 2))) * EARTH_RADIUS;

    // 转换方位
    double tmp_azim_2to1 = cos(lat1_rad) * cos(lat2_rad) * cos(lon_rad_diff_2to1) + sin(lat1_rad) * sin(lat2_rad);
    tmp_azim_2to1 = sqrt(1 - pow(tmp_azim_2to1, 2));
    tmp_azim_2to1 = cos(lat2_rad) * sin(lon_rad_diff_2to1) / tmp_azim_2to1;
    tmp_azim_2to1 = asin(tmp_azim_2to1) * RAD2DEG;

    if (isnan(tmp_azim_2to1))
    {
        if (lon_rad_diff_2to1 > 0)
            tmp_azim_2to1 = 90.0;
        else
        {
            if (fabs(lon_rad_diff_2to1) <= EPS)
                tmp_azim_2to1 = 0;
            else
                tmp_azim_2to1 = 270.0;
        }
    }

    // 第二象限
    if (lon_rad_diff_2to1 < 0 && lat_rad_diff_2to1 > 0)
    {
        tmp_azim_2to1 = 360 + tmp_azim_2to1;
    }
    // 三四象限
    if (fabs(lon_rad_diff_2to1) > 0 && lat_rad_diff_2to1 < 0)
    {
        tmp_azim_2to1 = 180 - tmp_azim_2to1;
    }

    _azim_2to1 = tmp_azim_2to1;
}
