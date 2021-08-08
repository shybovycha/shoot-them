#include <filesystem>
#include <iostream>
#include <sstream>

#include <glbinding/gl/gl.h>

#include <globjects/Buffer.h>
#include <globjects/Error.h>
#include <globjects/Framebuffer.h>
#include <globjects/Program.h>
#include <globjects/ProgramPipeline.h>
#include <globjects/Renderbuffer.h>
#include <globjects/Shader.h>
#include <globjects/Texture.h>
#include <globjects/Uniform.h>
#include <globjects/VertexArray.h>
#include <globjects/VertexAttributeBinding.h>
#include <globjects/base/File.h>
#include <globjects/base/StaticStringSource.h>
#include <globjects/globjects.h>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>

#ifdef WIN32
using namespace gl;
#endif

class AbstractDrawable
{
public:
    virtual void draw() = 0;

    virtual void drawInstanced(unsigned int instances) = 0;

    virtual void bind() = 0;

    virtual void unbind() = 0;
};

class AbstractMeshBuilder;

class AbstractMesh : public AbstractDrawable
{
    friend class AbstractMeshBuilder;

public:
    static std::shared_ptr<AbstractMeshBuilder> builder()
    {
        return std::make_shared<AbstractMeshBuilder>();
    }

    AbstractMesh(
        std::vector<glm::vec3> vertices,
        std::vector<glm::vec3> normals,
        std::vector<glm::vec2> uvs,
        std::vector<unsigned int> indices,
        std::vector<std::unique_ptr<globjects::Texture>> textures,
        std::unique_ptr<globjects::VertexArray> vao,
        std::unique_ptr<globjects::Buffer> vertexBuffer,
        std::unique_ptr<globjects::Buffer> indexBuffer,
        std::unique_ptr<globjects::Buffer> normalBuffer,
        std::unique_ptr<globjects::Buffer> uvBuffer
    ) :

        m_vertices(std::move(vertices)),
        m_indices(std::move(indices)),
        m_uvs(std::move(uvs)),
        m_normals(std::move(normals)),
        m_textures(std::move(textures)),
        m_vao(std::move(vao)),
        m_vertexBuffer(std::move(vertexBuffer)),
        m_indexBuffer(std::move(indexBuffer)),
        m_normalBuffer(std::move(normalBuffer)),
        m_uvBuffer(std::move(uvBuffer)),
        m_transformation(1.0f)
    {
    }

    void setTransformation(glm::mat4 transformation)
    {
        m_transformation = transformation;
    }

    glm::mat4 getTransformation() const
    {
        return m_transformation;
    }

    void draw() override
    {
        // number of values passed = number of elements * number of vertices per element
        // in this case: 2 triangles, 3 vertex indexes per triangle
        m_vao->drawElements(
            static_cast<gl::GLenum>(GL_TRIANGLES),
            m_indices.size(),
            static_cast<gl::GLenum>(GL_UNSIGNED_INT),
            nullptr);
    }

    void drawInstanced(unsigned int instances) override
    {
        m_vao->drawElementsInstanced(
            static_cast<gl::GLenum>(GL_TRIANGLES),
            m_indices.size(),
            static_cast<gl::GLenum>(GL_UNSIGNED_INT),
            nullptr,
            instances);
    }

    void bind() override
    {
        m_vao->bind();

        for (auto& texture : m_textures)
        {
            texture->bindActive(1);
        }
    }

    void unbind() override
    {
        for (auto& texture : m_textures)
        {
            texture->unbindActive(1);
        }

        m_vao->unbind();
    }

protected:
    std::unique_ptr<globjects::VertexArray> m_vao;

    std::unique_ptr<globjects::Buffer> m_vertexBuffer;
    std::unique_ptr<globjects::Buffer> m_indexBuffer;
    std::unique_ptr<globjects::Buffer> m_normalBuffer;
    std::unique_ptr<globjects::Buffer> m_uvBuffer;

    std::vector<std::unique_ptr<globjects::Texture>> m_textures;

    std::vector<unsigned int> m_indices;
    std::vector<glm::vec3> m_vertices;
    std::vector<glm::vec3> m_normals;
    std::vector<glm::vec2> m_uvs;

    glm::mat4 m_transformation;
};

class AbstractMeshBuilder
{
public:
    AbstractMeshBuilder() : m_positionAttributeIndex(0), m_normalAttributeIndex(1), m_uvAttributeIndex(2)
    {
    }

    AbstractMeshBuilder* addVertices(std::vector<glm::vec3> vertices)
    {
        m_vertices.insert(m_vertices.end(), std::make_move_iterator(vertices.begin()), std::make_move_iterator(vertices.end()));

        return this;
    }

    AbstractMeshBuilder* addIndices(std::vector<unsigned int> indices)
    {
        m_indices.insert(m_indices.end(), std::make_move_iterator(indices.begin()), std::make_move_iterator(indices.end()));

        return this;
    }

    AbstractMeshBuilder* addNormals(std::vector<glm::vec3> normals)
    {
        m_normals.insert(m_normals.end(), std::make_move_iterator(normals.begin()), std::make_move_iterator(normals.end()));

        return this;
    }

    AbstractMeshBuilder* addUVs(std::vector<glm::vec2> uvs)
    {
        m_uvs.insert(m_uvs.end(), std::make_move_iterator(uvs.begin()), std::make_move_iterator(uvs.end()));

        return this;
    }

    AbstractMeshBuilder* addTexture(std::unique_ptr<globjects::Texture> texture)
    {
        m_textures.push_back(std::move(texture));

        return this;
    }

    AbstractMeshBuilder* addTextures(std::vector<std::unique_ptr<globjects::Texture>> textures)
    {
        m_textures.insert(m_textures.end(), std::make_move_iterator(textures.begin()), std::make_move_iterator(textures.end()));

        return this;
    }

    AbstractMeshBuilder* setPositionAttributerIndex(unsigned int positionAttributeIndex)
    {
        m_positionAttributeIndex = positionAttributeIndex;

        return this;
    }

    AbstractMeshBuilder* setNormalAttributerIndex(unsigned int normalAttributeIndex)
    {
        m_normalAttributeIndex = normalAttributeIndex;

        return this;
    }

    AbstractMeshBuilder* setUVAttributerIndex(unsigned int uvAttributeIndex)
    {
        m_uvAttributeIndex = uvAttributeIndex;

        return this;
    }

