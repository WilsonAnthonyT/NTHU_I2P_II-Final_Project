//
// Cuphead-style Ejojo Boss
// Features multiple phases, telegraphed attacks, and classic Cuphead mechanics
//

#include "EjojoBoss.h"

#include "ArcherSkelly.h"
#include "EnemyBullet/EnemyRedBullet.h"
#include "Engine/Resources.hpp"
#include "Scene/SelectProfileScene.h"

EjojoBoss::EjojoBoss(std::string img, int x, int y) : FlyingEnemy("play/EjojoBoss.png", x, y, 1000, 100.0f, 500, 5, 5, 10),
                                                      rng(std::random_device{}()) {
    // Base setup - position locked to right side
    Size = Engine::Point(PlayScene::BlockSize*3, PlayScene::BlockSize*3);
    Position = Engine::Point(PlayScene::GetClientSize().x - PlayScene::BlockSize*3,
                           PlayScene::GetClientSize().y/2);

    // Cuphead-style boss attributes
    currentPhase = 1;
    maxPhases = 3;
    phaseTransitionHP = {350, 150};
    phaseTransitionDuration = 2.0f;
    phaseTransitionTimer = 0.0f;
    isTransitioning = false;
    phaseChangedDuringTransition = false;

    // Movement - limited vertical movement only
    movementSpeed = PlayScene::BlockSize * 1.5f;
    movementBoundsX = Position.x;
    movementBoundstopY = PlayScene::BlockSize * 2.0f;
    movementBoundsbottomY = PlayScene::MapHeight * PlayScene::BlockSize - 4 * PlayScene::BlockSize;


    // Attack patterns - all firing leftward
    attackPatterns = {
        // Phase 1 patterns
        {
            {"BulletRain", 0.75f, 0.5f},    // Straight left bullets
            {"SpiralShot", 0.75f, 0.1f}     // Spiral to left
        },
        // Phase 2 patterns
        {
            {"WaveAttack", 1.0f, 0.2f},    // Wave pattern left
            {"ChargeBeam", 0.7f, 0.1f},    // Charging laser left
            {"MiniSpawn", 0.8f, 0.1f}      // Minions that attack left
        },
        // Phase 3 patterns
        {
            {"DoubleSpiral", 0.75f, 0.1f}, // Double spiral left
            {"BulletHell", 0.5f, 0.1f},    // Bullet hell left
            {"FinalAssault", 0.25f, 0.1f}    // Combined assault left
        }
    };

    currentAttack = "BulletRain";
    attackCooldown = 0.0f;
    attackDuration = 0.0f;
    bulletSpeed = PlayScene::BlockSize * 3.5f;

    // Visual effects
    telegraphColor = al_map_rgba(255, 50, 50, 150);
    telegraphTimer = 0.0f;
    telegraphDuration = 0.75f;

    SetupPhase(currentPhase);

    //animation
    for (int i = 1; i <= 4; i++) {
        walkAnimation.push_back(Engine::Resources::GetInstance().GetBitmap("animation/EjojoBoss-" + std::to_string(i) + ".png"));
    }
    for (int i = 1; i <= 4; i++) {
        idleAnimation.push_back(Engine::Resources::GetInstance().GetBitmap("animation/EjojoBoss-" + std::to_string(i) + ".png"));
    }
}

void EjojoBoss::SetupPhase(int phase) {
    switch(phase) {
        case 1: {
            // Initial phase - simpler attacks
            fixedAltitude = PlayScene::BlockSize * 8;
            movementSpeed = PlayScene::BlockSize * 1.5f;
            break;
        }
        case 2: {
            // More aggressive phase
            fixedAltitude = PlayScene::BlockSize * 8;
            movementSpeed = PlayScene::BlockSize * 2.0f;
            LightEffect *light2 = new LightEffect(Position.x, Position.y);
            light2->Size.x = Size.x/2;
            light2->Size.y = Size.y/2;
            getPlayScene()->EffectGroup->AddNewObject(light2);
            // Add phase-specific visual effect
            break;
        }
        case 3: {
            // Final desperate phase
            fixedAltitude = PlayScene::BlockSize * 4;
            movementSpeed = PlayScene::BlockSize * 2.5f;
            LightEffect *light3 = new LightEffect(Position.x, Position.y + Size.y/2);
            light3->Size.x = Size.x/2;
            light3->Size.y = Size.y/2;
            getPlayScene()->EffectGroup->AddNewObject(light3);
            // Add rage visual effects
            break;
        }
    }

    //Position.y = PlayScene::GetClientSize().y - fixedAltitude;
}

