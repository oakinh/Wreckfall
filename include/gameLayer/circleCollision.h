#pragma once

bool isCircleColliding(const glm::vec2& pos1, float r1, const glm::vec2& pos2, float r2, float spriteScale);
void resolveCircleCollision(glm::vec2& pos1, float r1, glm::vec2& pos2, float r2, float spriteScale);