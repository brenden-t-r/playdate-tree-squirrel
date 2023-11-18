#include <stdio.h>
#include <stdlib.h>

#include "pd_api.h"
#include "collision.h"

static PlaydateAPI* pd = NULL;
void setPDPtr(PlaydateAPI* p) {
    pd = p;
}
static int update(void* userdata);
LCDBitmap *loadImageAtPath(const char *path);
void preloadImages(void);
static void drawSquirrel(int dir);
int titleScreenCheck();
int deathScreenCheck();
void telemetry();
void acornUpdate();
void balanceBarUpdate();
int acornGetPlacement();
void collisionDetection();
void spiderUpdate();
void reset();

int SQUIRREL_CURRENT;

#define SHOW_TELEMETRY 0
#define SHOW_DEBUG_COLLIDERS 0
#define TITLE_SCREEN_ENABLED 1

const char* fontpath = "/System/Fonts/Asheville-Sans-14-Bold.pft";
LCDFont* font = NULL;

// cached images
LCDBitmap *sq_middle_0 = NULL;
LCDBitmap *sq_middle_1 = NULL;
LCDBitmap *sq_left_1a = NULL;
LCDBitmap *sq_left_1b = NULL;
LCDBitmap *sq_left_2a = NULL;
LCDBitmap *sq_left_2b = NULL;
LCDBitmap *sq_right_1a = NULL;
LCDBitmap *sq_right_1b = NULL;
LCDBitmap *sq_right_2a = NULL;
LCDBitmap *sq_right_2b = NULL;
LCDBitmap *acorn = NULL;
LCDBitmap *title_screen[18];
LCDBitmap *balance_bar = NULL;
LCDBitmap *tree = NULL;
LCDBitmap *death_screen = NULL;
LCDBitmap *spider1 = NULL;
LCDBitmap *spider2 = NULL;

void preloadImages(void) {
    sq_middle_0 = loadImageAtPath("images/Squirrel_M0.png");
    sq_middle_1 = loadImageAtPath("images/Squirrel_M1.png");
    sq_left_1a = loadImageAtPath("images/Squirrel_L1A.png");
    sq_left_1b = loadImageAtPath("images/Squirrel_L1B.png");
    sq_left_2a = loadImageAtPath("images/Squirrel_L2A.png");
    sq_left_2b = loadImageAtPath("images/Squirrel_L2B.png");
    sq_right_1a = loadImageAtPath("images/Squirrel_R1A.png");
    sq_right_1b = loadImageAtPath("images/Squirrel_R1B.png");
    sq_right_2a = loadImageAtPath("images/Squirrel_R2A.png");
    sq_right_2b = loadImageAtPath("images/Squirrel_R2B.png");
    acorn = loadImageAtPath("images/Acorn.png");
    tree = loadImageAtPath("images/TreeTexture.png");
    balance_bar = loadImageAtPath("images/BalanceBar.png");
    death_screen = loadImageAtPath("images/DeathScreen.png");
    spider1 = loadImageAtPath("images/Spider-1.png");
    spider2 = loadImageAtPath("images/Spider-2.png");
    char* path = NULL;
    for (int i = 1; i <= 18; i++) {
        pd->system->formatString(&path, "images/TitleScreen/Intro Screen Gamepad %d.png", i);
        title_screen[i - 1] = loadImageAtPath(path);
    }
    SQUIRREL_CURRENT = 0;
}

LCDBitmap *loadImageAtPath(const char *path)
{
    const char *outErr = NULL;
    LCDBitmap *img = pd->graphics->loadBitmap(path, &outErr);
    if ( outErr != NULL ) {
        pd->system->logToConsole("Error loading image at path '%s': %s", path, outErr);
    }
    return img;
}

// *************** //
// SOUND CONTROLLER
// *************** //
// cached sounds
FilePlayer *filePlayer;
SamplePlayer *sp;
AudioSample *sq_see_acorns[3];
AudioSample *sq_eat_acorns[3];
AudioSample *sq_relief[6];
AudioSample *sq_see_spider[3];
AudioSample *sq_uhoh[13];
AudioSample *birdChirping;
AudioSample *spiderChatter;
AudioSample *death;
float volume_left = 1;
float volume_right = 1;
void preloadSounds();
void playSound(AudioSample *s, int loop);

