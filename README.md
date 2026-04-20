# HonorsTesting
Procedural generation of a hex grid to create a map suitable for isometric genres.

Developed with Unreal Engine 5 with a mixture of blueprints and C++
(As of 20/04/25):

## World
Contains two level generation objects (BP_HexLayer & BP_HexGrid)
Selecting these will expose parameters to configure the grid.

### BP_HexGrid
Debug version of map generation techniques.
Max 1 layer and all grid information (tile coords, tile tags, tile mesh instances) publicly exposed to inspect and adjust generation techniques.
Should only use small grids (10x10 or 20x20) as UE will struggle to load all tile information.

### BP_HexLayers
Release version of map generation techniques.
Grid tile information is obscured and unavailable for editor inspection to improve generation performance.
Contains methods to save/load generation layouts.

## Generation Parameters
Parameters that govern how the map layout is generated.
Some parameters are only available in specific generation objects and will be specified (Release or Debug) otherwise stated parameters are shared.

### World Parameters (Release only)
Denotes the activation of the generation as well as specifying how many layers to construct. 
Level separation adjusts the distance between each layer.

### Metrics (Release only)
Variables that display grid generation information (Path coverage, isolated patches of paths (islands))

### Saved Parameters (Release only)
Specifies which layout type to save/load.
Controls when to save/load a layout (note activating both load and save together will cause a crash).
Specifies which data asset object to save/load info to.

### Grid Properties
Specifies map layout properties: Grid size and path starting point.

### Drunkard Config
Specifies Path size and max number of iterations for the algorithm to perform.

### Perlin Landscape Config
Specifies max height of landscape tiles, as well as noise parameters.

### Perlin Worms Config
Specifies characteristics of worms for path generation.
Parameters adjust the number, length, and noise sample of each worm.

### DLA Config
Specifies starting area characteristics using Perlin worms.
Specifies type of DLA to perform as well as how large the path generated should be.

### Cellular Config
Specifies type of cellular automata to perform.
Specifies iteration number for the automata to stop at.
Specifies neighbour distance (currently only implemented to use 1st nearest neighbour)
Code sequence allows user to edit the wolfram neighbour codes used to determine tile states (only applicable when Wolfram type is selected).
Game of life config specifies the parameters used to determine current tile state from all neighbouring tiles.
Contains Tags containers that can edit which tile tags are used for checking, applying or reverting to for customising which tile types of the automata applies to.

### Tile Config
Can inspect and change default meshes for each type of instance mesh.
Can adjust assigned tags used by the generation methods.

### Operation Config
Adjusts  map generation using the specified operations.

Initialise Grid (Debug only)
- Generates the map layout with specified techniques, will generate layout within the editor.
- Read only value in BP_HexLayers as generation is instantiated within the World Parameters method.

## Known bugs & issues
- Editor performance slows when using hex grid object, this progressively gets worse as the hex grid size increases.
    - this is alleviated by using the hex layers object instead (at the cost of debug info)
- Activating the generation while save and load layout are both active will either cause the program to freeze and/or crash.
