# S0ixEnabler

It enables S0 Low Power Idle mode (a.k.a. Modern Standby, including Connected Standby and Disconnected Standby) on your computer. It makes Windows experience on old MacBooks better by providing:

* Near-instant screen on when you open the lid
* The ability to play music when the lid is closed

It have been tested on a MacBook Pro 13 inch (2013 late).

> 这个东西不是是个大学生都能在几分钟里写出来一个的嘛——[Ben Wang](https://github.com/imbushuo)，2020-02-09

## WARNING

**This is not an end-user product. This is pure experimental.**

S0ix might not be fully supported by your hardware, firmware or operating system. Enabling S0ix might **overheat your device** and **cause fire** in extreme environments. The developer is not responsible for anything that happen to you or your computer.

Other things that might happen depending on your hardware configuration:

* Windows fail to boot
* Unable to wake up from sleep
* Mobile discrete GPU refuse to shutdown during sleep
* Desktop discrete GPU refuse to turn on after wake up
* USB devices unavailable after wake up
* Faster power drain during sleep
* Fan not stopping during sleep
* Fan not spinning after wake up

## Requirements

* A UEFI firmware (some old EFI firmwares might be supported as well)
* A CPU supporting S0ix (Intel Haswell/4th gen core or later, AMD Picasso/Ryzen 3 mobile or later)
* Firmware has S0ix disabled (otherwise you don't need this; use `powercfg /a` on Windows to check)
* If you have rotational storage connected, enable [IgnoreCsComplianceCheck](https://docs.microsoft.com/en-us/windows-hardware/customize/power-settings/power-controls-ignorecscompliancecheck) for Connected Standby

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