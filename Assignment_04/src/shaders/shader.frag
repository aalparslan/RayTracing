#version 120

// Output Color
//out vec4 color;

// Texture-related data;
uniform sampler2D rgbTexture;

// Data from Vertex Shader
varying vec2 textureCoordinate;
varying vec3 vertexNormal; // For Lighting computation
varying vec3 ToLightVector; // Vector from Vertex to Light;
varying vec3 ToCameraVector; // Vector from Vertex to Camera;
void main() {
    
    // Assignment Constants below
    // get the texture color
   // vec4 textureColor = texture(rgbTexture, textureCoordinate);
    vec4 textureColor = texture2D(rgbTexture, textureCoordinate);
    
    // apply Phong shading by using the following parameters
    vec4 ka = vec4(0.25,0.25,0.25,1.0); // reflectance coeff. for ambient
    vec4 Ia = vec4(0.3,0.3,0.3,1.0); // light color for ambient
    vec4 Id = vec4(1.0, 1.0, 1.0, 1.0); // light color for diffuse
    vec4 kd = vec4(1.0, 1.0, 1.0, 1.0); // reflectance coeff. for diffuse
    vec4 Is = vec4(1.0, 1.0, 1.0, 1.0); // light color for specular
    vec4 ks = vec4(1.0, 1.0, 1.0, 1.0); // reflectance coeff. for specular
    int specExp = 100; // specular exponent
    
    // compute ambient component
    vec3 ambient = (ka * Ia).xyz;
    // compute diffuse component
    float cosT = dot(vertexNormal, ToLightVector) > 0.0f ? dot(vertexNormal, ToLightVector): 0.0f;
    vec3 diffuse = (Id * kd).xyz * cosT;
    // compute specular component
    vec3 reflectedDirection = reflect(-ToLightVector, vertexNormal);
    float spec = pow(max(dot(ToCameraVector, reflectedDirection), 0.0), specExp);
    vec3 specular = spec * (Is * ks).xyz;
    
    // compute the color using the following equation
     vec4 color = vec4(clamp( textureColor.xyz * vec3(ambient + diffuse + specular), 0.0, 1.0), 1.0);
    gl_FragColor = color;
}
