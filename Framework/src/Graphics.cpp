#include "Resha.h"
#include <assert.h>

#include <GL/gl3w.h>

int32_t CompileShader(const char* shader, ShaderType type, std::string& log)
{
    log.clear();
    int32_t id = -1;
    switch (type)
    {
    case ShaderType::GEOMETRY:
    {
        id = glCreateShader(GL_GEOMETRY_SHADER);
    }
    break;
    case  ShaderType::FRAGMENT:
    {
        id = glCreateShader(GL_FRAGMENT_SHADER);
    }
    break;
    case ShaderType::VERTEX:
    {
        id = glCreateShader(GL_VERTEX_SHADER);
    }
    break;
    }
    glShaderSource(id, 1, &shader, NULL);
    glCompileShader(id);
    GLint success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        constexpr size_t LOG_ARRAY_SIZE = 256;
        char temp[LOG_ARRAY_SIZE] = {};
        glGetShaderInfoLog(id, sizeof(temp), NULL, temp);
        log = temp;
        return -1;
    }
    return id;
}

Program CreateProgram(const char* geometryShader,
                      const char* vertexShader,
                      const char* fragmentShader,
                      std::string& log)
{
    Program program;
    log.clear();
    int32_t gsId = -1, vsId = -1, fsId = -1;
    if (geometryShader)
    {
        gsId = CompileShader(geometryShader, ShaderType::GEOMETRY, log);
        if (gsId < 0)
        {
            return program;
        }
    }
    if (vertexShader)
    {
        vsId = CompileShader(vertexShader, ShaderType::VERTEX, log);
        if (vsId < 0)
        {
            return program;
        }
    }
    if (fragmentShader)
    {
        fsId = CompileShader(fragmentShader, ShaderType::FRAGMENT, log);
        if (fsId < 0)
        {
            return program;
        }
    }

    if (program.id != -1)
    {
        glDeleteProgram(program.id);
    }
    program.id = glCreateProgram();

    if (gsId >= 0) glAttachShader(program.id, gsId);
    if (fsId >= 0) glAttachShader(program.id, fsId);
    if (vsId >= 0) glAttachShader(program.id, vsId);
    glLinkProgram(program.id);

    GLint success = 0;
    glGetProgramiv(program.id, GL_LINK_STATUS, &success);
    program.valid = success;
    if (!success)
    {
        constexpr size_t LOG_ARRAY_SIZE = 256;
        char temp[LOG_ARRAY_SIZE] = {};
        glGetProgramInfoLog(program.id, sizeof(temp), NULL, temp);
        log = temp;
    }
    if (gsId >= 0) glDeleteShader(gsId);
    if (fsId >= 0) glDeleteShader(fsId);
    if (vsId >= 0) glDeleteShader(vsId);

    if (success)
    {
        if (geometryShader)
        {
            program.geometryShader = geometryShader;
        }
        if (vertexShader)
        {
            program.vertexShader = vertexShader;
        }
        if (fragmentShader)
        {
            program.fragmentShader = fragmentShader;
        }
    }
    return program;
}

uint32_t GenerateTexture()
{
    uint32_t id;
    glGenTextures(1, &id);
    return id;
}

void UpdateTexture(uint32_t textureId, size_t width, size_t height, Color* rgbaData)
{
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgbaData);
}

MeshRenderInfo CreateSurfaceMeshRenderInfo(SurfaceMesh& mesh)
{
    MeshRenderInfo result;

    const std::vector<Vec3d> vertexNormals = CalculateVertexNormals(mesh, BuildConnectivity(mesh));

    result.box = CalculateBoundingBox(mesh);
    result.verticesCount = mesh.vertices.size();
    result.facesCount = mesh.faces.size();
    result.id = mesh.id;

    struct VertexInfo
    {
        Vec3f position;
        Vec3f normal;
    };
    std::vector<VertexInfo> vertices(result.verticesCount);
    for (size_t i = 0; i < result.verticesCount; i++)
    {
        vertices[i].position.x = mesh.vertices[i].x;
        vertices[i].normal.x = vertexNormals[i].x;
        vertices[i].position.y = mesh.vertices[i].y;
        vertices[i].normal.y = vertexNormals[i].y;
        vertices[i].position.z = mesh.vertices[i].z;
        vertices[i].normal.z = vertexNormals[i].z;
    }

    const uint32_t* indicies = (const uint32_t*)(mesh.faces.data());

    glGenVertexArrays(1, &result.vertexBufferObject);
    glBindVertexArray(result.vertexBufferObject);
    glGenBuffers(1, &result.vertexBufferId);
    glGenBuffers(1, &result.elementBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, result.vertexBufferId);
    glBufferData(GL_ARRAY_BUFFER, result.verticesCount * sizeof(VertexInfo), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexInfo), (void*)offsetof(VertexInfo, position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexInfo), (void*)offsetof(VertexInfo, normal));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, result.elementBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, result.facesCount * sizeof(Triangle), indicies, GL_STATIC_DRAW);
    glBindVertexArray(0);
    return result;
}


Mat4 CameraGetViewMatrix(const Camera& c)
{
    return c.viewMatrix;
}

Mat4 CameraGetProjectionMatrix(const Camera& c, size_t width, size_t height)
{
    const double farClip = c.farClipRatio * c.lengthScale;
    const double nearClip = c.nearClipRatio * c.lengthScale;
    const double fovRad = Deg2Rad(c.fov);
    const double aspectRatio = width / (double)height;
    return Perspective(fovRad, aspectRatio, nearClip, farClip);
}

