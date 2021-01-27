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
uniform sampler2D rgbTexture;
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
    return  heightFactor * texture2D(rgbTexture,uv).r;
    
}


vec3 findNormal(in vec3 position, in float tOffset){
    
    float topTriangleHeight = findHeight(vec2(1.0 + tOffset - ((position.x)/widthTexture),1.0 - ((position.z -1.0)/ heightTexture)));
    float leftTriangleHeight = findHeight( vec2(1.0 + tOffset - ((position.x -1.0)/  widthTexture), 1.0 - ((position.z)/ heightTexture)));
    float rightTriangleHeight = findHeight(vec2( 1.0 + tOffset  - ((position.x +1.0)/widthTexture), 1.0 - ((position.z)/heightTexture)));
    float bottomLeftTriangleHeight = findHeight(vec2(1.0 + tOffset - ((position.x -1.0)/widthTexture), 1.0 - ((position.z +1.0)/ heightTexture)));
    float bottomTriangleHeight = findHeight(vec2(1.0 + tOffset - ((position.x )/ widthTexture), 1.0 - ((position.z +1.0)/ heightTexture)));
    float topRightTriangleHeight = findHeight(vec2( 1.0 + tOffset - ((position.x +1.0)/widthTexture), 1.0 - ((position.z -1.0)/ heightTexture)));
    
    
    vec3 bottomLeftTrianglePosition = vec3(position.x-1, rightTriangleHeight, position.z+1);
    vec3 leftTrianglePosition = vec3(position.x -1, leftTriangleHeight, position.z);
    vec3 rightTrianglePosition = vec3(position.x +1, rightTriangleHeight, position.z);
    vec3 topTrianglePosition = vec3(position.x, rightTriangleHeight, position.z-1);
    vec3 bottomTrianglePosition = vec3(position.x, rightTriangleHeight, position.z+1);
    vec3 topRightTrianglePosition = vec3(position.x +1, rightTriangleHeight, position.z-1);
    
    
    vec3 toTopRightVector = topRightTrianglePosition - position;
    vec3 toBottomVector = bottomTrianglePosition - position;
    vec3 toBottomLeftVector = bottomLeftTrianglePosition - position;
    vec3 toTopVector = topTrianglePosition - position;
    vec3 toRightVector = rightTrianglePosition - position;
    vec3 toLeftVector = leftTrianglePosition - position;
    
    
    vec3 NormalDueToTopRightAndTopVectors = normalize(cross(toTopRightVector,toTopVector));
    vec3 NormalDueToBottomAndRightVectors = normalize(cross(toBottomVector, toRightVector ));
    vec3 NormalDueToLeftAndBottomLeftVectors = normalize(cross(toLeftVector, toBottomLeftVector));
    vec3 NormalDueToRightAndTopRightVectors = normalize(cross(toRightVector, toTopRightVector));
    vec3 NormalDueToBottomLeftAndBottomVectors = normalize(cross(toBottomLeftVector, toBottomVector));
    vec3 NormalDueToTopAndLeftVectors = normalize(cross(toTopVector, toLeftVector));
    
    
    vec3 finalNormal = NormalDueToRightAndTopRightVectors
    + NormalDueToTopRightAndTopVectors
    + NormalDueToTopAndLeftVectors
    + NormalDueToLeftAndBottomLeftVectors
    + NormalDueToBottomLeftAndBottomVectors
    + NormalDueToBottomAndRightVectors;
    
    return normalize(finalNormal) ;
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
