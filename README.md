# Simulator Design

```java
/* Main Interfaces */
interface IUpdatable {
    virtual Update();
}

// Interface for an object that takes game state changing calls
// Useful for state syncer to mock state
interface ICommandTaker {
    MoveSoldier();
    MoveVillager();
    AttackUnit();
    BuildFactory();
}

// Can give commands to a command taker
interface ICommandGiver {
    RunCommands(ICommandTaker* state, PlayerState* player_state);
}

/* State */
// Main state, takes commands as a CommandTaker
class State implements IUpdatable, ICommandTaker {
    IMap map;
    IScore score;
    IPathPlanner path_planner;
    IGoldManager gold_manager;
    List<Villager> villagers;
    List<Soldier> soldiers;
    List<Factory> factories;
    List<Vector> gold_mine_locations; // Perhaps
    @Override MoveSoldier();
    @Override MoveVillager();
    @Override AttackUnit();
    @Override BuildFactory();
}

interface IMap {
    GetSize();
    GetElementSize();
    GetElementByOffSet();
    GetElementByPosition();
}
class Map implements IMap {
    int element_size;
    TerrainType terrain_type;
    List<List<MapElement*>> map_elements;
    @Override GetSize();
    @Override GetElementSize();
    @Override GetElementByOffSet();
    @Override GetElementByPosition();
}
enum TerrainType {LAND, WATER, GOLD_MINE}

// Implements the game score, can be added once core game logic is down
interface IScore {
    AddScore(PlayerId player, int score);
    ReduceScore(PlayerId player, int score);
}
class Score implements IScore {...}

// Takes a player_state, and passes it to a CommandTaker, an intermediate betwen the driver and state
// Note that PlayerState will probably mean a list of PlayerState objects (one per player)
interface IStateSyncer {
    UpdateMainState();
    UpdatePlayerState(PlayerState player_state);
}
class StateSyncer implements IStateSyncer {
    ICommandTaker state;
    ICommandGiver command_giver;
    // Call the command taker
    @Override UpdateMainState(PlayerState player_states);
    // Build the new player states from state
    @Override UpdatePlayerState(PlayerState player_state);
}

class CommandGiver implements ICommandGiver {
    // Make calls to the corresponding command taker methods
    @Override RunCommands(ICommandTaker state, PlayerState player_state);
}

interface IGoldManager {
Decrease(PlayerId player_id, int amount);
Increase(PlayerId player_id, int amount);
}
class GoldManager implements IGoldManager {...}

interface IPathPlanner {
    GetNextPosition(Vector source, Vector destination);
}
class PathPlanner implements IPathPlanner {...}

/* Actors */
class Actor implements IUpdatable {
    IGoldManager gold_manager;
    Vector position;
    int hp;
    GoldManager GetGoldManager();
}

class Unit extends Actor {
    IPathPlanner path_planner;
    int speed;
    int range;
    PathPlanner GetPathPlanner();
}

class Soldier extends Unit {
    int attack_power;
    Actor attack_target;
    SoldierState state;
    static SoldierBuilder Builder() { return new SoldierBuilder(); }
    @Override Update();
}

class Villager extends Unit {
    VillagerState state;
    static VillagerBuilder Builder() { return new VillagerBuilder(); }
    @Override Update();
}

class Factory extends Actor {
    int production_rate; // in frames per unit? 
    FactoryState state;
    static FactoryBuilder Builder() { return new FactoryBuilder(); }
    @Override Update();
}

/* Action States */
interface IActorState {
    void Enter();
    IActorState Update();
    void Exit();
}

class VillagerState implements IActorState { Villager villager;...} // Parent villager
class VillagerIdleState extends VillagerState {}
class VillagerAttackState extends VillagerState {}
class VillagerMoveState extends VillagerState {}
class VillagerMoveToBuildState extends VillagerState {}
class VillagerBuildState extends VillagerState {}
class VilagerMiningState extends VillagerState {}
class VillagerDeadState extends VillagerState {}

class FactoryState implements IActorState { Factory factory;...} // Parent factory
class FactoryUnbuiltState extends FactoryState {}
class FactoryIdleState extends FactoryState {}
class FactoryProductionState extends FactoryState {}
class FactoryDeadState extends FactoryState {}

class SoldierState implements IActorState { Soldier soldier;...} // Parent soldier
class SoldierIdleState extends SoldierState {}
class SoldierAttackState extends SoldierState {}
class SoldierMoveState extends SoldierState {}
class SoldierPursuitState extends SoldierState {}
class SoldierDeadState extends SoldierState {}
// We probably don't need dead states, since we don't have respawning,
// but it's nice to think about when building the state machine logic. Can drop later if unnecessary

// Optional builder classes, in case it looks like it'll clean up code
class SoldierBuilder {
    Soldier Build();
    SoldierBuilder setParam(param); // One setter for each class constructor member
}
class VillagerBuilder {
    Villager Build();
    VillagerBuilder setParam(param); // One setter for each class constructor member
}
class FactoryBuilder {
    Factory Build();
    FactoryBuilder setParam(param); // One setter for each class constructor member
}

/* Player State Design */


/**
 * Define a name for each soldier state
 */
enum class SoldierState {
    // Soldier is doing nothing
    IDLE,
    // Soldier is moving
    MOVE,
    // Soldier is currently attacking another actor
    ATTACK,
    // Soldier is dead
    DEAD
}

/**
* Define a name for each villager state
*/
enum class VillagerState {
    // Villager is doing nothing
    IDLE,
    // Villager is moving
    MOVE,
    // Villager is currently attacking another actor
    ATTACK,
    // Villager is currently moving to attack another actor
    MINE,
    // Villager is currently moving to build target
    BUILD,
    // Villager is dead
    DEAD
}

/**
* Define a name for each villager state
*/
enum class FactoryState {
    // Factory is not yet completely built
    UNBUILT,
    // Factory is built and idle
    IDLE,
    // Factory is currently producing villager
    VILLAGER_PRODUCTION,
    // Factory is currently producing soldier
    SOLDIER_PRODUCTION,
    // Factory is destroyed
    DESTROYED
}

/**
* Struct holding information about a soldier unit
*/
public class Soldier {
    int id;
    physics::Vector position;
    int hp;
    SoldierState state;

    int target;

    physics::Vector destination;
}

/**
* Struct holding information about a villager unit
*/
public class Villager {
    int id;
    physics::Vector position;
    int hp;
    VillagerState state;

    int target;
    int build_target;

    physics::Vector destination;
}

/**
* Struct holding information about a factory unit
*/
public class Factory {
    int id;
    physics::Vector position;
    int hp;
    FactoryState state;
    int build_percent;

    boolean suicide;
}

/**
* Struct holding information about each map grid element
*/
public class MapElement {
    TerrainType type;
}

public class State {
    List<List<MapElement>> map;

    List<Soldier> soldiers;

    List<Soldier> enemy_soldiers;

    List<Villager> villagers;

    List<Villager> enemy_villagers;

    List<Factory> factories;

    List<Factory> enemy_factories;

    int num_soldiers;

    int num_enemy_soldiers;

    int num_villagers;

    int num_enemy_villagers;

    int num_factories;

    int num_enemy_factories;

    int score;

    int money;
}
```
