#include <cstdio>
#include <cstring>
#include <deque>
#include <string>
#include <optional>
#include <stb_image.h>
#include "image.h"
#include "encode.h"

bool verbose = false;
int qFactor = 1;

std::string getFilename(const std::string &name) {
    size_t begin = 0, end = name.length();

    size_t slash = name.find_last_of("/\\");
    if (slash != std::string::npos) begin = slash + 1;

    size_t dot = name.find_last_of('.');
    if (dot != std::string::npos) end = dot;

    return name.substr(begin, end - begin);
}

template<typename T>
bool putFileContents(const std::string &name, const std::vector<T> &contents) {
    FILE *f = fopen(name.c_str(), "wb");
    if (!f) return false;

    fwrite(&contents[0], sizeof(T), contents.size(), f);

    fclose(f);

    return true;
}

std::optional<bitstream_t> convertFrame(const std::string &path, bool colorMode, bool padding) {
    image_t img;
    img.data = stbi_load(path.c_str(), &img.width, &img.height, nullptr, img.channels);
    if (img.data == nullptr) {
        printf("Unable to load %s (%s)\n", path.c_str(), stbi_failure_reason());
        return {};
    }

    int mustBeMultipleOf = 8;
    if (colorMode) mustBeMultipleOf = 16;

    if (img.width % mustBeMultipleOf != 0 || img.height % mustBeMultipleOf != 0) {
        printf("Dimensions of the input file must be a multiple of %d (%dx%d given).\n", mustBeMultipleOf, img.width,
               img.height);
        return {};
    }

    bitstream_t bitstream;

    if (colorMode) {
        bitstream = encodeImageToColorBitstream(img);
    } else {
        bitstream = encodeImageToMonoBitstream(img);
    }

    stbi_image_free(img.data);

    if (padding) {
        bitstream.align(0x20 * 4);
    }

    if (verbose) printf("Converted %dx%d image to MDEC stream (%lu bytes, qFactor=%d).\n", img.width, img.height,
           bitstream.size() * 2, qFactor);

    return bitstream;
}

int main(int argc, char **argv) {
    std::deque<std::string> args;
    for (int i = 1; i < argc; i++) {
        args.emplace_back(argv[i]);
    }

    if (args.empty()) {
        printf("usage: mdec-tools input.png [output.mdec]\n"
               "options:\n"
               " --verbose    - Verbose mode (default - off)\n"
               " --color      - Encode as color (Cr, Cb + 4*Y) (default - mono)\n"
               " --no-padding - Disable padding to 32 * 32bit (default - pad)\n"
               " -q=qFactor   - Set quantization factor (default - 1)\n"
               " --movie      - convert multiple input frames and output them into \n"
               "                a single binary (offset and sizes in stdout)\n"
        );
        return 1;
    }

    bool padding = true;
    bool colorMode = false;
    bool movieMode = false;
    // Find flags here
    for (auto it = args.begin(); it != args.end();) {
        if (*it == "--verbose") {
            verbose = true;
            it = args.erase(it);
        } else if (*it == "--color") {
            colorMode = true;
            it = args.erase(it);
        } else if (*it == "--no-padding") {
            padding = false;
            it = args.erase(it);
        } else if (*it == "--movie") {
            movieMode = true;
            it = args.erase(it);
        } else if (it->rfind("-q=") == 0) {
            auto svalue = it->substr(it->find('=') + 1);
            qFactor = std::stoi(svalue);
            if (qFactor < 1 || qFactor > 63) {
                printf("Invalid qFactor %d, defaulting to 1\n", qFactor);
                qFactor = 1;
            }
            it = args.erase(it);
        } else {
            it++;
        }
    }

    if (args.size() < 1 || (!movieMode && args.size() > 2)) {
        printf("Invalid argument count - expected input and output path\n");
        return 1;
    }
    std::string dstPath;

    if (!movieMode) {
        std::string srcPath = args.front();
        args.pop_front();

        if (!args.empty()) {
            dstPath = args.front();
            args.pop_front();
        } else {
            dstPath = getFilename(srcPath) + ".mdec";
        }

        if (srcPath == dstPath) {
            printf("srcPath == dstPath, exiting.\n");
            return 1;
        }

        auto bitstream = convertFrame(srcPath, colorMode, padding);
        if (!bitstream) return 1;

        putFileContents(dstPath, *bitstream);
    } else {
        dstPath = args.back();
        args.pop_back();

        if (args.empty()) {
            printf("No input frames provided.\n");
            return 1;
        }

        FILE* fout = fopen(dstPath.c_str(), "wb");

        struct frame_t {
            size_t offset;
            size_t size;
        };
        std::vector<frame_t> frames;

        bitstream_t movie;
        for (auto &in : args) {
            auto frame = convertFrame(in, colorMode, padding);
            if (!frame) return 1;

            frames.push_back(frame_t{
                    .offset = movie.size()*2,
                    .size = frame->size()*2
            });
            movie += *frame;
        }

        printf("Movie size %d kBytes\n", movie.size()*2);

        fprintf(fout, "#pragma once\n");
        fprintf(fout, "#include <stddef.h>\n");
        fprintf(fout,
                "struct frame_t {\n"
                "    size_t offset;\n"
                "    size_t size;\n"
                "};\n"
        );
        fprintf(fout, "constexpr size_t FRAME_COUNT = %u;\n", (int)frames.size());
        fprintf(fout, "frame_t frames[FRAME_COUNT] = {\n");
        for (auto& frame: frames) {
            fprintf(fout, "    { 0x%x, 0x%x },\n", (int)frame.offset, (int)frame.size);
        }
        fprintf(fout, "};\n");

        fprintf(fout, "uint8_t movieBitstream[%u] = {\n", movie.size()*2);
        for (int i = 0; i< movie.size(); i++) {
            uint16_t word = movie[i];
            fprintf(fout, "0x%02x, 0x%02x, ", word & 0xff, (word >> 8) & 0xff);
            if ((i+1) % 8 == 0) {
                fprintf(fout, "\n");
            }
        }
        fprintf(fout, "};\n");
        fclose(fout);
    }

    return 0;
}
