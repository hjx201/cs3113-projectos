#include "Game.h"

#define HEIGHT 8 //8
#define WIDTH 12 //12
#define LINEWIDTH 0.1

#define XVIEWBOUND 2
#define YVIEWBOUND 2

#define MAX_ENEMY_COUNT 45
#define MAX_BULLET_COUNT 30


#define BULLET_COOLDOWN .2
#define BULLET_SPEED 7

#define MIN_ASTEROID_SPEED 1
#define MAX_ASTEROID_SPEED 1.5

#define NUM_WAVES 5

#define INVINCIBLE_TIME 1;

#include <string>

void Game::initWalls() {
    state.walls = new Entity[4];

    //GLuint lineText = Util::LoadTexture("line.png");

    for (int i = 0; i < 4; i++) {
        state.walls[i].entityType = WALL;
        state.walls[i].isActive = true;
        //state.walls[i].textureID = lineText;
    }
    state.walls[0].height = HEIGHT + LINEWIDTH;
    state.walls[0].width = LINEWIDTH;
    state.walls[1].height = HEIGHT + LINEWIDTH;
    state.walls[1].width = LINEWIDTH;
    state.walls[2].width = WIDTH + LINEWIDTH;
    state.walls[2].height = LINEWIDTH;
    state.walls[3].width = WIDTH + LINEWIDTH;
    state.walls[3].height = LINEWIDTH;

    state.walls[0].position = glm::vec3(WIDTH / 2, 0.0f, 0.0f);
    state.walls[1].position = glm::vec3(-1* WIDTH / 2, 0.0f, 0.0f);
    state.walls[2].position = glm::vec3(0.0f, HEIGHT / 2, 0.0f);
    state.walls[3].position = glm::vec3(0.0f , -1* HEIGHT / 2, 0.0f);

    for (int i = 0; i < 4; i++) {
        state.walls[i].Update(0, NULL, NULL, 0, 0, 0, 0, 0);
    }

    //ViewAnchors = new glm::vec3[4];
    //ViewAnchors[0] = glm::vec3(WIDTH / 2 - XVIEWBOUND, HEIGHT / 2 - YVIEWBOUND, 0);
    //ViewAnchors[1] = glm::vec3(WIDTH / 2 - XVIEWBOUND, -HEIGHT / 2 + YVIEWBOUND, 0);
    //ViewAnchors[2] = glm::vec3(-WIDTH / 2 + XVIEWBOUND, -HEIGHT / 2 + YVIEWBOUND, 0);
    //ViewAnchors[3] = glm::vec3(-WIDTH / 2 + XVIEWBOUND, HEIGHT / 2 - YVIEWBOUND, 0);
}

void Game::initPlayer() {
    // Initialize Player
    GLuint playertext = Util::LoadTexture("gunthing.png");

    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->isActive = true;
    
    state.player->textureID = playertext;
    state.player->position = glm::vec3(0.0f, 0.0f, 0.0f);

    state.player->height = 0.7f;
    state.player->width = 0.7f;

    state.player->rotation = 0.0f;

    state.player->inFrame = true;
}

void Game::initAsteroids() {
    state.enemies = new Entity*[MAX_ENEMY_COUNT];
    GLuint enemytext = Util::LoadTexture("burger.png");

    for (int i = 0; i < MAX_ENEMY_COUNT; i++) {
        state.enemies[i] = new Entity();
        state.enemies[i]->isActive = false;
        state.enemies[i]->entityType = ENEMY;
        state.enemies[i]->textureID = enemytext;
        //state.enemies[i].position = ViewAnchors[i];
        //state.enemies[i].textureID = enemytext;
    }

    //state.enemies[0].position = glm::vec3(WIDTH / 2, HEIGHT / 2, 0);
    //state.enemies[1].position = glm::vec3(-WIDTH / 2, HEIGHT / 2, 0);
    //state.enemies[2].position = glm::vec3(WIDTH / 2, -HEIGHT / 2, 0);
    //state.enemies[3].position = glm::vec3(-WIDTH / 2, -HEIGHT / 2, 0);
   
    //initialize all entities
    //set all isactives to false
    activeAsteroids = 0;
}

