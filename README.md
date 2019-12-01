# nrf_utils

This repo is providing various tools and utilitaries to: 
* `code`: manage Firmware through code formating, easy version handling and other tools for compilation
* `debug`: Make Firmware debugging fast and accessible 
* `deploy`: Make CI/CD and deployment easier

Note: Python script are compatible with Python 3.7.

## Code subdirectory

### CMake

Use CMake to enjoy the power of CLion.

### Format

I'm a big fan of clang-format. You'll find my customized formating file in [code/format/.clang-format](code/format/.clang-format). That file can be used directly in your editor. More info [here](https://clang.llvm.org/docs/ClangFormat.html).

You won't have any excuse for bad formating!

### Version

[SemVer](https://semver.org/) is a great way to define how versions are handled into your project. That how I'm used to track versions for the Application.

Versioning is better when automated. I've been using a file called `version.ini`, located at the root of the project containing both the Application and the Bootloader to track version in that manner: 

	* `Major.Minor.Patch`, same as SemVer, for Application version.
		* `Major` increased between each major version that are not backward compatible for the end user: API update, new bootloader breaking stuff between the two versions, new SoftDevice or SDK version for example.
		* `Minor` increased between each Production release.
		* `Patch` increased between new DFU packages ready to be tested (Staging or Preprod stages).
	* `Version`, using one number, for Bootloader version.
	* `Version`, using one number, for Hardware version.

I setup a script to write that version into `version.ini`, and two `version.h` files for both your Application and Bootloader projects.

```
# Setup version = 1.2.3
$ python gen_version.py -f path/to/version.ini -i path/to/version.h" -v 1.2.3

# Increment patch: version will be 1.2.4
$ python gen_version.py -f path/to/version.ini -i path/to/version.h" -p

# Increment minor: version will be 1.3.0
$ python gen_version.py -f path/to/version.ini -i path/to/version.h" -m
```

...Note: In order to go further, you must ensure that a build is uniquely identified, [follow that article from Memfault for more information](https://interrupt.memfault.com/blog/gnu-build-id-for-firmware).
