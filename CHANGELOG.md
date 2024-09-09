# 1.0.0

- Bug fixes on AnalogLs
- Delete Docs
- Middleware refactoring
- Update CI

# 1.1.0

- Update all mcu to esp32s3
- Add master/slave/control class
- Update examples
- Simplify bus protocol for shorter frames
- Flash slave board on bus
- Save module info in eFuse
- Add driver ADS114S08
- Add driver AD5413
- Add the possibility to initialize a slave board with a given 'SN'
- Add compatibility with Arduino 'Serial' library
- Add function to get digital output current

Board which are ready to use :
- OI-Core/OI-CoreLite
- OI-Discrete

# 1.2.0

- Refactor Middleware in API
- Add board init control
- Add Middleware and Module for OIMixed
- Add Middleware and Module for OIAnalogLS
- Add Middleware and Module for OIDc
- Add Middleware and Module for OIRelayHP
- Add Middleware and Module for OIStepper

Board which are ready to use :
- OI-Core/OI-CoreLite
- OI-Discrete
- OI-Mixed
- OIStepper
- OIRelayHP
- OIDc

# 1.3.0

### Features

- Compatibility with old boards (discrete-s2, stepper-s2).
- Fix SLIP packet frame sending errors