void preloadSounds() {
    filePlayer = pd->sound->fileplayer->newPlayer();
    sp = pd->sound->sampleplayer->newPlayer();
    sq_see_acorns[0] = pd->sound->sample->load("sounds/squirrel/Acorn1.wav");
    sq_see_acorns[1]  = pd->sound->sample->load("sounds/squirrel/Acorn2.wav");
    sq_see_acorns[2]  = pd->sound->sample->load("sounds/squirrel/Acorn3.wav");
    sq_eat_acorns[0]  = pd->sound->sample->load("sounds/squirrel/Eat1.wav");
    sq_eat_acorns[1] = pd->sound->sample->load("sounds/squirrel/Eat2.wav");
    sq_eat_acorns[2] = pd->sound->sample->load("sounds/squirrel/Eat3.wav");
    sq_relief[0] = pd->sound->sample->load("sounds/squirrel/Relief1.wav");
    sq_relief[1] = pd->sound->sample->load("sounds/squirrel/Relief2.wav");
    sq_relief[2] = pd->sound->sample->load("sounds/squirrel/Relief3.wav");
    sq_relief[3] = pd->sound->sample->load("sounds/squirrel/Relief4.wav");
    sq_relief[4] = pd->sound->sample->load("sounds/squirrel/Relief5.wav");
    sq_relief[5] = pd->sound->sample->load("sounds/squirrel/Relief6.wav");
    sq_see_spider[0] = pd->sound->sample->load("sounds/squirrel/Spider1.wav");
    sq_see_spider[1] = pd->sound->sample->load("sounds/squirrel/Spider2.wav");
    sq_see_spider[2] = pd->sound->sample->load("sounds/squirrel/Spider3.wav");
    sq_uhoh[0] = pd->sound->sample->load("sounds/squirrel/Uhoh1.wav");
    sq_uhoh[1] = pd->sound->sample->load("sounds/squirrel/Uhoh2.wav");
    sq_uhoh[2] = pd->sound->sample->load("sounds/squirrel/Uhoh3.wav");
    sq_uhoh[3] = pd->sound->sample->load("sounds/squirrel/Uhoh4.wav");
    sq_uhoh[4] = pd->sound->sample->load("sounds/squirrel/Uhoh5.wav");
    sq_uhoh[5] = pd->sound->sample->load("sounds/squirrel/Uhoh6.wav");
    sq_uhoh[6] = pd->sound->sample->load("sounds/squirrel/Uhoh7.wav");
    sq_uhoh[7] = pd->sound->sample->load("sounds/squirrel/Uhoh8.wav");
    sq_uhoh[8] = pd->sound->sample->load("sounds/squirrel/Uhoh9.wav");
    sq_uhoh[9] = pd->sound->sample->load("sounds/squirrel/Uhoh10.wav");
    sq_uhoh[10] = pd->sound->sample->load("sounds/squirrel/Uhoh11.wav");
    sq_uhoh[11] = pd->sound->sample->load("sounds/squirrel/Uhoh12.wav");
    sq_uhoh[12] = pd->sound->sample->load("sounds/squirrel/Uhoh13.wav");
    birdChirping = pd->sound->sample->load("sounds/misc/BirdsChirping.wav");
    spiderChatter = pd->sound->sample->load("sounds/misc/SpiderChatter.wav");
    death = pd->sound->sample->load("sounds/misc/Death.wav");
}

void playSound(AudioSample *s, int loop) {
    pd->sound->sampleplayer->setSample(sp, s);
    pd->sound->sampleplayer->play(sp, loop, 1);
}

void playSeeAcorn() {
    int i = rand() % 3; // NOLINT(*-msc50-cpp)
    playSound(sq_see_acorns[i], 1);
}

void playEatAcorn() {
    int i = rand() % 3; // NOLINT(*-msc50-cpp)
    playSound(sq_eat_acorns[i], 1);
}

