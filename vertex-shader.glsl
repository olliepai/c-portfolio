attribute vec3 position;

//perspectiveMatrix is for everything we draw, it sets the perspectiveMatrix
uniform mat4 perspectiveMatrix;
// modelMatrix: modifies the position, scale, rotation for each model
uniform mat4 modelMatrix;
// viewMatrix: changes position of camera, but models will look like they stay in the same position
uniform mat4 viewMatrix;

void main() {
  gl_Position = perspectiveMatrix * viewMatrix * modelMatrix * vec4(position, 1);
}
