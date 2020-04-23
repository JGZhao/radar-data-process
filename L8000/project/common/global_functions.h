#ifndef GLOBAL_FUNCTIONS_H
#define GLOBAL_FUNCTIONS_H

/*
 * 功能: 循环相加, augend + addend, 结果在[floor, floor + range) 之间
 */
int circularlyAddInt(int augend, int addend, int floor, int range);
double circularlyAddDouble(double augend, double addend, double floor, double range);

/*
 * 功能: 判断是否在一个区间内, input 是否在[floor, ceil) 之间, 其中floor <= ceil, 且 min <= floor, ceil <= max
 */
bool isBetweenFloorAndCeilInt(int input, int floor, int ceil, int min, int max);
bool isBetweenFloorAndCeilDouble(double input, double floor, double ceil, double min, double max);

/*
 * 功能: 判断是否在一个区间内, input 是否在[floor, ceil) 之间, 其中floor <= ceil, 且 -180 <= floor, ceil <= 180
 */
bool isBetweenFloorAndCeilDegPlusMinus180(double input, double floor_deg, double ceil_deg);

/*
 * 功能: 距离方位转换经纬度
 *     输入：
 *        目标1的经纬度（度）
 *        目标2相对目标1的距离（m）
 *        目标2相对目标1的方位角（度）
 *     输出：
 *        目标2的经纬度（度）
 */
void convertRangeAzimToLonLat(double _lon1, double _lat1, double _range_2to1, double _azim_2to1, double& _lon2, double& _lat2);

/*
 * 功能: 经纬度转换距离方位
 *     输入：
 *        目标1的经纬度（度）
 *        目标2的经纬度（度）
 *     输出：
 *        目标2相对目标1的距离（m）和方位（度）
 */
void convertLonLatToRangeAzim(double _lon1, double _lat1, double _lon2, double _lat2, double& _range_2to1, double& _azim_2to1);

#endif // GLOBAL_FUNCTIONS_H
