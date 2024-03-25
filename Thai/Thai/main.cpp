#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <vector>
#include <ctime>
#define f first
#define s second
using namespace std;


clock_t lastShootTime = clock();

SDL_Renderer* renderer = NULL;
SDL_Window* window = NULL;
SDL_Surface* backgroundSurface = NULL;
SDL_Texture* backgroundTexture = NULL;
SDL_Surface* characterSurface = NULL;
SDL_Texture* characterTexture = NULL;
SDL_Texture* bullet = NULL; 
SDL_Texture* threatObject = NULL;

SDL_Texture* explosionTexture = NULL;
SDL_Texture* bulletTexture = NULL; 

SDL_Rect characterRect = { 0, 0, 100, 150 }; 
bool quit = false;

vector<pair<int, int>> toadodan;


struct ThreatObject {
    int x, y;
    
    
    int speed;
    char direction;
    clock_t lastShootTime;
    ThreatObject(int _x, int _y, int _speed, char _direction) : x(_x), y(_y), speed(_speed), direction(_direction), lastShootTime(clock()) {}
};



struct Bullet {
    int x, y; 
    int speed; 
    SDL_Texture* texture; 

    Bullet(int _x, int _y, int _speed, SDL_Texture* _texture) : x(_x), y(_y), speed(_speed), texture(_texture) {}
};
vector<Bullet> threatBullets; 

struct Explosion {
    SDL_Rect rect;
    int frames; 
};

vector<Explosion> explosions; 

vector<ThreatObject> threatObjects; 

void inbackground();
void innhanvat(int x, int y);
void indan();
void inThreatObject();
void moveThreatObjects();
bool checkCollision(SDL_Rect a, SDL_Rect);
void shootThreatObjects();


int main(int argc, char* args[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return -1;
    }

    window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 680, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Surface* imagebullet = IMG_Load("bullet.png"); 
    backgroundSurface = IMG_Load("background.jpg");
    SDL_Surface* threatObjectSurface = IMG_Load("threatObject.png");
    bullet = SDL_CreateTextureFromSurface(renderer, imagebullet); 
    threatObject = SDL_CreateTextureFromSurface(renderer, threatObjectSurface);
    
    
    
    SDL_Surface* explosionSurface = IMG_Load("heart.png");
    explosionTexture = SDL_CreateTextureFromSurface(renderer, explosionSurface);
    SDL_FreeSurface(explosionSurface);


    if (!backgroundSurface) {
        
    }
    backgroundTexture = SDL_CreateTextureFromSurface(renderer, backgroundSurface);
    SDL_FreeSurface(backgroundSurface);

    characterSurface = IMG_Load("spaceship.png");
    if (!characterSurface) {
        
    }
    SDL_SetColorKey(characterSurface, SDL_TRUE, SDL_MapRGB(characterSurface->format, 255, 255, 255));
    characterTexture = SDL_CreateTextureFromSurface(renderer, characterSurface);
    SDL_FreeSurface(characterSurface);

    SDL_Event e;
    int mouseX = 0, mouseY = 0; 

   

    threatObjects.push_back(ThreatObject{ 100, 200, 1, 'R' });
    threatObjects.push_back(ThreatObject{ 300, 400, 1, 'L' }); 
    threatObjects.push_back(ThreatObject{ 200, 300, 1, 'R' });

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEMOTION) {
               
                mouseX = e.button.x;
                mouseY = e.button.y;
                if (e.type == SDL_MOUSEBUTTONDOWN)
                {
                    toadodan.push_back({mouseX-25,mouseY-50});
                }
                innhanvat(mouseX, mouseY);
            }
        }
        
       
        inbackground(); 
        innhanvat(mouseX, mouseY); 
        inThreatObject();
        moveThreatObjects();
        indan();
        shootThreatObjects();
        
        
        SDL_RenderPresent(renderer); 
        SDL_RenderClear(renderer);
        SDL_Delay(10);
        
    }

    

    SDL_Quit();
    return 0;
}

void inbackground() {
    SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
    for (int i = 0; i < explosions.size(); ++i) {
        SDL_RenderCopy(renderer, explosionTexture, NULL, &explosions[i].rect);
    }

}

