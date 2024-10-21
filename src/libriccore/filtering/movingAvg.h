#pragma once
#include <stdint.h>

class MovingAvg{

public:

MovingAvg(uint8_t numSamples):
_numSamples(numSamples)
{};


void update(float data){
    _MovingAvgSamples.push_back(data);

            if (_MovingAvgSamples.size() > _numSamples)
            {
                _MovingAvgSamples.pop_front();   
            }

            float MovingAverageSum = 0.0;
            
            for (float sample: _MovingAvgSamples)
            {
                MovingAverageSum += sample;
            }

            _avg = MovingAverageSum*_sampleCoeff;
}

float getAvg(){

    return _avg;

}

uint8_t _numSamples;
std::deque<float> _MovingAvgSamples;
float _data;
float _sampleCoeff = 1.0/_numSamples;
float _avg;

private:

    


};