#version 430

//In and Out Data
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec4 vertexColor;
in vec3 vertexNormal; 

//Data sent to fragment shader
out vec2 fragTexCoord;
out vec4 fragColor;
out vec3 fragPosition;
out vec3 fragNormal; // For Light Calculations in Fragment Shader

// Data thats from cpu
uniform mat4 mvp; // Model->World, World->View, View->Projection
uniform mat4 matModel; //Model->World Used For Lighting
uniform mat4 matNormal; //Local Normal to World Normal

uniform float time;

// Constants
#define E 2.718281828459
#define PI 3.141592653589

// Wave Properties
const int NUM_WAVES = 24;
float AMP = 1.0;
float FREQ = 0.3;
float SPEED = 4.5;

//Adjustments Per Loop
const float AMPMult = 0.78;
const float FREQMult = 1.2;
const float SPEEDMult = 1.015;

//Declartions
float calcRotate(vec2 UV, float angle);
float innerWave(vec2 UV, float freq, float speed, float time, float angle);


void main() {
    vec3 finalPos = vertexPosition;
    vec2 UV = vertexPosition.xz;

    float currentAngle = 0.0;

    //Calculation Of Wave 
    float waveSum = 0.0;
    float currentWave = 0.0;

    float innerPart = 0.0; //freq(X + time*speed)
    float sinePart = 0.0; //a * sin(freq(X + time*speed))
    float sharedDevPart = 0.0; //e^((a*sin(b((cos(theta)*x+sin(theta)*y)+t))-1) * a*cos(b((cos(theta)*x+sin(theta)*y)+t)) * b

    // Partial Derivatives for Wave
    float ddx = 0.0; // Binormal i think
    float ddy = 0.0; // Tangent I think

    //Full Function is e^((a*sin(b((cos(theta)*x+sin(theta)*y)+t))-1)
    // Original function from GPU gems is the Gernster Wave but like idk its complicated
    //Calculation is split to prevent recalculating when doing the deritvatives
    for (int i = 1; i <= NUM_WAVES; i++) {
        innerPart = innerWave(UV, FREQ, SPEED, time, currentAngle);
        sinePart = AMP * sin(innerPart);
        currentWave = exp(sinePart - 1);  //Full Wave Function

        //Calculating the partial derivatives
        // for X: e^((a*sin(b((cos(theta)*x+sin(theta)*y)+t))-1) * a*cos(b((cos(theta)*x+sin(theta)*y)+t)) * b * cos(theta)
        // for Y: e^((a*sin(b((cos(theta)*x+sin(theta)*y)+t))-1) * a*cos(b((cos(theta)*x+sin(theta)*y)+t)) * b * sin(theta)
        sharedDevPart = currentWave * (AMP * cos(innerPart)) * FREQ;

        // The Normals get really noisy when you add up the really small waves
        // So idk how to fix it so I'm just going to add a limit here for now
        if (i <= NUM_WAVES/2) { 
            ddx += sharedDevPart * cos(currentAngle);
            ddy += sharedDevPart * sin(currentAngle);
        }
        // Adjusts Angle and Makes Waves Smaller
        FREQ *= FREQMult;
        AMP *= AMPMult;
        SPEED *= SPEEDMult;
        currentAngle = float(i) * 0.5;

        waveSum += currentWave;
   }

    finalPos.y += waveSum;

    //Calculates the normal (basically cross product) then normalizes it
    vec3 calcNormal = vec3(-ddx, 1.0, -ddy);
    calcNormal = vec3(matNormal * vec4(calcNormal, 0.0)); // Turns Normal into World Space
    fragNormal = normalize(calcNormal);

    // Finalize data stuff 
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;
    fragPosition = vec3(matModel * vec4(finalPos, 1.0));

    gl_Position = mvp * vec4(finalPos, 1.0);
}

float calcRotate(vec2 UV, float angle) {
    float final = UV.x * cos(angle) + UV.y * sin(angle);
    return final;
}

float innerWave(vec2 UV, float freq, float speed, float time, float angle) {
    // This calculates the freq(X + time*speed) Part
    float X = calcRotate(UV, angle);

    float sineResult = freq * (X + (time * speed));
    return sineResult;
}