void CameraFitBBox(Camera& c, const BBox& box)
{
    c.center = CalculateBBoxCenter(box);
    c.lengthScale = Length(box.max - box.min);

    const Mat4 Tobj = Translate(Identity(), c.center * -1.0);
    const Mat4 Tcam = Translate(Identity(), Vec3d{ 0.0, 0.0f, -1.5 * c.lengthScale });

    c.viewMatrix = Tcam * Tobj;
    c.fov = Camera::DEFAULT_FOV;
    c.nearClipRatio = Camera::DEFAULT_NEAR_CLIP;
    c.farClipRatio = Camera::DEFAULT_FAR_CLIP;
}

void CameraGetFrame(const Camera& c, Vec3d& look, Vec3d& up, Vec3d& right)
{
    Mat3 r;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            r.elements[j][i] = c.viewMatrix.elements[i][j];
        }
    }
    look = r * Vec3d{ 0.0, 0.0, -1.0 };
    up = r * Vec3d{ 0.0, 1.0, 0.0 };
    right = r * Vec3d{ 1.0, 0.0, 0.0 };
}

void CameraProcessZoom(Camera& c, double amount)
{
    if (amount == 0.0)
    {
        return;
    }
    // Translate the camera forwards and backwards
    const double movementScale = c.lengthScale * 0.1;
    static const Mat4 eye = Identity();
    const Mat4 camSpaceT = Translate(eye, Vec3d{ 0.0, 0.0, movementScale * amount });
    c.viewMatrix = camSpaceT * c.viewMatrix;
}

void CameraProcessRotate(Camera& c, Vec2d start, Vec2d end)
{
    if (Length(start - end) == 0)
    {
        return;
    }
    // Get frame
    Vec3d frameLookDir, frameUpDir, frameRightDir;
    CameraGetFrame(c, frameLookDir, frameUpDir, frameRightDir);

    const Vec2d dragDelta = end - start;
    const double delTheta = 2.0 * dragDelta.x;
    const double delPhi = 2.0 * dragDelta.y;

    // Translate to center
    c.viewMatrix = Translate(c.viewMatrix, c.center);
    // Rotation about the vertical axis
    const Mat4 thetaCamR = Rotate(Identity(), delTheta, frameUpDir);
    c.viewMatrix = c.viewMatrix * thetaCamR;
    // Rotation about the horizontal axis
    const Mat4 phiCamR = Rotate(Identity(), -delPhi, frameRightDir);
    c.viewMatrix = c.viewMatrix * phiCamR;
    // Undo centering
    c.viewMatrix = Translate(c.viewMatrix, c.center * -1);
}

void CameraProcessTranslate(Camera& c, Vec2d delta)
{
    if (Length(delta) == 0)
    {
        return;
    }
    const double movementScale = c.lengthScale * 0.6;
    const Mat4 camSpaceT = Translate(Identity(), Vec3d{ delta.x, delta.y, 0.0 } *movementScale);
    c.viewMatrix = camSpaceT * c.viewMatrix;
}

void RenderMesh(const RenderBuffer& buffer, const Program& program, const MeshRenderInfo& info)
{
    glBindFramebuffer(GL_FRAMEBUFFER, buffer.frameBufferId);
    glUseProgram(program.id);
    glBindVertexArray(info.vertexBufferObject);
    const size_t dataSize = 3 * info.facesCount;
    glDrawElements(GL_TRIANGLES, dataSize, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

RenderBuffer CreateRenderBuffer(size_t width, size_t height)
{
    RenderBuffer result;
    result.width = width;
    result.height = height;
    // framebuffer configuration
    glGenFramebuffers(1, &result.frameBufferId);
    glBindFramebuffer(GL_FRAMEBUFFER, result.frameBufferId);
    // create a color attachment texture
    result.textureId = GenerateTexture();
    glBindTexture(GL_TEXTURE_2D, result.textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, result.width, result.height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, result.textureId, 0);
    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    glGenRenderbuffers(1, &result.renderBufferId);
    glBindRenderbuffer(GL_RENDERBUFFER, result.renderBufferId);
    // use a single renderbuffer object for both a depth AND stencil buffer.
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, result.width,
                          result.height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER,
                              result.renderBufferId);  // now actually attach it
    // now that we actually created the framebuffer and added all attachments we
    // want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        assert(false);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return result;
}

void ReizeRenderBuffer(RenderBuffer& buffer, size_t width, size_t heigth)
{
    buffer.width = width;
    buffer.height = heigth;
    // framebuffer configuration
    glBindFramebuffer(GL_FRAMEBUFFER, buffer.frameBufferId);
    glBindTexture(GL_TEXTURE_2D, buffer.textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, buffer.width, buffer.height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glBindRenderbuffer(GL_RENDERBUFFER, buffer.renderBufferId);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, buffer.width, buffer.height);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ClearRenderBuffer(const RenderBuffer& buffer, Color c)
{
    glBindFramebuffer(GL_FRAMEBUFFER, buffer.frameBufferId);
    glEnable(GL_DEPTH_TEST);
    glClearColor(c.r / 255.0f, c.g / 255.0f, c.b / 255.0f, c.a / 255.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, buffer.width, buffer.height);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SetProgramUniformV3f(const Program& program, const char* name, const float data[3])
{
    glUseProgram(program.id);
    glUniform3f(glGetUniformLocation(program.id, name), data[0], data[1], data[2]);
    glUseProgram(0);
}

void SetProgramUniformM4x4f(const Program& program, const char* name, const float data[16])
{
    glUseProgram(program.id);
    glUniformMatrix4fv(glGetUniformLocation(program.id, name), 1, GL_FALSE, data);
    glUseProgram(0);
}
