# LoR_Core_Ps4control

Contains the configurations for controlling a MiniBot via PS4 controller.

**Arduino Board selection:** ESP32 dev module 

**User Guide to PS4 Control :** [Download User Guide](https://tr.ee/PGy3wb1Fbj)

**LoR_Core Module :** [Download Data Sheet](https://tr.ee/klJ5_gKmH-)

**Beginners Guide to minibots:** [Download beginner's Guide](https://tr.ee/QhCzRJD69O)

**Link Tree :** [Useful Links to Web Pages and Documentation](https://linktr.ee/LordofRobots)


This Arduino code is designed for an ESP32 microcontroller to manage a robot using a PS4 controller. It features two drive styles: omni directional drive (Mecanum drive) and a standard tank-style drive. It also provides LED indication of control inputs and power/battery status of the PS4 controller. There are several functionalities in the script to manage drive controls, battery checks, LED displays, motor control, and PS4 controller connection.

Key functions of the code:

- **Omni directional drive (Mecanum drive) or standard tank-style drive control**: Depending on the "MecanumDrive_Enabled" boolean variable, the script can run either in omni directional (Mecanum) or standard drive mode. The motion control function (`Motion_Control`) calculates motor speeds based on joystick input from the PS4 controller.

- **LED indication**: Depending on the PS4 controller connection status, the LED strip changes colors. It displays a rainbow pattern when the PS4 controller is connected, and a red color when there's no controller input or when in standby mode.

- **Battery check for PS4 controller**: The code includes a function to check the PS4 controller's battery status (`PS4controller_BatteryCheck`). This function also sets different LED flash rates and colors on the PS4 controller depending on battery levels.

- **Motor control**: There are functions that handle the output to the motors (`Set_Motor_Output`, `Motor_Control`, `Motor_STOP`). These functions manage motor speed ramping and limit the maximum and minimum speeds. The `Set_Motor_Output` function also supports the inversion of the motor direction.

- **LED strip control**: The code controls an Adafruit NeoPixel LED strip to display various colors. There are two functions, `NeoPixel_SetColour` and `NeoPixel_Rainbow`, to set a specific color or display a rainbow pattern on the LED strip respectively.

- **Controller rumble**: The `Rumble_Once` function enables controller vibration when connected for the first time.

- **Serial communications**: The code sends debug messages and core system status over USB serial communication.

In the `setup()` function, the pins are initialized, the PS4 controller is connected, the LED strip and PWM functionalities are set up, and the serial communications are started. The main loop (`loop()`) handles PS4 controller input, adjusts the LED display, controls motion, and checks the connection status.

#Known Issues 

-2023/09/08
   
   **Problem:** Controller stops connecting to MiniBot. LED on controller flashes only 2 or 3 times the shuts off without connecting.
  
   **Solution:** Under "Tools" Enable "Erase all Flash before sketch Upload"

<img width="487" alt="Screenshot 2023-09-08 174440" src="https://github.com/LordofRobots/LoR_Core_PS4control/assets/131721361/08ee79ff-745d-4ae9-a0fc-ed397223b722">