void playRelief() {
    int i = rand() % 6; // NOLINT(*-msc50-cpp)
    playSound(sq_relief[i], 1);
}

void playSeeSpider() {
    int i = rand() % 3; // NOLINT(*-msc50-cpp)
    playSound(sq_see_spider[i], 1);
}

void playUhOh() {
    int i = rand() % 13; // NOLINT(*-msc50-cpp)
    playSound(sq_uhoh[i], 1);
}

// *************** //

#ifdef _WINDLL
__declspec(dllexport)
#endif
int eventHandler(PlaydateAPI* pd, PDSystemEvent event, uint32_t arg)
{
	(void)arg; // arg is currently only used for event = kEventKeyPressed

	if ( event == kEventInit )
	{
        setPDPtr(pd);
        preloadImages();
        preloadSounds();

		const char* err;
		font = pd->graphics->loadFont(fontpath, &err);
		if ( font == NULL )
			pd->system->error("%s:%i Couldn't load font %s: %s", __FILE__, __LINE__, fontpath, err);

		// Note: If you set an update callback in the kEventInit handler, the system assumes the game is pure C and doesn't run any Lua code in the game
        srand(pd->system->getSecondsSinceEpoch(NULL));
        pd->system->setUpdateCallback(update, pd);
	}
	
	return 0;
}

int IS_TITLE_SCREEN = 1;
int IS_TITLE_SCREEN_TRANSITION = 0;
int IS_DEATH_SCREEN = 0;
int TITLE_SCREEN_TRANSITION_SPEED = 10;
int title_screen_anim_counter = 0;
int title_screen_anim_index = 0;
int title_screen_transition_x = 0;
int played_sound_death = 0;
int played_sound_birds = 0;
int squirrel_x = 80;
int squirrel_y = 60;
int TREE_SPEED = 7;
int TREE_Y_B_START = -1440;
int tree_y = -1440 + 240;
int tree_y_b = -1440;
int score = 0;
unsigned int acorn_last_time = 0;
int acorn_x = -1;
int acorn_y = -100;
int acorn_eaten = 0;
int acorn_spotted = 0;
int ACORN_SPEED = 2;
int FONT_HEIGHT = 14;
int X_OFFSET_LEFT = 120;
int X_OFFSET_MIDDLE = 185;
int X_OFFSET_RIGHT = 250;
int spider_anim_frames = 0;
int spider_x = -1;
int spider_y = -100;
int spider_spotted = 0;
unsigned int spider_last_time = 0;
int SPIDER_SPEED = 2;
char* scoreStr = NULL;

static int update(void* userdata)
{
	PlaydateAPI* pd = userdata;

    pd->graphics->setBackgroundColor(kColorBlack);
    pd->graphics->clear(kColorBlack);
	pd->graphics->setFont(font);
    pd->graphics->setDrawMode(kDrawModeCopy);

    // TITLE SCREEN
#if TITLE_SCREEN_ENABLED
    if (titleScreenCheck()) {
        return 1;
    }
#endif

    // IS DEATH SCREEN
    if (deathScreenCheck()) {
        return 1;
    }

#if SHOW_TELEMETRY
    // TELEMETRY
    pd->graphics->setDrawMode(kDrawModeInverted);
    telemetry();
#endif

    // TREE, SCROLLING BACKGROUND
    pd->graphics->setDrawMode(kDrawModeCopy);
    if (tree_y < 0) {
        pd->graphics->drawRotatedBitmap(tree, -95, tree_y, 0, 0, 0, 1, 1);
        tree_y += TREE_SPEED;
    }
    else {
        pd->graphics->drawRotatedBitmap(tree, -95, tree_y_b, 0, 0, 0, 1, 1);
        pd->graphics->drawRotatedBitmap(tree, -95, tree_y, 0, 0, 0, 1, 1);
        tree_y += TREE_SPEED;
        tree_y_b += TREE_SPEED;

        if (tree_y >= 240) {
            tree_y = tree_y_b;
            tree_y_b = TREE_Y_B_START;
        }
    }

    // SQUIRREL
    drawSquirrel(SQUIRREL_CURRENT);

    // BALANCE BAR
    balanceBarUpdate();

    // ACORN
    acornUpdate();

    // SPIDER
    spiderUpdate();

#if SHOW_TELEMETRY
    char* str = NULL;
    pd->system->formatString(&str, "%d", spider_y);
    pd->graphics->setDrawMode(kDrawModeInverted);
    pd->graphics->drawText(str, strlen(str), kASCIIEncoding, 10, 60);
    pd->graphics->setDrawMode(kDrawModeCopy);
    pd->system->realloc(str, 0);
#endif

    // COLLISION DETECTION
    collisionDetection();

    pd->graphics->setDrawMode(kDrawModeInverted);
    pd->system->formatString(&scoreStr, "Score: %d", score);
    pd->graphics->drawText(scoreStr, strlen(scoreStr), kASCIIEncoding, 10, 215);
    pd->system->drawFPS(0,0);
	return 1;
}

