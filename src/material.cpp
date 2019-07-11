#include <algine/material.h>

#include <GL/glew.h>
#include <nlohmann/json.hpp>
#include <algine/io.h>
#include <algine/constants.h>

namespace algine {
// struct Material
void Material::recycle() {
    glDeleteTextures(1, &ambientTexture);
    glDeleteTextures(1, &diffuseTexture);
    glDeleteTextures(1, &specularTexture);
    glDeleteTextures(1, &normalTexture);
    glDeleteTextures(1, &reflectionTexture);
    glDeleteTextures(1, &jitterTexture);
}

// struct AlgineMTL
bool AlgineMTL::load(const std::string &path) {
    if (!io::exists(path.c_str())) {
        std::cout << "AlgineMTL::load: " << path << " not found\n";
        return false;
    }
    /**
     * [
     *      {
     *          "name":"material name",
     *          -----------------------
     *          "reflectionTex":"/path/to/reflection/texture",
     *          "jitterTex":"jitter/texture"
     *          -----------------------
     *          "reflection":0.0,
     *          "jitter":1.0
     *      }
     * ]
     **/
    nlohmann::json j = nlohmann::json::parse(io::read(path));

    for (size_t i = 0; i < j.size(); i++) {
        // sie - set if exists
        #define sie(key, var) \
            if (j[i].find(key) != j[i].end()) var = j[i][key]

        AlgineMT amt;
        amt.name = j[i][ALGINE_MATERIAL_NAME];

        sie(ALGINE_MATERIAL_AMBIENT_TEX, amt.texAmbientPath);
        sie(ALGINE_MATERIAL_DIFFUSE_TEX, amt.texDiffusePath);
        sie(ALGINE_MATERIAL_SPECULAR_TEX, amt.texSpecularPath);
        sie(ALGINE_MATERIAL_NORMAL_TEX, amt.texNormalPath);
        sie(ALGINE_MATERIAL_REFLECTION_TEX, amt.texReflectionPath);
        sie(ALGINE_MATERIAL_JITTER_TEX, amt.texJitterPath);

        sie(ALGINE_MATERIAL_REFLECTION, amt.reflection);
        sie(ALGINE_MATERIAL_JITTER, amt.jitter);

        sie(ALGINE_MATERIAL_AMBIENT_STRENGTH, amt.ambientStrength);
        sie(ALGINE_MATERIAL_DIFFUSE_STRENGTH, amt.diffuseStrength);
        sie(ALGINE_MATERIAL_SPECULAR_STRENGTH, amt.specularStrength);
        sie(ALGINE_MATERIAL_SHININESS, amt.shininess);

        materials.push_back(amt);

        #undef sie
    }

    return true;
}

int AlgineMTL::forName(const std::string &name) {
    for (size_t i = 0; i < materials.size(); i++) if (materials[i].name == name) return i;
    return -1;
}

}