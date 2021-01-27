#version 120

// Color texture, which is processed firstly!
uniform sampler2D colorTexture;

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
varying vec2 vertexWorldXZCoordinate;


void main() {


  // Assignment Constants below
  // get the texture color
  vec4 textureColor = texture2D(colorTexture, vertexWorldXZCoordinate);

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
  float cosT = 1.; // dot(normalVector, ToLightVector) > 0.0f ? dot(normalVector, ToLightVector): 0.0f;
  vec3 diffuse = (Id * kd).xyz * cosT;
  // compute specular component
  vec3 reflectedDirection = vec3(1., 1., 1.); //reflect(-ToLightVector, normalVector);
  float spec = 0.0; // pow(max(dot(ToCameraVector, reflectedDirection), 0.0), specExp);
  vec3 specular = spec * (Is * ks).xyz;

  // compute the color using the following equation
  gl_FragColor = vec4(clamp( textureColor.xyz * vec3(ambient + diffuse + specular), 0.0, 1.0), 1.0);
  //gl_FragColor = vec4(heightFactor, 0., 0., 1.0);
}