int bb_x_offset = 5;
int bb_x_l = 110 + 5;
int bb_x_c = 200 + 5;
int bb_x_r = 290 + 5;
int bb_x = 200 + 5; // bb_x_c

const int bb_l_2 = 110 + 5 + 25;
const int bb_l_1 = 110 + 5 + 60;
const int bb_r_1 = 290 + 5 - 60;
const int bb_r_2 = 290 + 5 - 25;

enum bb_state {
    l_2, l_1, c, r_1, r_2
};
int bb_current = c;

void balanceBarUpdate() {
    pd->graphics->fillRect(115, 220, 180, 20, kColorBlack);
    pd->graphics->drawBitmap(balance_bar, 77, 90, 0);
    pd->graphics->fillEllipse(bb_x, 226, 8, 8, 0, 0, kColorWhite);

    PDButtons current, pushed, released;
    pd->system->getButtonState(&current, &pushed, &released);
    if (kButtonLeft & current && bb_x < bb_x_r) {
       bb_x -= 8;
    }
    if (kButtonRight & current && bb_x > bb_x_l) {
       bb_x += 8;
    }

    float crankChange = pd->system->getCrankChange();
    if (crankChange < 0) {
        pd->system->logToConsole("crankChangeStr < 0");
        bb_x += (int)(crankChange * 10);
    }
    else if (crankChange > 0) {
        pd->system->logToConsole("crankChangeStr > 0");
        bb_x += (int)(crankChange * 10);
    }

    if (bb_x >= bb_l_1 && bb_x <= bb_r_1) {
        // Center sprite
        if (bb_current != c) {
            bb_current = c;
            SQUIRREL_CURRENT = 0;
        }
    } else if (bb_x < bb_l_1 && bb_x >= bb_l_2) {
        // Left 1 sprite
        if (bb_current != l_1) {
            bb_current = l_1;
            SQUIRREL_CURRENT = -1;
        }
    } else if (bb_x < bb_l_2 && bb_x > bb_x_l) {
        // Left 2 sprite
        if (bb_current != l_2) {
            bb_current = l_2;
            SQUIRREL_CURRENT = -2;
        }
    } else if (bb_x <= bb_x_l) {
        // Death left
        bb_x = bb_x_c;
        SQUIRREL_CURRENT = 0;
        IS_DEATH_SCREEN = 1;
    } else if (bb_x > bb_r_1 && bb_x <= bb_r_2) {
        // Right 1 sprite
        if (bb_current != r_1) {
            bb_current = r_1;
            SQUIRREL_CURRENT = 1;
        }
    } else if (bb_x > bb_r_2 && bb_x < bb_x_r) {
        // Right 2 sprite
        if (bb_current != r_2) {
            bb_current = r_2;
            SQUIRREL_CURRENT = 2;
        }
    } else if (bb_x >= bb_x_r) {
        // Death right
        bb_x = bb_x_c;
        SQUIRREL_CURRENT = 0;
        IS_DEATH_SCREEN = 1;
    } else {
        pd->system->logToConsole("Shouldn't happen balanceBarUpdate()");
    }
}

