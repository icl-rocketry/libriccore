#pragma once
#include <stdint.h>

class movingAvg{

public:

movingAvg(int numSamples, int samples):
    numSamples(numSamples),
    samples(samples)
    {};

void update(float data){
    MovingAvgSamples.push_back(data);

    if (MovingAvgSamples.size() > numSamples)
            {
                MovingAvgSamples.pop_front();   
            }
            float MovingAverageSum = 0.0;
            for (float sample: MovingAvgSamples)
            {
                MovingAverageSum += sample;
            }

            avg = MovingAverageSum*sampleCoeff;
}

float getAvg(float data, int numSamples, int samples){

    return avg;

}

private:

    int numSamples;
    int samples;
    float data;
    std::deque<float> MovingAvgSamples;
    float sampleCoeff = 1.0/numSamples;
    float avg;


};