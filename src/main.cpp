/*
 * main.cpp
 *
 *  Created on: 7 июля 2018 г.
 *      Author: shmykov
 */

#include <iostream>
#include <stdexcept>

#include "jpeg_decompressor.h"
#include "jpeg_compressor.h"

int main(int argc, char* argv[]) {
    const char commandLineHelpMessage[] = "Uasage: jpeg_again_compress <src> <dst> [ quality ]";

    if (argc != 3) {
        std::cout << commandLineHelpMessage << std::endl;
        return 0;
    }

    try {
        Jpeg::Decompressor decompressor;
        Jpeg::Compressor compressor;

        decompressor.Open(argv[1]);
        compressor.Open(argv[2]);

        compressor.SetColorComponents(decompressor.GetColorComponents(), JCS_RGB);
        compressor.SetHeight(decompressor.GetHeight());
        compressor.SetWidth(decompressor.GetWidth());
        compressor.SetQuality(1);

        while (!decompressor.Eof()) {
            compressor.WriteRow(decompressor.ReadRow());
        }

    } catch (std::runtime_error &err) {
        std::cout << "ERROR: " << err.what() << std::endl;
    } catch (...) {
        std::cout << "Undefined error" << std::endl;
    }

    return 0;
}