static void drawSquirrel(int dir) {
    LCDBitmap *a, *b;
    if (dir == -1) {
        a = sq_left_1a;
        b = sq_left_1b;
    } else if (dir == -2) {
        a = sq_left_2a;
        b = sq_left_2b;
    } else if (dir == 1) {
        a = sq_right_1a;
        b = sq_right_1b;
    } else if (dir == 2) {
        a = sq_right_2a;
        b = sq_right_2b;
    } else {
        a = sq_middle_0;
        b = sq_middle_1;
    }

    double elapsed = (double) pd->system->getElapsedTime();
    double speed = 0.1;
    if (elapsed > speed) {
        pd->graphics->drawBitmap(a, squirrel_x, squirrel_y, 0);
        if (elapsed > speed * 2) {
            pd->system->resetElapsedTime();
        }
    } else {
        pd->graphics->drawBitmap(b, squirrel_x, squirrel_y, 0);
    }
}

int acornGetPlacement() {
    int dir = rand() % 3; // random int between 0 and 2; NOLINT(*-msc50-cpp)
    if (dir == 0) return X_OFFSET_LEFT;
    else if (dir == 1) return X_OFFSET_MIDDLE;
    else return X_OFFSET_RIGHT;
}

void acornUpdate() {
    unsigned int acorn_time_new = pd->system->getCurrentTimeMilliseconds();
    unsigned int delta_time = acorn_time_new - acorn_last_time;
    if (delta_time > 3000 || acorn_last_time == 0) {
        if (!acorn_eaten) {
            if (acorn_x == -1) {
                acorn_x = acornGetPlacement();
            }
            pd->graphics->drawRotatedBitmap(acorn, acorn_x, acorn_y, 0, 0, 0, 0.66f, 0.66f);
        }
        if (!acorn_spotted) {
            if (acorn_y > -50 && acorn_y < 240) {
                pd->system->logToConsole("Acorn");
                playSeeAcorn();
                acorn_spotted = 1;
            }
        }
        acorn_y += ACORN_SPEED;
        if (acorn_y >= 240) {
            acorn_last_time = acorn_time_new;
            acorn_y = -100;
            acorn_x = -1;
            acorn_eaten = 0;
            acorn_spotted = 0;
        }
    }
#if SHOW_TELEMETRY
    char* str = NULL;
    pd->system->formatString(&str, "%d", acorn_y);
    pd->graphics->setDrawMode(kDrawModeInverted);
    pd->graphics->drawText(str, strlen(str), kASCIIEncoding, 10, 40);
    pd->graphics->setDrawMode(kDrawModeCopy);
    pd->system->realloc(str, 0);
#endif
}

