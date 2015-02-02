Camel up computer
=================

> In Camel Up, up to eight players bet on five racing camels, trying to suss out which will place first and second in a quick race around a pyramid. The earlier you place your bet, the more you can win — should you guess correctly, of course. Camels don't run neatly, however, sometimes landing on top of another one and being carried toward the finish line. Who's going to run when? That all depends on how the dice come out of the pyramid dice shaker, which releases one die at a time when players pause from their bets long enough to see who's actually moving!

This application makes it possible to play against the computer or analyze a situation for *2-player Camel Up games*!

## Installation

```
apt-get install build-essentials
make
```

## Providing the board situation
See the file `setup.txt` for an example initial situation, and see setup-meaning.txt for a complete description. NOTE: currently the program assumes you enter the camels from back to front! So the last camel in the race is numbered `0` and the currently winning camel `4`.

## Analyze!
Analyze the situation by running

```
./main [number of moves to think ahead]
```

Sample output would be:

```
> ./main 5
Rolling gives -0.131 coins
A card of camel #4 gives 1.055 coins
A card of camel #3 gives 0.313 coins
A card of camel #2 gives 0.488 coins
A card of camel #1 gives 0.220 coins
A card of camel #0 gives 0.810 coins
Expected score at depth 5: 1.06
Number of nodes: 1286685

(Hits, misses, collisions): (609357, 8326, 64)
Take a card from the pile of camel #4
```

NOTE: scores are relative. That is: +2 means that the computer will have an expected amount of 2 points more than the other player.