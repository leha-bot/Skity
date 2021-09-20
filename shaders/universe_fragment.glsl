#version 330 core

// these macros are same as GLVertex2::VertexType
#define VERTEX_TYPE_STROKE_AA 100.0
#define VERTEX_TYPE_NONE 0.0
#define VERTEX_TYPE_LINE_EDGE 1.0
#define VERTEX_TYPE_LINE_CAP 2.0
#define VERTEX_TYPE_LINE_BEVEL_JOIN 3.0
#define VERTEX_TYPE_LINE_ROUND_JOIN 4.0
#define VERTEX_TYPE_QUAD_IN 5.0
#define VERTEX_TYPE_QUAD_OUT 6.0
#define VERTEX_TYPE_LINE_ROUND 7.0

// these macros are same as GLShader::UniversealShader::Type
#define USER_FRAGMENT_TYPE_STENCIL 0

uniform vec4 UserColor;
uniform ivec4 UserData1;
uniform vec4 UserData2;

// [x, y]
in vec2 vPos;
// [mix, u, v]
in vec3 vPosInfo;

// final fragment color
out vec4 FragColor;

// Determin UserInput color
// this can be:
//  1. pure color
//  2. gradient color
//  3. texture color
vec4 CalculateUserColor() {
  // TODO implement other type color
  return vec4(UserColor.xyz * UserColor.w, UserColor.w);
}

// line edge aa alpha
float CalculateLineEdgeAlpha(float y) {
  float feathe = y;
  if (abs(feathe) < 0.9) {
    return 1.0;
  }

  float alpha = 1.0 - abs(feathe);
  alpha /= 0.1;
  return alpha;
}
// line cap alpha
float CalculateLineCapAlpha() {
  float alpha = vPosInfo.y;
  return alpha * CalculateLineEdgeAlpha(vPosInfo.z);
}

float CalculateRoundCapAlpha() {
  vec2 center = vPosInfo.yz;
  float strokeRadius = UserData2.x * 0.5;
  float dist = length(vPos - center);

  return CalculateLineEdgeAlpha(dist / strokeRadius);
}

// Determin fragment alpha
// this may generate alpha gradient if needs anti-alias
float CalculateFragmentAlpha(float posType) {
  if (posType == VERTEX_TYPE_LINE_EDGE) {
    return CalculateLineEdgeAlpha(vPosInfo.y);
  }
  if (posType == VERTEX_TYPE_LINE_CAP) {
    return CalculateLineCapAlpha();
  }
  if (posType == VERTEX_TYPE_LINE_ROUND) {
    return CalculateRoundCapAlpha();
  }
  return 1.0;
}

bool NeedDiscard(float posType) {
  if (posType == VERTEX_TYPE_LINE_ROUND) {
    vec2 center = vPosInfo.yz;
    float strokeRadius = UserData2.x * 0.5;
    if (length(vPos - center) > strokeRadius) {
      discard;
      return true;
    }
  }

  return false;
}

void main() {
  // stencil, no need to calculate color
  if (UserData1.x == USER_FRAGMENT_TYPE_STENCIL) {
    FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    return;
  }

  bool needAA = false;
  float posType = vPosInfo.x;
  if (posType > VERTEX_TYPE_STROKE_AA) {
    posType -= VERTEX_TYPE_STROKE_AA;
    needAA = true;
  } else if (posType != 0.0) {
    needAA = true;
  }

  if (NeedDiscard(posType)) {
    return;
  }

  vec4 finalColor = CalculateUserColor();
  float finalAlpha = 1.0;
  if (needAA) {
    finalAlpha *= CalculateFragmentAlpha(posType);
  }

  FragColor = finalColor * finalAlpha;
}