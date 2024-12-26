#ifndef MAIN_CHARACTER_H
#define MAIN_CHARACTER_H
#include <core.h>

extern Animation idleMcAnimation;
extern Animation movingMcAnimation;

extern Entity mainCharacter;

void updateAnimation(Animation& animation, float deltaTime);

#endif // MAIN_CHARACTER_H