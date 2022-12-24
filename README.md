# Sandbox Flipper Zero application

This is a sandbox application for the [Flipper Zero][f0] for me to experiment
with code.

# Functionality

Currently, this app shows a blank screen, sets the LED to blue, and quits when
the back button is pressed.

# Setup

[Install the latest official firmware][f0-update] on your Flipper Zero device.

Clone the official [Flipper Zero firmware repository][f0-firmware-repo], and
check out the version tag for the currently released firmware version:

```shell
git clone https://github.com/flipperdevices/flipperzero-firmware
git checkout 0.74.2  # Use tag for latest official firmware release
```

Change to the firmware's `applications_user` subdirectory and clone this
repository:

```shell
cd applications_user
git clone https://github.com/smkent/f0-sandbox-app
```

# Usage

All of these commands should be run in the `flipperzero-firmware` repository
root directory.

Compile only:

```shell
./fbt fap_sandbox_app
```

Compile, copy to Flipper Zero via USB, and launch app:

```shell
./fbt launch_app APPSRC=sandbox_app
```

Once copied, the app can be started on the Flipper Zero by launching
`Applications` -> `Misc` -> `Sandbox App`


[f0]: https://flipperzero.one
[f0-update]: http://update.flipperzero.one/
[f0-firmware-repo]: https://github.com/flipperdevices/flipperzero-firmware
