/*
 * jpeg_decompressor.cpp
 *
 *  Created on: 7 июля 2018 г.
 *      Author: shmykov
 */

#include "jpeg_decompressor.h"
#include <stdexcept>
#include <iostream>

Jpeg::Decompressor::Decompressor()
{
    m_InputFile = NULL;
    memset(&m_DecompressInfo, 0, sizeof(m_DecompressInfo));
    memset(&m_JpegError, 0, sizeof(m_JpegError));
    m_isStartDecompress = false;
    m_OutputBuffer = NULL;
}

Jpeg::Decompressor::~Decompressor()
{
    Close();
}

void Jpeg::Decompressor::Open(const char *filename)
{
    m_InputFile = fopen(filename, "rb");
    if (m_InputFile == NULL) {
        std::string errorMessage;

        errorMessage.append("Filed open file: ");
        errorMessage.append(filename);
        throw std::runtime_error(errorMessage);
    }

    PrepareErrorHandling();
    SetupJpegDecompress();
}

void Jpeg::Decompressor::Close()
{
    if (m_isStartDecompress) {
        TearDownJpegDecompress();
        /* This is an important step since it will release a good deal of memory. */
        jpeg_destroy_decompress(&m_DecompressInfo);

        m_isStartDecompress = false;
    }

    /* close file */
    if (m_InputFile) {
        fclose(m_InputFile);
        m_InputFile = NULL;
    }

    if (m_OutputBuffer) {
        /* NOT need to free, buffer, if
         * we allocate buffer with JPOOL_IMAGE flag
         * buffer was free in call jpeg_destroy_decompress()
         */
        m_OutputBuffer = NULL;
    }

    memset(&m_DecompressInfo, 0, sizeof(m_DecompressInfo));
    memset(&m_JpegError, 0, sizeof(m_JpegError));
}

bool Jpeg::Decompressor::Eof() const
{
    return m_DecompressInfo.output_scanline >= m_DecompressInfo.output_height;
}

JSAMPROW Jpeg::Decompressor::ReadRow()
{
    if (!m_isStartDecompress) {
        m_isStartDecompress = true;
    }

    if (Eof()) {
        throw std::runtime_error("End of file!");
    }

    jpeg_read_scanlines(&m_DecompressInfo, m_OutputBuffer, 1);

    return m_OutputBuffer[0];
}

JDIMENSION Jpeg::Decompressor::GetWidth() const
{
    return m_DecompressInfo.output_width;
}

JDIMENSION Jpeg::Decompressor::GetHeight() const
{
    return m_DecompressInfo.output_height;
}

JDIMENSION Jpeg::Decompressor::GetColorComponents() const
{
    return m_DecompressInfo.output_components;
}

void Jpeg::Decompressor::SetupJpegDecompress()
{
    JDIMENSION row_stride = 0;

    /* Now we can initialize the JPEG decompression object. */
    jpeg_create_decompress(&m_DecompressInfo);

    /* Step 2: specify data source (eg, a file) */

    jpeg_stdio_src(&m_DecompressInfo, m_InputFile);

    /* Step 3: read file parameters with jpeg_read_header() */

    jpeg_read_header(&m_DecompressInfo, TRUE);
    /* We can ignore the return value from jpeg_read_header since
     *   (a) suspension is not possible with the stdio data source, and
     *   (b) we passed TRUE to reject a tables-only JPEG file as an error.
     * See libjpeg.doc for more info.
     */

    /* Step 4: set parameters for decompression */

    /* In this example, we don't need to change any of the defaults set by
     * jpeg_read_header(), so we do nothing here.
     */

    /* Step 5: Start decompressor */

    jpeg_start_decompress(&m_DecompressInfo);
    /* We can ignore the return value since suspension is not possible
     * with the stdio data source.
     */

    /* We may need to do some setup of our own at this point before reading
     * the data.  After jpeg_start_decompress() we have the correct scaled
     * output image dimensions available, as well as the output colormap
     * if we asked for color quantization.
     * In this example, we need to make an output work buffer of the right size.
     */
    /* JSAMPLEs per row in output buffer */

    row_stride = m_DecompressInfo.output_width * m_DecompressInfo.output_components;
    /* Make a one-row-high sample array that will go away when done with image */
    m_OutputBuffer = (*m_DecompressInfo.mem->alloc_sarray)
                     ((j_common_ptr) &m_DecompressInfo, JPOOL_IMAGE, row_stride, 1);
}

void Jpeg::Decompressor::TearDownJpegDecompress()
{
    /* Step 7: Finish decompression */
    jpeg_finish_decompress(&m_DecompressInfo);
}

void Jpeg::Decompressor::PrepareErrorHandling()
{
    m_DecompressInfo.err = jpeg_std_error(&m_JpegError);
    m_JpegError.error_exit = ErrorHandler;
}

void Jpeg::Decompressor::ErrorHandler(j_common_ptr cinfo)
{
    char jpegLastErrorMsg[JMSG_LENGTH_MAX];
    /* Create the message */
    (*( cinfo->err->format_message))(cinfo, jpegLastErrorMsg);

    throw std::runtime_error(jpegLastErrorMsg);
}
