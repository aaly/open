#include "Sky.hpp"

/**********************************
 *
 * Ease Functions
 *
***********************************/


float Sky::easeInExpo (float t,float b , float c, float d)
{
    return (t==0) ? b : c * pow(2, 10 * (t/d - 1)) + b;
}

float Sky::easeOutExpo(float t,float b , float c, float d)
{
    return (t==d) ? b+c : c * (-pow(2, -10 * t/d) + 1) + b;
}

float Sky::easeInOutExpo(float t,float b , float c, float d)
{
    if (t==0) return b;
    if (t==d) return b+c;
    if ((t/=d/2) < 1) return c/2 * pow(2, 15 * (t - 1)) + b;
    return c/2 * (-pow(2, -10 * --t) + 2) + b;
}

float Sky::easeInCirc (float t,float b , float c, float d)
{
    return -c * (sqrt(1 - (t/=d)*t) - 1) + b;
}

float Sky::easeOutCirc(float t,float b , float c, float d)
{
    return c * sqrt(1 - (t=t/d-1)*t) + b;
}

float Sky::easeInOutCirc(float t,float b , float c, float d)
{
    if ((t/=d/2) < 1) return -c/2 * (sqrt(1 - t*t) - 1) + b;
    return c/2 * (sqrt(1 - t*(t-=2)) + 1) + b;
}

float Sky::easeInSine (float t,float b , float c, float d)
{
    return -c * cos(t/d * (PI/2)) + c + b;
}

float Sky::easeOutSine(float t,float b , float c, float d)
{
    return c * sin(t/d * (PI/2)) + b;
}

float Sky::easeInOutSine(float t,float b , float c, float d)
{
    //return -c/2 * (cos(PI*t/d) - 1) + b;
    return (c*t)/d;
}

