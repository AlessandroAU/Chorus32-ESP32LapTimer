#pragma once

//http://www.schwietering.com/jayduino/filtuino/index.php

//Low pass bessel filter order=2 alpha1=0.02, ie 20hz at 1000hz sample rate
class IRAM_ATTR FilterBeLp2
{
  public:
    FilterBeLp2()
    {
      v[0] = 0.0;
      v[1] = 0.0;
    }
  private:
    double v[3];
  public:
    double step(double x) //class II
    {
      v[0] = v[1];
      v[1] = v[2];
      v[2] = (5.593440209108096160e-3 * x)
             + (-0.75788377219702429688 * v[0])
             + (1.73551001136059190877 * v[1]);
      return
        (v[0] + v[2])
        + 2 * v[1];
    }
};


//Low pass bessel filter order=2 alpha1=0.001
//Low pass bessel filter order=2 alpha1=5.0E-5

//Low pass bessel filter order=2 alpha1=0.0002
class IRAM_ATTR FilterBeLp2Slow
{
  public:
    FilterBeLp2Slow()
    {
      v[0] = 0.0;
      v[1] = 0.0;
    }
  private:
    double v[3];
  public:
    double step(double x) //class II
    {
      v[0] = v[1];
      v[1] = v[2];
      v[2] = (6.378909348514483213e-7 * x)
             + (-0.99723520262946085957 * v[0])
             + (1.99723265106572145378 * v[1]);
      return
        (v[0] + v[2])
        + 2 * v[1];
    }
};
