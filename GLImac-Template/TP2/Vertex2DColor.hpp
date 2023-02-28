#include <glimac/glm.hpp>

class Vertex2DColor {
    public:
        glm::vec2 position;
        glm::vec3 color;

    public:
        Vertex2DColor() = default;

        Vertex2DColor(glm::vec2 pos, glm::vec3 col)
            : position(pos), color(col){};//{ position.x *= 1 / aspectRatio; };
};