#include "HelixProcessor.h"

namespace Helix 
{ 
    HelixProcessor::HelixProcessor() 
    { 
        std::memset(m_prev.data(), 0.0f, sizeof(float) * 2);
    }

    HelixProcessor::~HelixProcessor() 
    { 

    }

    void HelixProcessor::prepareToPlay(int samplesPerBlockExpected, double sampleRate) 
    { 
        m_sampleRate = sampleRate;
        SDSP::RBJ::highShelf(m_coeffs.target(0), sampleRate, 700, m_highDampingGain, 1.0f);
        std::memcpy(m_coeffs.current(0), m_coeffs.target(0), sizeof(double) * 6);
        SDSP::RBJ::lowShelf(m_coeffs.target(1), sampleRate, 300, -m_highDampingGain, 1.0f);
        std::memcpy(m_coeffs.current(1), m_coeffs.target(1), sizeof(double) * 6);

        for(auto i = 0; i < 2; i++) { 
            m_highShelves[i].setCoefficients(m_coeffs.target(0), 0);    
            m_highShelves[i].setCoefficients(m_coeffs.target(1), 1);
        }
        for(auto& d : m_delayLines) { 
            d.prepare(samplesPerBlockExpected, sampleRate);
            d.setMaxDelaySeconds(2.0f);
            d.setDelay(m_delayTimeSeconds * sampleRate);
        }
        for(auto& f : m_frequencyDelays) { 
            f.prepareToPlay(samplesPerBlockExpected, sampleRate);
        }
        m_smoothedFeedback.reset(sampleRate, 0.1);
        m_smoothedFeedback.setCurrentAndTargetValue(m_feedback);
        m_hasBeenPrepared = true;
    }

    void HelixProcessor::processBlock(juce::AudioBuffer<float>& buffer) 
    {
        auto* read = buffer.getArrayOfReadPointers();
        auto* write = buffer.getArrayOfWritePointers();
        for(auto sample = 0; sample < buffer.getNumSamples(); sample++) { 
            if(m_samplesUntilUpdate == 0) { 
                SDSP::RBJ::highShelf(m_coeffs.target(0), m_sampleRate, 750, m_highDampingGain, 1.0f);
                SDSP::RBJ::lowShelf(m_coeffs.target(1), m_sampleRate, 300, -m_highDampingGain, 1.0f);
                m_samplesUntilUpdate = m_updateRate;
            }
            interpolateCoeffs();
            float feedback = m_smoothedFeedback.getNextValue(); 
            for(auto channel = 0; channel < buffer.getNumChannels(); channel++) { 
                float current = m_delayLines[channel].getNextSample(read[channel][sample] + (m_prev[channel] * feedback));
                m_prev[channel] = m_frequencyDelays[channel].processSample(static_cast<size_t>(buffer.getNumChannels()), current);
                m_prev[channel] = m_highShelves[channel].processSample(m_prev[channel]);
                write[channel][sample] = current;
            }
            --m_samplesUntilUpdate;
        }

    }

    void HelixProcessor::releaseResources() 
    { 

    } 

    void HelixProcessor::setDelayTime(float newDelayTimeSeconds) { 
        m_delayTimeSeconds = newDelayTimeSeconds;
        if(m_hasBeenPrepared) { 
            for(auto& l : m_delayLines) { 
                l.setDelay(newDelayTimeSeconds * m_sampleRate);
            }
        }
    }

    void HelixProcessor::setFeedback(float newFeedback) { 
        m_feedback = newFeedback;
        if(m_hasBeenPrepared) { 
            m_smoothedFeedback.setTargetValue(m_feedback);
        }
    }

    void HelixProcessor::setHighDamping(float newGain) { 
        m_highDampingGain = newGain;
    }
    void HelixProcessor::setDryWet(float newDryWet) { 

    }

    void HelixProcessor::interpolateCoeffs() 
    { 
        m_coeffs.interpolate();
        for(auto& f : m_highShelves) { 
            f.setCoefficients(m_coeffs.current(0), 0);
            f.setCoefficients(m_coeffs.current(1), 1);
        }
    }
}