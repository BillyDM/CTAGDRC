/* A simple first-order highpass filter. Bypassed if
   freqHz == 20.0 */
class HighpassFilter
{
public:
    HighpassFilter() = default;

    // Prepares the HighpassFilter with a sampleRate
    void prepare(const double& sampleRate);

    void setFreqHz(const float freqHz);

    void copyCoeffFrom(const HighpassFilter& other);

    // Processes the given buffer of samples
    void process(float* src, int numSamples);

    void reset();

private:
    float m_a0{1.0}, m_b1{0.0};
    float m_xz1{0.0}, m_yz1{0.0};
    float m_freqHz{20.0};
    double m_sampleRate{0.0};
};