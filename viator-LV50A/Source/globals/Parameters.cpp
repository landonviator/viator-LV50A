#include "Parameters.h"


ViatorParameters::Params::Params()
{
    initSliderParams();
    initButtonParams();
    initMenuParams();
}

void ViatorParameters::Params::initSliderParams()
{
    using skew = SliderParameterData::SkewType;
    using type = SliderParameterData::NumericType;
    
    // filter 1
    _sliderParams.push_back({ViatorParameters::filter1GainID, ViatorParameters::filter1GainName, -15.0f, 15.0f, 0.0f, skew::kNoSkew, 0.0, type::kFloat});
    _sliderParams.push_back({ViatorParameters::filter1QID, ViatorParameters::filter1QName, 0.05f, 0.95f, 0.3f, skew::kSkew, 0.3, type::kFloat});
    _sliderParams.push_back({ViatorParameters::filter1CutoffID, ViatorParameters::filter1CutoffName, 30.0f, 400.0f, 100.0, skew::kSkew, 100.0, type::kFloat});
    
    // filter 2
    _sliderParams.push_back({ViatorParameters::filter2GainID, ViatorParameters::filter2GainName, -15.0f, 15.0f, 0.0f, skew::kNoSkew, 0.0, type::kFloat});
    _sliderParams.push_back({ViatorParameters::filter2QID, ViatorParameters::filter2QName, 0.05f, 0.95f, 0.3f, skew::kSkew, 0.3, type::kFloat});
    _sliderParams.push_back({ViatorParameters::filter2CutoffID, ViatorParameters::filter2CutoffName, 75.0f, 1000.0f, 250.0f, skew::kSkew, 250.0f, type::kFloat});
    
    // filter 3
    _sliderParams.push_back({ViatorParameters::filter3GainID, ViatorParameters::filter3GainName, -15.0f, 15.0f, 0.0f, skew::kNoSkew, 0.0, type::kFloat});
    _sliderParams.push_back({ViatorParameters::filter3QID, ViatorParameters::filter3QName, 0.05f, 0.95f, 0.3f, skew::kSkew, 0.3, type::kFloat});
    _sliderParams.push_back({ViatorParameters::filter3CutoffID, ViatorParameters::filter3CutoffName, 800.0f, 12500.0f, 5000.0f, skew::kSkew, 5000.0f, type::kFloat});
    
    // filter 4
    _sliderParams.push_back({ViatorParameters::filter4GainID, ViatorParameters::filter4GainName, -15.0f, 15.0f, 0.0f, skew::kNoSkew, 0.0, type::kFloat});
    _sliderParams.push_back({ViatorParameters::filter4QID, ViatorParameters::filter4QName, 0.05f, 0.95f, 0.3f, skew::kSkew, 0.3, type::kFloat});
    _sliderParams.push_back({ViatorParameters::filter4CutoffID, ViatorParameters::filter4CutoffName, 2500.0f, 20000.0f, 10000.0f, skew::kSkew, 10000.0f, type::kFloat});
    _filterSliderParams = _sliderParams;
    
    _sliderParams.push_back({ViatorParameters::hpCutoffID, ViatorParameters::hpCutoffID, 20.0f, 20000.0f, 20.0f, skew::kSkew, 1000.0, type::kFloat});
    _sliderParams.push_back({ViatorParameters::lpCutoffID, ViatorParameters::lpCutoffID, 20.0f, 20000.0f, 20000.0f, skew::kSkew, 1000.0, type::kFloat});
    _passFilterSliderParams.push_back({ViatorParameters::hpCutoffID, ViatorParameters::hpCutoffName, 20.0f, 20000.0f, 20.0f, skew::kSkew, 1000.0, type::kFloat});
    _passFilterSliderParams.push_back({ViatorParameters::lpCutoffID, ViatorParameters::lpCutoffName, 20.0f, 20000.0f, 20000.0f, skew::kSkew, 1000.0, type::kFloat});
    
    // volume
    _sliderParams.push_back({ViatorParameters::volumeID, ViatorParameters::volumeName, -30.0f, 30.0f, 0.0f, skew::kNoSkew, 0.0, type::kFloat});
}

void ViatorParameters::Params::initButtonParams()
{
    _buttonParams.push_back({ViatorParameters::lowShelfToggleID, ViatorParameters::lowShelfToggleName, false});
    _buttonParams.push_back({ViatorParameters::highShelfToggleID, ViatorParameters::highShelfToggleName, false});
}

void ViatorParameters::Params::initMenuParams()
{
}
