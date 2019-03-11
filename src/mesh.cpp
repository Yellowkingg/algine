#ifndef ALGINE_MESH_CPP
#define ALGINE_MESH_CPP

#include <vector>

#include <GL/glew.h>

#include <assimp/Importer.hpp>  // C++ importer interface
#include <assimp/scene.h>       // Output data structure

#include "material.cpp"

#define BUFF_COUNT 6

namespace algine {
struct Mesh {
    std::vector<float> vertices, normals, texCoords, tangents, bitangents;
    std::vector<GLuint> indices;
    Material mat;
    
    GLuint buffers[BUFF_COUNT] = { 0 };

    inline GLuint getVerticesBuffer() {
        return buffers[0];
    }

    inline GLuint getNormalsBuffer() {
        return buffers[1];
    }

    inline GLuint getTexCoordsBuffer() {
        return buffers[2];
    }

    inline GLuint getTangentsBuffer() {
        return buffers[3];
    }

    inline GLuint getBitangentsBuffer() {
        return buffers[4];
    }

    inline GLuint getIndicesBuffer() {
        return buffers[5];
    }

    void genBuffers() {
        glGenBuffers(BUFF_COUNT, buffers);

        #define mkArrayBuffer(buffIndex, array) \
            glBindBuffer(GL_ARRAY_BUFFER, buffers[buffIndex]); \
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * array.size(), &array[0], GL_STATIC_DRAW);

        mkArrayBuffer(0, vertices); // vertices
        if (normals.size() > 0) mkArrayBuffer(1, normals); // normals
        if (texCoords.size() > 0) mkArrayBuffer(2, texCoords); // texCoords
        if (tangents.size() > 0) mkArrayBuffer(3, tangents); // tangents
        if (bitangents.size() > 0) mkArrayBuffer(4, bitangents); // bitangents

        #undef mkArrayBuffer

        // indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[5]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW);

        // to default
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void delBuffers() {
        glDeleteBuffers(BUFF_COUNT, buffers);
    }

    void delMaterial() {
        mat.recycle();
    }

    void recycle() {
        delBuffers();
        delMaterial();
    }

    static Mesh processMesh(const aiMesh *aimesh, const aiScene *scene, AlgineMTL *amtl = nullptr) {
        Mesh mesh;

        for (size_t i = 0; i < aimesh->mNumVertices; i++) {
            // vertices
            mesh.vertices.push_back(aimesh->mVertices[i].x);
            mesh.vertices.push_back(aimesh->mVertices[i].y);
            mesh.vertices.push_back(aimesh->mVertices[i].z);
            
            // normals
            if (aimesh->HasNormals()) {
                mesh.normals.push_back(aimesh->mNormals[i].x);
                mesh.normals.push_back(aimesh->mNormals[i].y);
                mesh.normals.push_back(aimesh->mNormals[i].z);
            }

            // texCoords
            if (aimesh->HasTextureCoords(0)) {
                mesh.texCoords.push_back(aimesh->mTextureCoords[0][i].x);
                mesh.texCoords.push_back(aimesh->mTextureCoords[0][i].y);
            }

            // tangents and bitangents
            if (aimesh->HasTangentsAndBitangents()) {
                mesh.tangents.push_back(aimesh->mTangents[i].x);
                mesh.tangents.push_back(aimesh->mTangents[i].y);
                mesh.tangents.push_back(aimesh->mTangents[i].z);

                mesh.bitangents.push_back(aimesh->mBitangents[i].x);
                mesh.bitangents.push_back(aimesh->mBitangents[i].y);
                mesh.bitangents.push_back(aimesh->mBitangents[i].z);
            }
        }

        // faces
        for (size_t i = 0; i < aimesh->mNumFaces; i++) {
            for (size_t j = 0; j < aimesh->mFaces[i].mNumIndices; j++) {
                mesh.indices.push_back(aimesh->mFaces[i].mIndices[j]);
            }
        }

        // material
        aiString tmp_str;
        aiMaterial *aimat = scene->mMaterials[aimesh->mMaterialIndex];

        mesh.mat.name = aimat->GetName().C_Str();
        
        aimat->GetTexture(aiTextureType_AMBIENT, 0, &tmp_str);
        mesh.mat.texAmbientPath = tmp_str.C_Str();

        aimat->GetTexture(aiTextureType_DIFFUSE, 0, &tmp_str);
        mesh.mat.texDiffusePath = tmp_str.C_Str();

        aimat->GetTexture(aiTextureType_SPECULAR, 0, &tmp_str);
        mesh.mat.texSpecularPath = tmp_str.C_Str();

        aimat->GetTexture(aiTextureType_NORMALS, 0, &tmp_str);
        mesh.mat.texNormalPath = tmp_str.C_Str();

        aimat->Get(AI_MATKEY_SHININESS, mesh.mat.shininess);

        if (amtl != nullptr) {
            tmp_str = aimat->GetName();
            int index = amtl->forName(tmp_str.C_Str());
            if (index >= 0) {
                AlgineMT amt = amtl->materials[index];

                // algine specific material params
                mesh.mat.texReflectionPath = amt.texReflectionPath;
                mesh.mat.texJitterPath = amt.texJitterPath;
                mesh.mat.ambientStrength = amt.ambientStrength;
                mesh.mat.diffuseStrength = amt.diffuseStrength;
                mesh.mat.specularStrength = amt.specularStrength;
                mesh.mat.reflection = amt.reflection;
                mesh.mat.jitter = amt.jitter;

                // material params
                // sine - set if not empty
                #define sine(name) \
                    if (!amt.name.empty()) mesh.mat.name = amt.name
                sine(texAmbientPath);
                sine(texDiffusePath);
                sine(texSpecularPath);
                sine(texNormalPath);
                if (amt.shininess != -1) mesh.mat.shininess = amt.shininess;
                #undef sine
            }
        }

        // the result is undefined if shininess ≤ 0
        // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/pow.xhtml
        // FLT_EPSILON - min positive value for float
        if (mesh.mat.shininess == 0) mesh.mat.shininess = FLT_EPSILON;

        return mesh;
    }
};
}

#undef BUFF_COUNT

#endif /* ALGINE_MESH_CPP */