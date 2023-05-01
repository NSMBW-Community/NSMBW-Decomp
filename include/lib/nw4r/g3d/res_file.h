#pragma once

namespace nw4r {
    namespace g3d {
        class ResFile {
        public:
            bool CheckRevision() const;
            void Init();
            void Bind(ResFile);

            void *data;
        };
    }
}