void EjojoBoss::StartPhaseTransition() {
    isTransitioning = true;
    phaseTransitionTimer = 0.0f;
    currentAttack = "";

    //Create transition effects
    ExplosionEffect* exp = new ExplosionEffect(Position.x + (rand() % 200 - 100), Position.y + (rand() % 100 - 50) );
    exp->Size.x = Size.x/2;
    exp->Size.y = Size.y/2;
    getPlayScene()->EffectGroup->AddNewObject(exp);
}

void EjojoBoss::HandlePhaseTransition(float deltaTime) {
    phaseTransitionTimer += deltaTime;

    // Visual effects during transition
    if (phaseTransitionTimer >= phaseTransitionDuration * 0.5f &&
        currentPhase < maxPhases &&
        !phaseChangedDuringTransition) {

        // Only increment by 1 phase
        currentPhase = std::min(currentPhase + 1, maxPhases);
        SetupPhase(currentPhase);
        phaseChangedDuringTransition = true;
        }

    // End transition
    if (phaseTransitionTimer >= phaseTransitionDuration) {
        isTransitioning = false;
        phaseChangedDuringTransition = false;
        ChooseNextAttack();
    }
}

void EjojoBoss::ChooseNextAttack() {
    if (attackPatterns.size() < currentPhase) {
        printf("Error: No attack patterns for phase %d\n", currentPhase);
        return;
    }

    auto& phasePatterns = attackPatterns[currentPhase-1];
    std::uniform_int_distribution<int> dist(0, phasePatterns.size()-1);
    int index = dist(rng);

    currentAttack = phasePatterns[index].name;
    attackDuration = phasePatterns[index].duration;
    attackCooldown = phasePatterns[index].cooldown;

    printf("Chose attack: %s (Duration: %.1f, Cooldown: %.1f)\n",
           currentAttack.c_str(), attackDuration, attackCooldown);

    telegraphTimer = telegraphDuration;
}

void EjojoBoss::ExecuteAttack(float deltaTime) {
    attackDuration -= deltaTime;

    if (telegraphTimer > 0) {
        telegraphTimer -= deltaTime;
        printf("Telegraphing: %.2f/%.2f\n", telegraphTimer, telegraphDuration);
        if (telegraphTimer <= 0) {
            printf("Performing attack: %s\n", currentAttack.c_str());
            PerformAttack();
        }
        return;
    }

    if (attackDuration > 0) {
        printf("Attack in progress: %s (%.1f remaining)\n",
               currentAttack.c_str(), attackDuration);
        ContinueAttack();
    } else {
        attackCooldown -= deltaTime;
        printf("Attack cooldown: %.1f remaining\n", attackCooldown);
        if (attackCooldown <= 0) {
            currentAttack = "";
            printf("Attack finished, ready for next\n");
        }
    }
}

void EjojoBoss::UpdateMovement(float deltaTime) {
    // Only vertical movement while staying on right side
    if (currentPhase == 1) {
        // Simple up-down movement
        if (Position.y <= movementBoundstopY || Position.y >= movementBoundsbottomY) {
            movementSpeed *= -1;
        }
        Position.y += movementSpeed * deltaTime;
    }
    else if (currentPhase == 2) {
        // Figure-8 pattern (vertical only)
        float t = patternTimer * 0.5f;
        Position.y = (movementBoundstopY + movementBoundsbottomY)/2 +
                    sin(2*t) * PlayScene::BlockSize*2;
    }
    else if (currentPhase == 3) {
        // Chaotic vertical movement
        if (patternTimer >= 1.0f) {
            patternTimer = 0.0f;
            std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
            targetVelocityY = dist(rng) * movementSpeed * 0.5f;
        }

        float lerpAmount = deltaTime * 2.0f;
        lerpAmount = std::clamp(lerpAmount, 0.0f, 1.0f);
        VelocityY = VelocityY + lerpAmount * (targetVelocityY - VelocityY);
        Position.y += VelocityY * deltaTime;

        // Keep within bounds
        Position.y = std::max(movementBoundstopY,
                            std::min(Position.y, movementBoundsbottomY));
    }

    patternTimer += deltaTime;
}

