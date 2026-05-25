#version 430

//In and Out Data
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec4 vertexColor;
in vec3 vertexNormal; 

//Data sent to fragment shader
out vec2 fragTexCoord;
out vec4 fragColor;
out vec3 outNormal; // For Light Calculations in Fragment Shader

// Data thats from cpu
uniform mat4 mvp;
uniform float time;

// Constants
#define E 2.718281828459
#define PI 3.141592653589

// Wave Properties
const int NUM_WAVES = 24;
float AMP = 1.3;
float FREQ = 10;
float SPEED = 0.15;

//Adjustments Per Loop
const float AMPMult = 0.8;
const float FREQMult = 1.2;
const float SPEEDMult = 1.07;

//Declartions
float calcRotate(vec2 UV, float angle);
float innerWave(vec2 UV, float freq, float speed, float time, float angle);


void main() {
    vec3 finalPos = vertexPosition;
    vec2 UV = vertexTexCoord;

    float currentAngle = 0.0;

    //Calculation Of Wave 
    float waveSum = 0.0;
    float currentWave = 0.0;

    float innerPart = 0.0; //freq(X + time*speed)
    float sinePart = 0.0; //a * sin(freq(X + time*speed))
    float sharedDevPart = 0.0;

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
        // for X: e^((a*sin(b((cos(theta)*x+sin(theta)*y)+t))-1) * a*cos(b((cos(theta)*x+sin(theta)*y)+t)) * b*cos(theta)
        // for Y: e^((a*sin(b((cos(theta)*x+sin(theta)*y)+t))-1) * a*cos(b((cos(theta)*x+sin(theta)*y)+t)) * b*sin(theta)
        sharedDevPart = currentWave * (AMP * cos(innerPart)) * FREQ;
        ddx += sharedDevPart * cos(currentAngle);
        ddy += sharedDevPart * sin(currentAngle);
        
        // Adjusts Angle and Makes Waves Smaller
        FREQ *= FREQMult;
        AMP *= AMPMult;
        SPEED * SPEEDMult;
        currentAngle = float(i) * 0.5;

        waveSum += currentWave;
   }
    finalPos.y += waveSum;

    //Calculates the normal (basically cross product) then normalizes it
    vec3 calcNormal = vec3(-ddx, -ddy, 1);
    outNormal = normalize(calcNormal);

    // Finalize data stuff 
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;
    gl_Position = mvp * vec4(finalPos, 1.0) ;
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
