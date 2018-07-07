/*
 * jpeg_compressor.h
 *
 *  Created on: 7 июля 2018 г.
 *      Author: shmykov
 */

#ifndef JPEG_COMPRESSOR_H_
#define JPEG_COMPRESSOR_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

extern "C" {

#include "jpeglib.h"

}

namespace Jpeg {

class Compressor {
public:
    static const int defaultQuality = 1024;

    Compressor();
    ~Compressor();

    /* deny copy */
    Compressor(const Compressor &right);
    ssize_t operator = (const Compressor &right);

    void Open(const char *filename);
    void Close();

    void WriteRow(JSAMPROW row);

    void SetWidth(JDIMENSION width); /* set scaled image width */
    void SetHeight(JDIMENSION height); /* set scaled image height */
    void SetColorComponents(JDIMENSION colorComponents, J_COLOR_SPACE colorSpace); /* set # of color components, color space */
    void SetQuality(int quality);

protected:
    void SetupJpegCompress();
    void TearDownJpegCompress();

    void PrepareErrorHandling();
    static void ErrorHandler(j_common_ptr cinfo);

private:
    FILE *m_OutputFile; /* target file */
    struct jpeg_compress_struct m_CompressInfo;
    struct jpeg_error_mgr m_JpegError;
    bool m_isStartCompress;
    int m_Quality;
};

} /* end namespace Jpeg */

#endif /* JPEG_COMPRESSOR_H_ */
