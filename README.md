# TreeMinder
Water drinking activity tracker (Computer Interfacing IoT Project)
# Hardware Used
- Cortex-M3 STM32 w/ RTC,LCD, UARTs
- NodeMCU
- Servo
- Water tap
- Coke bottle
# HW Feature (C + IO Controlling)
- Calculate the water amount (in ml -> glasses) should the user drink in a day by using these factors
  - Age
  - Weight
- Calculate the times should the user drink a glass of water by using these factors
  - Wake time
  - Sleep time
- Dispense the calculated amount of water when the clock reach calculated periods of the day
- Use Built-In Real-Time Clock of STM32 board (RTC) for a clock
- LCD Screen displays info and decent GUI
- Forest of 7 tree (Show in GUI) represents user's drinking activity of each time of a day (Green, Wither, -) for each week.

# IoT Feature (Arduino C,PHP)
- Can be controlled by LINE Messaging application by chatting with a bot
 - Manually Open / Close tap
 - Set Age,Weight,Wake,Sleep,Name,Clock
