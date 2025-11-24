# HonorsTesting
Procedural generation of a hex grid to create a map suitable for isometric genres.

Developed with Unreal Engine 5 with a mixture of blueprints and c++
(As of 19/11/25):

## World
Level currently contains a prototype level with a hexgrid and hex layer object. Selecting these will expose parameters to begin/adjust the grid.

## Grid formation
Can specify column and row dimensions for hex grid.
Note that the larger the grid the longer the program takes to populate the grid with tiles and consequentially the longer the algorithms take to generate their respective areas.

## Path Generation
Currently using Drunkards walk to generate pathways on grid. Path tiles are denoted as black tiles.
Can change the starting point of the path generation by specifying a grid coordinate (X = row, Y = column). This is displayed as a red tile on the starting grid.
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

## Voronoi Regions
Can toggle the formation of biomes within editor. 
Using the editor supplied number, starting region "seeds" are placed randomly throught the grid. Each tile then calculates which seed it is closest to and is assigned that region.
This process is random and due to the small number of different regions may result in large singular regions
There are currently 5 unique regions: Lava (red), Water (blue), Ice (light blue), Moss (light green) and Rock (brown)

## Known bugs & issues
- Current generation methods are non-persistant when modifying each generation type
- Editor performance slows when selecting hex grid object, this progressively gets worse as the hex grid size increases.
    - this is alleviated by using the hex layers object instead (at the cost of debug info)
- Iteration count not working for hex layer object
