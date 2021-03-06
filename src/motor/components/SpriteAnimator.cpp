#include "SpriteAnimator.hpp"
#include "motor/Entity.hpp"
#include "motor/Gfx.hpp"
#include "motor/Time.hpp"
#include "motor/logging.hpp"

void SpriteAnimator::Update()
{
    if (currentSprite)
    {
        timer += Time::Delta();
        while (timer > currentSprite->duration)
        {
            timer -= currentSprite->duration;
            Gfx::Sprite *previousSprite = currentSprite;
            currentSprite = currentSprite->nextFrame;
            if (!currentSprite)
            {
                break;
            }

            frame = currentSprite->frameNumber;
            if (frame == 0)
            {
                if (queuedDestroy)
                {
                    queuedDestroy->Destroy();
                    currentSprite = previousSprite;
                }
                else if (queuedSprite)
                {
                    currentSprite = queuedSprite;
                    frame = queuedSprite->frameNumber;
                    timer = 0;
                }
            }
        }
    }
}

Gfx::Sprite *SpriteAnimator::GetSprite()
{
    return currentSprite;
}

void SpriteAnimator::Play(const char *sprite)
{
    queuedSprite = nullptr;
    currentSprite = Gfx::GetSpriteData(sprite);
    if (currentSprite)
    {
        frame = currentSprite->frameNumber;
    }
    timer = 0;
}

void SpriteAnimator::PlayOnceThen(const char *first, const char *then)
{
    Play(first);
    queuedSprite = Gfx::GetSpriteData(then);
}

void SpriteAnimator::PlayOnceThenDestroy(const char *animation, Entity *entityToDestroy)
{
    Play(animation);
    queuedDestroy = entityToDestroy;
}
