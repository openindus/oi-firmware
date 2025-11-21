# OI-Documentation

## Dependencies

- [Python 3](https://www.python.org/downloads/)
- [Pip](https://pypi.org/project/pip/)
- [Doxygen](https://www.doxygen.nl/download.html)
- [CMake](https://cmake.org/download/)

## Installation

Create a virtual environment
```
python3 -m venv venv
```

Activate your environment
```
source venv/bin/activate
```

Install dependencies
```
pip3 install -r requirements.txt
```

## Build documentation

Current version:
```
doxygen doxygen/Doxyfile
make html
```

Multi-version (all branches/tags):
```
doxygen doxygen/Doxyfile
make multiversion
```

## Clean documentation

```
make clean
```