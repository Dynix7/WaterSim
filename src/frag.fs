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
float ambient = 0.75;
void main() {
    //vec4 texColor = texture(texture0, fragTexCoord); I currently don't have a texture

    vec3 lightDir = normalize(lightPos - fragPosition);
    float diffuseFactor = dot(fragNormal, lightDir); // 0 to 1
    diffuseFactor = clamp(diffuseFactor, 0.0, 1.0);

    vec3 baseColor = colDiffuse.rgb * fragColor.rgb;

    vec3 diffuse = diffuseFactor * baseColor * lightColor.rgb;
    vec3 ambientColor = baseColor * ambient;
    
    vec3 result = clamp(diffuse + ambientColor, 0.0, 1.0);
    vec4 scaledResult = vec4(result, 1.0);

    //finalColor = vec4(fragNormal * 0.5 + 0.5, 1.0);// For testing normals
    //finalColor = colDiffuse;
    finalColor = scaledResult;
}