    std::unique_ptr<AbstractMesh> build()
    {
        m_vertexBuffer = std::make_unique<globjects::Buffer>();

        m_vertexBuffer->setData(m_vertices, static_cast<gl::GLenum>(GL_STATIC_DRAW));

        m_indexBuffer = std::make_unique<globjects::Buffer>();

        m_indexBuffer->setData(m_indices, static_cast<gl::GLenum>(GL_STATIC_DRAW));

        m_vao = std::make_unique<globjects::VertexArray>();

        m_vao->bindElementBuffer(m_indexBuffer.get());

        m_vao->binding(0)->setAttribute(0);
        m_vao->binding(0)->setBuffer(m_vertexBuffer.get(), 0, sizeof(glm::vec3)); // number of elements in buffer, stride, size of buffer element
        m_vao->binding(0)->setFormat(3, static_cast<gl::GLenum>(GL_FLOAT)); // number of data elements per buffer element (vertex), type of data
        m_vao->enable(0);

        m_normalBuffer = std::make_unique<globjects::Buffer>();

        if (!m_normals.empty())
        {
            m_normalBuffer->setData(m_normals, static_cast<gl::GLenum>(GL_STATIC_DRAW));

            m_vao->binding(m_normalAttributeIndex)->setAttribute(m_normalAttributeIndex);
            m_vao->binding(m_normalAttributeIndex)->setBuffer(m_normalBuffer.get(), 0, sizeof(glm::vec3)); // number of elements in buffer, stride, size of buffer element
            m_vao->binding(m_normalAttributeIndex)->setFormat(3, static_cast<gl::GLenum>(GL_FLOAT)); // number of data elements per buffer element (vertex), type of data
            m_vao->enable(m_normalAttributeIndex);

            // TODO: enable the corresponding shader sections (those which require normal data)
        }

        m_uvBuffer = std::make_unique<globjects::Buffer>();

        if (!m_uvs.empty())
        {
            m_uvBuffer->setData(m_uvs, static_cast<gl::GLenum>(GL_STATIC_DRAW));

            m_vao->binding(m_uvAttributeIndex)->setAttribute(m_uvAttributeIndex);
            m_vao->binding(m_uvAttributeIndex)->setBuffer(m_uvBuffer.get(), 0, sizeof(glm::vec2)); // number of elements in buffer, stride, size of buffer element
            m_vao->binding(m_uvAttributeIndex)->setFormat(2, static_cast<gl::GLenum>(GL_FLOAT)); // number of data elements per buffer element (vertex), type of data
            m_vao->enable(m_uvAttributeIndex);

            // TODO: enable the corresponding shader sections (those which require UV data)
        }

        return std::make_unique<AbstractMesh>(
            m_vertices,
            m_normals,
            m_uvs,
            m_indices,
            std::move(m_textures),
            std::move(m_vao),
            std::move(m_vertexBuffer),
            std::move(m_indexBuffer),
            std::move(m_normalBuffer),
            std::move(m_uvBuffer));
    }

private:
    std::vector<glm::vec3> m_vertices;
    std::vector<glm::vec3> m_normals;
    std::vector<glm::vec2> m_uvs;
    std::vector<unsigned int> m_indices;
    std::vector<std::unique_ptr<globjects::Texture>> m_textures;

    std::unique_ptr<globjects::VertexArray> m_vao;
    std::unique_ptr<globjects::Buffer> m_vertexBuffer;
    std::unique_ptr<globjects::Buffer> m_indexBuffer;

    std::unique_ptr<globjects::Buffer> m_normalBuffer;
    std::unique_ptr<globjects::Buffer> m_uvBuffer;

    unsigned int m_positionAttributeIndex;
    unsigned int m_normalAttributeIndex;
    unsigned int m_uvAttributeIndex;
};

class SingleMeshModel : public AbstractDrawable
{
public:
    SingleMeshModel(std::unique_ptr<AbstractMesh> mesh) : m_mesh(std::move(mesh)), m_transformation(1.0f)
    {
    }

    void draw() override
    {
        m_mesh->draw();
    }

    void drawInstanced(unsigned int instances) override
    {
        m_mesh->drawInstanced(instances);
    }

    void bind() override
    {
        m_mesh->bind();
    }

    void unbind() override
    {
        m_mesh->unbind();
    }

    void setTransformation(glm::mat4 transformation)
    {
        m_transformation = transformation;
    }

    glm::mat4 getTransformation() const
    {
        return m_transformation;
    }

protected:
    std::unique_ptr<AbstractMesh> m_mesh;
    glm::mat4 m_transformation;
};

class MultiMeshModel : public AbstractDrawable
{
public:
    MultiMeshModel(std::vector<std::unique_ptr<AbstractMesh>> meshes) : m_meshes(std::move(meshes)), m_transformation(1.0f)
    {
    }

    void draw() override
    {
        for (auto& mesh : m_meshes)
        {
            mesh->draw();
        }
    }

    void drawInstanced(unsigned int instances) override
    {
        for (auto& mesh : m_meshes)
        {
            mesh->drawInstanced(instances);
        }
    }

    void bind() override
    {
        for (auto& mesh : m_meshes)
        {
            mesh->bind();
        }
    }

    void unbind() override
    {
        for (auto& mesh : m_meshes)
        {
            mesh->unbind();
        }
    }

    void setTransformation(glm::mat4 transformation)
    {
        // TODO: propagate onto meshes?
        m_transformation = transformation;
    }

    glm::mat4 getTransformation() const
    {
        return m_transformation;
    }

protected:
    std::vector<std::unique_ptr<AbstractMesh>> m_meshes;
    glm::mat4 m_transformation;
};

class AssimpModel : public MultiMeshModel
{
public:
    AssimpModel(std::vector<std::unique_ptr<AbstractMesh>> meshes) : MultiMeshModel(std::move(meshes))
    {
    }

    static std::unique_ptr<AssimpModel> fromAiNode(const aiScene* scene, aiNode* node, std::vector<std::filesystem::path> materialLookupPaths = {})
    {
        std::vector<std::unique_ptr<AbstractMesh>> meshes;

        processAiNode(scene, node, materialLookupPaths, meshes);

        return std::make_unique<AssimpModel>(std::move(meshes));
    }

protected:
    static void processAiNode(const aiScene* scene, aiNode* node, std::vector<std::filesystem::path> materialLookupPaths, std::vector<std::unique_ptr<AbstractMesh>>& meshes)
    {
        for (auto t = 0; t < node->mNumMeshes; ++t)
        {
            auto mesh = fromAiMesh(scene, scene->mMeshes[node->mMeshes[t]], materialLookupPaths);
            meshes.push_back(std::move(mesh));
        }

        for (auto i = 0; i < node->mNumChildren; ++i)
        {
            auto child = node->mChildren[i];
            // auto childTransformation = parentTransformation + assimpMatrixToGlm(child->mTransformation);

            processAiNode(scene, child, materialLookupPaths, meshes);
        }
    }

