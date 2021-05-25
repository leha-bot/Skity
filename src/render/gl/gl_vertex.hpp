#ifndef SKITY_SRC_RENDER_GL_GL_VERTEX_HPP
#define SKITY_SRC_RENDER_GL_GL_VERTEX_HPP

#include <array>
#include <vector>

namespace skity {
class GLVertex {
  enum {
    GL_VERTEX_X = 0,
    GL_VERTEX_Y = 1,
    GL_VERTEX_TYPE = 2,
    GL_VERTEX_U = 3,
    GL_VERTEX_V = 4,
    GL_VERTEX_SIZE = 5,
    GL_VERTEX_UV_OFFSET = GL_VERTEX_TYPE * sizeof(float),
    GL_VERTEX_STRIDE = GL_VERTEX_SIZE * sizeof(float),
  };

 public:
  enum {
    GL_VERTEX_TYPE_NORMAL = 0,
    GL_VERTEX_TYPE_QUAD = 1,
  };
  using VertexData = std::array<float, GL_VERTEX_SIZE>;
  uint32_t AddPoint(float x, float y, uint32_t type, float u, float v);
  uint32_t AddPoint(VertexData const& data);
  void AddFront(uint32_t v1, uint32_t v2, uint32_t v3);
  void AddBack(uint32_t v1, uint32_t v2, uint32_t v3);
  VertexData GetVertex(uint32_t index);

  uint32_t FrontCount() const { return front_index.size(); }
  uint32_t BackCount() const { return back_index.size(); }

 private:
  std::vector<float> vertex_buffer;
  std::vector<uint32_t> front_index;
  std::vector<uint32_t> back_index;
};

}  // namespace skity

#endif  // SKITY_SRC_RENDER_GL_GL_VERTEX_HPP