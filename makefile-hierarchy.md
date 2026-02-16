
Makefile in the app:

- Sets device, sets `APP_SOURCE_PATH` and `APP_HEADER_PATH`
- Includes `AppMakefile.mk`

`AppMakefile.mk`:

- Sets the output path names
- Includes three other Makefiles in the same directory
- Uses the `TRACE_LD` variable to link elf binaries

`Configuration.mk`:

- Sets the toolchain programs used
- Sets up `Q` as the compilation trace output, only if `$(V)` env. var. nonempty
- Takes the device specified in the app's `Makefile` from it configures some
variables like the device model, RAM and flash sizes, etc.
- Sets the variable `LINKER_SCRIPT` corresponding to the variables set above
- Uses `LINKER_INCLUDES` set by `Includes.mk` so that `INCLUDE "nrf_common.ld"` works
- This linker script handles memory and uncommon section, then includes `nrf_common.ld`
- Prints info about what we are compiling for

`Includes.mk`:

- Sets `REPO_HEADER_FILES`, which sets includes based on this repo
- For the two supported boards, adds seperate system C and assembly files
- Sets `SDK_ROOT` to `/sdk/nrf5_sdk_15.3.0(_thread)` depending on if `USE_THREAD` is set
- If the softdevice model is not blank, adds the appropriate softdevice file
- Then sets compile flags based on the above. `SDK_LINKER_PATHS` gets the `mdk` folder

`Program.mk`:

- Sets the command line tools to use, like `nrfutil`
- Sets command line flags for these tools
- Provides definitions for the makefile commands like `flash`, which use the JLink utility



