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
int NUM_WAVES = 20;
float AMP = 1.2;
float FREQ = 10;
float SPEED = 0.15;

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

    // Partial Derivatives for Wave
    float ddx = 0.0; // Binormal i think
    float ddy = 0.0; // Tangent I think

    //Full Function is e^(a * sin(freq(X + time*speed)))
    //Calculation is split to prevent recalculating when doing the deritvatives
    for (int i = 1; i <= NUM_WAVES; i++) {
        innerPart = innerWave(UV, FREQ, SPEED, time, currentAngle);
        sinePart = AMP * sin(innerPart);
        currentWave = exp(sinePart - 1);  //Full Wave Function

        //Calculating the partial derivatives
        //lowk just in desmos guessing until graphs match
        //Current thingy: e^{\left(a\left(\sin\left(p\left(\cos x+\sin x+t\right)\right)\right)-1\right)}\cdot ap\cos\left(p\left(\cos x+\sin x+t\right)\right)\cdot\left(\cos x-\sin x\right)

        // Adjusts Angle and Makes Waves Smaller
        FREQ *= 1.2;
        AMP *= 0.8;
        currentAngle = float(i) * 0.5;

        waveSum += currentWave;
   }

    finalPos += waveSum;

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

float innerWave(vec2 UV, float freq, float speed, float time, float angle) {
    //Full Function is e^(a * sin(freq(X + time*speed)))
    // This calculates the freq(X + time*speed) Part
    float X = calcRotate(UV, angle);

    float sineResult = freq * (X + (time * speed));
    return sineResult;
}
