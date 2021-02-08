# Why CMake

CMake can be useful when you want to develop Firmware using CLion, which to my mind is the best IDE, but it doesn't come for free :( 

Shell script is based on [that repo](https://github.com/Jumperr-labs/nrf5-sdk-clion). Check it out for more info.

In CLion, Preferences > Build, Execution, Deployment > CMake, add `CMake options`:

```
-DCMAKE_TOOLCHAIN_FILE=nrf_utils/code/cmake/arm-gcc-toolchain.cmake
```