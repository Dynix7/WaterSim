#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include <stdio.h>
#define GLSL_VERSION 430

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

// Struct for Wave Properties
struct ShaderProperties {
    // Vertex Shader
    int numWaves;

    float startAmp;
    float startFreq;
    float startSpeed;

    float ampMult;
    float freqMult;
    float speedMult;
    float warpStrength;

    //Fragment Shader
    Vector4 lightColor;
    float ambient;
    float specFactor;
    float specMult;

    Vector3 viewPos;
    Vector3 lightPos;
    // Shader Locations
    int locations[14];
};

typedef enum {
    numWavesLoc = 0, // 0

    startAmpLoc,
    startFreqLoc,
    startSpeedLoc,

    ampMultLoc,
    freqMultLoc,
    speedMultLoc,
    warpStrengthLoc,

    lightColorLoc,
    ambientLoc,
    specFactorLoc,
    specMultLoc,

    viewPosLoc,
    lightPosLoc
} ShaderLocations;

//Camera Setup
Camera camera = {
    .position = (Vector3) {-40.0, 15.0, 0.0},
    .target = (Vector3) {0.0, 0.0, 0.0},
    .up = (Vector3) {0.0, 1.0, 0.0}, //X, Y, Z with Y up
    .fovy = 50.0,
    .projection = CAMERA_PERSPECTIVE   
};

//Positions
Vector3 planeCenter = {0.0, 0.0, 0.0};
Vector3 lightCenter = {270.0, 70.0, -15.0};
Vector3 origin = {0.0, 0.0, 0.0};

struct ShaderProperties wave = {
    // Vertex Shader
    .numWaves = 24,
    
    .startAmp = 1.3,
    .startFreq = 0.3,
    .startSpeed = 4.5,

    .ampMult = 0.78,
    .freqMult = 1.2,
    .speedMult = 1.02,
    .warpStrength = 2.2,

    // Fragment Shader
    .lightColor = (Vector4) {0.745, 0.918, 1.0, 1.0}, // Pretty Close to White
    .ambient = 0.55,
    .specFactor = 128.0,
    .specMult = 3.5,
    
    .viewPos = camera.position,
    .lightPos = lightCenter,
    .locations = {0}
};


// Other Globals
float time = 0.0;

void getLocations(Shader waterShader, struct ShaderProperties *wave);
void updateWaveProperties(Shader waterShader, struct ShaderProperties *wave);

int main() {
    // Setup Window
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Chill Water fr");
    SetTargetFPS(240);
    DisableCursor();

    // Shader Setup
    Shader waterShader = LoadShader("src/water.vs", "src/water.fs");
    Shader skyboxShader = LoadShader("src/sky.vs", "src/sky.fs");

    int timeLocation = GetShaderLocation(waterShader, "time");
    getLocations(waterShader, &wave);

    int cubemapType = MATERIAL_MAP_CUBEMAP;
    int environmentMapLoc = GetShaderLocation(skyboxShader, "environmentMap");

    //Skybox Model
    Mesh cube = GenMeshCube(1.0, 1.0, 1.0);
    Model skybox = LoadModelFromMesh(cube);
    skybox.materials[0].shader = skyboxShader;

    //Load Skybox
    Image skyboxImage = LoadImage("assets/Cubemap/Cubemap_Sky_06-512x512.png");
    TextureCubemap skyboxCubemap = LoadTextureCubemap(skyboxImage, CUBEMAP_LAYOUT_AUTO_DETECT);
    skybox.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture = skyboxCubemap;
    UnloadImage(skyboxImage);

    // Load Plane and assign shader
    Mesh planeMesh = GenMeshPlane(75, 75, 255, 255);
    Model planeModel = LoadModelFromMesh(planeMesh);
    planeModel.materials[0].shader = waterShader;


    // Tells Shader skybox is a cubemap
    SetShaderValue(skyboxShader, environmentMapLoc, &cubemapType, SHADER_UNIFORM_INT);

    // Main Loop
    while (!WindowShouldClose()) {
        //Things To Update Per loop
        UpdateCamera(&camera, CAMERA_FREE);
        time = (float) GetTime();

        SetShaderValue(waterShader, timeLocation, &time, SHADER_UNIFORM_FLOAT);
        updateWaveProperties(waterShader, &wave);
        
        //Any Rendering Stuff
        BeginDrawing();
            ClearBackground(WHITE);
            
            BeginMode3D(camera);
                //Draws SkyBox
                BeginShaderMode(skyboxShader);
                    rlDisableBackfaceCulling();
                    rlDisableDepthMask();
                    DrawModel(skybox, origin, 50.0, WHITE);
                    rlEnableBackfaceCulling();
                    rlEnableDepthMask();
                EndShaderMode();

                // Draws Light and Water
                DrawSphere(lightCenter, 0.3, WHITE); // just to show location of light

                BeginShaderMode(waterShader);
                    rlDisableBackfaceCulling();
                    DrawModel(planeModel, planeCenter, 1.0, DARKBLUE);     
                    //DrawModelWires(planeModel, planeCenter, 1.0, RAYWHITE);
                    rlEnableBackfaceCulling();
                EndShaderMode();

            EndMode3D();
            DrawFPS(5, 5);

            // char cameraPosText[64] = "";
            // snprintf(cameraPosText, sizeof(cameraPosText), "%.1f, %.1f, %.1f", 
            // camera.position.x, camera.position.y,camera.position.z);
            // DrawText(cameraPosText, 1280/2, 720/2, 20, BLACK);
        EndDrawing();
    }

    //Unload stuf and close window
    UnloadModel(planeModel);
    UnloadModel(skybox);
    UnloadShader(waterShader);
    UnloadShader(skyboxShader);
    CloseWindow();
    return 0;
}