int draw_offset = 0;
int draw_offset_x = -25;
int draw_offset_dir = 0;
int titleScreenCheck() {
    if (IS_TITLE_SCREEN) {
        if (!pd->sound->fileplayer->isPlaying(filePlayer)) {
            // Start title music
            pd->sound->fileplayer->loadIntoPlayer(filePlayer, "sounds/music/Intro.mp3");
            pd->sound->fileplayer->play(filePlayer, 0);
        }

        pd->graphics->setDrawOffset(draw_offset_x, draw_offset);
        if (draw_offset_dir) {
            draw_offset += 1;
        } else {
            draw_offset -= 1;
        }
        if (draw_offset <= -110) {
            draw_offset_dir = 1;
        }
        if (draw_offset >= 0) {
            draw_offset_dir = 0;
        }

        if (title_screen_anim_index == 15) {
            title_screen_anim_index += 1;
        }

        pd->graphics->drawRotatedBitmap(title_screen[title_screen_anim_index], 0, 0, 0, 0, 0, 1,1);
        // Animation, every 2 frames
        title_screen_anim_counter += 1;
        if (title_screen_anim_counter % 3 == 0) {
            title_screen_anim_index += 1;
            if (title_screen_anim_index == 18) {
                title_screen_anim_index = 0;
                title_screen_anim_counter = 0;
            }
        }
        PDButtons current, pushed, released;
        pd->system->getButtonState(&current, &pushed, &released);
        if (kButtonA& released) {
            IS_TITLE_SCREEN = 0;
            IS_TITLE_SCREEN_TRANSITION = 1;
            title_screen_transition_x = 0;
            score = 0;
        }
        return 1;
    }
    if (IS_TITLE_SCREEN_TRANSITION) {
        // birds chirping
        if (!played_sound_birds) {
            playSound(birdChirping, 1);
            played_sound_birds = 1;
        }

        // fade out music
        float volume_fade_speed = 1 / (400.0f / (float)TITLE_SCREEN_TRANSITION_SPEED);
        pd->sound->fileplayer->getVolume(filePlayer, &volume_left, &volume_right);
        pd->sound->fileplayer->setVolume(filePlayer, volume_left - volume_fade_speed, volume_right - volume_fade_speed);

        // move
        pd->graphics->drawRotatedBitmap(title_screen[0], title_screen_transition_x, 0, 0, 0, 0, 1, 1);
        title_screen_transition_x -= TITLE_SCREEN_TRANSITION_SPEED;
        if (title_screen_transition_x < -640) {
            IS_TITLE_SCREEN_TRANSITION = 0;
            pd->sound->sampleplayer->stop(sp);
            pd->sound->sampleplayer->setVolume(sp, 1, 1);
            pd->sound->fileplayer->stop(filePlayer);
            pd->sound->fileplayer->setVolume(filePlayer, 1, 1);
            pd->sound->fileplayer->loadIntoPlayer(filePlayer, "sounds/music/Main.mp3");
            pd->sound->fileplayer->play(filePlayer, 0);
            pd->graphics->setDrawOffset(0, 0);
        }
        return 1;
    }
    return 0;
}

int deathScreenCheck() {
    if (IS_DEATH_SCREEN) {

        if (pd->sound->fileplayer->isPlaying(filePlayer)) {
            pd->sound->fileplayer->stop(filePlayer);
        }
        if (!played_sound_death) {
            playSound(death, 1);
            played_sound_death = 1;
        }
        pd->graphics->drawRotatedBitmap(death_screen, 0, 0, 0, 0, 0, 0.66f, 0.66f);
        pd->graphics->fillRect(0, 140, 400, 80, kColorBlack);
        pd->graphics->setDrawMode(kDrawModeInverted);
        pd->graphics->drawText("Press A To Try Again", strlen("Press A To Try Again"), kASCIIEncoding, 100, 160);
        pd->graphics->setDrawMode(kDrawModeCopy);
        PDButtons current, pushed, released;
        pd->system->getButtonState(&current, &pushed, &released);
        if (kButtonA& released) {
            reset();
            IS_DEATH_SCREEN = 0;
            IS_TITLE_SCREEN = 1;
            IS_TITLE_SCREEN_TRANSITION = 0;
            played_sound_death = 0;
            played_sound_birds = 0;
            pd->sound->sampleplayer->stop(sp);
        }
        return 1;
    }
    return 0;
}

struct box_collider CD_SQUIRREL_L2 = {{65, 43},40, 40, TOP_LEFT};
struct box_collider CD_SQUIRREL_L1 = {{84,23}, 40, 40, TOP_LEFT};
struct box_collider CD_SQUIRREL_C = {{106, 20}, 40, 40, TOP_LEFT};
struct box_collider CD_SQUIRREL_R1 = {{130,23}, 40, 40, TOP_LEFT};
struct box_collider CD_SQUIRREL_R2 = {{151, 43}, 40, 40, TOP_LEFT};
struct box_collider CD_ACORN = {{0, 0, }, 40, 40, TOP_LEFT};
struct box_collider CD_SPIDER = {{0, 0}, 30, 10, CENTER};

