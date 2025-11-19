# HonorsTesting
Procedural generation of a hex grid to create a map suitable for isometric genres.

Developed with Unreal Engine 5 with a mixture of blueprints and c++
(As of 19/11/25):

## World
Level currently contains a prototype level with a singular hexgrid object. Selecting this object will expose parameters to begin/adjust the hex grid.

## Grid formation
Can specify column and row dimensions for hex grid.
Note that the larger the grid the longer the program takes to populate the grid with tiles and consequentially the longer the algorithms take to generate their respective areas.

## Path Generation
Currently using Drunkards walk to generate pathways on grid. Path tiles are denoted as black tiles.
Can change the starting point of the path generation by specifying a grid coordinate. This is displayed as a red tile until on a non generated grid.
Can change the size of path and limit the number of iterations for the algorithm to attempt to generate the path
Path will always be generated however, the generated path may not always match the specified size. 
This could be due for a number of reasons:
- path size is too large for the current grid size
- starting point is too close to boundary tiles
- number of iterations is too low

## Landscape Generation
Currently using a simple UE default perlin noise technique to scale landscape tiles height.
Landscape tiles are denoted as green tiles.
Can adjust the max height for the algorithm to scale the tiles to.
Can adjust the scaling factor of the algorithm which adjusts the uniformity of the tiles heights.

## Known bugs
- Path generation does not persist when toggling the landscape generation, a new path is generated each time.
    - Path tiles generated this way can overlap causing multiple tile types existing on same coordinate
    - Currently only generate path once landscape is generated.
- Editor performance slows when selecting hex grid object, this progressively gets worse as the hex grid size increases.
- Adjusting the start point for the pathways while grid is formed causes overlap with tiles, toggle initialise grid to resolve.
