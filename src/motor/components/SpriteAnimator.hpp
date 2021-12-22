#include "motor/typedefs.hpp"
#include "motor/Gfx.hpp"

class SpriteAnimator
{
public:
    void Update();
    Gfx::Sprite *GetSprite();
    void Play(const char *sprite);
    void PlayOnceThen(const char *first, const char *then);

private:
    int32 frame = 0;
    float timer = 0;
    Gfx::Sprite *queuedSprite = nullptr;
    Gfx::Sprite *currentSprite = nullptr;
};