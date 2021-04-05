# Tetris-8051
## Tetris game on 8051 microcontroller
#### Basic components used
- Programming in C
- AT89C5131A microcontroller board
- 16x2 LCD display
- USB UART module (PL2303)
- Realterm Serial Capture Program
- Keil uVision5 as IDE
- ATMEL FLIP for flashing HEX file to uC

#### Objectives
- Follow basic principles of [Tetris](https://en.wikipedia.org/wiki/Tetris)
- Game to be played with 3 keyboard keys, characters of which to be transmitted to uC via UART
- Blocks to be implemented (with rotation wherever possible) are in the [pdf](https://github.com/NeelkamalBhuyan/Tetris-8051/blob/main/tetris.pdf) provided
- Pseudo random block generator using [LSFR](https://en.wikipedia.org/wiki/Linear-feedback_shift_register)
- 'q' to go up, 'a' to go down and 'r' to change orientation of block
- Game ends when incoming block cannot be accomodated in the display
- When current game ends, score is displayed, which is the number of blocks player could accomodate on the screen, along with highest score until then
- Game restarts, with highest score saved

#### Basic Methodology
- First game was developed without the rotation of the block, to get the essentials right.
- One variable to store state of LSFR
- One variable to store block number (remainder when LSFR is divided by 3)
- One variable to keep track of cursor of moving block, two in case of 2-line blocks
- Two variables (prev_cursor1 and prev_cursor2) to keep track of rightmost occupied space of LCD, one for each line
- One cycle is equivalent to incoming block stopping on the screen, at the leftmost possible position it can fit in
- At the end of each cycle, score is incremented and prev_cursor1 and prev_cursor2 are updated to new values
- When any one of prev_cursor1 or prev_cursor2 crosses the threshold (meaning that line is fully occupied and another block is trying to fit in), current game is terminated
- Score and Highest score are displayed to 5 seconds and game restarts, with highest score retained

#### Implementation of rotation of blocks
- An extra variable (rot) is used to keep track of orientation of block type
- For different block types, rot has different values and ranges
- If a single line block (that can be rotated) becomes 2-line block, and additional cursor is activated to keep track of its position on 2nd line (similar to other 2-line blocks)

#### Demonstration
[![](http://img.youtube.com/vi/lHs0zsjWFJk/0.jpg)](http://www.youtube.com/watch?v=lHs0zsjWFJk "")

#### Additional Credits
1. Prof. V Raj Babu, Prof.  S. Vijayakumaran, Prof. Sachin Patkar and Prof. Dinesh Sharma
2. EE337 TAs for providing [LCD header file](https://github.com/NeelkamalBhuyan/Tetris-8051/blob/main/lcd.h) and [Serial Comm file](https://github.com/NeelkamalBhuyan/Tetris-8051/blob/main/serial.c)
