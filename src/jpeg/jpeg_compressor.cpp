/*
 * jpeg_compressor.cpp
 *
 *  Created on: 7 июля 2018 г.
 *      Author: shmykov
 */

#include "jpeg_compressor.h"
#include <stdexcept>
#include <iostream>

Jpeg::Compressor::Compressor()
{
    m_OutputFile = NULL;
    memset(&m_CompressInfo, 0, sizeof(m_CompressInfo));
    memset(&m_JpegError, 0, sizeof(m_JpegError));
    m_isStartCompress = false;
    m_Quality = defaultQuality;
}

Jpeg::Compressor::~Compressor()
{
    Close();
}

void Jpeg::Compressor::Open(const char *filename)
{
    m_OutputFile = fopen(filename, "wb");
    if (m_OutputFile == NULL) {
        std::string errorMessage;

        errorMessage.append("Filed open file: ");
        errorMessage.append(filename);
        throw std::runtime_error(errorMessage);
    }

    PrepareErrorHandling();
    SetupJpegCompress();
}

void Jpeg::Compressor::Close()
{
    if (m_isStartCompress) {
        TearDownJpegCompress();
        m_isStartCompress = false;
    }

    /* After finish_compress, we can close the output file. */
    if (m_OutputFile) {
        fclose(m_OutputFile);
        m_OutputFile = NULL;
    }

    /* Step 7: release JPEG compression object */

    /* This is an important step since it will release a good deal of memory. */
    jpeg_destroy_compress(&m_CompressInfo);

    memset(&m_CompressInfo, 0, sizeof(m_CompressInfo));
    memset(&m_JpegError, 0, sizeof(m_JpegError));
}

void Jpeg::Compressor::WriteRow(JSAMPROW row)
{
    if (!m_isStartCompress) {
        jpeg_set_defaults(&m_CompressInfo);
        /* Now you can set any non-default parameters you wish to.
         * Here we just illustrate the use of quality (quantization table) scaling:
         */
        jpeg_set_quality(&m_CompressInfo, m_Quality, TRUE /* limit to baseline-JPEG values */);

        /* Step 4: Start compressor */

        /* TRUE ensures that we will write a complete interchange-JPEG file.
         * Pass TRUE unless you are very sure of what you're doing.
         */
        jpeg_start_compress(&m_CompressInfo, TRUE);

        m_isStartCompress = true;
    }

    jpeg_write_scanlines(&m_CompressInfo, &row, 1);
}

void Jpeg::Compressor::SetWidth(JDIMENSION width)
{
    if (m_isStartCompress) {
        throw std::runtime_error("Failed set width, when compress started");
    }

    m_CompressInfo.image_width = width;     /* image width and height, in pixels */
}

void Jpeg::Compressor::SetHeight(JDIMENSION height)
{
    if (m_isStartCompress) {
        throw std::runtime_error("Failed set height, when compress started");
    }

    m_CompressInfo.image_height = height;
}

void Jpeg::Compressor::SetColorComponents(JDIMENSION colorComponents, J_COLOR_SPACE colorSpace)
{
    if (m_isStartCompress) {
        throw std::runtime_error("Failed set color components, when compress started");
    }

    m_CompressInfo.input_components = colorComponents; /* # of color components per pixel */
    m_CompressInfo.in_color_space = colorSpace;     /* colorsspace of input image */
}

void Jpeg::Compressor::SetQuality(int quality)
{
    if (m_isStartCompress) {
        throw std::runtime_error("Failed set quality, when compress started");
    }

    m_Quality = quality;
}

void Jpeg::Compressor::SetupJpegCompress()
{
    /* Now we can initialize the JPEG compression object. */
    jpeg_create_compress(&m_CompressInfo);

    jpeg_stdio_dest(&m_CompressInfo, m_OutputFile);
}

void Jpeg::Compressor::TearDownJpegCompress()
{
    jpeg_finish_compress(&m_CompressInfo);
}

void Jpeg::Compressor::PrepareErrorHandling()
{
    m_CompressInfo.err = jpeg_std_error(&m_JpegError);
    m_JpegError.error_exit = ErrorHandler;
}

void Jpeg::Compressor::ErrorHandler(j_common_ptr cinfo)
{
    char jpegLastErrorMsg[JMSG_LENGTH_MAX];
    /* Create the message */
    (*( cinfo->err->format_message))(cinfo, jpegLastErrorMsg);

    throw std::runtime_error(jpegLastErrorMsg);
}
