#pragma once

#include "typedefs.hpp"
#include "logging.hpp"

class Entity
{
public:
    bool enabled = true;

    inline void Create() {}

    virtual void Update() = 0;
};