#include <algine/core/Renderbuffer.h>

#include <algine/core/texture/Texture.h>
#include <algine/core/Engine.h>

#include <algine/gl.h>

#define SOP_BOUND_PTR Engine::getBoundRenderbuffer()
#define SOP_OBJECT_TYPE SOPConstants::RenderbufferObject
#define SOP_OBJECT_ID m_id
#define SOP_OBJECT_NAME SOPConstants::RenderbufferStr
#include "internal/SOP.h"
#include "internal/SOPConstants.h"

#include "internal/PublicObjectTools.h"

using namespace algine::internal;

namespace algine {
vector<RenderbufferPtr> Renderbuffer::publicObjects;

Renderbuffer::Renderbuffer()
    : m_id(),
      m_format(),
      m_width(),
      m_height()
{
    glGenRenderbuffers(1, &m_id);
}

Renderbuffer::~Renderbuffer() {
    glDeleteRenderbuffers(1, &m_id);
}

void Renderbuffer::bind() {
    commitBinding()
    glBindRenderbuffer(GL_RENDERBUFFER, m_id);
}

void Renderbuffer::setFormat(uint format) {
    m_format = format;
}

void Renderbuffer::setWidth(uint width) {
    m_width = width;
}

void Renderbuffer::setHeight(uint height) {
    m_height = height;
}

void Renderbuffer::setDimensions(uint width, uint height) {
    m_width = width;
    m_height = height;
}

void Renderbuffer::update() {
    checkBinding()
    glRenderbufferStorage(GL_RENDERBUFFER, m_format, m_width, m_height);
}

void Renderbuffer::unbind() {
    checkBinding()
    commitUnbinding()
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

uint Renderbuffer::getFormat() const {
    return m_format;
}

uint Renderbuffer::getWidth() const {
    return m_width;
}

uint Renderbuffer::getHeight() const {
    return m_height;
}

uint Renderbuffer::getId() const {
    return m_id;
}

RenderbufferPtr Renderbuffer::getByName(const string &name) {
    return PublicObjectTools::getByName<RenderbufferPtr>(name);
}

Renderbuffer* Renderbuffer::byName(const string &name) {
    return PublicObjectTools::byName<Renderbuffer>(name);
}
}
