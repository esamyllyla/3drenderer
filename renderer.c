#include <stdint.h>
#include "rendererheader.h"
#include "object.c"

int init = 0;

static vertex3d objVertices[10000];
static int objFaces[10000][3];
static int objFaces[10000][3];
static int VerticeAmount;
static int FaceAmount;

void ReadObject()
{
    for(int i = 0; i < CFileVerticeAmount; i++)
    {
        objVertices[i].x = CFileVertices[i].x;
        objVertices[i].y = CFileVertices[i].y;
        objVertices[i].z = CFileVertices[i].z;
    }
    for(int i = 0; i < CFileFaceAmount; i++)
    {
        objFaces[i][0] = CFileFaces[i][0];
        objFaces[i][1] = CFileFaces[i][1];
        objFaces[i][2] = CFileFaces[i][2];
    }
    VerticeAmount = CFileVerticeAmount;
    FaceAmount = CFileFaceAmount;
}

uint32_t RGBToColor(uint32_t a, uint32_t r, uint32_t g, uint32_t b)
{
    uint32_t color = (a << 24 | b << 16 | g << 8 | r);
    return color;
}

void ConvertY(float *Y)
{
    *Y = SCREEN_HEIGHT - *Y;
}

float Normalize(float X, float Min, float Max, float targetMin, float targetMax)
{
    float NormalizedX = (X - Min) / (Max - Min);
    NormalizedX *= (targetMax - targetMin);
    NormalizedX += targetMin;
    return NormalizedX;
}

void UnitVector(float vector[3])
{
    float magnitude = sqrtf(powf(vector[0], 2) + powf(vector[1], 2) + powf(vector[2], 2));
    vector[0] = vector[0] / magnitude;
    vector[1] = vector[1] / magnitude;
    vector[2] = vector[2] / magnitude;
}

