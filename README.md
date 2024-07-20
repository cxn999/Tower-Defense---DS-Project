Endless Tower Defense Game
Overview
This is an endless tower defense game developed using SFML in C++. The game features various scenes, including a menu, settings, credits, and the main play scene. The player must defend their tower against endless waves of enemies. The project is organized into multiple header and source files for better structure and maintainability.

Files and Descriptions
Main File
main.cpp: The entry point of the game. It initializes the game engine and starts the main game loop.
Header and Source Files

Action
Action.h / Action.cpp: Defines actions that entities can perform, such as moving or attacking.
Action: Represents an action with a name and type.
name(): Returns the name of the action.
type(): Returns the type of the action.

Animation
Animation.h / Animation.cpp: Manages animations for different game entities.
Animation: Represents an animation with a sprite, frame count, speed, and size.
update(): Updates the animation to show the next frame.
hasEnded(): Checks if the animation has ended.
getName(): Returns the name of the animation.
getSize(): Returns the size of the animation.
getSprite(): Returns the sprite of the animation.
getFrameCount(): Returns the frame count of the animation.

Assets
Assets.h / Assets.cpp: Handles loading and storing game assets like textures, sounds, and fonts.
addTexture(name, path): Adds a texture from a file path.
addSoundBuffer(name, path): Adds a sound buffer from a file path.
addMusic(name, path): Adds music from a file path.
addFont(name, path): Adds a font from a file path.
addAnimation(name, animation): Adds an animation.
getTexture(name): Retrieves a texture by name.
getSoundBuffer(name): Retrieves a sound buffer by name.
getSound(name): Retrieves a sound by name.
getMusic(name): Retrieves music by name.
getFont(name): Retrieves a font by name.
getAnimation(name): Retrieves an animation by name.
loadFromFile(path): Loads assets from a file.

Components
Components.h: Defines various components that can be attached to entities, such as position, velocity, and health.
Component: Base class for all components.
CTransform: Represents the position, previous position, velocity, and movement state of an entity.
CInput: Handles input states like click and right-click.
CBoundingBox: Defines the size and half-size for collision detection.
CAnimation: Manages animations and their repeat state.
CState: Maintains the state and direction of an entity.
CLevel: Tracks the level and maximum level of an entity.
CHealth: Manages health, total health, and previous health of an entity.
CAttack: Represents the damage an entity can deal.
CRange: Defines the radius and target state for range-based interactions.
CType: Stores the type of an entity.
CDelay: Handles attack delays.
CFocus: Manages the entity that an entity is focused on.

Entity
Entity.h / Entity.cpp: Represents game entities with unique IDs and manages their components.
Entity: Represents a game entity with a unique ID, tag, and active state. It contains methods for adding, removing, and accessing components, as well as managing the entity's lifecycle.
id(): Returns the unique ID of the entity.
isActive(): Checks if the entity is active.
tag(): Returns the tag of the entity.
destroy(): Marks the entity as inactive.

EntityManager
EntityManager.h / EntityManager.cpp: Manages all game entities, including their creation, destruction, and updates.
EntityManager: Manages the collection of entities, including adding, updating, and removing entities. It also integrates a quadtree for efficient spatial queries.
init(): Initializes the entity manager.
removeDeadEntities(EntityVec& vec): Removes inactive entities from a given vector.
update(): Updates the state of the entity manager, including adding new entities and removing dead ones.
addEntity(const std::string& tag): Adds a new entity with a specified tag.
getEntities(): Returns a vector of all entities.
getEntities(const std::string& tag): Returns a vector of entities with a specified tag.
queryRange(sf::FloatRect range): Queries entities within a specified range using the quadtree.

GameEngine
GameEngine.h / GameEngine.cpp: The core of the game, handling the game loop, scene transitions, and overall game state.
GameEngine: Initializes and runs the game, manages scenes, handles user input, and controls the game state.
GameEngine(const std::string& path): Constructor that initializes the game engine with a specified asset path.
init(const std::string& path): Loads assets and sets up the game window.
update(): Updates the current scene and handles user input.
sUserInput(): Processes user input events.
currentScene(): Returns the current active scene.
run(): Main game loop that continues running while the game is active.
quit(): Quits the game by stopping the game loop.
getAssets(): Returns the game assets.
changeScene(const std::string& sceneName, std::shared_ptr<Scene> scene, bool endCurrentScene): Changes the current scene to a new scene.
window(): Returns the game window.
isRunning(): Checks if the game is still running.
getVolume(): Returns the current volume level.
getMusic(): Returns the music state.
setVolume(float volume): Sets the game volume.
setMusic(bool music): Sets the music state.
startMusic(const std::string& musicName): Starts playing a specified music track.
stopMusic(): Stops the current music track.
setMusicState(bool musicOn): Sets the music playback state.
switchToMusic(const std::string& musicName): Switches to a specified music track.

