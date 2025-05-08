#pragma once
#include <lib/nw4r/g3d/res_mdl.hpp>

namespace nw4r {
    namespace g3d {
        class ResFile {
        public:
            ResFile(void *p = nullptr) : data(p) {}
            bool CheckRevision() const;
            void Init();
            void Bind(ResFile);

            nw4r::g3d::ResMdl GetResMdl(const char *name) const;

            void *data;
        };
    }
}
