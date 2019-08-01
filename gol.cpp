#include <iostream>
#include <SDL2/SDL.h>

using namespace std;

SDL_Window *win = NULL;
SDL_Surface *scr = NULL;
SDL_Rect r;

int SCREEN_WIDTH, SCREEN_HEIGHT;
int M, N;

int **galaxy;

int init()
{
    srand(time(NULL));
    if (SDL_Init(SDL_INIT_VIDEO))
    {
        cerr<<"sdl init failed"<<endl;
        return 1;
    }
    if ((win = SDL_CreateWindow("Fullscreen mode", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_SHOWN)) == NULL)
    {
        cerr<<"Failed to create window"<<endl;
        return 2;
    }
    scr = SDL_GetWindowSurface(win);

    SDL_DisplayMode mode;
    if (SDL_GetCurrentDisplayMode(0, &mode))
    {
        cerr<<"SDL_GetCurrentDisplayMode() failed"<<endl;
        return 3;
    }
    N = (SCREEN_WIDTH = mode.w) / 2;
    M = (SCREEN_HEIGHT = mode.h) / 2;

    galaxy = new int*[M];
    for (int i=0; i<M; i++)
    {
        galaxy[i] = new int[N];
        for (int j=0; j<N; j++)
            galaxy[i][j] = rand() % 2;
    }

    r.w = r.h = 2;

    return 0;
}

void quit()
{
    SDL_DestroyWindow(win);
    SDL_Quit();
    for (int i=0; i<M; i++)
        delete [] galaxy[i];
    delete [] galaxy;
}

void display()
{
    SDL_FillRect(scr, NULL, SDL_MapRGB(scr->format, 0, 0, 0));
    for (int i=0; i<M; i++)
        for (int j=0; j<N; j++)
        {
            if (galaxy[i][j])
            {
                r.x = j * 2;
                r.y = i * 2;
                SDL_FillRect(scr, &r, SDL_MapRGB(scr->format, (255*j/N)%255, 255, (255*i/M)%255));
            }
        }
    SDL_UpdateWindowSurface(win);
}

void update()
{
    int **gnew = new int*[M];
    for (int i=0; i<M; i++)
    {
        int im=(i>0)?i-1:M-1, ip=(i<M-1)?i+1:0;
        gnew[i] = new int[N];
        for (int j=0; j<N; j++)
        {
            int jm=(j>0)?j-1:N-1, jp=(j<N-1)?j+1:0;
            int nearby=galaxy[im][jm]+galaxy[im][j]+galaxy[im][jp]+galaxy[i][jm]+galaxy[i][jp]+galaxy[ip][jm]+galaxy[ip][j]+galaxy[ip][jp];
            gnew[i][j]=((nearby==3)||(nearby+galaxy[i][j]==3))?1:0;
        }
    }
    for (int i=0; i<M; i++)
    {
        for (int j=0; j<N; j++)
            galaxy[i][j]=gnew[i][j];
        delete [] gnew[i];
    }
    delete [] gnew;
}

void dopause()
{
    SDL_Rect pause;
    int e = SCREEN_HEIGHT / 20;
    pause.h = pause.w = e * 5;
    pause.x = SCREEN_WIDTH / 2 - pause.w / 2;
    pause.y = SCREEN_HEIGHT / 2 - pause.h / 2;
    SDL_FillRect(scr, &pause, SDL_MapRGB(scr->format, 255, 0, 0));

    pause.h = (pause.w = e) * 3;
    pause.x = SCREEN_WIDTH/2 - 3*e/2;
    pause.y = SCREEN_HEIGHT/2 - 3*e/2;
    SDL_FillRect(scr, &pause, SDL_MapRGB(scr->format, 255, 255, 255));

    pause.x = SCREEN_WIDTH/2 + e/2;
    SDL_FillRect(scr, &pause, SDL_MapRGB(scr->format, 255, 255, 255));

    SDL_UpdateWindowSurface(win);
}

int main()
{
    if (init())
        return 1;

    SDL_Event e;
    bool run = true;
    bool pause = false, paused = false;

    while (run)
    {
        while (SDL_PollEvent(&e))
            if ((e.type==SDL_KEYDOWN))
            {
                switch (e.key.keysym.sym)
                {
                    case SDLK_q:
                        run = false;
                        break;
                    case SDLK_ESCAPE: case SDLK_p:
                        pause = !pause;
                        paused = false;
                        break;
                }
            }
        SDL_Delay(0);
        if (pause)
        {
            if (!paused)
            {
            dopause();
            paused = true;
            }
        }
        if (!pause)
        {
            display();
            update();
        }
    }

    quit();
    return 0;
}