Physics
Physics.h / Physics.cpp: Manages the physics system, including collision detection and response.
GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b): Calculates the overlap between two entities based on their current positions and bounding boxes.
GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b): Calculates the overlap between two entities based on their previous positions and bounding boxes.
GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b):
Calculates the overlap in the x and y directions between the bounding boxes of two entities based on their current positions.
GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b):
Calculates the overlap in the x and y directions between the bounding boxes of two entities based on their previous positions.

Quadtree
Quadtree.h / Quadtree.cpp: Implements a quadtree data structure for efficient spatial partitioning and collision detection.
Quadtree: Represents the quadtree data structure, which manages the insertion, clearing, and querying of entities within spatial bounds.
insert(std::shared_ptr<Entity> entity): Inserts an entity into the quadtree.
clear(): Clears the quadtree.
query(sf::FloatRect range): Queries entities within a specified range.
Quadtree(sf::FloatRect bounds): Constructor that initializes the quadtree with specified bounds.
insert(std::shared_ptr<Entity> entity):
Inserts an entity into the quadtree node if it intersects with the node's bounds.
Subdivides the node if the maximum number of entities is exceeded.
clear(): Deletes the root node and initializes a new root node.
query(sf::FloatRect range): Recursively queries entities within the specified range, adding them to a vector of found entities.
subdivide(QuadtreeNode node): Subdivides a quadtree node into four child nodes.
query(QuadtreeNode node, sf::FloatRect range, std::vector<std::shared_ptr<Entity>>& found):
Recursively queries a quadtree node and its children for entities within the specified range.


Scene
Scene.h / Scene.cpp: Abstract base class for all scenes in the game, providing a common interface for initialization, updating, and rendering.
Scene: Manages entities and actions within a scene.
setPaused(bool paused): Pauses or resumes the scene.
update(): Updates the scene. (Pure virtual)
sDoAction(const Action& action): Performs the given action in the scene. (Pure virtual)
sRender(): Renders the scene. (Pure virtual)
doAction(const Action& action): Calls the sDoAction method.
registerAction(int inputKey, const std::string& actionName): Registers an action with the specified key.
width() const: Returns the width of the game window.
height() const: Returns the height of the game window.
currentFrame() const: Returns the current frame of the scene.
hasEnded() const: Checks if the scene has ended.
getActionMap() const: Returns the action map.

Scene Credits
Scene_Credits.h / Scene_Credits.cpp: Implements the credits scene, displaying information about the game creators.
Scene_Credits: Derived from Scene, it displays the credits.
init(): Initializes the credits scene.
update(): Updates the credits scene.
onEnd(): Ends the credits scene.
sDoAction(const Action& action): Performs the given action in the credits scene.
sRender(): Renders the credits scene.

Scene Menu
Scene_Menu.h / Scene_Menu.cpp: Implements the main menu scene, allowing players to start the game, view settings, or see credits.
Scene_Menu: Derived from Scene, it displays the main menu.
init(): Initializes the menu scene.
update(): Updates the menu scene.
onEnd(): Ends the menu scene.
sDoAction(const Action& action): Performs the given action in the menu scene.
sRender(): Renders the menu scene.

