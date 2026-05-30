#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#define GLSL_VERSION 430

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

//Camera Setup
Camera camera = {
    .position = (Vector3) {30.0, 15.0, -30.0},
    .target = (Vector3) {0.0, 0.0, 0.0},
    .up = (Vector3) {0.0, 1.0, 0.0}, //X, Y, Z with Y up
    .fovy = 60.0,
    .projection = CAMERA_PERSPECTIVE   
};

//Positions
Vector3 planeCenter = {0.0, -10.0, 0.0};
Vector3 lightCenter = {-15.0, 25.0, -45.0};

// Other Globals
float time = 0.0;

int main() {
    // Setup Window
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Chill Water fr");
    SetTargetFPS(240);
    DisableCursor();

    // Shader Setup
    Shader waterShader = LoadShader("src/vert.vs", "src/frag.fs");


    int timeLocation = GetShaderLocation(waterShader, "time");
    waterShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(waterShader, "viewPos");
    int lightLocation = GetShaderLocation(waterShader, "lightPos");

    // Load Plane and assign shader
    Mesh planeMesh = GenMeshPlane(50, 50, 255, 255);
    Model planeModel = LoadModelFromMesh(planeMesh);
    planeModel.materials[0].shader = waterShader;

    // Main Loop
    while (!WindowShouldClose()) {
        //Things To Update Per loop
        UpdateCamera(&camera, CAMERA_FREE);
        time = (float) GetTime();

        SetShaderValue(waterShader, timeLocation, &time, SHADER_UNIFORM_FLOAT);
        SetShaderValue(waterShader, lightLocation, &lightCenter, SHADER_UNIFORM_VEC3);
        SetShaderValue(waterShader, waterShader.locs[SHADER_LOC_VECTOR_VIEW], &camera.position, SHADER_UNIFORM_VEC3);
        
        //Any Rendering Stuff
        BeginDrawing();
            ClearBackground(SKYBLUE);
            
            BeginMode3D(camera);
                DrawSphere(lightCenter, 2.5, YELLOW); // just to show location of light

                BeginShaderMode(waterShader);
                    rlDisableBackfaceCulling();
                    DrawModel(planeModel, planeCenter, 1.0, DARKBLUE);     
                    DrawModelWires(planeModel, planeCenter, 1.0, RAYWHITE);
                    rlEnableBackfaceCulling();
                EndShaderMode();

            EndMode3D();
            DrawFPS(5, 5);
        EndDrawing();
    }

    //Unload stuf and close window
    UnloadModel(planeModel);
    UnloadShader(waterShader);
    CloseWindow();
    return 0;
}

