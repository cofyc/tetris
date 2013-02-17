# The Tetris

## Overview

**CTetris**, tetris game under terminal, written in c.

## Build & Install

	# need ncurses
    $ make          
    $ make install

## How to play
    
    $ ct    # start the game,
    $ ct -h # see cli options

When you are plaing, you can see instructions and score on the right side of game screen.

You can type 'q' or <CTRL-C> to terminate game.

### Moving the Tetriminos

Typing 'h', 'j', 'k', 'l' letters, you can adjust where and how the Tetriminos fall. 
   
By pressing 'h', 'l', you can slide the falling Tetriminos from side to side.

By pressing 'k', you can rotate the Tetriminos 90 degrees clockwise.

### Hard Drop and Soft Drop

The *Soft Drop* is performed by pressing the 'j'     letter, the Tetrimino will fall much faster.

The *Hard Drop* is performed by pressing the space bar button to cause the Tetrimino to fall straight down.

## Playing on Mac OS X

I strongly recommend iTerm2 (<http://www.iterm2.com/>), because Mac OS X native Terminal cannot hide the terminal cursor which is very annoying!.

## References

0. http://en.wikipedia.org/wiki/Ncurses
1. http://invisible-island.net/ncurses/ncurses-intro.html
