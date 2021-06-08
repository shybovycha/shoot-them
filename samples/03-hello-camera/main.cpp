#include <iostream>
#include <string>
#include <sstream>
#include <memory>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <glbinding/gl/gl.h>
#include <glbinding/Version.h>
#include <glbinding-aux/ContextInfo.h>
#include <glbinding-aux/types_to_string.h>

#include <globjects/globjects.h>
#include <globjects/base/File.h>
#include <globjects/logging.h>

#include <globjects/Error.h>
#include <globjects/Buffer.h>
#include <globjects/Program.h>
#include <globjects/ProgramPipeline.h>
#include <globjects/Shader.h>
#include <globjects/VertexArray.h>
#include <globjects/VertexAttributeBinding.h>
#include <globjects/base/StaticStringSource.h>

#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>

int main() {
  sf::Context context;

  sf::ContextSettings settings;
  settings.depthBits = 24;
  settings.stencilBits = 8;
  settings.antialiasingLevel = 4;
  settings.majorVersion = 3;
  settings.minorVersion = 2;
  settings.attributeFlags = sf::ContextSettings::Attribute::Core;

  sf::Window window(sf::VideoMode(800, 600), "Hello OpenGL!", sf::Style::Default, settings);

  globjects::init([&](const char* name) {
      return context.getFunction(name);
  });

  globjects::DebugMessage::enable();// enable automatic messages if KHR_debug is available

  globjects::DebugMessage::setCallback([](const globjects::DebugMessage& message) {
      std::cout << "[DEBUG] " << message.message() << std::endl;
  });

  std::cout << "[INFO] Initializing..." << std::endl;

  std::cout << "[INFO] Creating shaders..." << std::endl;

  std::cout << "[INFO] Compiling vertex shader...";

  auto vertexProgram = globjects::Program::create();
  auto vertexShaderSource = globjects::Shader::sourceFromFile("media/vertex.glsl");
  auto vertexShaderTemplate = globjects::Shader::applyGlobalReplacements(vertexShaderSource.get());
  auto vertexShader = globjects::Shader::create(static_cast<gl::GLenum>(GL_VERTEX_SHADER), vertexShaderTemplate.get());

  std::cout << "done" << std::endl;

  std::cout << "[INFO] Compiling fragment shader...";

  auto fragmentProgram = globjects::Program::create();
  auto fragmentShaderSource = globjects::Shader::sourceFromFile("media/fragment.glsl");
  auto fragmentShaderTemplate = globjects::Shader::applyGlobalReplacements(fragmentShaderSource.get());
  auto fragmentShader = globjects::Shader::create(static_cast<gl::GLenum>(GL_FRAGMENT_SHADER), fragmentShaderTemplate.get());

  std::cout << "done" << std::endl;

  std::cout << "[INFO] Linking shader programs...";

  vertexProgram->attach(vertexShader.get());
  fragmentProgram->attach(fragmentShader.get());

  std::cout << "done" << std::endl;

  std::cout << "[INFO] Creating rendering pipeline...";

  auto programPipeline = globjects::ProgramPipeline::create();

  programPipeline->useStages(vertexProgram.get(), gl::GL_VERTEX_SHADER_BIT);
  programPipeline->useStages(fragmentProgram.get(), gl::GL_FRAGMENT_SHADER_BIT);

  std::cout << "done" << std::endl;

  std::cout << "[INFO] Creating VAO...";

  auto planeBuffer = globjects::Buffer::create();

  planeBuffer->setData(
    std::array<glm::vec3, 4> {
      { glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(1, 1, 0) }
    },
    static_cast<gl::GLenum>(GL_STATIC_DRAW)
  );

  auto vao = globjects::VertexArray::create();

  vao->binding(0)->setAttribute(0);
  vao->binding(0)->setBuffer(planeBuffer.get(), 0, sizeof(glm::vec3));
  vao->binding(0)->setFormat(3, static_cast<gl::GLenum>(GL_FLOAT));
  vao->enable(0);

  std::cout << "done" << std::endl;

  std::cout << "[INFO] Done initializing" << std::endl;

  const float fov = 45.0f;

  const float cameraMoveSpeed = 1.0f;
  const float cameraRotateSpeed = 100.0f;

  glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
  glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 cameraLeft = glm::vec3(1.0f, 0.0f, 0.0f);

  sf::Clock clock;

  glm::vec2 previousMousePos = glm::vec2(sf::Mouse::getPosition().x, sf::Mouse::getPosition().y);

  while (window.isOpen()) {
    sf::Event event;

    // measure time since last frame, in seconds
    float deltaTime = static_cast<float>(clock.restart().asSeconds());

    std::ostringstream title;

    title << "Hello camera [frame render time, sec: " << deltaTime << "]";

    window.setTitle(title.str());

    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
        break;
      }

      if (event.type == sf::Event::MouseMoved) {
          glm::vec2 currentMousePos = glm::vec2(sf::Mouse::getPosition().x, sf::Mouse::getPosition().y);
          glm::vec2 mouseDelta = currentMousePos - previousMousePos;

          float horizontalAngle = (mouseDelta.x / window.getSize().x) * -1 * deltaTime * cameraRotateSpeed * fov;
          float verticalAngle = (mouseDelta.y / window.getSize().y) * -1 * deltaTime * cameraRotateSpeed * fov;

          cameraUp = glm::rotate(cameraUp, verticalAngle, cameraLeft);
          cameraLeft = glm::rotate(cameraLeft, horizontalAngle, glm::vec3(0, 1, 0));

          previousMousePos = currentMousePos;
      }
    }

    glm::vec3 cameraFront = glm::normalize(glm::cross(cameraUp, cameraLeft));

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
      cameraPos += cameraFront * cameraMoveSpeed * deltaTime;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
      cameraPos -= cameraFront * cameraMoveSpeed * deltaTime;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
      cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraMoveSpeed * deltaTime;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
      cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraMoveSpeed * deltaTime;
    }

    glm::mat4 projection = glm::perspective(glm::radians(fov), (float) window.getSize().x / (float) window.getSize().y, 0.1f, 100.0f);

    glm::mat4 view = glm::lookAt(
            cameraPos,
            cameraPos + cameraFront,
            cameraUp);

    glm::mat4 model = glm::mat4(1.0f); // identity

    vertexProgram->setUniform("model", model);
    vertexProgram->setUniform("view", view);
    vertexProgram->setUniform("projection", projection);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, window.getSize().x, window.getSize().y);

    programPipeline->use();

    vao->drawArrays(static_cast<gl::GLenum>(GL_TRIANGLE_STRIP), 0, 4);

    window.display();
  }

  return 0;
}
