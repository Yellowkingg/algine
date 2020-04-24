#include <algine/texture/TextureTools.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

namespace algine {
namespace TextureTools {
void saveImage(const std::string &path, PixelData &pixelData, uint outComponents, bool flip) {
    const uint width = pixelData.width;
    const uint height = pixelData.width;

    auto *data = new ubyte[width * height * outComponents] {0};

    uint components = pixelData.componentsCount > outComponents ? outComponents : pixelData.componentsCount;

    for (size_t y = 0; y < height; y++) {
        for (size_t x = 0; x < width; x++) {
            for (size_t c = 0; c < components; c++) {
                int val = static_cast<int>(255 * pixelData.pixels[(y * width + x) * pixelData.componentsCount + c]);
                if (val > 255)
                    val = 255;
                if (val < 0)
                    val = 0;
                data[(y * width + x) * outComponents + c] = val;
            }
        }
    }

    stbi_flip_vertically_on_write(flip);
    stbi_write_bmp(path.c_str(), width, height, outComponents, data);

    delete[] data;
}
}
}