    static std::unique_ptr<AbstractMesh> fromAiMesh(const aiScene* scene, aiMesh* mesh, std::vector<std::filesystem::path> materialLookupPaths = {})
    {
        std::cout << "[INFO] Creating buffer objects...";

        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> uvs;

        std::vector<GLuint> indices;

        auto builder = AbstractMesh::builder();

        for (auto i = 0; i < mesh->mNumVertices; ++i)
        {
            glm::vec3 position(
                mesh->mVertices[i].x,
                mesh->mVertices[i].y,
                mesh->mVertices[i].z);

            vertices.push_back(position);

            if (mesh->HasNormals())
            {
                glm::vec3 normal(
                    mesh->mNormals[i].x,
                    mesh->mNormals[i].y,
                    mesh->mNormals[i].z);

                normals.push_back(normal);
            }

            if (mesh->HasTextureCoords(0))
            {
                glm::vec3 uv(
                    mesh->mTextureCoords[0][i].x,
                    mesh->mTextureCoords[0][i].y,
                    mesh->mTextureCoords[0][i].z);

                uvs.push_back(uv);
            }
        }

        for (auto i = 0; i < mesh->mNumFaces; ++i)
        {
            auto face = mesh->mFaces[i];

            for (auto t = 0; t < face.mNumIndices; ++t)
            {
                indices.push_back(face.mIndices[t]);
            }
        }

        std::cout << "done" << std::endl;

        std::cout << "[INFO] Loading textures...";

        std::vector<std::unique_ptr<globjects::Texture>> textures;

        if (mesh->mMaterialIndex >= 0)
        {
            auto material = scene->mMaterials[mesh->mMaterialIndex];

            for (auto i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE); ++i)
            {
                aiString str;
                material->GetTexture(aiTextureType_DIFFUSE, i, &str);

                std::string imagePath{ str.C_Str() };

                // TODO: extract the "std::string resolveFile(std::string)" helper
                /* std::vector<std::filesystem::path> lookupPaths = {
                    imagePath,
                    std::filesystem::path{ "../" + imagePath }
                };*/

                for (auto path : materialLookupPaths) {
                    std::cout << "[INFO] Looking up the DIFFUSE texture in " << path << "...";

                    const auto filePath = std::filesystem::path(path).append(imagePath);

                    if (std::filesystem::exists(filePath)) {
                        imagePath = filePath.string();
                        break;
                    }
                }

                std::cout << "[INFO] Loading DIFFUSE texture " << imagePath << "...";

                sf::Image textureImage;

                if (!textureImage.loadFromFile(imagePath))
                {
                    std::cerr << "[ERROR] Can not load texture" << std::endl;
                    continue;
                }

                textureImage.flipVertically();

                auto texture = std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_2D));

                texture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<GLint>(GL_LINEAR));
                texture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<GLint>(GL_LINEAR));

                texture->image2D(
                    0,
                    static_cast<gl::GLenum>(GL_RGBA8),
                    glm::vec2(textureImage.getSize().x, textureImage.getSize().y),
                    0,
                    static_cast<gl::GLenum>(GL_RGBA),
                    static_cast<gl::GLenum>(GL_UNSIGNED_BYTE),
                    reinterpret_cast<const gl::GLvoid*>(textureImage.getPixelsPtr()));

                textures.push_back(std::move(texture));
            }

            // TODO: also handle aiTextureType_DIFFUSE and aiTextureType_SPECULAR
        }

        std::cout << "done" << std::endl;

        return AbstractMesh::builder()
            ->addVertices(vertices)
            ->addIndices(indices)
            ->addNormals(normals)
            ->addUVs(uvs)
            ->addTextures(std::move(textures))
            ->build();
    }
};

class Terrain : public SingleMeshModel
{
public:
    Terrain(std::unique_ptr<AbstractMesh> mesh) : SingleMeshModel(std::move(mesh))
    {
    }

    static std::unique_ptr<Terrain> fromHeightmap(sf::Image heightmap, float step = 0.5f)
    {
        const auto width = heightmap.getSize().x;
        const auto height = heightmap.getSize().y;

        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> uvs;
        std::vector<unsigned int> indices;

        vertices.reserve(width * height);
        normals.reserve(width * height);
        uvs.reserve(width * height);
        indices.reserve(6 * (width - 1) * (height - 1));

        for (auto i = 0; i < height; ++i)
        {
            for (auto t = 0; t < width; ++t)
            {
                const auto color = heightmap.getPixel(t, i);

                float x = t * step;
                float y = color.r / 255.0f; // heightmaps are greyscale so all the components (r, g & b) of each pixel will have the same value
                float z = i * step;

                glm::vec3 position(x, y, z);
                glm::vec2 uv(i / static_cast<float>(width - 1), t / static_cast<float>(height - 1));

                vertices.push_back(position);
                uvs.push_back(uv);
            }
        }

        for (auto i = 0; i < height - 1; ++i)
        {
            for (auto t = 0; t < width - 1; ++t)
            {
                /*
                * [(i+1)*size + t] [(i+1)*size+t+1]
                * [i*size + t]     [i*size + t + 1]
                *
                * x.x
                * |\.
                * x-x
                */
                indices.push_back(((i + 1) * width) + t);
                indices.push_back((i * width) + t + 1);
                indices.push_back((i * width) + t);

                /*
                *
                * x-x
                * .\|
                * x.x
                */
                indices.push_back(((i + 1) * width) + t);
                indices.push_back(((i + 1) * width) + t + 1);
                indices.push_back((i * width) + t + 1);

                glm::vec3 v0 = vertices[i * width + t];
                glm::vec3 v1 = vertices[(i + 1) * width + t];
                glm::vec3 v2 = vertices[i * width + t + 1];

                glm::vec3 normal = glm::cross(v1 - v0, v2 - v0);
                normals.push_back(normal);
            }

            // last vertex of the row
            {
                glm::vec3 v0 = vertices[i * width + width - 1];
                glm::vec3 v1 = vertices[(i + 1) * width + width - 1];
                glm::vec3 v2 = vertices[i * width + width - 1 + 1];

                glm::vec3 normal = glm::cross(v2 - v0, v1 - v0);
                normals.push_back(normal);
            }
        }

        // last vertex of the last row and last column
        {
            glm::vec3 v0 = vertices[height * width - 1];
            glm::vec3 v1 = vertices[(height - 1) * width - 1];
            glm::vec3 v2 = vertices[height * width - 2];

            glm::vec3 normal = glm::cross(v2 - v0, v1 - v0);
            normals.push_back(normal);
        }

        auto mesh = AbstractMesh::builder()
            ->addVertices(vertices)
            ->addIndices(indices)
            ->addNormals(normals)
            ->addUVs(uvs)
            ->build();

        return std::make_unique<Terrain>(std::move(mesh));
    }
};