void collisionDetection() {
    struct box_collider sq_cd;
    switch (SQUIRREL_CURRENT) {
        case -2:
            sq_cd = CD_SQUIRREL_L2;
            break;
        case -1:
            sq_cd = CD_SQUIRREL_L1;
            break;
        case 0:
            sq_cd = CD_SQUIRREL_C;
            break;
        case 1:
            sq_cd = CD_SQUIRREL_R1;
            break;
        case 2:
            sq_cd = CD_SQUIRREL_R2;
            break;
        default:
            sq_cd = CD_SQUIRREL_C;
            // Shouldn't happen
    }
    struct vector2 sq_pos = {
            squirrel_x, squirrel_y
    };
    struct vector2 acorn_pos = {
            acorn_x, acorn_y
    };

    if (!acorn_eaten) {
        int collision = detect(sq_pos, acorn_pos, sq_cd, CD_ACORN);
        if (collision) {
            score += 1;
            acorn_eaten = 1;
            playEatAcorn();
        }
    }
#if SHOW_DEBUG_COLLIDERS
    pd->graphics->drawRect(sq_pos.x, sq_pos.y, sq_cd.width, sq_cd.height, kColorWhite);
    pd->graphics->drawRect(acorn_pos.x, acorn_pos.y, CD_ACORN.width, CD_ACORN.height, kColorWhite);
#endif

    struct vector2 spider_pos = {
            spider_x, spider_y
    };
    int collision = detect(sq_pos, spider_pos, sq_cd, CD_SPIDER);
    if (collision) {
        IS_DEATH_SCREEN = 1;
    }
#if SHOW_DEBUG_COLLIDERS
    pd->graphics->drawRect(spider_cd_x, spider_cd_y, spider_cd_w, spider_cd_h, kColorWhite);
#endif
}

void spiderUpdate() {
    //1 = 0,1,2,3
    //2 = 4,5,6,7
    unsigned int spider_time_new = pd->system->getCurrentTimeMilliseconds();
    unsigned int delta_time = spider_time_new - spider_last_time;
    if (delta_time > 20000) {
        if (spider_x == -1) {
            spider_x = acornGetPlacement(); // rename
        }
        if (!spider_spotted) {
            if (spider_y > -20 && spider_y < 240) {
                playSeeSpider();
                spider_spotted = 1;
            }
        }

        spider_y += SPIDER_SPEED;
        if (spider_y >= 380) {
            // Y offset to reach bottom of screen is higher due to the rotation
            spider_last_time = spider_time_new;
            spider_y = -100;
            spider_x = -1;
            spider_spotted = 0;
            playRelief();
        }

        if (spider_anim_frames >= 4) {
            pd->graphics->drawRotatedBitmap(spider2, spider_x, spider_y, -110, 0.5f,0.5f, 0.5f, 0.5f);
            if (spider_anim_frames >= 7) {
                spider_anim_frames = 0;
            }
        }
        else {
            pd->graphics->drawRotatedBitmap(spider1, spider_x, spider_y, -110, 0.5f,0.5f, 0.5f, 0.5f);
        }
        spider_anim_frames += 1;
    }
}

void reset() {
    played_sound_death = 0;
    played_sound_birds = 0;
    squirrel_x = 80;
    squirrel_y = 60;
    tree_y = -1440 + 240;
    tree_y_b = -1440;
    score = 0;
    acorn_last_time = 0;
    acorn_x = -1;
    acorn_y = -100;
    acorn_eaten = 0;
    acorn_spotted = 0;
    spider_anim_frames = 0;
    spider_x = -1;
    spider_y = -100;
    spider_spotted = 0;
    spider_last_time = 0;
}

