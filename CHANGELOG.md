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

# 1.6.1

### Features

- Updated Controller and  Master/Slave architecture.
- Added a function to reset slave modules when the master starts.
- Added functions to set/get advanced stepper motor parameter.

### Bugfix

- Fixed OI-Library compilation errors.

# 1.7.0

### Features

- Added CLI for advanced stepper motor parameters.
- Added reset function.
- Added function to get the supply voltage.
- Added handle error function.
- Set duty cycle in the range of 0-100%.
- Added function to attach flag interrupt.
- Added overcurrent threshold callback function.
- Added documentation for the API.
- Updated CI/CD to push the library, binaries, and documentation.

# 1.7.1

### Features

- API refactoring.
- Added function to syncronize the led state.
- Added function to register module event callback.
- Reset AnalogInputsLS sensor list when the Master starts.
- Added function to trigger the limit switch.

# 1.7.2

- Added Stepper motor CLI commands.
- Fix Stepper errors.
- Fix Library compilation error.