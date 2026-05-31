#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include <stdio.h>
#define GLSL_VERSION 430

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

struct waveProperties {
    int NUM_WAVES;

    float startAmp;
    float startFreq;
    float startSpeed;

    float ampMult;
    float freqMult;
    float speedMult;
    float warpStrength;
};

struct waveProperties wave {
    .NUM_WAVES = 24,
    
    .startAmp = 1.1,
    .startFreq = 0.3,
    .startSpeed = 4.5,

    .ampMult = 0.78,
    .freqMult = 1.2,
    .speedMult = 1.02,
    .warpStrength = 2.1
};

int locations[9] = {0};

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
Vector3 lightCenter = {90.0, 45.0, -5.0};
Vector3 origin = {0.0, 0.0, 0.0};

// Other Globals
float time = 0.0;

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
    int lightLocation = GetShaderLocation(waterShader, "lightPos");
    int numWavesLocation = GetShaderLocation(waterShader, "NUM_WAVES");
    waterShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(waterShader, "viewPos");

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
        SetShaderValue(waterShader, lightLocation, &lightCenter, SHADER_UNIFORM_VEC3);
        SetShaderValue(waterShader, numWavesLocation, &wave.NUM_WAVES, SHADER_UNIFORM_INT);
        SetShaderValue(waterShader, waterShader.locs[SHADER_LOC_VECTOR_VIEW], &camera.position, SHADER_UNIFORM_VEC3);
        
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
