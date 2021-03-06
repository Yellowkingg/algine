#include <algine/core/texture/Texture2DManager.h>

#include <algine/core/texture/Texture2D.h>
#include <algine/core/JsonHelper.h>

#include <tulz/Path.h>

#include "internal/ConfigStrings.h"
#include "internal/PublicObjectTools.h"

using namespace std;
using namespace tulz;
using namespace nlohmann;
using namespace algine::internal;

namespace algine {
Texture2DManager::Texture2DManager() {
    m_type = TextureManager::Type::Texture2D;
    m_defaultParams = Texture2D::defaultParams();
}

void Texture2DManager::setPath(const string &path) {
    m_path = path;
}

std::string Texture2DManager::getPath() const {
    return m_path;
}

Texture2DPtr Texture2DManager::get() {
    return PublicObjectTools::getPtr<Texture2DPtr>(this);
}

Texture2DPtr Texture2DManager::create() {
    if (m_type != TextureManager::Type::Texture2D)
        throw runtime_error("Invalid texture type. Use a different manager");

    Texture2DPtr texture = make_shared<Texture2D>();
    texture->setName(m_name);
    texture->setFormat(m_format);

    texture->bind();

    if (!m_path.empty()) {
        texture->fromFile(Path::join(m_workingDirectory, m_path), m_dataType);
    } else {
        texture->setDimensions(m_width, m_height);
        texture->update();
    }

    texture->setParams(m_params.empty() ? m_defaultParams : m_params);

    texture->unbind();

    PublicObjectTools::postCreateAccessOp("Texture2D", this, texture);

    return texture;
}

void Texture2DManager::import(const JsonHelper &jsonHelper) {
    using namespace Config;

    const json &config = jsonHelper.json;

    if (config.contains(File) && config[File].contains(Path))
        m_path = config[File][Path];

    TextureManager::import(jsonHelper);
}

JsonHelper Texture2DManager::dump() {
    JsonHelper config;

    if (!m_path.empty())
        config.json[Config::File][Config::Path] = m_path;

    m_writeFileSection = !m_path.empty();

    config.append(TextureManager::dump());

    return config;
}
}