int main()
{
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 3;
    settings.minorVersion = 2;
    settings.attributeFlags = sf::ContextSettings::Attribute::Core;

#ifdef SYSTEM_DARWIN
    auto videoMode = sf::VideoMode(2048, 1536);
#else
    auto videoMode = sf::VideoMode(1024, 768);
#endif

    sf::Window window(videoMode, "Hello, Anti-aliasing!", sf::Style::Default, settings);

    globjects::init([](const char* name) {
        return sf::Context::getFunction(name);
    });

    globjects::DebugMessage::enable(); // enable automatic messages if KHR_debug is available

    globjects::DebugMessage::setCallback([](const globjects::DebugMessage& message) {
        std::cout << "[DEBUG] " << message.message() << std::endl;
    });

    std::cout << "[INFO] Initializing..." << std::endl;

    std::cout << "[INFO] Creating shaders..." << std::endl;

    std::cout << "[INFO] Compiling shadow mapping vertex shader...";

    auto shadowMappingVertexSource = globjects::Shader::sourceFromFile("media/shadow-mapping.vert");
    auto shadowMappingVertexShaderTemplate = globjects::Shader::applyGlobalReplacements(shadowMappingVertexSource.get());
    auto shadowMappingVertexShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_VERTEX_SHADER), shadowMappingVertexShaderTemplate.get());

    if (!shadowMappingVertexShader->compile())
    {
        std::cerr << "[ERROR] Can not compile vertex shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling shadow mapping fragment shader...";

    auto shadowMappingFragmentSource = globjects::Shader::sourceFromFile("media/shadow-mapping.frag");
    auto shadowMappingFragmentShaderTemplate = globjects::Shader::applyGlobalReplacements(shadowMappingFragmentSource.get());
    auto shadowMappingFragmentShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_FRAGMENT_SHADER), shadowMappingFragmentShaderTemplate.get());

    if (!shadowMappingFragmentShader->compile())
    {
        std::cerr << "[ERROR] Can not compile fragment shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[DEBUG] Linking shadow mapping shaders..." << std::endl;

    auto shadowMappingProgram = std::make_unique<globjects::Program>();
    shadowMappingProgram->attach(shadowMappingVertexShader.get(), shadowMappingFragmentShader.get());

    auto shadowMappingLightSpaceUniform = shadowMappingProgram->getUniform<glm::mat4>("lightSpaceMatrix");
    auto shadowMappingModelTransformationUniform = shadowMappingProgram->getUniform<glm::mat4>("modelTransformation");

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling shadow rendering vertex shader...";

    auto shadowRenderingVertexShaderSource = globjects::Shader::sourceFromFile("media/shadow-rendering.vert");
    auto shadowRenderingVertexShaderTemplate = globjects::Shader::applyGlobalReplacements(shadowRenderingVertexShaderSource.get());
    auto shadowRenderingVertexShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_VERTEX_SHADER), shadowRenderingVertexShaderTemplate.get());

    if (!shadowRenderingVertexShader->compile())
    {
        std::cerr << "[ERROR] Can not compile shadow rendering vertex shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling shadow rendering fragment shader...";

    auto shadowRenderingFragmentShaderSource = globjects::Shader::sourceFromFile("media/shadow-rendering.frag");
    auto shadowRenderingFragmentShaderTemplate = globjects::Shader::applyGlobalReplacements(shadowRenderingFragmentShaderSource.get());
    auto shadowRenderingFragmentShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_FRAGMENT_SHADER), shadowRenderingFragmentShaderTemplate.get());

    if (!shadowRenderingFragmentShader->compile())
    {
        std::cerr << "[ERROR] Can not compile chicken fragment shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Linking shadow rendering shader...";

    auto shadowRenderingProgram = std::make_unique<globjects::Program>();
    shadowRenderingProgram->attach(shadowRenderingVertexShader.get(), shadowRenderingFragmentShader.get());

    auto shadowRenderingModelTransformationUniform = shadowRenderingProgram->getUniform<glm::mat4>("model");
    auto shadowRenderingViewTransformationUniform = shadowRenderingProgram->getUniform<glm::mat4>("view");
    auto shadowRenderingProjectionTransformationUniform = shadowRenderingProgram->getUniform<glm::mat4>("projection");
    auto shadowRenderingLightSpaceMatrixUniform = shadowRenderingProgram->getUniform<glm::mat4>("lightSpaceMatrix");

    auto shadowRenderingLightPositionUniform = shadowRenderingProgram->getUniform<glm::vec3>("lightPosition");
    auto shadowRenderingLightColorUniform = shadowRenderingProgram->getUniform<glm::vec3>("lightColor");
    auto shadowRenderingCameraPositionUniform = shadowRenderingProgram->getUniform<glm::vec3>("cameraPosition");

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling bloom blur vertex shader...";

    auto bloomBlurVertexSource = globjects::Shader::sourceFromFile("media/bloom-blur.vert");
    auto bloomBlurVertexShaderTemplate = globjects::Shader::applyGlobalReplacements(bloomBlurVertexSource.get());
    auto bloomBlurVertexShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_VERTEX_SHADER), bloomBlurVertexShaderTemplate.get());

    if (!bloomBlurVertexShader->compile())
    {
        std::cerr << "[ERROR] Can not compile vertex shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compilingbloom blur fragment shader...";

    auto bloomBlurFragmentSource = globjects::Shader::sourceFromFile("media/bloom-blur.frag");
    auto bloomBlurFragmentShaderTemplate = globjects::Shader::applyGlobalReplacements(bloomBlurFragmentSource.get());
    auto bloomBlurFragmentShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_FRAGMENT_SHADER), bloomBlurFragmentShaderTemplate.get());

    if (!bloomBlurFragmentShader->compile())
    {
        std::cerr << "[ERROR] Can not compile fragment shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[DEBUG] Linking bloom blur shaders..." << std::endl;

    auto bloomBlurProgram = std::make_unique<globjects::Program>();
    bloomBlurProgram->attach(bloomBlurVertexShader.get(), bloomBlurFragmentShader.get());

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compiling bloom output vertex shader...";

    auto bloomOutputVertexSource = globjects::Shader::sourceFromFile("media/bloom-output.vert");
    auto bloomOutputVertexShaderTemplate = globjects::Shader::applyGlobalReplacements(bloomOutputVertexSource.get());
    auto bloomOutputVertexShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_VERTEX_SHADER), bloomOutputVertexShaderTemplate.get());

    if (!bloomOutputVertexShader->compile())
    {
        std::cerr << "[ERROR] Can not compile vertex shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Compilingbloom output fragment shader...";

    auto bloomOutputFragmentSource = globjects::Shader::sourceFromFile("media/bloom-output.frag");
    auto bloomOutputFragmentShaderTemplate = globjects::Shader::applyGlobalReplacements(bloomOutputFragmentSource.get());
    auto bloomOutputFragmentShader = std::make_unique<globjects::Shader>(static_cast<gl::GLenum>(GL_FRAGMENT_SHADER), bloomOutputFragmentShaderTemplate.get());

    if (!bloomOutputFragmentShader->compile())
    {
        std::cerr << "[ERROR] Can not compile fragment shader" << std::endl;
        return 1;
    }

    std::cout << "done" << std::endl;

    std::cout << "[DEBUG] Linking bloom output shaders..." << std::endl;

    auto bloomOutputProgram = std::make_unique<globjects::Program>();
    bloomOutputProgram->attach(bloomOutputVertexShader.get(), bloomOutputFragmentShader.get());

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Loading 3D model...";

    Assimp::Importer importer;

    auto houseScene = importer.ReadFile("media/house1.1.obj", 0);

    if (!houseScene)
    {
        std::cerr << "failed: " << importer.GetErrorString() << std::endl;
        return 1;
    }

    auto houseModel = AssimpModel::fromAiNode(houseScene, houseScene->mRootNode, { "media" });

    // INFO: this transformation is hard-coded specifically for Chicken.3ds model
    houseModel->setTransformation(glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(2.0f)), glm::vec3(0.0f, 0.75f, 0.0f)));

    auto tableScene = importer.ReadFile("media/table.obj", 0);

    if (!tableScene)
    {
        std::cerr << "failed: " << importer.GetErrorString() << std::endl;
        return 1;
    }

    auto tableModel = AssimpModel::fromAiNode(tableScene, tableScene->mRootNode, { "media" });

    tableModel->setTransformation(glm::rotate(glm::scale(glm::mat4(1.0f), glm::vec3(1.0f)), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

    auto lanternScene = importer.ReadFile("media/lantern.obj", 0);

    if (!lanternScene)
    {
        std::cerr << "failed: " << importer.GetErrorString() << std::endl;
        return 1;
    }

    auto lanternModel = AssimpModel::fromAiNode(lanternScene, lanternScene->mRootNode, { "media" });

    lanternModel->setTransformation(glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(-1.75f, 3.85f, -0.75f)), glm::vec3(0.5f)));

    // TODO: extract this to material class
    sf::Image lanternEmissionMapImage;

    if (!lanternEmissionMapImage.loadFromFile("media/lantern_emission.png"))
    {
        std::cerr << "[ERROR] Can not load texture" << std::endl;
        return 1;
    }

    lanternEmissionMapImage.flipVertically();

    auto lanternEmissionMapTexture = std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_2D));

    lanternEmissionMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<GLint>(GL_LINEAR));
    lanternEmissionMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<GLint>(GL_LINEAR));

    lanternEmissionMapTexture->image2D(
        0,
        static_cast<gl::GLenum>(GL_RGBA8),
        glm::vec2(lanternEmissionMapImage.getSize().x, lanternEmissionMapImage.getSize().y),
        0,
        static_cast<gl::GLenum>(GL_RGBA),
        static_cast<gl::GLenum>(GL_UNSIGNED_BYTE),
        reinterpret_cast<const gl::GLvoid*>(lanternEmissionMapImage.getPixelsPtr()));

    // TODO: extract this to material class
    sf::Image lanternSpecularMapImage;

    if (!lanternSpecularMapImage.loadFromFile("media/lantern_specular.png"))
    {
        std::cerr << "[ERROR] Can not load texture" << std::endl;
        return 1;
    }

    lanternSpecularMapImage.flipVertically();

    auto lanternSpecularMapTexture = std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_2D));

    lanternSpecularMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<GLint>(GL_LINEAR));
    lanternSpecularMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<GLint>(GL_LINEAR));

    lanternSpecularMapTexture->image2D(
        0,
        static_cast<gl::GLenum>(GL_RGBA8),
        glm::vec2(lanternSpecularMapImage.getSize().x, lanternSpecularMapImage.getSize().y),
        0,
        static_cast<gl::GLenum>(GL_RGBA),
        static_cast<gl::GLenum>(GL_UNSIGNED_BYTE),
        reinterpret_cast<const gl::GLvoid*>(lanternSpecularMapImage.getPixelsPtr()));

    auto scrollScene = importer.ReadFile("media/scroll.obj", 0);

    if (!scrollScene)
    {
        std::cerr << "failed: " << importer.GetErrorString() << std::endl;
        return 1;
    }

    auto scrollModel = AssimpModel::fromAiNode(scrollScene, scrollScene->mRootNode, { "media" });

    scrollModel->setTransformation(glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 3.85f, 0.0f)), glm::vec3(0.5f)));

    auto quadScene = importer.ReadFile("media/quad.obj", 0);

    if (!quadScene)
    {
        std::cerr << "failed: " << importer.GetErrorString() << std::endl;
        return 1;
    }

    auto quadModel = AssimpModel::fromAiNode(quadScene, quadScene->mRootNode);

    std::cout << "done" << std::endl;

    std::cout << "[DEBUG] Initializing framebuffers...";

    std::cout << "[DEBUG] Initializing shadowMapTexture...";

    auto shadowMapTexture = std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_2D));

    shadowMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<gl::GLenum>(GL_LINEAR));
    shadowMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<gl::GLenum>(GL_LINEAR));

    shadowMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_S), static_cast<gl::GLenum>(GL_CLAMP_TO_BORDER));
    shadowMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_T), static_cast<gl::GLenum>(GL_CLAMP_TO_BORDER));

    shadowMapTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_BORDER_COLOR), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    shadowMapTexture->image2D(
        0,
        static_cast<gl::GLenum>(GL_DEPTH_COMPONENT),
        glm::vec2(2048, 2048),
        0,
        static_cast<gl::GLenum>(GL_DEPTH_COMPONENT),
        static_cast<gl::GLenum>(GL_FLOAT),
        nullptr);

    std::cout << "done" << std::endl;

    std::cout << "[DEBUG] Initializing shadows frame buffer...";

    auto shadowFramebuffer = std::make_unique<globjects::Framebuffer>();
    shadowFramebuffer->attachTexture(static_cast<gl::GLenum>(GL_DEPTH_ATTACHMENT), shadowMapTexture.get());

    shadowFramebuffer->printStatus(true);

    std::cout << "done" << std::endl;

    std::cout << "[DEBUG] Initializing bloom frame buffer...";

    const auto antiAliasingSamples = 4;

    auto bloomColorTexture = std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_2D_MULTISAMPLE));

    // these are supposed to be glTexParamf instead of glTexParami for multisampled textures, which globjects::Texture::setParameter does get wrong
    // bloomColorTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<gl::GLenum>(GL_LINEAR));
    // bloomColorTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<gl::GLenum>(GL_LINEAR));

    // bloomColorTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_S), static_cast<gl::GLenum>(GL_CLAMP_TO_EDGE));
    // bloomColorTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_T), static_cast<gl::GLenum>(GL_CLAMP_TO_EDGE));

    bloomColorTexture->image2DMultisample(
        antiAliasingSamples,
        static_cast<gl::GLenum>(GL_RGB16F),
        glm::vec2(window.getSize().x, window.getSize().y),
        static_cast<gl::GLboolean>(true));

    auto bloomBrightnessTexture = std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_2D_MULTISAMPLE));

    // bloomBrightnessTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<gl::GLenum>(GL_LINEAR));
    // bloomBrightnessTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<gl::GLenum>(GL_LINEAR));

    // bloomBrightnessTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_S), static_cast<gl::GLenum>(GL_CLAMP_TO_EDGE));
    // bloomBrightnessTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_T), static_cast<gl::GLenum>(GL_CLAMP_TO_EDGE));

    bloomBrightnessTexture->image2DMultisample(
        antiAliasingSamples,
        static_cast<gl::GLenum>(GL_RGB16F),
        glm::vec2(window.getSize().x, window.getSize().y),
        static_cast<gl::GLboolean>(true));

    auto renderBuffer = std::make_unique<globjects::Renderbuffer>();

    renderBuffer->storageMultisample(antiAliasingSamples, static_cast<gl::GLenum>(GL_DEPTH24_STENCIL8), window.getSize().x, window.getSize().y);

    auto bloomFramebuffer = std::make_unique<globjects::Framebuffer>();

    bloomFramebuffer->attachTexture(static_cast<gl::GLenum>(GL_COLOR_ATTACHMENT0), bloomColorTexture.get());
    bloomFramebuffer->attachTexture(static_cast<gl::GLenum>(GL_COLOR_ATTACHMENT1), bloomBrightnessTexture.get());

    bloomFramebuffer->attachRenderBuffer(static_cast<gl::GLenum>(GL_DEPTH_STENCIL_ATTACHMENT), renderBuffer.get());

    // tell framebuffer it actually needs to render to **BOTH** textures, but does not have to output anywhere (last NONE argument, iirc)
    bloomFramebuffer->setDrawBuffers({ static_cast<gl::GLenum>(GL_COLOR_ATTACHMENT0), static_cast<gl::GLenum>(GL_COLOR_ATTACHMENT1), static_cast<gl::GLenum>(GL_NONE) });

    bloomFramebuffer->printStatus(true);

    auto temporaryOutputTexture = std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_2D));

    temporaryOutputTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<gl::GLenum>(GL_LINEAR));
    temporaryOutputTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<gl::GLenum>(GL_LINEAR));

    temporaryOutputTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_S), static_cast<gl::GLenum>(GL_CLAMP_TO_EDGE));
    temporaryOutputTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_T), static_cast<gl::GLenum>(GL_CLAMP_TO_EDGE));

    temporaryOutputTexture->image2D(
        0,
        static_cast<gl::GLenum>(GL_RGB16F),
        glm::vec2(window.getSize().x, window.getSize().y),
        0,
        static_cast<gl::GLenum>(GL_RGB),
        static_cast<gl::GLenum>(GL_FLOAT),
        nullptr);

    auto temporaryOutputFramebuffer = std::make_unique<globjects::Framebuffer>();

    temporaryOutputFramebuffer->attachTexture(static_cast<gl::GLenum>(GL_COLOR_ATTACHMENT0), temporaryOutputTexture.get());

    temporaryOutputFramebuffer->printStatus(true);

    auto bloomBlurTexture1 = std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_2D));

    bloomBlurTexture1->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<gl::GLenum>(GL_LINEAR));
    bloomBlurTexture1->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<gl::GLenum>(GL_LINEAR));

    bloomBlurTexture1->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_S), static_cast<gl::GLenum>(GL_CLAMP_TO_EDGE));
    bloomBlurTexture1->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_T), static_cast<gl::GLenum>(GL_CLAMP_TO_EDGE));

    bloomBlurTexture1->image2D(
        0,
        static_cast<gl::GLenum>(GL_RGB16F),
        glm::vec2(window.getSize().x, window.getSize().y),
        0,
        static_cast<gl::GLenum>(GL_RGBA),
        static_cast<gl::GLenum>(GL_FLOAT),
        nullptr);

    auto bloomBlurFramebuffer1 = std::make_unique<globjects::Framebuffer>();

    bloomBlurFramebuffer1->attachTexture(static_cast<gl::GLenum>(GL_COLOR_ATTACHMENT0), bloomBlurTexture1.get());

    bloomBlurFramebuffer1->printStatus(true);

    auto bloomBlurTexture2 = std::make_unique<globjects::Texture>(static_cast<gl::GLenum>(GL_TEXTURE_2D));

    bloomBlurTexture2->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<gl::GLenum>(GL_LINEAR));
    bloomBlurTexture2->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<gl::GLenum>(GL_LINEAR));

    bloomBlurTexture2->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_S), static_cast<gl::GLenum>(GL_CLAMP_TO_EDGE));
    bloomBlurTexture2->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_T), static_cast<gl::GLenum>(GL_CLAMP_TO_EDGE));

    bloomBlurTexture2->image2D(
        0,
        static_cast<gl::GLenum>(GL_RGB16F),
        glm::vec2(window.getSize().x, window.getSize().y),
        0,
        static_cast<gl::GLenum>(GL_RGBA),
        static_cast<gl::GLenum>(GL_FLOAT),
        nullptr);

    auto bloomBlurFramebuffer2 = std::make_unique<globjects::Framebuffer>();

    bloomBlurFramebuffer2->attachTexture(static_cast<gl::GLenum>(GL_COLOR_ATTACHMENT0), bloomBlurTexture2.get());

    bloomBlurFramebuffer2->printStatus(true);

    std::cout << "done" << std::endl;

    std::cout << "[INFO] Done initializing" << std::endl;

    // taken from lantern position
    glm::vec3 lightPosition = glm::vec3(-1.75f, 6.5f, -0.75f);

    const float fov = 45.0f;

    const float cameraMoveSpeed = 1.0f;
    const float cameraRotateSpeed = 10.0f;

    glm::vec3 cameraPos = glm::vec3(0.0f, 6.0f, 5.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 cameraRight = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 cameraForward = glm::normalize(glm::cross(cameraUp, cameraRight));

    sf::Clock clock;

    glEnable(static_cast<gl::GLenum>(GL_DEPTH_TEST));

#ifndef WIN32
    auto previousMousePos = glm::vec2(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
#endif

    while (window.isOpen())
    {
        sf::Event event{};

        // measure time since last frame, in seconds
        float deltaTime = static_cast<float>(clock.restart().asSeconds());

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
                break;
            }
        }

#ifdef WIN32
        if (!window.hasFocus())
        {
            continue;
        }
#endif

        glm::vec2 currentMousePos = glm::vec2(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);

#ifdef WIN32
        glm::vec2 mouseDelta = currentMousePos - glm::vec2((window.getSize().x / 2), (window.getSize().y / 2));
        sf::Mouse::setPosition(sf::Vector2<int>(window.getSize().x / 2, window.getSize().y / 2), window);
#else
        glm::vec2 mouseDelta = currentMousePos - previousMousePos;
        previousMousePos = currentMousePos;
#endif

        float horizontalAngle = (mouseDelta.x / static_cast<float>(window.getSize().x)) * -1 * deltaTime * cameraRotateSpeed * fov;
        float verticalAngle = (mouseDelta.y / static_cast<float>(window.getSize().y)) * -1 * deltaTime * cameraRotateSpeed * fov;

        cameraForward = glm::rotate(cameraForward, horizontalAngle, cameraUp);
        cameraForward = glm::rotate(cameraForward, verticalAngle, cameraRight);

        cameraRight = glm::normalize(glm::rotate(cameraRight, horizontalAngle, cameraUp));

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
            {
                cameraPos += cameraForward * cameraMoveSpeed * deltaTime * 10.0f;
            }
            else
            {
                cameraPos += cameraForward * cameraMoveSpeed * deltaTime;
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
            {
                cameraPos -= cameraForward * cameraMoveSpeed * deltaTime * 10.0f;
            }
            else
            {
                cameraPos -= cameraForward * cameraMoveSpeed * deltaTime;
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
            {
                cameraPos -= glm::normalize(glm::cross(cameraForward, cameraUp)) * cameraMoveSpeed * deltaTime * 10.0f;
            }
            else
            {
                cameraPos -= glm::normalize(glm::cross(cameraForward, cameraUp)) * cameraMoveSpeed * deltaTime;
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
            {
                cameraPos += glm::normalize(glm::cross(cameraForward, cameraUp)) * cameraMoveSpeed * deltaTime * 10.0f;
            }
            else
            {
                cameraPos += glm::normalize(glm::cross(cameraForward, cameraUp)) * cameraMoveSpeed * deltaTime;
            }
        }

        glm::mat4 cameraProjection = glm::perspective(glm::radians(fov), (float)window.getSize().x / (float)window.getSize().y, 0.1f, 100.0f);

        glm::mat4 cameraView = glm::lookAt(
            cameraPos,
            cameraPos + cameraForward,
            cameraUp);

        const float nearPlane = 0.1f;
        const float farPlane = 10.0f;
        glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);

        glm::mat4 lightView = glm::lookAt(lightPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4 lightSpaceMatrix = lightProjection * lightView;

        ::glViewport(0, 0, 2048, 2048);

        // first render pass - shadow mapping

        shadowFramebuffer->bind();

        ::glClearColor(static_cast<gl::GLfloat>(1.0f), static_cast<gl::GLfloat>(1.0f), static_cast<gl::GLfloat>(1.0f), static_cast<gl::GLfloat>(1.0f));
        ::glClear(GL_DEPTH_BUFFER_BIT);
        shadowFramebuffer->clearBuffer(static_cast<gl::GLenum>(GL_DEPTH), 0, glm::vec4(1.0f));

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        // cull front faces to prevent peter panning the generated shadow map
        glCullFace(GL_FRONT);

        shadowMappingProgram->use();

        shadowMappingLightSpaceUniform->set(lightSpaceMatrix);

        shadowMappingModelTransformationUniform->set(houseModel->getTransformation());

        houseModel->bind();
        houseModel->draw();
        houseModel->unbind();

        shadowMappingModelTransformationUniform->set(tableModel->getTransformation());

        tableModel->bind();
        tableModel->draw();
        tableModel->unbind();

        shadowMappingModelTransformationUniform->set(lanternModel->getTransformation());

        lanternModel->bind();
        lanternModel->draw();
        lanternModel->unbind();

        shadowMappingModelTransformationUniform->set(scrollModel->getTransformation());

        // scroll model needs culling to be disabled since this is a modified plane, so...
        glDisable(GL_CULL_FACE);

        scrollModel->bind();
        scrollModel->draw();
        scrollModel->unbind();

        glEnable(GL_CULL_FACE);

        shadowFramebuffer->unbind();

        shadowMappingProgram->release();

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        // second pass - switch to normal shader and render picture with depth information to the bloom framebuffer

        bloomFramebuffer->bind();

        ::glViewport(0, 0, static_cast<GLsizei>(window.getSize().x), static_cast<GLsizei>(window.getSize().y));
        ::glClearColor(static_cast<gl::GLfloat>(0.0f), static_cast<gl::GLfloat>(0.0f), static_cast<gl::GLfloat>(0.0f), static_cast<gl::GLfloat>(1.0f));
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shadowRenderingProgram->use();

        shadowRenderingLightPositionUniform->set(lightPosition);
        shadowRenderingLightColorUniform->set(glm::vec3(1.0, 1.0, 1.0));
        shadowRenderingCameraPositionUniform->set(cameraPos);

        shadowRenderingProjectionTransformationUniform->set(cameraProjection);
        shadowRenderingViewTransformationUniform->set(cameraView);
        shadowRenderingLightSpaceMatrixUniform->set(lightSpaceMatrix);

        // draw chicken

        shadowMapTexture->bindActive(0);

        shadowRenderingProgram->setUniform("shadowMap", 0);
        shadowRenderingProgram->setUniform("diffuseTexture", 1);

        shadowRenderingModelTransformationUniform->set(houseModel->getTransformation());

        houseModel->bind();
        houseModel->draw();
        houseModel->unbind();

        shadowRenderingModelTransformationUniform->set(tableModel->getTransformation());

        tableModel->bind();
        tableModel->draw();
        tableModel->unbind();

        shadowRenderingModelTransformationUniform->set(lanternModel->getTransformation());

        shadowRenderingProgram->setUniform("emissionColor", glm::vec3(0.807f, 0.671f, 0.175f));

        lanternSpecularMapTexture->bindActive(2);
        lanternEmissionMapTexture->bindActive(3);

        shadowRenderingProgram->setUniform("specularMapTexture", 2);
        shadowRenderingProgram->setUniform("emissionMapTexture", 3);

        lanternModel->bind();
        lanternModel->draw();
        lanternModel->unbind();

        lanternSpecularMapTexture->unbindActive(2);
        lanternEmissionMapTexture->unbindActive(3);

        shadowRenderingModelTransformationUniform->set(scrollModel->getTransformation());

        glDisable(GL_CULL_FACE);

        scrollModel->bind();
        scrollModel->draw();
        scrollModel->unbind();

        glEnable(GL_CULL_FACE);

        shadowMapTexture->unbindActive(0);

        shadowRenderingProgram->release();

        bloomFramebuffer->unbind();

        // copy data from the multisampled framebuffer to non-multisampled textures
        // since the bloomFramebuffer has two color attachments, we need to blit twice
        // but doing so removes the need in some additional conditions when running the blur

        bloomFramebuffer->bind(static_cast<gl::GLenum>(GL_READ_FRAMEBUFFER));
        temporaryOutputFramebuffer->bind(static_cast<gl::GLenum>(GL_DRAW_FRAMEBUFFER));

        bloomFramebuffer->blit(
            static_cast<gl::GLenum>(GL_COLOR_ATTACHMENT0),
            std::array<gl::GLint, 4>{ 0, 0, static_cast<int>(window.getSize().x), static_cast<int>(window.getSize().y) },
            temporaryOutputFramebuffer.get(),
            std::vector<gl::GLenum>{ static_cast<gl::GLenum>(GL_COLOR_ATTACHMENT0) },
            std::array<gl::GLint, 4>{ 0, 0, static_cast<int>(window.getSize().x), static_cast<int>(window.getSize().y) },
            static_cast<gl::ClearBufferMask>(GL_COLOR_BUFFER_BIT),
            static_cast<gl::GLenum>(GL_NEAREST));

        // same as
        // glReadBuffer(GL_COLOR_ATTACHMENT0);
        // glDrawBuffer(GL_COLOR_ATTACHMENT0);
        // glBlitFramebuffer(0, 0, window.getSize().x, window.getSize().y, 0, 0, window.getSize().x, window.getSize().y, static_cast<gl::ClearBufferMask>(GL_COLOR_BUFFER_BIT), static_cast<gl::GLenum>(GL_NEAREST));

        temporaryOutputFramebuffer->unbind();
        bloomFramebuffer->unbind();

        bloomFramebuffer->bind(static_cast<gl::GLenum>(GL_READ_FRAMEBUFFER));
        bloomBlurFramebuffer2->bind(static_cast<gl::GLenum>(GL_DRAW_FRAMEBUFFER));

        bloomFramebuffer->blit(
            static_cast<gl::GLenum>(GL_COLOR_ATTACHMENT0),
            std::array<gl::GLint, 4>{ 0, 0, static_cast<int>(window.getSize().x), static_cast<int>(window.getSize().y) },
            bloomBlurFramebuffer2.get(),
            std::vector<gl::GLenum>{ static_cast<gl::GLenum>(GL_COLOR_ATTACHMENT0) },
            std::array<gl::GLint, 4>{ 0, 0, static_cast<int>(window.getSize().x), static_cast<int>(window.getSize().y) },
            static_cast<gl::ClearBufferMask>(GL_COLOR_BUFFER_BIT),
            static_cast<gl::GLenum>(GL_NEAREST));

        bloomFramebuffer->unbind();
        bloomBlurFramebuffer2->unbind();

        // third pass - blur the data stored in the bloom framebuffer with two-pass Gauss blur

        bloomBlurProgram->use();

        const auto blurPasses = 10;

        // for the initial blur pass, use the texture from the bloomFramebuffer as an input

        // we do not need anything extra here, since the bloomBlurFramebuffer2 (which we read from) will already contain the data from the bloomBrightnessTexture

        bloomBlurProgram->setUniform("blurInput", 0);

        for (auto i = 0; i < blurPasses; ++i)
        {
            // bind one framebuffer to write blur results to and bind the texture from another framebuffer to read input data from (for this blur stage)
            if (i % 2 == 0)
            {
                // bind the new target framebuffer to write blur results to
                bloomBlurFramebuffer1->bind();
                // bind the texture from the previous blur pass to read input data for this stage from
                bloomBlurTexture2->bindActive(0);
                // tell shader that we want to use horizontal blur
                bloomBlurProgram->setUniform("isHorizontalBlur", true);
            }
            else
            {
                // bind the new target framebuffer to write blur results to
                bloomBlurFramebuffer2->bind();
                // bind the texture from the previous blur pass to read input data for this stage from
                if (i > 0)
                    bloomBlurTexture1->bindActive(0);
                // tell shader that we want to use vertical blur
                bloomBlurProgram->setUniform("isHorizontalBlur", false);
            }

            // render quad with the texture from the active texture
            quadModel->bind();
            quadModel->draw();
            quadModel->unbind();

            if (i % 2 == 0)
            {
                // unbind the active framebuffer
                bloomBlurFramebuffer1->unbind();
                // unbind the active texture
                bloomBlurTexture2->unbindActive(0);
            }
            else
            {
                bloomBlurFramebuffer2->unbind();
                bloomBlurTexture1->unbindActive(0);
            }
        }

        bloomBlurProgram->release();

        // fourth pass - render the result onto the quad and to the temporary framebuffer, to allow for anti-aliasing

        ::glViewport(0, 0, static_cast<GLsizei>(window.getSize().x), static_cast<GLsizei>(window.getSize().y));
        ::glClearColor(static_cast<gl::GLfloat>(0.0f), static_cast<gl::GLfloat>(0.0f), static_cast<gl::GLfloat>(0.0f), static_cast<gl::GLfloat>(1.0f));
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        bloomOutputProgram->use();

        // here we use our temporary non-multisampled texture as one of the inputs for merge bloom effect stages, since OUR shader would only work with one sample layer
        temporaryOutputTexture->bindActive(0);
        bloomBlurTexture2->bindActive(1);

        bloomOutputProgram->setUniform("colorOutput", 0);
        bloomOutputProgram->setUniform("blurOutput", 1);

        quadModel->bind();
        quadModel->draw();
        quadModel->unbind();

        temporaryOutputTexture->unbindActive(0);
        bloomBlurTexture2->unbindActive(1);

        bloomOutputProgram->release();

        // done rendering the frame

        window.display();
    }

    return 0;
}