# Procedural World Generator
## COMP3016 Coursework Part C2 - The Interactive Software Prototype
### Overview
The Procedural World Generator is for the purpose of establishing a pseudo-randomised 3-dimensional world that a given user is able to traverse. Its main features include procedurally generating natural style terrain composed of two distinguishable biomes determined by height level: grassland and tundra. This project’s central focus is on world building and user interaction in relation to it. There is also a [video demonstration](https://www.youtube.com/watch?v=H-5Apw2nG-k) of the project.
### Additions
None of the following additions to this project were implemented due to various constraints. Originally, there were going to be procedurally generated artificial structures. Examples to be implemented were trees, rocks and cacti. There was also going to be a world time that would perpetually update, which was to be represented through a sun, moon and their effects on the world’s ambient lighting. Animation was also going to be used where appropriate, such as for tree leaves blowing in wind. Weather was also going to be introduced in the form of random weather cycling between three weather systems: clear, rain and snow. Lastly, sounds were also going to be implemented. This was to come in the form of ambient sounds, such as bird noises. User generated sounds were also originally going to be implemented, such as the sound of the friction of movement while walking against different types of ground. Potentially, sounds may have also been developed to be altered by the environmental context, such as their ability to deliver the sensation of reverb and echo in caves.
### User Interaction
#### Execution of Program
In order to execute the program, unzip the Procedural World Generator Executable.zip file. Next, run the Coursework2.exe file. Ensure that all the files alongside the executable are not moved in relation to it.
#### Controls
##### Description
The main features regarding user interaction are the ability of the user to move forwards, left, right and backwards, as well as change their field of vision with the mouse. Originally, collision detection was going to be implemented, which would have allowed for walking, sprinting and jumping, however various constraints prevented this. Additional unimplemented features include player health and the ability for the player to lose health where appropriate. Examples of how this was going to be implemented are through fall damage and touching sharp or prickly objects, such as cacti.
##### Inputs
|Action|Binding|Description|
|--|--|--|
|Forward|W|Moves the player forward.|
|Left|A|Moves the player left.|
|Backward|S|Moves the player backward.|
|Right|D|Moves the player right.|
|Close and Exit|Esc (Escape)|Closes the program window and exits the program.|
|Direction|Mouse Movement|Rotates the field of vision in relation to the direction of the mouse movement.|
|Render Toggle|1|Switches the rendering system between triangles and wireframe.|
### Implementation
#### Overview
The majority of the C++ OpenGL code; that is, all the non-sample code given for the coursework, is contained exclusively within the Project.h and Project.cpp files. No classes are utilised within the Project.h and Project.cpp files, however the sample code for loading shaders is encapsulated within a class. However, there are classes developed for coursework 2 that exist within the project that have yet to be used. Specifically, the file reader class, derivative of COMP3016's Coursework 1, exists within the project. Two other classes, the Mesh and Model classes, were going to be used for model loading. They also exist within the project, however they are commented out.
#### Technologies
The Procedural World Generator will use the OpenGL 4 graphics API with the C++ programming language. Since OpenGL is being utilised, the GLSL language will also be used for shaders. The third-party [Fast Noise Lite](https://github.com/Auburn/FastNoiseLite) C++ header file is also used within this project for generating Perlin noise for the map generation.
#### The Initialisation Function
The init (initialisation) function is called by the main function in order to do most of the CPU work for generating the procedural world. In order as written, it sets up the shaders for the map, generates the vertices and indices for the map and lastly sends them to the GPU in the form of vertex buffer objects.
#### Procedural Generation
The procedurally generated map is created through three stages. Initially, a two loop deep nested loop is used to generate the height map; it is there that the noise function is used. Next, one loop (with some loop nesting) is used to generate the rest of the map’s vertices; that is, the x and z planes for the map’s width in 3D space. Additionally, it also generates the colours in respect of biomes based on relevant conditions. Lastly, a loop is used in order to generate the indices for the map.
### Novelty
#### Overview
The Procedural World Generator is unique to other similar projects in that it uses a height map in order to determine the biome. At relatively low heights, grass is generated, indicated by the green colour. At relatively high heights, a white colour is used to signify that due to the decreased temperature higher in the atmosphere that snow is present. The result is that there are two biomes: a grassland and a mountainous tundra biome.
#### Comparison to Similar Projects
A project that is similar to the Procedural World Generator is the [Procedural Terrain Generator (With Tessellation) OpenGL](https://github.com/stanfortonski/Procedural-Terrain-Generator-OpenGL). Both generate procedural worlds with OpenGL, however the Procedural World Generator project differs in that it generates the aforementioned height-based tundra biome which appears on the peaks of mountains. Alternatively, the similar Procedural Terrain Generator project generates water at relatively low levels in its world.
#### Conception and Inspiration
The idea to create a procedurally generated world came from two paramount sources. The inspiration for its focus on user interaction comes from how user interaction was implemented within the world in the game The Legend of Zelda: Breath of the Wild developed by Nintendo. However, due to various constraints, the finished Procedural World Generator project does not include any notable features with this regard. The focus on procedural generation and world building; that is, having a vibrant and complex procedural world, comes from a separate project of the same developer as the Procedural World Generator project. This project currently exists closed source under the provisional name Dragon Survival Game Project, in which a vast and relatively complex procedurally generated world will likely be implemented.
#### Initiation
##### First-Party
The only developments that existed prior to the initiation of the Procedural World Generator project were those from the COMP3016 Coursework 1 task to develop a C++ file reader tailored towards model loading for OpenGL. The file reader is implemented into the Procedural World Generator, however it has yet to be used due to various constraints. Originally, the file reader was given by default with stub methods for the sake of development for the COMP3016 Coursework 1. The most recent version, as implemented into the project, is therefore otherwise entirely developed by the creator of the Procedural World Generator project.
##### Third-Party
The Procedural World Generator uses the OpenGL 4 graphics API with the C++ programming language. Since OpenGL is being utilised, the GLSL language is also used for shaders. Additionally, Fast Noise Lite’s C++ header file, FastNoiseLite.h, was used in order to generate the Perlin Noise map for the procedural generation. Lastly, the LoadShaders.h and LoadShaders.cpp files given by default for the COMP3016 Coursework 2 module are included for the use of loading any required shaders.
### References
- [Procedural Terrain Generator (With Tessellation) OpenGL](https://github.com/stanfortonski/Procedural-Terrain-Generator-OpenGL) used as a comparison to the Procedural World Generator Project in this document
- [Fast Noise Lite](https://github.com/Auburn/FastNoiseLite) is used to generate the noise map used for the procedural terrain generation in the project
### Appendix
|Feature|Description|Implementation|
|--|--|--|
|Natural terrain|A world with forestry, desert & caves.|Partially|
|Player traversal|Walking, sprinting & jumping.|Partially|
|Structures|Trees, rocks, cacti, artificial structures, etc.|Discontinued|
|Health & food|Health loss where appropriate, with naturally spawning apples (on trees) that can be collected & eaten to regenerate health.|Discontinued|
|Time|A sun, moon & time change that affects the ambient lighting of the world.|Discontinued|
|Animation|Where appropriate, animation of certain objects in the world, such as leaves on trees.|Discontinued|
|Weather|Random cycling through clear, rain & snow weather systems.|Discontinued|
|Sound|Ambient sounds, such as bird noises; movement sounds; environmentally affected sounds, such as echoes in caves.|Discontinued|
