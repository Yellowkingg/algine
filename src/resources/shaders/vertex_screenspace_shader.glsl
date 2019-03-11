/*
 * @author Congard
 * dbcongard@gmail.com
 * t.me/congard
 * gitlab.com/congard
 */

#version 330

in vec3 inPos;
in vec2 inTexCoord;
out vec2 texCoord;

void main() {
	texCoord = inTexCoord;
	gl_Position = vec4(inPos, 1.0);
}