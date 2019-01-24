# Project King, The Dark Souls PvP AI
Based on Metal-Crow's original code, this one is edited to effectively use Chaos Blade rather than curved swords.
Be sure to check out the original, as well as Metal-Crow's many other projects here! https://github.com/metal-crow

Leaving much of the description the same/similar to his as it's just instructions to make things work.
  
## Setup
* Install [Vjoy](http://vjoystick.sourceforge.net/site/index.php/download-a-install).
* Create Vjoy controller with the config: x,y,z,rx,ry,20 buttons,1 discrete pov
* Install [Cheat Engine](http://www.cheatengine.org)
* Download [Technojacker's DARKSOULS.CT table](https://drive.google.com/folderview?id=0B_f11g1DlLhDV1RfV0VSdnBfOVE&usp=sharing)  
 * Cheat Engine and the table are only used to lock the camera's x rotation to 3.141592. My method for doing this is a script that AOB scans `F3 0F 11 83 44 01 00 00 80 BB 63 02 00 00 00`, noops, then lock camera x via CE freeze.
    * Note: This is map dependent
      * For Oolacile township, set the rotation to 3.141592 and enable the OolicelMap setting.
      * For Sunlight alter, set the rotation to 3.141592 and disable the OolicelMap setting.
      * For Undead burg, set the rotation to 0.0 and enable the OolicelMap setting.

 * The helper table I used (modified from Technojacker's) is [here](https://github.com/metal-crow/Dark-Souls-PvP-AI/blob/master/DARKSOULS_AI_Help.CT)
* Download the source code and compile with [FANN](http://leenissen.dk). Add the FANN dlls and the vJoyInterface dll to the exe's folder.
* Train the defense neural network (see the methods in InitalizeFANN, use the main method in there with them) and put the .net file in a folder in the .exe's location called "Neural Nets"
 * Or use the already trained nets in the latest release
* (If you want the gui) Import the folder for the GUI into eclipse and run that. Or run the included .jar file.
* Run the PvP AI whenever the opponent is ready.

## Neural Networks
Defense Network Inputs:  

  * Array of distance between AI and enemy over 0.5 second period 
  * The angle the enemy is off from directly in front of the player  
  * The enemy velocity  
  * The rotation difference between the enemy and the player  

Attack Network Inputs:  

  * Array of distance between AI and enemy over 5 second period
  * Estimated stamina of enemy
  * Poise of enemy 
  * poise damage of AI's attack
  * the AI's current poise
  * base poise damage of enemy's attack
  * array of AI's HP over time
  * stamina of AI
  * array of enemy animation types
  * current bleed build up
  * NOTES:
    * usually tell how they attack by how long it's been since they last attacked

## TODO 

  * With time, this release will hopefully be edited to use many weapons more effectively
  * modify attack behaviors to be more effective and consistent (mainly kicks, which are inconsistent currently)
  * fix him trying to toggle out of fast weaps? 

## Using:   
http://vjoystick.sourceforge.net (VJOY)  
http://leenissen.dk (FANN)  
