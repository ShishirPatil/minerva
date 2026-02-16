nrf52840dk BLE OTA DFU Walkthrough
==================================

The following are instructions to update firmware over-the-air on the nrf52840dk using the nrfConnect app on an Android phone.  
Code was compiled using linux distro, nrf5sdk v16.0.0. Chip is pca10056. 

This walkthrough adds clarifying information as supplementary to the steps in this <a href="https://devzone.nordicsemi.com/nordic/nordic-blog/b/blog/posts/getting-started-with-nordics-secure-dfu-bootloader">walkthrough</a>.

Please follow all the steps in the link unless directed so.

Preparation
===========
For nrfutil, you may have issues with python. The best solution is to clone the source for v6.0.0 or higher. Currently this version is a release candidate. Follow the readme to install.

Step A
======
After cloning nrf5sdk, the first step is to generate signing keys.

Step A1 & A2
============
We will be flashing the dfu secure bootloader provided by nordic.

    cd <nrf5sdk>/examples/dfu

Store the generated private and public keys in the dfu folder. 
Name the public key dfu_public_key.c or it won't compile.

Step B2
=======
Disregard this step.

Step B3
=======

    cd <nrf5sdk>/examples/dfu/secure_bootloader/pca10056_s140_ble/armgcc
    make

Additional Step
===============
Now we need to program the softdevice and bootloader. You can read these links for more information about programming the <a href="https://infocenter.nordicsemi.com/index.jsp?topic=%2Fsdk_nrf5_v16.0.0%2Fgetting_started_softdevice.html&anchor=getting_started_sd">SoftDevice</a> and the <a href="https://infocenter.nordicsemi.com/index.jsp?topic=%2Fsdk_nrf5_v16.0.0%2Flib_bootloader.html&anchor=lib_bootloader_programming">Bootloader</a>.

Make sure the board is plugged into your computer and switched on. You can verify using the nrfConnect application.

    nrfjprog --eraseall
    cd <nrf5sdk>/examples/dfu/secure_bootloader/pca10056_s140_ble/armgcc
    make flash_softdevice

Note which softdevice version number is generated. In this example it was s140_nrf52_7.0.1

    cd _build
    nrfjprog --reset --program nrf52840_xxaa_s140.hex --family NRF52 --sectoranduicrerase

Step C
======
We will be building the ble_app_buttonles_dfu app.

    cd <nrf5sdk>/examples/ble_peripheral/pca10056/s140/armgcc
    make
    cd _build

To generate a zipped package you need to check the FWID number corresponding to the softdevice version number. The table is located at this <a href="https://github.com/NordicSemiconductor/pc-nrfutil/">link</a>.

Note in this example s140_nrf52_7.0.1 corresponds with 0xCA

    nrfutil pkg generate --hw-version 52 --application-version 1 --application nrf52840_xxaa.hex --sd-req 0xCA --key-file <nrf5sdk>/examples/dfu/private.key app_dfu_package.zip

Step D1
=======
You already completed this step.

Step D3
=======
Scan for devices and you should see DFUTARG. Connect to it and press the DFU button. The upload distribution packet. It should flash OTA. Going forward you should scan for devices again and connect to NORDIC_BUTTONLESS. You can initiate a dfu the same way and upload any packaged application generated in the same way as above.

Additional helpful links
========================
https://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.sdk5.v12.2.0%2Fble_sdk_app_dfu_bootloader.html
https://www.nordicsemi.com/-/media/DocLib/Other/User_Guides/nrfutilv16pdf.pdf
https://infocenter.nordicsemi.com/index.jsp?topic=%2Fug_nrf5x_cltools%2FUG%2Fcltools%2Fnrf5x_nrfjprogexe.html
https://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.sdk5.v12.2.0%2Fble_sdk_app_dfu_bootloader.html
https://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.sdk5.v12.2.0%2Fble_sdk_app_buttonless_dfu.html
https://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.sdk5.v12.2.0%2Flib_bootloader_modules.html
https://infocenter.nordicsemi.com/index.jsp?topic=%2Fsdk_nrf5_v16.0.0%2Flib_bootloader_dfu_validation.html&anchor=lib_dfu_image
https://infocenter.nordicsemi.com/index.jsp?topic=%2Fug_nrfutil%2FUG%2Fnrfutil%2Fnrfutil_intro.html
https://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.sdk5.v12.2.0%2Flib_bootloader_dfu_process.html

Time to set up workflow
=======================
18 hours

Challenges
==========
1. Filtering through documentation to find valid information applicable to the specific hardware. No one sufficient document from Nordic explaining end to end process of OTA over BLE. Spread out over many documents.
2. nrfutil tool developed by nordic to generate signing keys and package firmware updates is only compatible with python 2.
3. Steps to compile and flash the bootloader, softdevice, application, or any combination of the three are inconsistent. Multiple ways, but unclear/non-existant official documentation.
4. Poor documentation explaining the differences between bootloaders.
5. Softdevice version is automatically chosen by makefile. When packaging up a firmware update a hex parameter corresponding to the softdevice version is poorly documented.
6. No mention in official documentation of the importance that these following steps exist and must be completed in order. Device must be erased, softdevice is flashed onto device, then bootloader, then application must be flashed OTA. 
7. Unless precise steps are followed, the dfu only partially completes, with no feedback for reason behind failure.

