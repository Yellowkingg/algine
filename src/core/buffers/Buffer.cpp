#include <algine/core/buffers/Buffer.h>

#include <algine/core/Engine.h>

#include <algine/templates.h>
#include <algine/gl.h>

#include <string>
#include <cassert>

#include "internal/SOP.h"

using namespace algine;
using namespace std;
using namespace tulz;

#ifdef ALGINE_SECURE_OPERATIONS
#include "internal/SOPConstants.h"
#define SOP_BOUND_PTR getBoundBuffer(m_target)
#define SOP_OBJECT_TYPE getBufferObject(m_target)
#define SOP_OBJECT_ID m_id
#define SOP_OBJECT_NAME getBufferObjectName(m_target)

inline void* getBoundBuffer(const uint target) {
    switch (target) {
        case GL_ARRAY_BUFFER:
            return Engine::getBoundArrayBuffer();
        case GL_ELEMENT_ARRAY_BUFFER:
            return Engine::getBoundIndexBuffer();
        case GL_UNIFORM_BUFFER:
            return Engine::getBoundUniformBuffer();
        default:
            assert(0);
    }
}

inline uint getBufferObject(const uint target) {
    switch (target) {
        case GL_ARRAY_BUFFER:
            return SOPConstants::ArrayBufferObject;
        case GL_ELEMENT_ARRAY_BUFFER:
            return SOPConstants::IndexBufferObject;
        case GL_UNIFORM_BUFFER:
            return SOPConstants::UniformBufferObject;
        default:
            assert(0);
    }
}

inline string getBufferObjectName(const uint target) {
    switch (target) {
        case GL_ARRAY_BUFFER:
            return SOPConstants::ArrayBufferStr;
        case GL_ELEMENT_ARRAY_BUFFER:
            return SOPConstants::IndexBufferStr;
        case GL_UNIFORM_BUFFER:
            return SOPConstants::UniformBufferStr;
        default:
            assert(0);
    }
}
#endif

namespace algine {
Buffer::Buffer() {
    glGenBuffers(1, &m_id);
}

Buffer::~Buffer() {
    glDeleteBuffers(1, &m_id);
}

void Buffer::bind() const {
    commitBinding()
    glBindBuffer(m_target, m_id);
}

void Buffer::unbind() const {
    checkBinding()
    commitUnbinding()
    glBindBuffer(m_target, 0);
}

void Buffer::setData(const uint size, const void *data, const uint usage) {
    checkBinding()
    glBufferData(m_target, size, data, usage);
}

void Buffer::updateData(const uint offset, const uint size, const void *data) {
    checkBinding()
    glBufferSubData(m_target, offset, size, data);
}

Array<byte> Buffer::getData(uint offset, uint size) {
    checkBinding()

    if (size == 0)
        return {};

    tulz::Array<byte> array(size);
    glGetBufferSubData(m_target, offset, size, array.array());

    return array;
}

uint Buffer::size() const {
    checkBinding()

    int size;
    glGetBufferParameteriv(m_target, GL_BUFFER_SIZE, &size);

    return size;
}

uint Buffer::getId() const {
    return m_id;
}

uint Buffer::getType() const {
    return m_target;
}

Buffer::Buffer(const uint target): Buffer() {
    this->m_target = target;
}
}