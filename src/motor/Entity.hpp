#pragma once

#include "typedefs.hpp"

class Entity
{
public:
    bool enabled = true;

    virtual void Update() = 0;
};