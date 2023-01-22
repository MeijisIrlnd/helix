#pragma once 
#include <JuceHeader.h>
#include <audio/FrequencyDelay.h>
namespace Helix 
{ 
    class HelixProcessor 
    { 
    public:
        HelixProcessor();
        ~HelixProcessor();
        void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
        void processBlock(juce::AudioBuffer<float>& buffer);
        void releaseResources();

        void setDelayTime(float delayTimeSeconds);
        void setFeedback(float newFeedback);
        void setDryWet(float newDryWet);

        void setHighDamping(float gainDB);
    private: 
        void interpolateCoeffs();
        double m_sampleRate;
        bool m_hasBeenPrepared{false};
        std::array<float, 2> m_prev;
        std::array<SDSP::CircularBuffer<float>, 2> m_delayLines;
        std::array<Helix::FrequencyDelay, 2> m_frequencyDelays;
        float m_feedback{0.3f};
        float m_delayTimeSeconds{0.05f};
        SDSP::SmoothedFilterCoefficients<2> m_coeffs;
        std::array<SDSP::BiquadCascade<2>, 2> m_highShelves;
        juce::SmoothedValue<float> m_smoothedFeedback;
        const int m_updateRate = 100;
        int m_samplesUntilUpdate = 0;
        float m_highDampingGain{-1.5f};
    };
}