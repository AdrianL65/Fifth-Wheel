# Fifth-Wheel

The equipment used for this project consist of an electric scooter brushless motor wheel, a motorbike driver, A 24 volt rechargeable battery, LEDs, a potentiometer, a switch, buttons, two inductive sensors, PLA filament for 3D printing, steel tubes and sheets, aluminum plates, a set of bearings and a microcontroller (KL25Z). 
The design consist of making an intergraded attachment that holds its battery, motor driver, electric scooter wheel, PCB and control panel in one place. The previous, so that the attachment can be transferable between different standard wheelchairs. The Wheelchair has two modes of operation automatic 

My main role in this project was to program the microcontroller (KL25Z) to establish the motor’s velocity according to the user’s input (via a potentiometer or via the velocity logged into the system by the user’s stroke on the main wheels; the election between the two types of input is determined via a button) and receive data from a set of sensors (accelerometer, inductive sensors, encoder, temperature sensor, potentiometer, 2 buttons, additionally the motor has a built in encoder that outputs the motors frequency) to detect If the user is on a ramp and grant additional acceleration, to monitor the battery life of the whole system and to measure the motor driver’s temperature. Additionally programmed a set of actuators (LEDs, motor itself) to ensure the user is notified of any safety hazards and the system does not operate while under dangerous parameters. Protocols used include IC2 and UART. My second role was the design of the user’s control panel as well as the 3D printing process.  

My main role in this project was to program the microcontroller (KL25Z) to receive inputs and actuate the system accordingly. Regarding the microcontroller’s inputs, the system counts with a battery consumption sensory circuit, a potentiometer (for velocity control), a temperature sensor (for the driver’s safety), additionally the motor has a built in encoder that outputs the motors frequency. By receiving the previous inputs the microcontroller then actuates the velocity of the electric motor and operates the signaling LEDs in the user’s control panel. My second role was the design of the user’s control panel as well as the 3D printing process.  