void EjojoBoss::PerformAttack() {
    auto scene = getPlayScene();
    if (!scene) return;

    if (currentAttack == "BulletRain") {
        // Vertical line of bullets firing left
        float spacing = Size.y / 6;
        for (int i = -3; i <= 3; i++) {
            scene->EnemyBulletGroup->AddNewObject(
                new EnemyFireBullet(
                    Engine::Point(Position.x, Position.y + i*spacing),
                    Engine::Point(-1, 0),  // Left direction
                    ALLEGRO_PI,            // Facing left
                    this,
                    bulletSpeed
                )
            );
        }
    }
    else if (currentAttack == "SpiralShot") {
        static float spiralAngle = 0;
        for (int i = 0; i < 8; i++) {
            float angle = spiralAngle + (ALLEGRO_PI/4) * i;
            // Convert to leftward bias
            float dirX = -1 * abs(cos(angle)); // Always negative X
            float dirY = sin(angle);

            scene->EnemyBulletGroup->AddNewObject(
                new EnemyFireBullet(
                    Engine::Point(Position.x, Position.y),
                    Engine::Point(dirX, dirY),
                    atan2(dirY, dirX),
                    this,
                    bulletSpeed * 0.8f
                )
            );
        }
        spiralAngle += ALLEGRO_PI/16;
    }
    else if (currentAttack == "WaveAttack") {
        // Wave pattern firing left with slight vertical variation
        for (int i = -4; i <= 4; i++) {
            float offset = i * 0.2f;
            scene->EnemyBulletGroup->AddNewObject(
                new EnemyFireBullet(
                    Engine::Point(Position.x, Position.y),
                    Engine::Point(-1, offset), // Primary left direction
                    atan2(offset, -1),
                    this,
                    bulletSpeed * 1.2f
                )
            );
        }
    }
    else if (currentAttack == "ChargeBeam") {
        // Powerful leftward beam
        scene->EnemyBulletGroup->AddNewObject(
            new EnemyRedBullet(
                Engine::Point(Position.x, Position.y),
                Engine::Point(-1, 0), // Straight left
                ALLEGRO_PI,
                this,
                bulletSpeed * 3.0f
            )
        );
    }
    else if (currentAttack == "MiniSpawn") {
        // Spawn minions that attack leftward
        if (currentPhase == 2) {
            if (getPlayScene()->TotalArcherSkelly <= 4) {
                auto mini = new ArcherSkelly(
                    Position.x - Size.x/2,
                    Position.y
                );
                getPlayScene()->EnemyGroup->AddNewObject(mini);
            }
        }
    }
    else if (currentAttack == "DoubleSpiral") {
        // Dual spiral firing left
        static float spiralAngle1 = 0;
        static float spiralAngle2 = ALLEGRO_PI;
        for (int i = 0; i < 6; i++) {
            float angle1 = spiralAngle1 + (ALLEGRO_PI/4) * i;
            float angle2 = spiralAngle2 + (ALLEGRO_PI/4) * i;

            // Left-biased directions
            Engine::Point dir1(-0.7f - 0.3f*abs(cos(angle1)), sin(angle1));
            Engine::Point dir2(-0.7f - 0.3f*abs(cos(angle2)), sin(angle2));

            scene->EnemyBulletGroup->AddNewObject(
                new EnemyRedBullet(
                    Engine::Point(Position.x, Position.y),
                    dir1,
                    atan2(dir1.y, dir1.x),
                    this,
                    bulletSpeed * 0.9f
                )
            );
            scene->EnemyBulletGroup->AddNewObject(
                new EnemyRedBullet(
                    Engine::Point(Position.x, Position.y),
                    dir2,
                    atan2(dir2.y, dir2.x),
                    this,
                    bulletSpeed * 0.9f
                )
            );
        }
        spiralAngle1 += ALLEGRO_PI/16;
        spiralAngle2 += ALLEGRO_PI/16;
    }
    else if (currentAttack == "BulletHell") {
        // Concentrated leftward bullet hell
        for (int i = 0; i < 18; i++) {
            float angle = (ALLEGRO_PI/12) * i;
            // Convert to leftward cone
            float baseAngle = ALLEGRO_PI + (ALLEGRO_PI/4); // Left + 45 degree cone
            float variedAngle = baseAngle + (angle - ALLEGRO_PI/2) * 0.7f;

            scene->EnemyBulletGroup->AddNewObject(
                new EnemyRedBullet(
                    Engine::Point(Position.x, Position.y),
                    Engine::Point(cos(variedAngle), sin(variedAngle)),
                    variedAngle,
                    this,
                    bulletSpeed * 1.1f
                )
            );
        }
    }
    else if (currentAttack == "FinalAssault") {
        // Combined leftward assault
        // Bullet rain
        float spacing = Size.y / 8;
        for (int i = -6; i <= 6; i++) {
            scene->EnemyBulletGroup->AddNewObject(
                new EnemyRedBullet(
                    Engine::Point(Position.x, Position.y + i*spacing),
                    Engine::Point(-1, i*0.1f), // Slight fan out
                    atan2(i*0.1f, -1),
                    this,
                    bulletSpeed * 1.3f
                )
            );
        }

        // Tight spiral
        static float spiralAngle = 0;
        for (int i = 0; i < 18; i++) {
            float angle = spiralAngle + (ALLEGRO_PI/6) * i;
            Engine::Point dir(-0.8f - 0.2f*abs(cos(angle)), sin(angle)*0.6f);

            scene->EnemyBulletGroup->AddNewObject(
                new EnemyRedBullet(
                    Engine::Point(Position.x, Position.y),
                    dir,
                    atan2(dir.y, dir.x),
                    this,
                    bulletSpeed * 1.5f
                )
            );
        }
        spiralAngle += ALLEGRO_PI/24;
    }
}