void Game::initBullets() {
    state.bullets = new Entity * [MAX_BULLET_COUNT];
    GLuint bullettext = Util::LoadTexture("bullet.png");
    for (int i = 0; i < MAX_BULLET_COUNT; i++) {
        state.bullets[i] = new Entity();
        state.bullets[i]->entityType = BULLET;
        state.bullets[i]->isActive = false;
        state.bullets[i]->textureID = bullettext;
        state.bullets[i]->height = 0.2;
        state.bullets[i]->width = 0.2;
        state.bullets[i]->speed = BULLET_SPEED;
        state.bullets[i]->inFrame = true;
        //state.bullets[i]->velocity = glm::vec3(1.0f, 0.0f, 0.0f);
    }
    activeBullets = 0;
    bulletCooldown = 0;
}

void Game::initWaves() {
    currentWave = 0;
    waves = new Wave[NUM_WAVES];

    for (int i = 0; i < NUM_WAVES; i++) {
        waves[i].cooldown = 0;
        waves[i].enemiesRemaining = 0;
        waves[i].maxencount = 0;
        waves[i].min_asteroid_speed = 0;
        waves[i].max_asteroid_speed = 0;
    } //idk

    waves[0].cooldown = 3;
    waves[0].enemiesRemaining = 15;
    waves[0].maxencount = 15;
    waves[0].min_asteroid_speed = 1;
    waves[0].max_asteroid_speed = 1.5;

    waves[1].cooldown = 1.5;
    waves[1].enemiesRemaining = 30;
    waves[1].maxencount = 20;
    waves[1].min_asteroid_speed = 2;
    waves[1].max_asteroid_speed = 3;

    waves[2].cooldown = 0.9;
    waves[2].enemiesRemaining = 35;
    waves[2].maxencount = 25;
    waves[2].min_asteroid_speed = 2;
    waves[2].max_asteroid_speed = 3;

    waves[3].cooldown = 0.5;
    waves[3].enemiesRemaining = 50;
    waves[3].maxencount = 30;
    waves[3].min_asteroid_speed = 3;
    waves[3].max_asteroid_speed = 5;

    waves[4].cooldown = 0.5;
    waves[4].enemiesRemaining = 100;
    waves[4].maxencount = 45;
    waves[4].min_asteroid_speed = 4;
    waves[4].max_asteroid_speed = 6;
}

void Game::Initialize() {
    state.nextScene = -1;

    lives = 5;

    initWalls();
    initPlayer();
    initAsteroids();
    initBullets();

    initWaves();

    currentWave = 0;
    enemiesRemaining = waves[currentWave].enemiesRemaining;

}

void Game::ProcessInput(SDL_Event event, float cursorx, float cursory) {
    state.player->acceleration = glm::vec3(0);
    isFiring = false;

    switch (event.type) {
    case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
        case SDLK_LEFT:
            // Move the player left
            break;

        case SDLK_RIGHT:
            // Move the player right
            break;

        case SDLK_SPACE:
            // Some sort of action
            break;
        }
        break; // SDL_KEYDOWN
    }

    const Uint8* keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_A]) {
        state.player->acceleration.x = -2.0f;
    }
    else if (keys[SDL_SCANCODE_D]) {
        state.player->acceleration.x = 2.0f;
    }
    if (keys[SDL_SCANCODE_W]) {
        state.player->acceleration.y = 2.0f;
    }    
    else if (keys[SDL_SCANCODE_S]) {
        state.player->acceleration.y = -2.0f;
    }

    if (keys[SDL_SCANCODE_SPACE]) {
        isFiring = true;
    }

    //determine angle of fire here
    cursor_x = cursorx + -viewx;
    cursor_y = cursory + -viewy;

    if (glm::length(state.player->acceleration) > 1.0f) {
        state.player->acceleration = glm::normalize(state.player->acceleration);
    }
}

void Game::fire(float cursor_x, float cursor_y) {

    state.bullets[activeBullets]->isActive = true;
    state.bullets[activeBullets]->position = state.player->position;
    //state.bullets[activeBullets]->rotation = state.player->rotation;
    state.bullets[activeBullets]->velocity = glm::normalize(glm::vec3(cursor_x, cursor_y, 0.0f) - state.player->position);
    activeBullets += 1;
}

