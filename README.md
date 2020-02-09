# S0ixEnabler

Force enable S0 Low Power Idle mode on your computer.

## WARNING

S0ix might not be fully supported by your hardware, firmware or operating system. Enable S0ix might **overheat your device** and **cause fire**. The developer is not responsible for anything that happen to your computer.

Other things that might happen depending on your hardware configuration:

* Unable to recover from sleep
* Mobile discrete GPU refuse to shutdown during sleep
* Desktop discrete GPU refuse to turn on when waking up
* Faster power drain
* Fan not stopping during sleep
* Fan not spinning after wake up

## Requirements

* UEFI firmware (some old EFI firmwares, e.g. MacBook Pro 13inch late-2013, might be supported as well)

## Usage

Run `S0ixEnabler.efi` either manually or automatically before your OS loads.

### rEFInd

Put a release build of S0ixEnabler.efi into `ESP:\EFI\refind\drivers_%ARCH%` and reboot.

## Building

### Windows

Requirements:

* Visual Studio 2017 or higher
* C++ desktop development tools
* MSVC C++ build tools (for architectures you need)
* MSVC C++ Spectre-mitigated libs (for architectures you need)