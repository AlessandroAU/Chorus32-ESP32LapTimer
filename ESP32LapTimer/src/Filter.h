#pragma once

//http://www.schwietering.com/jayduino/filtuino/index.php


//Low pass bessel filter order=2 alpha1=0.01 
class  FilterBeLp2_10HZ
{
  public:
    FilterBeLp2_10HZ()
    {
      v[0]=0.0;
      v[1]=0.0;
    }
  private:
    float v[3];
  public:
    float step(float x) //class II 
    {
      v[0] = v[1];
      v[1] = v[2];
      v[2] = (1.492279154364811733e-3 * x)
         + (-0.87068345511128286685 * v[0])
         + (1.86471433849382361991 * v[1]);
      return 
         (v[0] + v[2])
        +2 * v[1];
    }
};


//Low pass bessel filter order=2 alpha1=0.02, ie 20hz at 1000hz sample rate
class FilterBeLp2_20HZ
{
  public:
    FilterBeLp2_20HZ()
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

//Low pass bessel filter order=2 alpha1=0.05 
class  FilterBeLp2_50HZ
{
  public:
    FilterBeLp2_50HZ()
    {
      v[0]=0.0;
      v[1]=0.0;
    }
  private:
    float v[3];
  public:
    float step(float x) //class II 
    {
      v[0] = v[1];
      v[1] = v[2];
      v[2] = (2.921062558939069298e-2 * x)
         + (-0.49774398476624526211 * v[0])
         + (1.38090148240868249019 * v[1]);
      return 
         (v[0] + v[2])
        +2 * v[1];
    }
};


//Low pass bessel filter order=2 alpha1=0.1 
class  FilterBeLp2_100HZ
{
  public:
    FilterBeLp2_100HZ()
    {
      v[0]=0.0;
      v[1]=0.0;
    }
  private:
    float v[3];
  public:
    float step(float x) //class II 
    {
      v[0] = v[1];
      v[1] = v[2];
      v[2] = (9.053999669813994622e-2 * x)
         + (-0.24114073878907091308 * v[0])
         + (0.87898075199651115597 * v[1]);
      return 
         (v[0] + v[2])
        +2 * v[1];
    }
};


//Low pass bessel filter order=2 alpha1=0.0002
class FilterBeLp2Slow
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