// ... (rest of the implementation remains the same)

void EjojoBoss::ContinueAttack() {
    // For attacks that need continuous execution (like sustained beams)
    if (currentAttack == "ChargeBeam" && fmod(attackDuration, 0.2f) < 0.1f) {
        auto scene = getPlayScene();
        scene->EnemyBulletGroup->AddNewObject(
                new EnemyFireBullet(
                    Engine::Point(Position.x, Position.y),
                    Engine::Point(0, 1),
                    atan2(1, 0),
                    this,
                    bulletSpeed * 1.2f
                )
            );
    }
}

void EjojoBoss::Update(float deltaTime) {
    printf("Update - Phase: %d, HP: %d, Transitioning: %d, CurrentAttack: %s\n",
          currentPhase, hp, isTransitioning,
          currentAttack.empty() ? "None" : currentAttack.c_str());
    if (isFalling) {
        HandleCrashSequence(deltaTime);
        return;
    }

    if (isTransitioning) {
        HandlePhaseTransition(deltaTime);
        return;
    }

    // Check for phase transition
    if (!isTransitioning && currentPhase < maxPhases && hp <= phaseTransitionHP[currentPhase-1]) {
        StartPhaseTransition();
        return;
    }

    // Update attack pattern
    if (currentAttack.empty()) {
        ChooseNextAttack();
    } else {
        ExecuteAttack(deltaTime);
    }

    // Update movement based on phase
    UpdateMovement(deltaTime);

    // Update visual effects
    UpdateVisualEffects(deltaTime);
    UpdateAnimation(deltaTime);

    Sprite::Update(deltaTime);
}


void EjojoBoss::UpdateVisualEffects(float deltaTime) {
    // Phase-specific visual effects
    if (currentPhase == 3) {
        // Rage mode effects
        if (rand() % 100 < 10) {
            // getPlayScene()->EffectGroup->AddNewObject(
            //     new ParticleEffect(
            //         Position.x + (rand() % 100 - 50),
            //         Position.y + (rand() % 100 - 50),
            //         "smoke",
            //         0.5f
            //     )
            // );
            // getPlayScene()->EffectGroup->AddNewObject(
            //     new ExplosionEffect(
            //         Position.x + (rand() % 100 - 50),
            //         Position.y + (rand() % 100 - 50)
            //     )
            // );
        }
    }
}

