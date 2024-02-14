#include "LExtractor.hh"
#include <numeric>

/*
Opposite to the original code, we can only handle one pixel per times
*/
void LExtractor::extract_sliding_window(const std::vector<double>& waveform, int width, const double sampling_rate_ghz, double& sum_, double& peak_time_)
{
    std::vector<double> cwf(waveform.size() + 1, 0.0);
    std::partial_sum(waveform.begin(), waveform.end(), cwf.begin() + 1);

    double max_sum = 0.0;
    int maxpos = 0;
    for( auto i = 0; i < cwf.size() - width; ++i )
    {
        double sum = cwf[i + width] - cwf[i];
        if( sum > max_sum )
        {
            max_sum = sum;
            maxpos = i;
        }
    }
    sum_ = max_sum;

    double time_num = 0;
    double time_den = 0;
    for(int i = maxpos; i < maxpos + width; ++i)
    {
        if( waveform[i] > 0)
        {
            time_num += waveform[i] * i;
            time_den += waveform[i];
        }
    }
    peak_time_  = (time_den > 0) ? time_num / time_den : (maxpos + width / 2.0);
    peak_time_ /= sampling_rate_ghz;
}

void LExtractor::subtract_baseline( std::vector<double>& waveform, const int window[2])
{
    double baseline = 0.0;
    for( auto i = window[0]; i < window[1]; ++i )
    {
        baseline += waveform[i];
    }
    baseline /= (window[1] - window[0]);
    for( auto& v : waveform )
    {
        v -= baseline;
    }
}


