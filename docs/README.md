# OI-Documentation

## Dependencies

- [Python 3](https://www.python.org/downloads/)
- [Pip](https://pypi.org/project/pip/)
- [Doxygen](https://www.doxygen.nl/download.html)
- [CMake](https://cmake.org/download/)

## Installation

Create a virtual environment
```
virtualenv -p python3 env
```

Activate your environment
```
source env/bin/activate
```
>To deactivate your environment use: deactivate

Install dependencies
```
pip3 install -r requirements.txt
```

## Build documentation

```
./build.py
```