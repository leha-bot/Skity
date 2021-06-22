#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <skity/geometry/point.hpp>
#include <skity/graphic/paint.hpp>
#include <skity/graphic/path.hpp>
#include <vector>

#include "common/test_common.hpp"
#include "src/render/gl/gl_mesh.hpp"
#include "src/render/gl/gl_path_visitor.hpp"
#include "src/render/gl/gl_vertex.hpp"

class GLPathMeshDemo : public test::TestApp {
 public:
  GLPathMeshDemo() : TestApp() {}
  ~GLPathMeshDemo() override = default;

 protected:
  void OnInit() override {
    mvp_ = glm::ortho<float>(0, 800, 600, 0, -100, 100);
    InitGL();
  }
  void OnDraw() override {
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_STENCIL_TEST);

    glUseProgram(stencil_program_);
    glUniformMatrix4fv(stencil_program_mvp_location_, 1, GL_FALSE, &mvp_[0][0]);

    mesh_.BindMesh();
    glColorMask(0, 0, 0, 0);
    glStencilMask(0x0F);

    glStencilFunc(GL_ALWAYS, 0x01, 0x0F);

    glStencilOp(GL_KEEP, GL_KEEP, GL_INCR_WRAP);
    mesh_.BindFrontIndex();
    DrawFront();
    glStencilOp(GL_KEEP, GL_KEEP, GL_DECR_WRAP);
    mesh_.BindBackIndex();
    DrawBack();

    glColorMask(1, 1, 1, 1);
    glStencilFunc(GL_NOTEQUAL, 0x00, 0x0F);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    mesh_.BindFrontIndex();
    DrawFront();
    mesh_.BindBackIndex();
    DrawBack();

    glDisable(GL_STENCIL_TEST);

    glUseProgram(mesh_program_);
    glUniformMatrix4fv(mesh_program_mvp_location_, 1, GL_FALSE, &mvp_[0][0]);

    mesh_.BindFrontIndex();
    DrawFront(GL_LINE_LOOP);
    mesh_.BindBackIndex();
    DrawBack(GL_LINE_LOOP);

    mesh_.UnBindMesh();
  }

  void DrawFront(GLenum mode = GL_TRIANGLES) {
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void*)(2 * sizeof(float)));

    glDrawElements(mode, front_count_, GL_UNSIGNED_INT, 0);
  }

  void DrawBack(GLenum mode = GL_TRIANGLES) {
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void*)(2 * sizeof(float)));

    glDrawElements(mode, back_count_, GL_UNSIGNED_INT, 0);
  }

  void InitGL() {
    glClearColor(0.3f, 0.4f, 0.5f, 1.0f);
    glClearStencil(0x00);

    InitShader();
    InitMesh();
  }

  void InitShader() {
    const char* stencil_vs_code = R"(
      #version 330 core
      layout(location = 0) in vec2 aPos;
      layout(location = 1) in vec3 aUV;
      uniform mat4 mvp;

      out vec3 TexCoord;

      void main() {
        gl_Position = mvp * vec4(aPos, 0.0, 1.0);
        TexCoord = aUV;
      }
    )";

    const char* stencil_fs_code = R"(
      #version 330 core

      in vec3 TexCoord;

      out vec4 FragColor;

      void main() {
        if (TexCoord.x == 1.0 && (TexCoord.y * TexCoord.y - TexCoord.z > 0)) {
          discard;
        }
        FragColor = vec4(1.0, 0.0, 0.0, 1.0);
      }
    )";

    stencil_program_ =
        test::create_shader_program(stencil_vs_code, stencil_fs_code);

    stencil_program_mvp_location_ =
        glGetUniformLocation(stencil_program_, "mvp");

    const char* mesh_vs_code = R"(
      #version 330 core
      layout(location = 0) in vec2 aPos;

      uniform mat4 mvp;
      void main() {
        gl_Position = mvp * vec4(aPos, 0.0, 1.0);
      }
    )";

    const char* mesh_fs_code = R"(
      #version 330 core

      out vec4 FragColor;

      void main() {
        FragColor = vec4(0.0, 1.0, 0.0, 1.0);
      } 
    )";

    mesh_program_ = test::create_shader_program(mesh_vs_code, mesh_fs_code);
    mesh_program_mvp_location_ = glGetUniformLocation(mesh_program_, "mvp");
  }

  void InitMesh() {
    skity::Paint paint;
    paint.setStyle(skity::Paint::kStroke_Style);
    paint.setStrokeWidth(5.f);
    paint.setStrokeCap(skity::Paint::kRound_Cap);
    paint.setStrokeJoin(skity::Paint::kRound_Join);

    skity::Path path;
    path.moveTo(10, 10);
    path.quadTo(256, 64, 128, 128);
    path.quadTo(10, 192, 250, 250);
    path.close();

    skity::Path path2;

    path2.moveTo(400, 10);
    path2.lineTo(500, 10);
    path2.lineTo(400, 110);
    path2.lineTo(500, 110);
    path2.close();

    skity::Path path3;
    path3.moveTo(20, 170);
    path3.conicTo(80, 170, 80, 230, 0.707f);
    path3.conicTo(80, 170, 20, 170, 0.25f);
    path3.close();

    skity::GLVertex gl_vertex;
    skity::GLPathVisitor::VisitPath(path, &gl_vertex);
    skity::GLPathVisitor::VisitPath(path2, &gl_vertex);
    skity::GLPathVisitor::VisitPath(path3, &gl_vertex);

    mesh_.Init();
    mesh_.BindMesh();
    mesh_.UploadVertexBuffer(gl_vertex.GetVertexData(),
                             gl_vertex.GetVertexDataSize());
    mesh_.UploadFrontIndex(gl_vertex.GetFrontIndexData(),
                           gl_vertex.GetFrontIndexDataSize());
    mesh_.UploadBackIndex(gl_vertex.GetBackIndexData(),
                          gl_vertex.GetBackIndexDataSize());
    mesh_.UnBindMesh();

    front_count_ = gl_vertex.FrontCount();
    back_count_ = gl_vertex.BackCount();
  }

 private:
  skity::GLMesh mesh_;
  glm::mat4 mvp_ = {};
  GLuint stencil_program_ = 0;
  GLint stencil_program_mvp_location_ = -1;

  GLuint mesh_program_ = 0;
  GLint mesh_program_mvp_location_ = -1;
  uint32_t front_count_ = 0;
  uint32_t back_count_ = 0;
};

int main(int argc, const char** argv) {
  GLPathMeshDemo demo;
  demo.Start();
  return 0;
}