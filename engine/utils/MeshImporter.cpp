#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Logging.h"
#include "MeshImporter.h"

namespace phi {
using namespace std;

unique_ptr<Mesh> MeshImporter::FromFile(const string &file, const string &name) {
    Assimp::Importer importer;
    auto scene = importer.ReadFile(file, aiProcess_Triangulate
                                                 | aiProcess_GenSmoothNormals);
    if (!scene) {
        PHI_LOG(ERROR, "Cannot import mesh '%s' [ %s ]", file.c_str(),
                importer.GetErrorString());
        return {};
    } else if (scene->mNumMeshes < 1) {
        PHI_LOG(WARNING, "File '%s' does not contain any meshes", file.c_str());
        return {};
    } else if (scene->mNumMeshes > 1) {
        PHI_LOG(WARNING, "File '%s' contains more than one mesh - only one of "
                         "the meshes will be loaded");
    }
    auto mesh = scene->mMeshes[0];
    assert(mesh->HasPositions());
    assert(mesh->HasNormals());
    auto result = make_unique<SimpleMesh>(name);

    glm::vec3 center{0,0,0};
    for (unsigned i = 0; i < mesh->mNumVertices; ++i) {
        center.x += mesh->mVertices[i].x;
        center.y += mesh->mVertices[i].y;
        center.z += mesh->mVertices[i].z;
    }
    center /= mesh->mNumVertices;

    for (unsigned i = 0; i < mesh->mNumFaces; ++i) {
        auto face = mesh->mFaces[i];
        SimpleMesh::Vertex v;
        for (unsigned j = 0; j < face.mNumIndices; ++j) {
            v.position.w = 1.0f;
            for (unsigned k = 0; k < 3; ++k) {
                v.position[k] = mesh->mVertices[face.mIndices[j]][k] - center[k];
                v.normal[k] = mesh->mNormals[face.mIndices[j]][k];
            }
            result->AppendVertex(v);
        }
    }
    PHI_LOG(INFO, "Loaded mesh '%s'", file.c_str());
    return move(result);
}

} // namespace phi
