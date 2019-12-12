# In progress #

## What is it? ##
1/2/3 button macros for:
 * reseting raids with save
 * reseting raids without save (manual save before encounter targeted pokemon) //to do
 * transfer whole box special trade 
 * farm watts
 
 Standard Joystick.c (in this project is for **reseting raids with save**)

## Wiring ##
![Image of board](http://yozen.ct8.pl/teensy/github/board.png)

## Promoted raid shiny hunting - How to use it? ##
Go to [shinyquagsire23/Switch-Fightstick](https://github.com/shinyquagsire23/Switch-Fightstick#compiling-and-flashing-onto-the-teensy-20) how to compile it for Teensy 2.0++.

* Connect Teensy to switch/dock (no other additional pads can be connected - JoyCons attached to the console are OK)
* Go to **Controllers > Change Grip/Order** menu and press **Button 2** few times. Your console should recognize new pad - great!
* Open the game and do [this](https://www.youtube.com/watch?v=G0GLuG8Z3IE) if you are trying to get any shiny Gigantamax Pokemon
* Set date to 1 day of December (can be any month with 31 days)
* Get watts from den and close it
* Save game
* Open the den (just open and do not click anything)
* Press **Button 1** to reset the den (macro will press Invite Others > Change date > Exit searching > re-enter the den). Also this button is set addtional LED to OFF)
* Press **Button 3** if you want to get inside the den and check if pokemon is shiny (it will execute the macro - open den > you have to visually check if shiny > macro will close game > start it once again > open den. And you are on the same screen as you was). Additional LED on the board will be ON. To show that **Button 3** was pressed. Sometimes you can forgot which button you have pressed so now you will know.
* Repeat clicking **Button 1** and **Button 3**

Why starting from 01.XX.2019?
Because macro is set to go over 31 days. When date is changed from 31 to 01 raid den needs different key input (den is not glowing). When date is change from 31 to 01 **Button 3** is disabled because pokemon from 31 and 01 is the same.

## Watt Generator ##
* Go to **Controllers > Change Grip/Order** menu and press **Button 2** few times. Your console should recognize new pad - great!
* Open the game
* Set date to 1 day of December (can be any month with 31 days)
* Get watts from den and close it
* Save game
* Open the den (just open and do not click anything)
* Press **Button 1** to reset the den (it will reset the raid den over and over). To stop unplug the teensy.

## Suprise trade full box ##
* Go to **Controllers > Change Grip/Order** menu and press **Button 2** few times. Your console should recognize new pad - great!
* Open the game
* Connect to internet **Y > +**.
* Go to pokemon boxes and select your box which will be traded (jut open it - For example Box 11 with your IV4/IV5 no longer usefull breeded pokemons).
* Close the box and main menu (you should see your character).
* Press **Button 1** to run macro.
* Wait for Blue Y-COM communicate that trade is over.
* Press **Button 2** to get pokemon (if new entry in pokeex or pokemon evolving keep pressing it)
* Repeat from **Button 1**

# Own macros #
Edit those lines: (leave those 3 from top)
```
static const command step[] = {
	// Setup controller
	{ NOTHING,   10 },
	{ TRIGGERS,   5 },
	{ NOTHING,   10 },
	
	// RAID RESET	
	{ A,          5 },
	{ NOTHING,   90 },
	{ HOME,       5 }, 
	{ NOTHING,   20 },
	{ RIGHT,      5 }, 
	{ NOTHING,    1 },
	{ RIGHT,      5 },
 ```