void innhanvat(int x, int y) {
    characterRect.x = x - (characterRect.w / 2);
    characterRect.y = y - (characterRect.h / 2);
    SDL_RenderCopy(renderer, characterTexture, NULL, &characterRect);
}
void indan() {
    SDL_Rect bulletRect;
    bulletRect.w = 50; 
    bulletRect.h = 50; 

    for (int i = 0; i < toadodan.size(); ++i) {
        bulletRect.x = toadodan[i].f;
        bulletRect.y = toadodan[i].s;

        SDL_RenderCopy(renderer, bullet, NULL, &bulletRect);
        toadodan[i].s -= 4; 

        
        for (int j = 0; j < threatObjects.size(); ++j) {
            SDL_Rect threatRect = { threatObjects[j].x, threatObjects[j].y, 50, 50 };
            if (checkCollision(bulletRect, threatRect)) {
                
                toadodan.erase(toadodan.begin() + i);
                threatObjects.erase(threatObjects.begin() + j);
                --i; 
                break; 

                
            }
        }
    }
    
    for (int j = 0; j < threatObjects.size(); ++j) {
        SDL_Rect threatRect = { threatObjects[j].x, threatObjects[j].y, 50, 50 };
        for (int i = 0; i < toadodan.size(); ++i) {
            SDL_Rect bulletRect = { toadodan[i].f, toadodan[i].s, 50, 50 };
            if (checkCollision(bulletRect, threatRect)) {
               
                Explosion explosion;
                explosion.rect.x = threatRect.x; 
                explosion.rect.y = threatRect.y;
                explosion.rect.w = threatRect.w; 
                explosion.rect.h = threatRect.h;
                explosion.frames = 10; 
                explosions.push_back(explosion);
                
            }
        }
    }
}
void inThreatObject()
{
    for (auto& obj : threatObjects) 
    {
        SDL_Rect toado = { obj.x, obj.y, 60, 60 }; 
        SDL_RenderCopy(renderer, threatObject, NULL, &toado); 
    }
}

void moveThreatObjects() {
    for (auto& obj : threatObjects) {
        obj.y += obj.speed; 

        
        if (obj.y > 680) {
            obj.y = -50;
            obj.x = rand() % (1280 - 50); 
        }
        while (threatObjects.size() < 5) {
            int x = rand() % (1280 - 50); 
            int y = -50; 
            int speed = 1; 
            char direction = 'D'; 
            threatObjects.push_back(ThreatObject{ x, y, speed, direction });
        }
    }
    
    for (int i = 0; i < explosions.size(); ++i) {
        explosions[i].frames--;
        if (explosions[i].frames <= 0) {
            explosions.erase(explosions.begin() + i); 
            i--; 
        }
    }
}

bool checkCollision(SDL_Rect a, SDL_Rect b) {
    if (a.x + a.w <= b.x) return false;
    if (a.x >= b.x + b.w) return false;
    if (a.y + a.h <= b.y) return false;
    if (a.y >= b.y + b.h) return false;
    return true;
}

void shootThreatObjects() {
    bool bulletShot = false; 
    for (auto& obj : threatObjects) {
        double timeSinceLastShoot = (double)(clock() - obj.lastShootTime) / CLOCKS_PER_SEC;
        if (timeSinceLastShoot >= 3.0 && !bulletShot) { 
            
            int bulletX = obj.x + 25;
            int bulletY = obj.y + 25;
            threatBullets.push_back(Bullet{ bulletX, bulletY, 2, bullet }); 
            obj.lastShootTime = clock(); 
            bulletShot = true; 
        }
    }

    
    SDL_Rect bulletRect;
    bulletRect.w = 25; 
    bulletRect.h = 25; 

    for (auto& bullet : threatBullets) {
        bulletRect.x = bullet.x;
        bulletRect.y = bullet.y;

        SDL_RenderCopy(renderer, bullet.texture, NULL, &bulletRect);
        bullet.y += bullet.speed; 

       
        SDL_Rect characterCollisionRect = { characterRect.x, characterRect.y, characterRect.w, characterRect.h };
        if (checkCollision(bulletRect, characterCollisionRect)) {
            

        }
    }
}



