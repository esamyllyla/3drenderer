
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define PI 3.14159265359
#define toRadians(x) (x * (PI / 180))

float sqrtf(float x);
float atan2f(float y, float x);
float sinf(float x);
float cosf(float x);
float tanf(float x);
float powf(float x, float y);
extern void debugToLogValue0(float val);
extern void debugToLogValue1(float val);

typedef struct
{
    float x;
    float y;
} vertex2d;

typedef struct
{
    float x;
    float y;
    float z;
} vertex3d;

typedef struct
{
  uint32_t *pixels;
  int width;
  int height;
} window;

typedef struct
{
    int width;
    int height;
    uint32_t *pixels;
} SpriteData;

typedef struct
{
    float u1;
    float u2;
    float u3;
    float v1;
    float v2;
    float v3;
} UV;

typedef struct
{
    int txt;
    int shading;
    uint32_t r;
    uint32_t g;
    uint32_t b;
} trianglesettings;

typedef struct
{
  float mouseInit;
  float leaveAngle;
  float relativeAngle;
  float leftAngle;
} mouseAxisState;

//Prototypes:
void ReadObject();
void Swap(float *a, float *b);
void ConvertY(float *Y);
float Normalize(float X, float Min, float Max, float targetMin, float targetMax);
void sort(float arr[], int length);
void UnitVector(float vector[3]);
void RotatePoint(float *x, float *y, float cx, float cy, float tick);
void crossProduct(float vect_A[3], float vect_B[3], float cross_P[3]);
float dotProduct(float vect_A[], float vect_B[], int len);
uint32_t RGBToColor(uint32_t a, uint32_t r, uint32_t g, uint32_t b);
void ProjectToScreen(float realX, float realY, float realZ, 
    float *ScreenX, float *ScreenY, float *ScreenZ);
void getBaryCentricWeight(vertex2d a, vertex2d b, vertex2d c, 
    float *w1, float *w2, float *w3, float pixelX, float pixelY);
void InterpolatedTriangleWUV3d(uint32_t *Buffer, float *ZBuffer, vertex3d aSpace, vertex3d bSpace,
    vertex3d cSpace, UV uv, SpriteData *spriteData, float dim, trianglesettings TriangleSettings);
void RenderUVSprite(uint32_t *Buffer, vertex2d a, vertex2d b, vertex2d c, vertex2d d, 
    SpriteData *spriteData);
void InterpolatedTriangle(uint32_t *Buffer, vertex2d a, vertex2d b, vertex2d c,
    int height, int width);
void drawSprite(uint32_t *Buffer, uint32_t *image, int width, int height, int startX, int startY);
void barycentricZbuffer(float *ZBuffer, uint32_t *screenBuffer, vertex2d a, vertex2d b, 
    vertex2d c, vertex3d spaceA, vertex3d spaceB, vertex3d spaceC);
void debugAnimatedBackground(window *Window, float dt);
void paintScreen(uint32_t *Buffer, uint32_t color);
void debugDrawDot(uint32_t *Buffer, int X, int Y, uint32_t color);
void MouseGrabbingByAxis(mouseAxisState *state, int isMouseDown, int prevMouseDown, int mouseAxis, float dt);
