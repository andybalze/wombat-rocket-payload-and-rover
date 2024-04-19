# Microcontroller GPIO Configurations

The current configuration for the motors does not match what is listed on the rover controller schematic. Changes were decided upon so that the left motor and right motor are driven by different motor drivers.

## Port B

0. Red LED: Open drain output
1. Right Motor 1: Push-pull PWM output.
2. Right Motor 2: Push-pull PWM output.
3. Left Motor 2: Push-pull PWM output. (Programmer MOSI)
4. Dispenser Inventory Sensor 1: Active high input. No internal
pull-up. (Programmer MISO)
5. Green LED: Open drain output. (Programmer SCLK)
6. Dispenser Manual Load Button: Active high input. No internal pull-up.
7. Lidar Power: Push-pull output.

## Port C

0. Lidar Sensor Data: ADC0
1. Accelerometer x Data: ADC1
2. Accelerometer y Data: ADC2
3. Accelerometer z Data: ADC3
4. Onboard Data Cube Power Switch: *Collaborate with Michael and Jordan.*
5. Dispenser Inventory Sensor 2: Active high input. No internal pull-up.
6. Programmer RST: Do not use.

## Port D

0. UART RX: No internal pull-up.
1. UART TX
2. Dispenser Mode Switch: Active low input. Internal pull-up.
3. Left Motor 1: Push-pull PWM output.
4. Dispenser Inventory Sensor 3: Active high input. No internal pull-up.
5. Dispenser Motor 2: Push-pull output. No PWM.
6. Dispenser Motor 1: Push-pull output. No PWM.
7. Dispenser Manual Unload Button: Active high input. No internal pull-up.
