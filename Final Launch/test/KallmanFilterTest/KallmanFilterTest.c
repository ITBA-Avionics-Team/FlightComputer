#include <stdio.h>
#include <math.h>

const int simulated_pressures[139] = {100833, 100833, 100833, 100833, 100833, 100833, 100833, 100833, 100833, 100833, 100833, 100833, 100833, 98522, 97353, 96081, 95049, 94231, 93621, 93137, 92812, 92578, 92645, 92868, 92756, 92924, 92913, 93059, 93115, 93216, 93250, 93384, 93441, 97539, 93655, 93700, 93790, 93880, 93948, 94072, 94378, 94231, 94299, 94378, 94503, 94617, 94651, 94810, 96334, 94947, 95015, 95118, 95278, 95323, 98264, 95495, 95587, 95678, 95782, 95862, 96127, 96023, 96069, 96150, 96207, 96288, 96357, 96438, 96519, 96623, 96669, 96738, 96831, 96900, 96981, 97121, 97179, 97190, 97295, 97341, 97458, 97598, 97621, 97563, 97609, 97691, 97761, 97796, 97843, 97889, 97936, 98018, 98393, 104180, 98287, 98240, 98393, 98393, 98440, 98522, 98616, 98639, 98698, 98745, 98816, 98875, 98957, 99075, 99512, 99264, 99299, 100523, 99406, 99489, 99571, 99702, 99749, 99809, 99880, 99963, 100034, 100129, 100368, 100463, 101927, 100487, 100582, 100606, 100726, 100762, 100821, 100893, 100893, 100893, 100893, 100893, 100893, 100893};
int filtered_pressures[139];

float _err_measure = 1;
float _err_estimate = 1;
float _q = 0.01;
float _current_estimate;
float _last_estimate;
float _kalman_gain;

float updateEstimate(float mea)
{
  _kalman_gain = _err_estimate/(_err_estimate + _err_measure);
  _current_estimate = _last_estimate + _kalman_gain * (mea - _last_estimate);
  _err_estimate =  (1.0 - _kalman_gain) * _err_estimate + fabs(_last_estimate-_current_estimate)*_q;
  _last_estimate=_current_estimate;

  return _current_estimate;
}

void setMeasurementError(float mea_e)
{
  _err_measure=mea_e;
}

void setEstimateError(float est_e)
{
  _err_estimate=est_e;
}

void setProcessNoise(float q)
{
  _q=q;
}

float getKalmanGain() {
  return _kalman_gain;
}

float getEstimateError() {
  return _err_estimate;
}


int main(int argc, char* argv[]) {
  for (int i = 0; i < 139; i++) {
    filtered_pressures[i] = updateEstimate(simulated_pressures[i]);
  }

  printf("measured,filtered\n");
  for (int i = 0; i < 139; i++) {
    printf("%d,%d\n", simulated_pressures[i], filtered_pressures[i]);
  }
}

void setup() {

}

void loop() {

}

