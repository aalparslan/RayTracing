#version 120

// Gray scale height texture data
uniform sampler2D heightTexture;
// How long are the world shapes
uniform float heightFactor;


// Texture dimensions
uniform float textureWidth;
uniform float textureHeight;

// Normal vector
varying vec3 normalVector;

// Vertex coordinate
varying vec2 vertexCanvasCoordinate;

float findHeight(in vec2 uv){
    return  heightFactor * texture2D(heightTexture, uv).r;

}

vec3 normal_calculation(){
  float topTriangleHeight        = findHeight(vec2(1.0 - ((gl_Vertex.x)/textureWidth),1.0 - ((gl_Vertex.z -1.0)/ textureHeight)));
  float leftTriangleHeight       = findHeight( vec2(1.0 - ((gl_Vertex.x -1.0)/  textureWidth), 1.0 - ((gl_Vertex.z)/ textureHeight)));
  float rightTriangleHeight      = findHeight(vec2( 1.0 - ((gl_Vertex.x +1.0)/textureWidth), 1.0 - ((gl_Vertex.z)/textureHeight)));
  float bottomLeftTriangleHeight = findHeight(vec2(1.0 - ((gl_Vertex.x -1.0)/textureWidth), 1.0 - ((gl_Vertex.z +1.0)/ textureHeight)));
  float bottomTriangleHeight     = findHeight(vec2(1.0 - ((gl_Vertex.x )/ textureWidth), 1.0 - ((gl_Vertex.z +1.0)/ textureHeight)));
  float topRightTriangleHeight   = findHeight(vec2( 1.0 - ((gl_Vertex.x +1.0)/textureWidth), 1.0 - ((gl_Vertex.z -1.0)/ textureHeight)));


  vec3 only_vertex3D = vec3(gl_Vertex.x, gl_Vertex.y, gl_Vertex.z);
  vec3 bottomLeftTrianglePosition = vec3(only_vertex3D.x-1, rightTriangleHeight, only_vertex3D.z+1);
  vec3 leftTrianglePosition       = vec3(only_vertex3D.x -1, leftTriangleHeight, only_vertex3D.z);
  vec3 rightTrianglePosition      = vec3(only_vertex3D.x +1, rightTriangleHeight, only_vertex3D.z);
  vec3 topTrianglePosition        = vec3(only_vertex3D.x, rightTriangleHeight, only_vertex3D.z-1);
  vec3 bottomTrianglePosition     = vec3(only_vertex3D.x, rightTriangleHeight, only_vertex3D.z+1);
  vec3 topRightTrianglePosition   = vec3(only_vertex3D.x +1, rightTriangleHeight, only_vertex3D.z-1);


  vec3 toTopRightVector   = topRightTrianglePosition   - only_vertex3D;
  vec3 toBottomVector     = bottomTrianglePosition     - only_vertex3D;
  vec3 toBottomLeftVector = bottomLeftTrianglePosition - only_vertex3D;
  vec3 toTopVector        = topTrianglePosition        - only_vertex3D;
  vec3 toRightVector      = rightTrianglePosition      - only_vertex3D;
  vec3 toLeftVector       = leftTrianglePosition       - only_vertex3D;


  vec3 NormalDueToTopRightAndTopVectors      = cross(toTopRightVector   , toTopVector);
  vec3 NormalDueToBottomAndRightVectors      = cross(toBottomVector     , toRightVector );
  vec3 NormalDueToLeftAndBottomLeftVectors   = cross(toLeftVector       , toBottomLeftVector);
  vec3 NormalDueToRightAndTopRightVectors    = cross(toRightVector      , toTopRightVector);
  vec3 NormalDueToBottomLeftAndBottomVectors = cross(toBottomLeftVector , toBottomVector);
  vec3 NormalDueToTopAndLeftVectors          = cross(toTopVector        , toLeftVector);


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
  // Calculate vertex coordinate to use in fragment shader
  vertexCanvasCoordinate = vec2(gl_Vertex.x/textureWidth, gl_Vertex.z/textureHeight);

  // Calculate the required normals!
  normalVector = normal_calculation();

  // MVP transform...
  gl_Position = ftransform();
}