Scene Play
Scene_Play::Scene_Play(GameEngine* gameEngine)`
Constructor: Initializes the `Scene_Play` object with a `GameEngine` pointer and calls the `init` function.
void Scene_Play::init() Initializes the scene by setting the highest score, registering keyboard actions, loading the level, and setting up the view and text elements.
void Scene_Play::sDoAction(const Action& action) Handles actions based on input events, such as toggling textures or collision boxes, pausing the game, quitting the game, and handling player movement.
void Scene_Play::update() Updates the game state by removing dead platforms, updating entities, handling collisions, movement, platform generation, animations, and rendering the scene.
void Scene_Play::spawnPlayer() Spawns the player entity, adding components such as animation, bounding box, transform, gravity, and state.
void Scene_Play::loadLevel() Loads the initial level by resetting the entity manager, spawning the player, and generating initial platforms with random properties.
void Scene_Play::sRender() Renders the game scene, including the player, background layers with parallax effects, platforms, score text, and UI elements for replay and quit options.
void Scene_Play::sAnimation() Updates the player's animation state based on its movement and equipped skin, ensuring the correct animation is displayed for jumping or falling.
void Scene_Play::sCollision() Handles collision detection between the player and platforms, updating the player's state and position accordingly. If the player falls off the screen, it destroys the player entity.
void Scene_Play::sMovement() Handles player movement based on input, updates the player's position and velocity, and manages the camera's view following the player smoothly.
void Scene_Play::onEnd() Ends the current scene and changes to the "MENU" scene.
void Scene_Play::sPlatformGeneration() Generates new platforms as the player progresses upward, using random distribution to determine platform types and properties.
sf::Vector2f lerp(const sf::Vector2f& start, const sf::Vector2f& end, float t)Utility function to linearly interpolate between two `sf::Vector2f` points based on a parameter `t`.

Scene play.h
Scene_Play(GameEngine* gameEngine)
Constructor: Initializes the `Scene_Play` object with a reference to the game engine.
void init() Initialization: Sets up the scene, initializing variables and preparing the scene for gameplay.
void update() Updates the game logic, including player movement, collisions, and other game mechanics.
void onEnd() End Scene: Called when the scene ends, performing any necessary cleanup or transitions.
void replay() Resets the scene to its initial state for replaying.
void sRemoveDeadPlatforms() Removes platforms that are no longer needed or off-screen.
void sPlatformGeneration() Handles the generation of new platforms within the scene.
void sAnimation() Manages animations within the scene.
void sMovement() Controls the movement of entities, including the player and other game objects.
void sCollision()Manages collision detection and response between entities.
void sRender()Renders the scene, including all entities, backgrounds, and UI elements.
void sDoAction(const Action& a) Executes an action, typically in response to user input.
void spawnPlayer() Creates and initializes the player entity in the scene.
void loadLevel() Loads the level data, including platforms, background, and other elements.

Scene Settings
Scene_Settings.h / Scene_Settings.cpp: Implements the settings scene, allowing players to configure game options.
Scene_Settings: Derived from Scene, it displays the settings.
Constructor (Scene_Settings(GameEngine gameEngine = nullptr)): Initializes the settings scene with the provided game engine pointer.
init(): Initializes the settings scene by setting up text elements, UI components (like sliders), and registering actions.
update(): Updates the settings scene, including handling entity updates and rendering the current state.
onEnd(): Transitions from the settings scene to the main menu scene.
sDoAction(const Action& action): Handles user actions (such as pressing keys or adjusting settings) within the settings scene. Actions include toggling music, adjusting volume, and navigating the menu.
sRender(): Renders the settings scene to the window, including the title text, menu options, volume slider, and other UI elements.
getVolumeColor(float volume): Determines the color of the volume slider based on the current volume level, transitioning from green to red.
updateVolumeSlider(): Updates the size and position of the volume slider based on the current volume level.

Vec2
Vec2.h / Vec2.cpp: Defines a 2D vector class used for positions, velocities, and other vector operations in the game.
Vec2: Represents a vector in 2D space with x and y coordinates.
Default Constructor: Initializes the vector to (0, 0).
Parameterized Constructor: Initializes the vector to the specified x and y values.
Operator Overloads:
==: Checks if two vectors are equal.
!=: Checks if two vectors are not equal.
+: Adds two vectors.
-: Subtracts one vector from another.
*: Multiplies a vector by a scalar.
/: Divides a vector by a scalar.
+=: Adds another vector to this vector.
-=: Subtracts another vector from this vector.
*=: Multiplies this vector by a scalar.
/=: Divides this vector by a scalar.
dist(const Vec2& rhs): Calculates the distance between this vector and another vector.


Other Files
Scene_Play.h / Scene_Play.cpp: Implements the main play scene, where the actual game takes place. It manages the game world, entities, and gameplay logic.

How to Compile and Run
1. Ensure you have SFML installed on your system.
Use a C++ compiler to compile the source files. For example, using g++:
sh
Copy code
g++ -std=c++11 -o tower_defense main.cpp Action.cpp Animation.cpp Assets.cpp Entity.cpp EntityManager.cpp GameEngine.cpp Physics.cpp Quadtree.cpp Scene.cpp Scene_Credits.cpp Scene_Menu.cpp Scene_Play.cpp Scene_Settings.cpp Vec2.cpp -lsfml-graphics -lsfml-window -lsfml-system

2. Run the compiled executable:
sh
Copy code
./tower_defense

3. Game Controls
Mouse: Interact with the game UI and place towers.
Keyboard: Use various keys to control the game (specific controls can be listed here).
