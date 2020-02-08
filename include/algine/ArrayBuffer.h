#ifndef ALGINE_ARRAYBUFFER_H
#define ALGINE_ARRAYBUFFER_H

#include <algine/Buffer.h>
#include <algine/templates.h>
#include <tulz/macros.h>

namespace algine {
class ArrayBuffer: public Buffer {
public:
    ArrayBuffer();

    implementVariadicCreate(ArrayBuffer)
    implementVariadicDestroy(ArrayBuffer)
};
}

#endif //ALGINE_ARRAYBUFFER_H
