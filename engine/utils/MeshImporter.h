#ifndef PHI_UTILS_MESH_IMPORTER_H
#define PHI_UTILS_MESH_IMPORTER_H
#include <memory>
#include <string>

#include "scene/Mesh.h"

namespace phi {

struct MeshImporter {
    static std::unique_ptr<phi::Mesh> FromFile(const std::string &file);
};

} // namespace phi

#endif
