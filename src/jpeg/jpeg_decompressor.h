/*
 * jpeg_decompressor.h
 *
 *  Created on: 7 июля 2018 г.
 *      Author: shmykov
 */

#ifndef JPEG_DECOMPRESSOR_H_
#define JPEG_DECOMPRESSOR_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

extern "C" {

#include "jpeglib.h"

}

namespace Jpeg {

class Decompressor {
public:
    Decompressor();
    ~Decompressor();

    /* deny copy */
    Decompressor(const Decompressor &right);
    ssize_t operator = (const Decompressor &right);


    void Open(const char *filename);
    void Close();

    bool Eof() const; /* check, this we read all file */
    JSAMPROW ReadRow();

    JDIMENSION GetWidth() const; /* scaled image width */
    JDIMENSION GetHeight() const; /* scaled image height */
    JDIMENSION GetColorComponents() const; /* # of color components returned */

protected:
    void SetupJpegDecompress();
    void TearDownJpegDecompress();

    void PrepareErrorHandling();
    static void ErrorHandler(j_common_ptr cinfo);

private:
    FILE *m_InputFile; /* source file */
    struct jpeg_decompress_struct m_DecompressInfo;
    struct jpeg_error_mgr m_JpegError;
    bool m_isStartDecompress;
    JSAMPARRAY m_OutputBuffer; /* Output row buffer */
};

} /* end namespace Jpeg */


#endif /* JPEG_DECOMPRESSOR_H_ */