void Game::spawnAsteroid(){
    if (activeAsteroids == waves[currentWave].maxencount) return;
    //also check if max quota has been reached

    state.enemies[activeAsteroids]->inFrame = false; //set inframe to false; all asteroids start outside the frame

    int rando = std::rand() % 4;    //pick which side of map it spawns on
    float randodec = float(std::rand()) / RAND_MAX;

    switch (rando) { //set random position
    case 0: //spawn on left wall
        state.enemies[activeAsteroids]->position.x = -WIDTH / 2 - 2; //x is fixed
        state.enemies[activeAsteroids]->position.y = randodec * (HEIGHT + 3) - (HEIGHT + 3) / 2.0f; //y spawns on rando 
        break;
    case 1: //spawn on right wall
        state.enemies[activeAsteroids]->position.x = WIDTH / 2 + 2;
        state.enemies[activeAsteroids]->position.y = randodec * (HEIGHT + 3) - (HEIGHT + 3) / 2.0f;
        break;
    case 2: //spawn on bottom wall
        state.enemies[activeAsteroids]->position.y = -HEIGHT / 2 - 2;
        state.enemies[activeAsteroids]->position.x = randodec * (WIDTH + 3) - (WIDTH + 3) / 2.0f;
        break;
    case 3: //spawn on top wall
        state.enemies[activeAsteroids]->position.y = HEIGHT / 2 + 2;
        state.enemies[activeAsteroids]->position.x = randodec * (WIDTH + 3) - (WIDTH + 3) / 2.0f;
        break;
    }

    state.enemies[activeAsteroids]->isActive = true;
    //pick random position outside the view matrix boundaries

    //select random target within the frame (maybe trim some off the edge so that nothing ever goes too far out the edge)
    glm::vec3 target = glm::vec3(std::rand() % (WIDTH - XVIEWBOUND) - ((WIDTH - XVIEWBOUND)/2.0f), std::rand() % (HEIGHT - YVIEWBOUND) - ((HEIGHT - YVIEWBOUND) / 2.0f), 0.0f);

    state.enemies[activeAsteroids]->speed = waves[currentWave].max_asteroid_speed * float(std::rand()) / RAND_MAX + waves[currentWave].min_asteroid_speed; //set speed to random
    
    state.enemies[activeAsteroids]->spin = float(std::rand()) / RAND_MAX * 3; //set spin to random float
    
    state.enemies[activeAsteroids]->rotation = 0;//std::rand() % 360; //set initial rotation to random
    
    state.enemies[activeAsteroids]->velocity = glm::normalize(target - state.enemies[activeAsteroids]->position);     //set velocity using target

    activeAsteroids++;    //add 1 to activeEnemies
}

int Game::rearrange(Entity** objects, int activeobjCount) {
    
    if (activeobjCount == 0) return 0;

    Entity* temp = NULL;
    int deadenemies = 0;


    while (!objects[activeobjCount - 1]->isActive) {
        activeobjCount--;
        deadenemies++;
        if (activeobjCount == 0) { 
            enemiesRemaining -= deadenemies;
            return 0;
        }
    } //trim array so last element is active

    for (int i = 0; i < activeobjCount; i++) {
        if (!objects[i]->isActive) {
            temp = objects[i];
            objects[i] = objects[activeobjCount - 1];//swap with last active object in the array
            objects[activeobjCount - 1] = temp;
            activeobjCount--;//decrement from active objects
            deadenemies++;
        }
    }

    if (objects[0]->entityType == ENEMY) enemiesRemaining -= deadenemies;

    return activeobjCount;
}

