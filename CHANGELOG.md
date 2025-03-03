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

# 1.4.0

### Features

- Added the AnalogLS API

### Bugfix

- Solved interrupt crash on OI-Core
- Wrong init on Modbus RTU

# 1.5.0

### Features

- Architecture update for Linux ARM.
- Added Encoder API.
- Added function to protect stepper motor driver.
- Added stepper function to set absolute position.
- Added option to force start the application.
- Overloads discrete inputs/outputs functions.
- Added module function to get temperature.

### Bugfix

- Fixed analog-read error (CLI).
- Change the USB host slave select pin.
- Inversion oƒ relay numbers on RelayHP.

# 1.6.0

### Features

- Updated architecture.
- Added Modem (4G LTE) API.