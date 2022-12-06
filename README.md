## What is the purpose of this project?

Economies are quite complex and unintuitive, especially when commodities contained within can be converted to others through different processes (i.e. converting raw aluminium into a can, then putting food in that can, and selling that product to the end consumer). The impact of small perturbations (or 'shocks') on the supply or demand of a particular commodity can have wide-reaching effects on the price of other commodities, even ones that initially appear unrelated. 

The goals of this project are three-fold:
 - Improve my C++, including the use of parallelization methods (MPI and OpenMP)
 - Model a market in its resting state.
 - Model shocks and (hopefully) predict their consequences


## Why the OSRS economy?

[Oldschool Runescape](https://en.wikipedia.org/wiki/Old_School_RuneScape) is an MMORPG with highly complex mechanics and systems. Items can be mined or harvested in raw forms, converted to processed forms, and then used to create products that are useful for a variety of purposes. Most items are tradeable on the [Grand Exchange](https://oldschool.runescape.wiki/w/Grand_Exchange), a centralised, automated bid system that allows players to sell or purchase items using the game's currency, gold pieces ("gp"). Because of this, OSRS's economy possesses the same level of detail and complexity as its other systems.

The game's [wiki](https://oldschool.runescape.wiki/w/Old_School_RuneScape) contains a large amount of information on the sources, uses and recipes for each and every item currently in the game. This wiki also has an [accurate record of historical prices](https://prices.runescape.wiki/osrs/) for all goods found on the Grand Exchange. Both of these services provide access to users through an API.

The retrieval of the structure and behaviour of the OSRS economy is made relatively easy through the use of the aforementioned APIs, while the market's complexity provides a sufficiently interesting challenge. Furthermore, a potential model's output can quite easily be compared against historical record to verify its accuracy. For these reasons, it is a suitable inspiration for the model.
