#ifndef ALGINE_TEXTURE_H
#define ALGINE_TEXTURE_H

#include <algine/types.h>
#include <GL/glew.h>
#include <string>
#include <map>
#include <tulz/macros.h>

#define COLOR_ATTACHMENT(n) (GL_COLOR_ATTACHMENT0 + n)
#define TEXTURE(n) (GL_TEXTURE0 + n)

namespace algine {
// TODO: remove this functions
void activeTexture(const uint &index);

void bindTexture2D(const uint &texture);

// Activate & Bind 2D texture
void texture2DAB(const uint &index, const uint &texture);

/**
 * Loading texture using stb_image.h
 * `path` - full path to texture
 */
uint loadTexture(const char *path, int internalformat = GL_RGB, uint format = GL_RGB, uint type = GL_UNSIGNED_BYTE);

size_t getTexComponentsCount(uint format);

/**
 * Reads whole texture
 */
float* getTexImage2D(uint texture, size_t width, size_t height, uint format);

/**
 * Reads whole texture
 * `target` - `GL_TEXTURE_CUBE_MAP_POSITIVE_X` and others
 */
float* getTexImageCube(GLenum target, GLuint texture, size_t width, size_t height, GLuint format);

/**
 * Reads pixels from framebuffer
 */
float* getPixelsFB(uint framebuffer, size_t x, size_t y, size_t width, size_t height, uint format);

/**
 * Reads pixels from texture
 */
float* getPixels(uint texture, size_t x, size_t y, size_t width, size_t height, uint format);

void saveTexImage(const float *image, size_t width, size_t height, size_t inComponents, const std::string &path, size_t outComponents, bool flip = true);

#define _implementVariadicCreate(T) \
template<typename...Args> \
static void create(Args&...args) { \
    T** arr[] = {&args...}; \
    for (usize i = 0; i < sizeof...(args); i++) \
        *arr[i] = new T(); \
}

#define _implementVariadicSetParams(Type) \
template<typename T, typename...Args> \
static void setParamsMultiple(const std::map<uint, T> &params, Args&...args) { \
    Type** arr[] = {&args...}; \
    for (usize i = 0; i < sizeof...(args); i++) { \
        (*arr[i])->bind(); \
        (*arr[i])->setParams(params); \
    } \
}

#define _implementVariadicDestroy(T) \
template<typename...Args> \
static void destroy(Args&...args) { \
    T** arr[] = {&args...}; \
    for (usize i = 0; i < sizeof...(args); i++) \
        deletePtr(*arr[i]); \
}

class Texture {
public:
    enum Params {
        MinFilter = GL_TEXTURE_MIN_FILTER,
        MagFilter = GL_TEXTURE_MAG_FILTER,
        WrapU = GL_TEXTURE_WRAP_S,
        WrapV = GL_TEXTURE_WRAP_T,
        WrapW = GL_TEXTURE_WRAP_R
    };

    enum Values {
        Nearest = GL_NEAREST,
        Linear = GL_LINEAR,
        ClampToEdge = GL_CLAMP_TO_EDGE,
        ClampToBorder = GL_CLAMP_TO_BORDER,
        Repeat = GL_REPEAT,
        MirroredRepeat = GL_MIRRORED_REPEAT,
        MirrorClampToEdge = GL_MIRROR_CLAMP_TO_EDGE
    };

    enum BaseFormats {
        DepthComponent = GL_DEPTH_COMPONENT,
        DepthStencil = GL_DEPTH_STENCIL,
        Red = GL_RED,
        RG = GL_RG,
        RGB = GL_RGB,
        RGBA = GL_RGBA
    };

    enum SizedFormats {
        Red8 = GL_R8,
        Red16 = GL_R16,
        RG8 = GL_RG8,
        RG16 = GL_RG16,
        RGB8 = GL_RGB8,
        RGBA8 = GL_RGBA8,
        RGBA16 = GL_RGBA16,
        Red16F = GL_R16F,
        RG16F = GL_RG16F,
        RGB16F = GL_RGB16F,
        RGBA16F = GL_RGBA16F,
        Red32F = GL_R32F,
        RG32F = GL_RG32F,
        RGB32F = GL_RGB32F,
        RGBA32F = GL_RGBA32F,
    };

    Texture();
    ~Texture();

    void bind();
    void use(uint slot); // activate + bind
    void setParams(const std::map<uint, uint> &params);
    void setParams(const std::map<uint, float> &params);

    /**
     * LOD - level of detail
     * Level 0 is the base image level. Level n is the nth mipmap reduction image
     */
    void setLOD(uint lod);

    /**
     * Specifies the number of color components and format of the texture
     * @param format
     */
    void setFormat(uint format);

    void setWidth(uint width);
    void setHeight(uint height);
    void setWidthHeight(uint width, uint height);

    // TODO: update and TextureCube are not very compatible, TextureCube has 6 targets, not 1 like Texture2D.
    //  Perhaps the 1st function should be made virtual, and the 2nd should be hidden?

    /// updates width / height, lod, format
    void update();

    /**
     * updates width / height, lod, format
     * @param dataFormat
     * @param dataType
     * @param data
     */
    void update(uint dataFormat, uint dataType, const void *data);

    // TODO: shader: rename > unbind
    void unbind();

    uint getLOD();
    uint getFormat();
    uint getWidth();
    uint getHeight();
    uint getId();

    _implementVariadicCreate(Texture)
    _implementVariadicSetParams(Texture)
    _implementVariadicDestroy(Texture)

    // TODO: remove
    static void create(uint *id);
    static void destroy(uint *id);

public:
    uint target = 0; // texture 2d, texture cube etc
    uint id = 0;
    uint
        lod = 0,
        format = RGB16F,
        width = 512,
        height = 512;

protected:
    explicit Texture(uint target);
    void texFromFile(const std::string &path, uint target, uint dataType = GL_UNSIGNED_BYTE, bool flipImage = true);
};

class Texture2D: public Texture {
public:
    Texture2D();

    void fromFile(const std::string &path, uint dataType = GL_UNSIGNED_BYTE, bool flipImage = true);
    static std::map<uint, uint> defaultParams();

    _implementVariadicCreate(Texture2D)
    _implementVariadicSetParams(Texture2D)
    _implementVariadicDestroy(Texture2D)
};

struct TextureCube: public Texture {
    enum Faces {
        Right = GL_TEXTURE_CUBE_MAP_POSITIVE_X,
        Left = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        Top = GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
        Bottom = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        Back = GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
        Front = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    };

    TextureCube();

    void fromFile(const std::string &path, uint face, uint dataType = GL_UNSIGNED_BYTE, bool flipImage = false);
    static std::map<uint, uint> defaultParams();

    _implementVariadicCreate(TextureCube)
    _implementVariadicSetParams(TextureCube)
    _implementVariadicDestroy(TextureCube)
};

#undef _implementVariadicCreate
#undef _implementVariadicSetParams
#undef _implementVariadicDestroy

}

#endif