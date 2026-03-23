# Object Pool Plugin

A lightweight, high-performance, fully replicated object pooling system for Unreal Engine 5.

Reuse actors instead of spawning and destroying them constantly. Ideal for projectiles, effects, pickups, and enemies.

## Features

* `UObjectPoolComponent` for easy pool management
* `APooledObject` base class with activation/deactivation events
* Configurable pool size and auto-despawn lifespan
* Server-authoritative spawning (RPC supported)
* Full multiplayer replication
* Blueprint Native Events for simple setup
* Improves performance by reducing spawn/destroy overhead
* No dependencies

## Installation

1) Create folder `Plugins/ObjectPoolPlugin` in your project
2) Copy the plugin files from this repo into it
3) Enable the plugin in the Editor
4) Restart Editor

## Quick Start

1) Add **Object Pool Component** to a manager actor (GameMode, Level Blueprint, or dedicated spawner recommended)
2) In the component details, set:
   - **Pooled Object Class** (any class inheriting from APooledObject)
   - **Pool Size**
3) Call `SpawnPooledObject(FTransform SpawnTransform)` to get an actor from the pool
4) When finished with the actor, call `Deactivate()` on it (or set a lifespan)

## Actor Setup

Create your actors by inheriting from **PooledObject** instead of Actor, then override:
* `PoolObjectActivate` → Show meshes, enable collision, start effects
* `PoolObjectDeactivate` → Hide meshes, disable collision, stop effects

## About

A clean and efficient object pooling solution designed for high-performance Unreal Engine games.
