#include <glimac/glm.hpp>

class Vertex2Duv {
    public:
        glm::vec2 position;
        glm::vec2 uv;

    public:
        Vertex2Duv() = default;

        Vertex2Duv(glm::vec2 pos, glm::vec2 uv)
            : position(pos), uv(uv){};//{ position.x *= 1 / aspectRatio; };
};