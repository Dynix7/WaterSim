#version 430

// Inputs and Outputs
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragPosition;
in vec3 fragNormal;

out vec4 finalColor;

uniform vec3 viewPos;
uniform vec3 lightPos;
uniform sampler2D texture0;
uniform vec4 colDiffuse;

vec4 lightColor = vec4(vec3(121.0, 217.0, 248.0)/255.0, 1.0);
const float ambient = 0.75;
const float specFactor = 128.0;
const float specMult = 4.5;

void main() {
    //vec4 texColor = texture(texture0, fragTexCoord); I currently don't have a texture
    vec3 normal = normalize(fragNormal);
    vec3 viewDir = normalize(viewPos - fragPosition);
    vec3 lightDir = normalize(lightPos - fragPosition);

    // Diffuse Factor Calculation
    float NdotL = dot(normal, lightDir); // 0 to 1
    float diffuseFactor = max(NdotL, 0.0);
    //diffuseFactor = pow(diffuseFactor, 1.3);


    // Fresnel Calculation wikipedia.org/wiki/Schlick's_approximation
    float R0 = 0.020332; // From Refractive Indices of Air and Water. 1.0 vs 1.333
    float normalDotView = max(dot(normal, viewDir), 0.0);
    float fresnel = R0 + (1.0 - R0) * pow((1.0 - normalDotView), 5); 
    // pow((1.0 - normalDotView), 5) can also be used by itself since like the other terms r basically just 1 


    //Specular Reflection Calculation
    float specular = 0.0;
    if (NdotL > 0.0) {
        vec3 halfVector = normalize(viewDir + lightDir);
        specular = max(dot(normal, halfVector), 0.0);
        specular = pow(specular, specFactor) * specMult * fresnel;
    }


    // Combining Everything
    vec3 baseColor = colDiffuse.rgb * fragColor.rgb;

    vec3 diffuse = diffuseFactor * baseColor * lightColor.rgb;
    vec3 ambientColor = baseColor * ambient;
    
    vec3 result = clamp(diffuse + ambientColor + specular, 0.0, 1.0);
    vec4 scaledResult = vec4(result, 1.0);

    //finalColor = vec4(normal * 0.5 + 0.5, 1.0);// For testing normals
    //finalColor = colDiffuse;
    finalColor = scaledResult;
}