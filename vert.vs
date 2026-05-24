#version 330

//In and Out Data
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec4 vertexColor;
in vec3 vertexNormal; 

out vec2 fragTexCoord;
out vec4 fragColor;
out vec3 calcNormal; // For Light Calculations in Fragment Shader

// Data tjats from cpu
uniform mat4 mvp;
uniform float time;

// Constants
#define E 2.718281828459
#define PI 3.141592653589

// Wave Properties
int NUM_WAVES = 16;
float AMP = 1;
float FREQ = 10;
float SPEED = 0.25;

//Declartions
float calcRotate(vec2 UV, float angle);
float wave(vec2 UV, float amp, float freq, float speed, float time, float angle);


void main() {
    vec3 finalPos = vertexPosition;
    vec2 UV = vertexTexCoord;

    float currentAngle = 0.0;

    //Calculation Of Wave 
    float waveSum = 0.0;
    float currentWave = 0.0;

    // Partial Derivatives for Wave
    float ddx = 0.0;
    float ddy = 0.0;
   for (int i = 1; i <= NUM_WAVES; i++) {
        currentWave = wave(UV, AMP, FREQ, SPEED, time, currentAngle);

   }


    //Calculation of Derivative and Normal



    // Finalize data stuff 
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;
    calcNormal = vertexNormal; // Will Change calculated normal later once I figure out the math
    gl_Position = mvp * vec4(finalPos, 1.0) ;
}

float calcRotate(vec2 UV, float angle) {
    float final = UV.x * cos(angle) + UV.y * sin(angle);
    return final;
}

float wave(vec2 UV, float amp, float freq, float speed, float time, float angle) {
    //Function is e^(a * sin(freq(X + time*speed)))
    float X = calcRotate(UV, angle);

    float sineResult = amp * sin(freq * (X + (time * speed)));
    float wave = exp(sineResult); //e^sineResult
    return wave;
}
