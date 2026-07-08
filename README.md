# Static-Lib
Static Lib (slib) is a wrapper around SDL3, SDL3_image and SDL3_mixer

# Support
Currently only Windows is supported. The library can be compiled for other Operating Systems since SDL3 is cross-platform, but slib os-specific function will have emtpy returns. ie, function that return pointers will return 'nullptr', functions that return booleans will return 'false'.
### These function are:
- OpenDll
- CloseDll
- GetFunction
- CpyFile
