/*
  ==============================================================================

    FrequencyDelay.h
    Created: 16 Jan 2023 10:04:04pm
    Author:  Syl

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include <queue>
namespace Helix
{
    class FrequencyDelay
    {
    public: 
        FrequencyDelay();
        ~FrequencyDelay();
        FrequencyDelay(const FrequencyDelay& other) = delete;
        FrequencyDelay(FrequencyDelay&& other) noexcept = delete;
        FrequencyDelay& operator=(const FrequencyDelay& other) = delete;
        FrequencyDelay& operator=(FrequencyDelay&& other) = delete;

        void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
        float processSample(const int bufferSize, float x);
        void releaseResources();

        void setHighestBinDelayTime(float delayTimeSeconds);
        void setFeedback(float feedback);
    private: 
        void stftCallback(float* data, size_t size);
        double m_sampleRate;
        int m_currentlyProcessingChannel{ 0 };
        std::function<void(float*, size_t)> m_stftCallback = [this](float* data, size_t size) { stftCallback(data, size); };
        std::array<SDSP::Fourier::STFT<256, 8>, 2> m_stfts;
        struct STFTPair {
            std::array<float, 2> items;
            float operator[](size_t idx) { return items[idx]; }

            STFTPair& operator+(STFTPair& other) {
                items[0] += other[0];
                items[1] += other[1];
                return *this;
            }

            STFTPair& operator*(float multiplier) {
                items[0] *= multiplier;
                items[1] *= multiplier;
                return *this;
            }
        };
        struct ComplexDelayLine { 
            std::array<juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Thiran>, 2> delayLines;
            void prepare(const juce::dsp::ProcessSpec& spec) { 
                for(auto& d : delayLines) { 
                    d.prepare(spec);
                }
            }

            void setMaximumDelayInSamples(int newMax) { 
                for(auto& d : delayLines) { 
                    d.setMaximumDelayInSamples(newMax);
                }
            }

            void setDelay(int newDelaySamples) { 
                for(auto& d : delayLines) { 
                    d.setDelay(newDelaySamples);
                }
            }

            SDSP_INLINE std::tuple<float, float> popSample() noexcept { 
                auto real = delayLines[0].popSample(0);
                auto imag = delayLines[1].popSample(0);
                return {real, imag};
            }

            SDSP_INLINE void pushSample(const std::tuple<float, float>& toPush) noexcept { 
                auto [real, imag] = toPush;
                delayLines[0].pushSample(0, real);
                delayLines[1].pushSample(0, imag);
            }
        };
        std::vector<ComplexDelayLine> m_delayLines;
        // 256 fucking delay lines LOL 
        bool m_hasBeenPrepared{false};
        //std::vector<SDSP::CircularBuffer<STFTPair> > m_delayLines;
        STFTPair m_prev;
        float m_feedback{ 0.0f };
        juce::SmoothedValue<float> m_smoothedFeedback;
        std::vector<float> m_accumulator, m_data;
        bool m_hasPerformedFirstTransform{ false };
        int m_samplesUntilUpdate{ 0 };
        int m_writePos{ 0 };
        
    };
}