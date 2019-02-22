attribute highp vec4 posAttr;
attribute lowp  vec4 colAttr;
attribute highp vec3 norAttr;
varying lowp  vec4 col;
varying highp vec3 nor;
uniform highp mat4 projMatrix;
uniform highp mat4 mvMatrix;
uniform highp mat3 norMatrix;
attribute mediump vec4 texAttr;
varying mediump vec4 texc;

void main() {
   texc = texAttr;
   col = colAttr;
   gl_Position = projMatrix * mvMatrix * posAttr;
   nor = norMatrix * norAttr;
}