glm::mat4 Game::Update(float deltaTime, glm::mat4 viewMatrix) {
    state.player->Update(deltaTime, state.player, state.enemies, MAX_ENEMY_COUNT, HEIGHT, WIDTH, cursor_x, cursor_y);

    if (asterCooldown <= 0 && activeAsteroids < enemiesRemaining) {
        spawnAsteroid();
        asterCooldown = waves[currentWave].cooldown;
    }
    asterCooldown -= deltaTime;

    for (int i = 0; i < activeAsteroids; i++) {
        state.enemies[i]->Update(deltaTime, state.player, state.enemies, MAX_ENEMY_COUNT, HEIGHT, WIDTH, cursor_x, cursor_y);
    }

    //update motion of all bullets as well
    if (isFiring) {
        if (bulletCooldown <= 0) {
            fire(cursor_x, cursor_y);
            bulletCooldown = BULLET_COOLDOWN;
        }
    }
    bulletCooldown -= deltaTime;

    for (int i = 0; i < activeBullets; i++) {
        state.bullets[i]->Update(deltaTime, state.player, state.enemies, MAX_ENEMY_COUNT, HEIGHT, WIDTH, cursor_x, cursor_y);
    }

    activeBullets = rearrange(state.bullets, activeBullets);
    activeAsteroids = rearrange(state.enemies, activeAsteroids);
    //viewMatrix = glm::mat4(1.0f);

    if (state.player->isDead) {
        lives -= 1;
        state.player->isDead = false;

        state.player->isInvincible = true;
        invulnerabletime = INVINCIBLE_TIME;
    }
    invulnerabletime -= deltaTime;
    
    if (invulnerabletime <= 0) {
        state.player->isInvincible = false;
    }

    if (lives <= 0) {
        state.nextScene = 3;//go to lose scene
    }

    if (enemiesRemaining <= 0) {
        currentWave++;
        if (currentWave >= NUM_WAVES) {
            state.nextScene = 2; //switch to next scene
        }
        else {
            enemiesRemaining = waves[currentWave].enemiesRemaining;
        }
    }

    viewMatrix = UpdateViewMatrix(viewMatrix);

    return viewMatrix;
}

glm::mat4 Game::UpdateViewMatrix(glm::mat4 viewMatrix) {    
    viewMatrix = glm::mat4(1.0f);

    float maxx = WIDTH / 2 - XVIEWBOUND;
    float maxy = HEIGHT / 2 - YVIEWBOUND;
    //update the viewmatrix
    //check player's postition, keep player in center so long it isnt close to the edges

    // check x
    if (state.player->position.x > maxx) {
        viewx = -maxx;            
    }
    else if (state.player->position.x < -maxx) {
        viewx = maxx;
    }
    else {
        viewx = -state.player->position.x;
    }
    
    if (state.player->position.y > maxy) {
        viewy = -maxy;
    }
    else if (state.player->position.y < -maxy) {
        viewy = maxy;
    }
    else {
        viewy = -state.player->position.y;
    }

    viewMatrix = glm::translate(viewMatrix, glm::vec3(viewx, viewy, 0));

    return viewMatrix;
}

void Game::Render(ShaderProgram* program) {
    //draw borders of edge
    for (int i = 0; i < 4; i++) {
        state.walls[i].Render(program);
    }

    state.player->Render(program);

    for (int i = 0; i < MAX_ENEMY_COUNT; i++) {
        state.enemies[i]->Render(program);
    }

    for (int i = 0; i < activeBullets; i++) {
        state.bullets[i]->Render(program);
    }



}

void Game::RenderUI(ShaderProgram* program) {
    Util::DrawText(program, fontTextureID, "Lives: " + std::to_string(lives), 0.5f, -0.25f, glm::vec3(-4.75f, 3.5f, 0));
    Util::DrawText(program, fontTextureID, "Wave " + std::to_string(currentWave + 1) + " of " + std::to_string(NUM_WAVES), 0.5f, -0.25f, glm::vec3(-4.75f, -3.5f, 0));
    Util::DrawText(program, fontTextureID, std::to_string(enemiesRemaining) + " burgers remain", 0.5f, -0.25f, glm::vec3(0.9f, 3.5f, 0));
    //if (activeBullets >= 1) {
        //Util::DrawText(program, fontTextureID, "Bullet velocity: " + std::to_string(state.bullets[activeBullets - 1]->velocity.x) + " " + std::to_string(state.bullets[activeBullets - 1]->velocity.y), 0.5f, -0.25f, glm::vec3(-2.75f, -1, 0));
    //}
}