void getLocations(Shader waterShader, struct ShaderProperties *wave) { //probably shouldve used &wave but im C pilled
    wave->locations[numWavesLoc] = GetShaderLocation(waterShader, "numWaves");

    wave->locations[startAmpLoc] = GetShaderLocation(waterShader, "startAmp");
    wave->locations[startFreqLoc] = GetShaderLocation(waterShader, "startFreq");
    wave->locations[startSpeedLoc] = GetShaderLocation(waterShader, "startSpeed");

    wave->locations[ampMultLoc] = GetShaderLocation(waterShader, "ampMult");
    wave->locations[freqMultLoc] = GetShaderLocation(waterShader, "freqMult");
    wave->locations[speedMultLoc] = GetShaderLocation(waterShader, "speedMult");
    wave->locations[warpStrengthLoc] = GetShaderLocation(waterShader, "warpStrength");

    wave->locations[lightColorLoc] = GetShaderLocation(waterShader, "lightColor");
    wave->locations[ambientLoc] = GetShaderLocation(waterShader, "ambient");
    wave->locations[specFactorLoc] = GetShaderLocation(waterShader, "specFactor");
    wave->locations[specMultLoc] = GetShaderLocation(waterShader, "specMult");

    wave->locations[viewPosLoc] = GetShaderLocation(waterShader, "viewPos");
    wave->locations[lightPosLoc] = GetShaderLocation(waterShader, "lightPos");

    waterShader.locs[SHADER_LOC_VECTOR_VIEW] = wave->locations[viewPosLoc];
}

void updateWaveProperties(Shader waterShader, struct ShaderProperties *wave) {
    wave->viewPos = camera.position;
    wave->lightPos = lightCenter;

    SetShaderValue(waterShader, wave->locations[numWavesLoc], &wave->numWaves, SHADER_UNIFORM_INT);
    
    SetShaderValue(waterShader, wave->locations[startAmpLoc], &wave->startAmp, SHADER_UNIFORM_FLOAT);
    SetShaderValue(waterShader, wave->locations[startFreqLoc], &wave->startFreq, SHADER_UNIFORM_FLOAT);
    SetShaderValue(waterShader, wave->locations[startSpeedLoc], &wave->startSpeed, SHADER_UNIFORM_FLOAT);

    SetShaderValue(waterShader, wave->locations[ampMultLoc], &wave->ampMult, SHADER_UNIFORM_FLOAT);
    SetShaderValue(waterShader, wave->locations[freqMultLoc], &wave->freqMult, SHADER_UNIFORM_FLOAT);
    SetShaderValue(waterShader, wave->locations[speedMultLoc], &wave->speedMult, SHADER_UNIFORM_FLOAT);
    SetShaderValue(waterShader, wave->locations[warpStrengthLoc], &wave->warpStrength, SHADER_UNIFORM_FLOAT);

    SetShaderValue(waterShader, wave->locations[lightColorLoc], &wave->lightColor, SHADER_UNIFORM_VEC4);
    SetShaderValue(waterShader, wave->locations[ambientLoc], &wave->ambient, SHADER_UNIFORM_FLOAT);
    SetShaderValue(waterShader, wave->locations[specFactorLoc], &wave->specFactor, SHADER_UNIFORM_FLOAT);
    SetShaderValue(waterShader, wave->locations[specMultLoc], &wave->specMult, SHADER_UNIFORM_FLOAT);

    SetShaderValue(waterShader, wave->locations[viewPosLoc], &wave->viewPos, SHADER_UNIFORM_VEC3);
    SetShaderValue(waterShader, wave->locations[lightPosLoc], &wave->lightPos, SHADER_UNIFORM_VEC3);
}