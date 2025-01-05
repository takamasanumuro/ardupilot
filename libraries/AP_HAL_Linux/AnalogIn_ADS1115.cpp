#include "AnalogIn_ADS1115.h"

AnalogSource_ADS1115::AnalogSource_ADS1115(int16_t pin):
    _pin(pin),
    _value(0.0f)
{
}

bool AnalogSource_ADS1115::set_pin(uint8_t pin)
{
    if (_pin == pin) {
        return true;
    }
    _pin = pin;
    return true;
}

float AnalogSource_ADS1115::read_average()
{
    return read_latest();
}

float AnalogSource_ADS1115::read_latest()
{
    return _value;
}

float AnalogSource_ADS1115::voltage_average()
{
    return _value;
}

float AnalogSource_ADS1115::voltage_latest()
{
    return _value;
}

float AnalogSource_ADS1115::voltage_average_ratiometric()
{
    return _value;
}

extern const AP_HAL::HAL &hal;

AnalogIn_ADS1115::AnalogIn_ADS1115()
{
    _adc = NEW_NOTHROW AP_ADC_ADS1115(); //!Driver is instantiated inside AnalogIn constructor
    _channels_number = _adc->get_channels_number(); //!Asks the driver how many channels can be allocated, one for each pin
}

//!Allocates a new AnalogSource_ADS1115 object and returns it
AP_HAL::AnalogSource* AnalogIn_ADS1115::channel(int16_t pin)
{
    WITH_SEMAPHORE(_semaphore);
    for (uint8_t j = 0; j < _channels_number; j++) {
        if (_channels[j] == nullptr) {
            _channels[j] = NEW_NOTHROW AnalogSource_ADS1115(pin);
            return _channels[j];
        }
    }

    hal.console->printf("Out of analog channels\n");
    return nullptr;
}

//!Called from main thread to initialize ADC sampling
void AnalogIn_ADS1115::init()
{
    _adc->init(); //!Initializes the driver and register periodic callback

    hal.scheduler->register_timer_process(FUNCTOR_BIND_MEMBER(&AnalogIn_ADS1115::_update, void));
}


void AnalogIn_ADS1115::_update()
{
    if (AP_HAL::micros() - _last_update_timestamp < 100000) {
        return;
    }

    adc_report_s reports[ADS1115_ADC_MAX_CHANNELS];

    size_t rc = _adc->read(reports, 6);2 //!Copies the data from the driver to the reports array

    for (size_t i = 0; i < rc; i++) {
        for (uint8_t j=0; j < rc; j++) {
            AnalogSource_ADS1115 *source = _channels[j];

            if (source != nullptr && reports[i].id == source->_pin) {
                source->_value = reports[i].data / 1000;
            }
        }
    }

    _last_update_timestamp = AP_HAL::micros();
}
