#include "include/HighpassFilter.h"
#define _USE_MATH_DEFINES
#include <cmath>

void HighpassFilter::prepare(const double& sampleRate)
{
    m_sampleRate = sampleRate;
    m_a0 = 1.0;
    m_b1 = 0.0;
    reset();
    setFreqHz(m_freqHz);
}

void HighpassFilter::setFreqHz(const float freqHz)
{
    if (freqHz < 20.1f) {
        m_freqHz = 20.0f;
        reset();
        return;
    }

    m_freqHz = freqHz;

    m_b1 = std::exp(-2.0 * M_PI * freqHz / m_sampleRate);
    m_a0 = (1.0 + m_b1) * 0.5;
}

void HighpassFilter::copyCoeffFrom(const HighpassFilter& other) {
    m_freqHz = other.m_freqHz;
    m_a0 = other.m_a0;
    m_b1 = other.m_b1;

    if (m_freqHz == 20.0f) {
        reset();
    }
}

void HighpassFilter::process(float* src, int numSamples)
{   
    if (m_freqHz == 20.0f) {
        return;
    }

    for (int i = 0; i < numSamples; ++i)
    {
        m_yz1 = (m_b1 * m_yz1) + (m_a0 * (src[i] - m_xz1));
        m_xz1 = src[i];
        src[i] = m_yz1;
    }
}

void HighpassFilter::reset()
{
    m_xz1 = 0.0;
    m_yz1 = 0.0;
}