void EjojoBoss::Draw() const {
    // Draw telegraph if active
    if (telegraphTimer > 0) {
        float progress = 1.0f - (telegraphTimer / telegraphDuration);
        float radius = Size.x * 0.6f * progress;

        al_draw_circle(
            Position.x, Position.y, radius,
            telegraphColor, 4.0f * progress
        );
    }

    // Phase-specific drawing
    ALLEGRO_COLOR tint = al_map_rgba(255, 255, 255, 255);
    if (currentPhase == 2) {
        tint = al_map_rgba(200, 200, 255, 255); // Blue tint
    } else if (currentPhase == 3) {
        tint = al_map_rgba(255, 150, 150, 255); // Red tint
    }

    if (isFalling) {
        // Crash effects
        ALLEGRO_COLOR crashTint = al_map_rgba_f(
            0.7f, 0.5f, 0.5f, 1.0f
        );
        al_draw_tinted_scaled_bitmap(bmp.get(), crashTint, 0, 0,
            al_get_bitmap_width(bmp.get()), al_get_bitmap_height(bmp.get()),
            Position.x - Size.x/2, Position.y - Size.y/2, Size.x, Size.y, 0);
    } else {
        // Normal drawing with phase tint
        al_draw_tinted_scaled_bitmap(bmp.get(), tint, 0, 0,
            al_get_bitmap_width(bmp.get()), al_get_bitmap_height(bmp.get()),
            Position.x - Size.x/2, Position.y - Size.y/2, Size.x, Size.y, 0);
    }

    if (Engine::IScene::DebugMode) {
        al_draw_rectangle(
            movementBoundsX, PlayScene::GetClientSize().y - fixedAltitude - PlayScene::BlockSize*2,
            movementBoundstopY, PlayScene::GetClientSize().y - fixedAltitude + PlayScene::BlockSize*2,
            al_map_rgb(0, 255, 0), 1.0f
        );

        const float healthBarWidth = abs(Size.x*3/4);
        const float healthBarHeight = PlayScene::BlockSize/15;
        const float healthBarOffset = PlayScene::BlockSize/6.4;

        float healthBarX = Position.x - healthBarWidth/2;
        float healthBarY = Position.y - Size.y/2 + healthBarOffset;

        al_draw_filled_rectangle(healthBarX, healthBarY,
                                healthBarX + healthBarWidth, healthBarY + healthBarHeight,
                                al_map_rgb(0, 0, 0));

        float healthRatio = static_cast<float>(hp) / static_cast<float>(MaxHp);
        al_draw_filled_rectangle(healthBarX, healthBarY,
                                healthBarX + healthBarWidth * healthRatio, healthBarY + healthBarHeight,
                                al_map_rgb(255, 0, 0));

        // Hitbox
        float halfSizeX = abs(Size.x / 2);
        float halfSizeY = abs(Size.y / 2);
        float left = Position.x - halfSizeX;
        float right = Position.x + halfSizeX;
        float top = Position.y - halfSizeY;
        float bottom = Position.y + halfSizeY;
        al_draw_rectangle(left, top, right, bottom, al_map_rgb(255, 255, 0), 3);
    }

}

void EjojoBoss::OnDeath() {
    if (!isFalling) {
        isFalling = true;
        fallVelocity = 0.0f;

        // Disable all normal behaviors
        currentAttack = "";

        // Initial explosion
        ExplosionEffect *exp = new ExplosionEffect(Position.x, Position.y);
        exp->Size = Engine::Point(Size.y, Size.y);
        getPlayScene()->EffectGroup->AddNewObject(exp);

        // Start crash sequence
        crashShakeTimer = crashDelay;

        // Screen shake effect
        // getPlayScene()->ShakeScreen(10.0f, 2.0f);
    }
}