void telemetry() {
    PDButtons current;
    PDButtons pushed;
    PDButtons released;
    pd->system->getButtonState(&current, &pushed, &released);
    int BUTTON_PRESSED_HEIGHT = 2 * FONT_HEIGHT;
    if (kButtonLeft & current) {
        pd->graphics->drawText("Left pressed", strlen("Left pressed"), kASCIIEncoding, 14, BUTTON_PRESSED_HEIGHT);
    }
    if (kButtonRight & current) {
        pd->graphics->drawText("Right pressed", strlen("Right pressed"), kASCIIEncoding, 14, BUTTON_PRESSED_HEIGHT);
    }
    if (kButtonUp & current) {
        pd->graphics->drawText("Up pressed", strlen("Up pressed"), kASCIIEncoding, 14, BUTTON_PRESSED_HEIGHT);
    }
    if (kButtonDown & current) {
        pd->graphics->drawText("Down pressed", strlen("Down pressed"), kASCIIEncoding, 14, BUTTON_PRESSED_HEIGHT);
    }
    if (kButtonA& current) {
        pd->graphics->drawText("A pressed", strlen("A pressed"), kASCIIEncoding, 14, BUTTON_PRESSED_HEIGHT);
    }
    if (kButtonB& current) {
        pd->graphics->drawText("B pressed", strlen("B pressed"), kASCIIEncoding, 14, BUTTON_PRESSED_HEIGHT);
    }

    // Crank checks
    int crankDocked = pd->system->isCrankDocked();
    float crankAngle = pd->system->getCrankAngle();
    float crankChange = pd->system->getCrankChange();
    char* crankAngleStr = NULL;
    char* crankChangeStr = NULL;
    pd->system->formatString(&crankAngleStr, "Crank Angle: %f", (double) crankAngle);
    pd->system->formatString(&crankChangeStr, "Crank Change: %f", (double) crankChange);
    int CRANK_HEIGHT = FONT_HEIGHT * 5;
    if (crankDocked) {
        pd->graphics->drawText("Crank docked", strlen("Crank docked"), kASCIIEncoding, 14, CRANK_HEIGHT);
    } else {
        pd->graphics->drawText("Crank undocked", strlen("Crank undocked"), kASCIIEncoding, 14, CRANK_HEIGHT);
    }
    pd->graphics->drawText(crankAngleStr, strlen(crankAngleStr), kASCIIEncoding, 14, CRANK_HEIGHT + FONT_HEIGHT);
    pd->graphics->drawText(crankChangeStr, strlen(crankChangeStr), kASCIIEncoding, 14, CRANK_HEIGHT + (FONT_HEIGHT*2));
    pd->system->realloc(crankAngleStr, 0);
    pd->system->realloc(crankChangeStr, 0);
}

void rotatedBoxCollider(int angle) {
    // counter-clockwise rotation point calc
    float sp_pox_x = spider_x;
    float sp_pos_y = spider_y;
    float spx = sp_pox_x;
    float spy = sp_pos_y;
    float spw = 128;
    float sph = 116;
    float sptrx = spx + spw;
    float sptry = spy;
    float spbrx = spx + spw;
    float spbry = spy + sph;
    float spblx = spx;
    float spbly = spy + sph;

    float translation_x = spx + (spw/2);
    float translation_y = spy + (sph/2);

    spx -= translation_x;
    spy -= translation_y;
    sptrx -= translation_x;
    sptry -= translation_y;
    spbrx -= translation_x;
    spbry -= translation_y;
    spblx -= translation_x;
    spbly -= translation_y;

    float a = angle * 3.14f / 180.0f;

    float spider_top_left_x = (spx * cosf(a)) + (spy * sinf(a));
    float spider_top_left_y = (-1 * spx * sinf(a)) + (spy * cosf(a));
    float spider_top_right_x = (sptrx * cosf(a)) + (sptry * sinf(a));
    float spider_top_right_y = (-1 * sptrx * sinf(a)) + (sptry * cosf(a));
    float spider_bottom_left_x = (spblx * cosf(a)) + (spbly * sinf(a));
    float spider_bottom_left_y = (-1 * spblx * sinf(a)) + (spbly * cosf(a));
    float spider_bottom_right_x = (spbrx * cosf(a)) + (spbry * sinf(a));
    float spider_bottom_right_y = (-1 * spbrx * sinf(a)) + (spbry * cosf(a));

    translation_x -= (spw/2);
    translation_y -= (sph/2);

    spider_top_left_x += translation_x;
    spider_top_left_y += translation_y;
    spider_top_right_x += translation_x;
    spider_top_right_y += translation_y;
    spider_bottom_left_x += translation_x;
    spider_bottom_left_y += translation_y;
    spider_bottom_right_x += translation_x;
    spider_bottom_right_y += translation_y;
}

