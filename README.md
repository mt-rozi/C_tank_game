# tank_game
This is a game similar to tank game on pegasus write in C and using Allegro library. 
Game written after a half-year course C,as a credit program.

library:
  sudo apt-get install liballegro4-dev

compilation:
  gcc main.c -o wy $(allegro-config --libs)

gameplay:
  moving: arrow keys
  shot: space
  escape: esc
