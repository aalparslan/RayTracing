#version 120


//layout(location = 0) in vec3 position;
//layout(location = 2) in vec3 normal;
//layout(location = 1) in vec2 texCoord;
// Data from CPU



uniform mat4 MVP; // ModelViewProjection Matrix
uniform vec3 cameraPosition;
uniform vec3 lightPosition;
uniform float heightFactor;

// Texture-related data
uniform sampler2D heightMapTexture;
uniform int widthTexture;
uniform int heightTexture;
uniform int textureOffset;

// Output to Fragment Shader
varying vec2 textureCoordinate; // For texture-color
varying vec3 vertexNormal; // For Lighting computation
varying vec3 ToLightVector; // Vector from Vertex to Light;
varying vec3 ToCameraVector; // Vector from Vertex to Camera;

//float findHeight(in vec2 uv ){
//    return  heightFactor * texture(rgbTexture, uv).r;
//
//}

float findHeight(in vec2 uv ){
    return  heightFactor * texture2D(heightMapTexture,uv).r;
    
}


vec3 findNormal(in vec3 M, in float tOffset){
    
    
    vec3 a = vec3(M.x   , 0, M.z+1);
    vec3 b = vec3(M.x+1 , 0, M.z+1);
    vec3 c = vec3(M.x+1 , 0, M.z);
    vec3 d = vec3(M.x   , 0, M.z-1);
    vec3 e = vec3(M.x-1 , 0, M.z-1);
    vec3 f = vec3(M.x-1 , 0, M.z);
    
    a = vec3(a.x, findHeight(vec2(1.0+tOffset-a.x/widthTexture, 1.0-a.z/heightTexture)), a.z);
    b = vec3(b.x, findHeight(vec2(1.0+tOffset-b.x/widthTexture, 1.0-b.z/heightTexture)), b.z);
    c = vec3(c.x, findHeight(vec2(1.0+tOffset-c.x/widthTexture, 1.0-c.z/heightTexture)), c.z);
    d = vec3(d.x, findHeight(vec2(1.0+tOffset-d.x/widthTexture, 1.0-d.z/heightTexture)), d.z);
    e = vec3(e.x, findHeight(vec2(1.0+tOffset-e.x/widthTexture, 1.0-e.z/heightTexture)), e.z);
    f = vec3(f.x, findHeight(vec2(1.0+tOffset-f.x/widthTexture, 1.0-f.z/heightTexture)), f.z);
    
    vec3 A = -normalize(cross(M - a, M - f));
    vec3 B = -normalize(cross(M - b, M - a));
    vec3 C = -normalize(cross(M - c, M - b));
    vec3 D = -normalize(cross(M - d, M - c));
    vec3 E = -normalize(cross(M - e, M - d));
    vec3 F = -normalize(cross(M - f, M - e));
    
    // return vec3(0, 1, 0);
    return normalize(A + B + C + D + E + F);
}

void main()
{
    
    vec4 position = gl_Vertex;
    vec3 normal =  gl_NormalMatrix * gl_Normal;// check this later
    float tOffset = textureOffset * ( 1.0f / widthTexture);
    
    // get texture value, compute height
    textureCoordinate.x = gl_MultiTexCoord0.x;
    textureCoordinate.y = gl_MultiTexCoord0.y;
    textureCoordinate.x += tOffset;
    // compute normal vector using also the heights of neighbor vertices
    vec3 newPosition = vec3(position.x, findHeight(textureCoordinate), position.z);
    vertexNormal = findNormal(newPosition, tOffset);
    // compute toLight vector vertex coordinate in VCS
    ToCameraVector = normalize(cameraPosition - newPosition);
    ToLightVector = normalize(lightPosition - newPosition);
    
    // set gl_Position variable correctly to give the transformed vertex position
    gl_Position = MVP* vec4(newPosition.xyz, 1);
    
}
