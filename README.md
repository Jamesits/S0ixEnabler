# S0ixEnabler

It enables S0 Low Power Idle mode (a.k.a. Modern Standby, including Connected Standby and Disconnected Standby) on your computer. 

## WARNING

**This is not an end-user product. This is pure expereimental.**

S0ix might not be fully supported by your hardware, firmware or operating system. Enabling S0ix might **overheat your device** and **cause fire** in extreme environments. The developer is not responsible for anything that happen to you or your computer.

Other things that might happen depending on your hardware configuration:

* Unable to wake up from sleep
* Mobile discrete GPU refuse to shutdown during sleep
* Desktop discrete GPU refuse to turn on after wake up
* USB devices unavailable after wake up
* Faster power drain during sleep
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
* QEMU

Open `S0ixEnabler.sln` in Visual Studio and click Build Solution. 