void sort(float arr[], int length)
{
    for (int i = 0; i < length - 1; i++) {
        for (int j = 0; j < length - i - 1; j++) {
            if (arr[j] < arr[j + 1]) {
                float temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

void RotatePoint(float *x, float *y, float cx, float cy, float tick)
{
    float dx = *x - cx;
    float dy = *y - cy;
    
    float r = sqrtf(dx*dx+dy*dy);
    float angle = atan2f(dy, dx) + tick;
    
    *x = cx + cosf(angle)*r;
    *y = cy + sinf(angle)*r;
}

void crossProduct(float vect_A[3], float vect_B[3], float cross_P[3])
{

    cross_P[0] = vect_A[1] * vect_B[2] - vect_A[2] * vect_B[1];
    cross_P[1] = vect_A[2] * vect_B[0] - vect_A[0] * vect_B[2];
    cross_P[2] = vect_A[0] * vect_B[1] - vect_A[1] * vect_B[0];
}

float dotProduct(float vect_A[], float vect_B[], int len)
{
    float product = 0;
    
    for(int i = 0; i < len; i++)
    {
        product = product + vect_A[i]*vect_B[i];
    }
    
    return product;
}

void ProjectToScreen(float realX, float realY, float realZ, 
    float *ScreenX, float *ScreenY, float *ScreenZ)
{
    float HalfWidth = (float)SCREEN_WIDTH / 2.0f;
    float HalfHeight = (float)SCREEN_HEIGHT / 2.0f;
    
    float tanHalfFov = (float)tanf(toRadians(90.0f/2.0f));
    
    *ScreenX = ((realX/(realZ * tanHalfFov)) * HalfWidth + HalfWidth);
    *ScreenY = ((realY/(realZ * tanHalfFov)) * HalfHeight + HalfHeight);
    *ScreenZ = (realZ * HalfWidth + HalfWidth);

    ConvertY(ScreenY);
}

void getBaryCentricWeight(vertex2d a, vertex2d b, vertex2d c, 
                          float *w1, float *w2, float *w3, float pixelX, float pixelY)
{
    float x1 = a.x;
    float x2 = b.x;
    float x3 = c.x;
    
    float y1 = a.y;
    float y2 = b.y;
    float y3 = c.y;
    
    float wa1 = (((y2-y3)*(pixelX-x3)) + ((x3-x2)*(pixelY-y3))) /
                (((y2-y3)*(x1-x3))     + ((x3-x2)*(y1-y3)));
    
    float wa2 = ((y3-y1)*(pixelX-x3) + (x1-x3)*(pixelY-y3)) /
         ((y2-y3)*(x1-x3)     + (x3-x2)*(y1-y3));
    
    float wa3 = 1 - wa1 - wa2;
    
    *w1 = wa1;
    *w2 = wa2;
    *w3 = wa3;
}

void InterpolatedTriangleWUV3d(uint32_t *Buffer, float *ZBuffer, vertex3d aSpace, vertex3d bSpace,
			       vertex3d cSpace, UV uv, SpriteData *spriteData, float dim,
			       trianglesettings TriangleSettings)
{
    int sprite_width = spriteData->width;
    int sprite_height = spriteData->height;

    vertex2d a;
    vertex2d b;
    vertex2d c;

    float ScreenZ;
    ProjectToScreen(aSpace.x, aSpace.y, aSpace.z, 
		&a.x, &a.y, &ScreenZ);
    ProjectToScreen(bSpace.x, bSpace.y, bSpace.z, 
        &b.x, &b.y, &ScreenZ);
    ProjectToScreen(cSpace.x, cSpace.y, cSpace.z, 
        &c.x, &c.y, &ScreenZ);

    float x1 = a.x;
    float x2 = b.x;
    float x3 = c.x;
    
    float y1 = a.y;
    float y2 = b.y;
    float y3 = c.y;

    float Ys[3] = {y1, y2, y3};
    
    sort(Ys, 3);
    
    float MinY = Ys[2];
    float MaxY = Ys[0];
    
    float Xs[3] = {x1, x2, x3};
    
    sort(Xs, 3);
    
    float MinX = Xs[2];
    float MaxX = Xs[0];

    //check2dBoundaries(&MinX, &MaxX, &MinY, &MaxY, 0, SCREEN_WIDTH, 0, SCREEN_HEIGHT);
    
    if(MinX < 0)
    {
        MinX = 0;
    }
    if(MinY < 0)
    {
        MinY = 0;
    }
    if(MaxX > SCREEN_WIDTH)
    {
        MaxX = SCREEN_WIDTH;
    }
    if(MaxY > SCREEN_HEIGHT)
    {
        MaxY = SCREEN_HEIGHT;
    }

    vertex2d ConvertedA = {x1, y1};
    vertex2d ConvertedB = {x2, y2};
    vertex2d ConvertedC = {x3, y3};
    
    float w1, w2, w3;
    float u1, u2, u3;
    float v1, v2, v3;

    u1 = uv.u1 / aSpace.z;
    u2 = uv.u2 / bSpace.z;
    u3 = uv.u3 / cSpace.z;
    v1 = uv.v1 / aSpace.z;
    v2 = uv.v2 / bSpace.z;
    v3 = uv.v3 / cSpace.z;

    for(int Y = MinY; Y < MaxY; ++Y)
    {
        for(int X = MinX; X < MaxX; ++X)
        {
            getBaryCentricWeight(ConvertedA, ConvertedB, ConvertedC, &w1, &w2, &w3, X, Y);
            
            if((w1 > 0) && (w2 > 0) && (w3 > 0))
            {
                uint32_t r;
                uint32_t g;
                uint32_t b;
                uint32_t a = 0xFF;

                float u = w1*u1 + w2*u2 + w3*u3;
                float v = w1*v1 + w2*v2 + w3*v3;

                float invZ = (w1/aSpace.z + w2/bSpace.z + w3/cSpace.z);

                u /= invZ;
                v /= invZ;

                int texX = (int)(u * (sprite_width));
                int texY = (int)(v * (sprite_height));

                uint32_t color;
                color = spriteData->pixels[texY * sprite_width + texX];

                r = color & 0x000000FF;
                g = (color >> 8) & 0x000000FF;
                b = (color >> 16) & 0x000000FF;

                if(!TriangleSettings.txt)
                {
                    r = TriangleSettings.r;
                    g = TriangleSettings.g;
                    b = TriangleSettings.b;
                }

                if(TriangleSettings.shading)
                {
                    if(dim < 1)
                    {
                        r *= dim;
                        g *= dim;
                        b *= dim;
                    }
                    else if(dim >= 1)
                    {
                        r += (dim - 1) * 128;
                        g += (dim - 1) * 128;
                        b += (dim - 1) * 128;

                        if(r > 255) r = 255;
                        if(g > 255) g = 255;
                        if(b > 255) b = 255;
                    }
                }

                color = RGBToColor(a, r, g, b);
                if(ZBuffer[Y*SCREEN_WIDTH + X] > 1/invZ)
                {
                    ZBuffer[Y*SCREEN_WIDTH + X] = (1/invZ);
                    Buffer[Y*SCREEN_WIDTH + X] = color;
                }
            }
        }
    }
}

void debugAnimatedBackground(window *Window, float offset)
{
    for(int X = 0; X < Window->width; X++)
	{
		for(int Y = 0; Y < Window->height; Y++)
		{
			uint32_t r = (uint8_t)X + offset;	
			uint32_t g = 0;	
			uint32_t b = (uint8_t)Y + offset;
			uint32_t a = 255;
			uint32_t color = RGBToColor(a, r, g, b);
			//AABBGGRR
			Window->pixels[Y*Window->width + X] = color;
		}
	}
}

void drawSprite(uint32_t *Buffer, uint32_t *image, int width, int height, int startX, int startY)
{
	int MaxX = startX + width;
	int MaxY = startY + height;
	if(startX < 0) startX = 0;
	if(startY < 0) startY = 0;
	
	for(int X = startX; X < MaxX; X++)
	{
	    for(int Y = startY; Y < MaxY; Y++)
	    {
		if(MaxX < SCREEN_WIDTH && MaxY < SCREEN_HEIGHT)
		{
		     uint32_t color = image[(Y-startY)*width + (X-startX)];

		     uint32_t r = color & 0x000000FF;
		     uint32_t g = (color >> 8) & 0x000000FF;
		     uint32_t b = (color >> 16) & 0x000000FF;
		     uint32_t a = 0xFF;

		     Buffer[Y*width + X] = RGBToColor(a, r, g, b);
		}
	    }
	}
}

void paintScreen(uint32_t *Buffer, uint32_t color)
{
    for(int i = 0; i < SCREEN_WIDTH*SCREEN_HEIGHT; i++)
    {
        Buffer[i] = color;
    }
}

void debugDrawDot(uint32_t *Buffer, int X, int Y, uint32_t color)
{
    if(0 < X && X < SCREEN_WIDTH && 0 < Y && Y < SCREEN_HEIGHT) Buffer[Y*SCREEN_WIDTH + X] = color;
}

void MouseGrabbingByAxis(mouseAxisState *state, int isMouseDown, int prevMouseDown, int mouseAxis, float dt)
{
    if(isMouseDown && !prevMouseDown)
	{
		state->mouseInit = (float)mouseAxis;
	}

	if(isMouseDown && prevMouseDown)
	{
		state->relativeAngle = -((float)mouseAxis - state->mouseInit) / 100.0f;
		state->leaveAngle = state->relativeAngle + state->leftAngle;
	}

	if(!isMouseDown && prevMouseDown)
	{
           state->leftAngle = state->leaveAngle;	
	}
	
	if(!isMouseDown && !prevMouseDown)
	{
           state->leaveAngle += dt*4;
           state->leftAngle += dt*4;
	}
}

uint32_t *render(float dt, int isMouseDown, int prevMouseDown, int mouseX, int mouseY)
{
  if(!init)
  {
    ReadObject();
    init = 1;
  }

  static uint32_t pixels[SCREEN_WIDTH*SCREEN_HEIGHT];
  static float ZBuffer0[SCREEN_WIDTH*SCREEN_HEIGHT];

  static mouseAxisState mouseXstate = {0, 0, 0, 0};
  static mouseAxisState mouseYstate = {0, 0, 0, 0};
  
    window WindowPixels;
    WindowPixels.pixels = pixels;
    WindowPixels.height = SCREEN_HEIGHT;
    WindowPixels.width = SCREEN_WIDTH;

    MouseGrabbingByAxis(&mouseXstate,
			 isMouseDown, prevMouseDown, mouseX, dt);

    MouseGrabbingByAxis(&mouseYstate,
			 isMouseDown, prevMouseDown, mouseY, dt);
    
    paintScreen(WindowPixels.pixels, 0);
    
    for(int i = 0; i < WindowPixels.height*WindowPixels.width; i++)
    {
        ZBuffer0[i] = 100.0f;
    }
    
    UV uv0;
    uv0.u1 = 0.0f;
    uv0.u2 = 0.0f;
    uv0.u3 = 1.0f;
    
    uv0.v1 = 0.0f;
    uv0.v2 = 1.0f;
    uv0.v3 = 0.0f;

    UV uv1;
    uv1.u1 = 1.0f;
    uv1.u2 = 1.0f;
    uv1.u3 = 0.0f;
    
    uv1.v1 = 1.0f;
    uv1.v2 = 0.0f;
    uv1.v3 = 1.0f;

    trianglesettings TriangleSettings;
    TriangleSettings.txt = 0;
    TriangleSettings.shading = 1;
    TriangleSettings.r = 77;
    TriangleSettings.g = 50;
    TriangleSettings.b = 255;

    float LightVector[3] = {1.0f, 1.0f, 0.0f,};
    UnitVector(LightVector);

    for(int i = 0; i < FaceAmount; i++)
    {
        int a = objFaces[i][0];
        int b = objFaces[i][1];
        int c = objFaces[i][2];
        
        vertex3d TriangleA = objVertices[a - 1];
        vertex3d TriangleB = objVertices[b - 1];
        vertex3d TriangleC = objVertices[c - 1];

        RotatePoint(&TriangleA.x, &TriangleA.z, 0.0, 1.3, mouseXstate.leaveAngle);
        RotatePoint(&TriangleB.x, &TriangleB.z, 0.0, 1.3, mouseXstate.leaveAngle);
        RotatePoint(&TriangleC.x, &TriangleC.z, 0.0, 1.3, mouseXstate.leaveAngle);
	
        RotatePoint(&TriangleA.y, &TriangleA.z, 0.0, 1.3, mouseYstate.leaveAngle);
        RotatePoint(&TriangleB.y, &TriangleB.z, 0.0, 1.3, mouseYstate.leaveAngle);
        RotatePoint(&TriangleC.y, &TriangleC.z, 0.0, 1.3, mouseYstate.leaveAngle);

        float TriVec1[3] = 
        {
            TriangleB.x - TriangleA.x,
            TriangleB.y - TriangleA.y,
            TriangleB.z - TriangleA.z,
        };

        float TriVec2[3] = 
        {
            TriangleC.x - TriangleA.x,
            TriangleC.y - TriangleA.y,
            TriangleC.z - TriangleA.z,
        };

        float TriangleNormal[3];
        crossProduct(TriVec1, TriVec2, TriangleNormal);
        float SightVect[3] = 
        {
            TriangleA.x,
            TriangleA.y,
            TriangleA.z,
        };

        UnitVector(TriangleNormal);

        if(dotProduct(TriangleNormal, SightVect, 3) < 0)
        {
            float dim = dotProduct(TriangleNormal, LightVector, 3) + 1;
            InterpolatedTriangleWUV3d(WindowPixels.pixels, ZBuffer0,
                TriangleA, TriangleB, TriangleC,
                uv1, 0, dim, TriangleSettings);
	}
	else
	{
            InterpolatedTriangleWUV3d(WindowPixels.pixels, ZBuffer0,
                TriangleA, TriangleB, TriangleC,
                uv1, 0, 1, TriangleSettings);
	}
    }
    
    return WindowPixels.pixels;
}