void EjojoBoss::HandleCrashSequence(float deltaTime) {
    if (isFalling) {
        bmp = Engine::Resources::GetInstance().GetBitmap("play/EjojoBoss-death.png");
        crashProgress = crashShakeTimer > 0 ? 1.0f :
                            std::max(0.5f, 1.0f - ((Position.y) / (getPlayScene()->MapHeight * PlayScene::BlockSize)));

        fallVelocity += PlayScene::Gravity * deltaTime;
        Position.y += fallVelocity * deltaTime;

        smokeTimer -= deltaTime;
        if (smokeTimer <= 0) {
            smokeTimer = 0.05f + (rand() % 100) * 0.001f;
        }

        explosionTimer -= deltaTime;
        if (explosionTimer <= 0) {
            getPlayScene()->EffectGroup->AddNewObject(
                new ExplosionEffect(
                    Position.x + (rand() % 100 - 50),
                    Position.y + (rand() % 100))
            );
            explosionTimer = 0.15f + (rand() % 100) * 0.001f;
        }

        float groundY = getPlayScene()->MapHeight * PlayScene::BlockSize - Size.y * 1.25f;
        if (Position.y >= groundY) {
            Position.y = groundY;

            // Final crash effects
            if (crashShakeTimer == 0) { // Only trigger once
                crashShakeTimer = crashDelay;

                ExplosionEffect* exp = new ExplosionEffect(Position.x, groundY);
                exp->Size = Engine::Point(Size.y/2, Size.y/2);

                // Big explosion
                getPlayScene()->EffectGroup->AddNewObject(exp);

                // Screen shake


                // Ground debris
                for (int i = 0; i < 15; i++) {
                    getPlayScene()->GroundEffectGroup->AddNewObject(
                        new DirtyEffect("play/dirty-1.png",
                                       0.5f + (rand() % 100)*0.01f,
                                       Position.x + (rand() % 200 - 100),
                                       groundY - Size.y * 1.25f)
                    );
                }

                // Fire effect
                for (int i = 0; i < 5; i++) {
                    getPlayScene()->EffectGroup->AddNewObject(
                        new LightEffect(Position.x + (rand() % 100 - 50),
                                       groundY - 20)
                    );
                }
            }

            // Countdown to removal
            crashShakeTimer -= deltaTime;
            if (crashShakeTimer <= 0) {
                getPlayScene()->MapId++;
                getPlayScene()->EnemyGroup->RemoveObject(GetObjectIterator());
                Engine::GameEngine::GetInstance().ChangeScene("story");

                PlayScene::total_money += this->money;

                //this 3 lines is for updating the profile.
                auto scene = getPlayScene();
                if (SelectProfileScene::isSaved && !SelectProfileScene::playerData[SelectProfileScene::getProfileID()-1].isWin) {
                    auto* newdata = new SelectProfileScene::textData();
                    newdata->level = scene->MapId;
                    newdata->coin_counts = PlayScene::total_money;
                    SelectProfileScene::WriteProfileData(newdata);
                }
                //----------------------------------------

                if (!SelectProfileScene::playerData[SelectProfileScene::getProfileID()-1].isWin) Engine::GameEngine::GetInstance().ChangeScene("story");
                else Engine::GameEngine::GetInstance().ChangeScene("stage-select");

                return;
            }
        }
    }
    // ... (keep your existing crash sequence code)
    // Add Cuphead-style victory effects when boss is finally defeated
    if (crashShakeTimer <= 0) {
        // Victory fanfare
        // getPlayScene()->PlaySample("victory.wav");

        // Soul contract effect (Cuphead-style)
        // getPlayScene()->EffectGroup->AddNewObject(
        //     new ContractEffect(Position.x, Position.y - 100)
        // );

        // Transition to next level after delay
    }
}

void EjojoBoss::UpdateAnimation(float deltaTime) {
    if (hp > 0) {
        animationTime += deltaTime;

        std::vector<std::shared_ptr<ALLEGRO_BITMAP>>* currentAnimation = nullptr;
        float* currentFrameDuration = nullptr;
        bool looping = true;

        if (std::abs(VelocityY) > 0.01f) {
            currentState = WALKING;
            currentAnimation = &walkAnimation;
            currentFrameDuration = &walkFrameDuration;
        }
        else {
            currentState = IDLE;
            currentAnimation = &idleAnimation;
            currentFrameDuration = &idleFrameDuration;
        }

        // Update animation frames
        if (currentAnimation && !currentAnimation->empty()) {
            if (animationTime >= *currentFrameDuration) {
                animationTime = 0;

                if (looping) {
                    currentFrame = (currentFrame + 1) % currentAnimation->size();
                }
                else if (currentFrame < currentAnimation->size() - 1) {
                    currentFrame++;
                }

                // Update bitmap
                // if (currentFrame >= 0 && currentFrame < currentAnimation->size()) {
                //     bmp = (*currentAnimation)[currentFrame];
                // } else {
                //     currentFrame = 0;  // Reset to safe value
                //     if (!currentAnimation->empty()) {
                //         bmp = (*currentAnimation)[0];
                //     }
                // }
                bmp = (*currentAnimation)[currentFrame];
                // Handle direction

            }
        }
        // Handle attack completion
        // Handle death completion
    }
    else {

    }

}
