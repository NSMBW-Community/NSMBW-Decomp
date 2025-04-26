#pragma once

// Contains definitions of classes whose structure has not been analyzed yet,
// but are still required, e.g. for function signatures.

// Please remove these classes and the include as soon as they are properly defined!

namespace nw4r {
namespace lyt {

namespace res {
    class Pane;
    class Picture;
    class TextBox;
    class Window;
}

class AnimTransformBasic;
class AnimResource;
class AnimTransform;
class AnimationLink;

class GroupContainer;

class ResBlockSet;

class Material;

class TexMap;

class WindowFrameSize;

} // namespace lyt

namespace ut {
    class Font;

    template <class T, int N>
    class LinkList {

    };

    template <typename T>
    class TagProcessorBase;
}

} // namespace nw4r

namespace m2d {

class AnmResV2_c;
class AnmGroup_c;

} // namespace m2d
