// Extras.h
#pragma once

#include <vector>
#include <box2d/box2d.h>
#include "BodyFactory.h"

// Entity and shape types
enum EntityType { SOURCE, TARGET, OBSTACLE };
enum ShapeType  { SHAPE_CIRCLE, SHAPE_BOX, SHAPE_POLYGON };

// Register entity type for collision lookup
void Extras_RegisterEntity(int idx, EntityType type);

// Creation APIs: dynamic vs static, for circle/box shapes
int Extras_CreateDynamicSource(ShapeType shape,
                               float x, float y,
                               float a, float b,
                               float density,
                               float friction,
                               float restitution);
int Extras_CreateStaticSource(ShapeType shape,
                              float x, float y,
                              float a, float b,
                              float density,
                              float friction,
                              float restitution);

int Extras_CreateDynamicTarget(ShapeType shape,
                               float x, float y,
                               float a, float b,
                               float density,
                               float friction,
                               float restitution);
int Extras_CreateStaticTarget(ShapeType shape,
                              float x, float y,
                              float a, float b,
                              float density,
                              float friction,
                              float restitution);

int Extras_CreateDynamicObstacle(ShapeType shape,
                                 float x, float y,
                                 float a, float b,
                                 float density,
                                 float friction,
                                 float restitution);
int Extras_CreateStaticObstacle(ShapeType shape,
                                float x, float y,
                                float a, float b,
                                float density,
                                float friction,
                                float restitution);

// Polygon-specific creation APIs
int Extras_CreateDynamicSourcePolygon(const std::vector<b2Vec2>& pts,
                                      float x, float y,
                                      float density,
                                      float friction,
                                      float restitution);
int Extras_CreateStaticSourcePolygon(const std::vector<b2Vec2>& pts,
                                     float x, float y,
                                     float density,
                                     float friction,
                                     float restitution);
int Extras_CreateDynamicTargetPolygon(const std::vector<b2Vec2>& pts,
                                      float x, float y,
                                      float density,
                                      float friction,
                                      float restitution);
int Extras_CreateStaticTargetPolygon(const std::vector<b2Vec2>& pts,
                                     float x, float y,
                                     float density,
                                     float friction,
                                     float restitution);
int Extras_CreateDynamicObstaclePolygon(const std::vector<b2Vec2>& pts,
                                        float x, float y,
                                        float density,
                                        float friction,
                                        float restitution);
int Extras_CreateStaticObstaclePolygon(const std::vector<b2Vec2>& pts,
                                       float x, float y,
                                       float density,
                                       float friction,
                                       float restitution);

// Process collisions and destroy targets
void Extras_ProcessCollisions();