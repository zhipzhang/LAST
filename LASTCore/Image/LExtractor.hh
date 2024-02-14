#include <vector>


class LExtractor
{
    static void extract_sliding_window(const std::vector<double>&, int width, const double sampling_rate_ghz, double& sum, double& peak_time);          // We only handle one pixel per time.
    static void subtract_baseline( std::vector<double>& waveform, const int window[2]);

};