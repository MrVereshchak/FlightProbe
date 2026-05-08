# FlightProbe

FlightProbe is a C++ Visual Studio project for reading pressure and IMU sensor data through Total Phase Aardvark adapters and calculating angle-of-attack related values.

## Requirements

- Windows
- Visual Studio 2026 or newer
- C++20 support
- Total Phase Aardvark drivers installed
- Total Phase Aardvark API package
- At least one Total Phase Aardvark adapter

## Setup

1. Clone the repository.

2. Open `FlightProbe.slnx` in Visual Studio.

3. Download the Total Phase Aardvark API package.

4. Copy the required Aardvark files into:

   `FlightProbe/FlightProbe/Vendor/`

   Required files:

   - `aardvark.h`
   - `aardvark.c`
   - `aardvark.dll`

   Use the x64 DLL for x64 builds.

5. In Visual Studio, select:

   `x64 Debug`

6. Build the solution.

7. Run the project.

## Runtime DLL note

The project uses a post-build event to copy:

`FlightProbe/FlightProbe/Vendor/aardvark.dll`

to the output folder:

`x64/Debug/`

or:

`x64/Release/`

If the program returns `AA_UNABLE_TO_LOAD_LIBRARY` or `-1`, check that `aardvark.dll` exists in the output folder next to `FlightProbe.exe`.