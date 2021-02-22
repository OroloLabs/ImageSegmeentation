// read/write jpegs and storing data
#include <stdio.h>
#include <jpeglib.h>   // read image
#include <setjmp.h>    // write iamge


#include <string>
#include <iostream>
#include <vector>
#include "LoadDataset.h"

namespace KMseg{

    /***************************************************
        Storing image data and characteristics in a struct
    ****************************************************/
    /*
    struct ImageObject {
        unsigned int Height, Width, NumChannels;
        std::string ImageName;
        unsigned long ImageDataSize;
        std::vector<unsigned char> Data;
        //unsigned char data[H*W]; // how ot implement arrray
        ImageObject(int H, int W, int NC, std::string imagename, std::vector<unsigned char> vec)
                :Height(H), Width(W), NumChannels(NC), ImageName(imagename), Data(vec) 
        {
            ImageDataSize = Height * Width * NumChannels;
        }
        
    };
    */
    //LoadDataset::LoadDataset();
    //LoadDataset::~LoadDataset(){ delete[] ImageObject; }

    /***************************************************
        To read a jpg image file 
    ****************************************************/
    ImageObject *LoadDataset::ReadJpegIntoImageObject( const char *path ){
        FILE *file = fopen( path, "rb" );
        if ( file == NULL ){
            return NULL;
        }

        struct jpeg_decompress_struct info;      //for our jpeg info
        struct jpeg_error_mgr err;               //the error handler

        info.err = jpeg_std_error( &err );     
        jpeg_create_decompress( &info );         //fills info structure

        jpeg_stdio_src( &info, file );    
        jpeg_read_header( &info, true );

        jpeg_start_decompress( &info );

        unsigned int w = info.output_width;
        unsigned int h = info.output_height;
        unsigned int numChannels = info.num_components; // 3 = RGB, 4 = RGBA
        unsigned long dataSize = w * h * numChannels;

        // read RGB(A) scanlines one at a time into jdata[]
        unsigned char *data = (unsigned char *)malloc( dataSize );
        unsigned char* rowptr;
        while ( info.output_scanline < h ){
            rowptr = data + info.output_scanline * w * numChannels;
            jpeg_read_scanlines( &info, &rowptr, 1 );
        }

        /********** To read a jpg image file ****************/
        std::vector<unsigned char> imagedata;
        for(int i = 0; i < dataSize;i++) 
            imagedata.push_back( data[i] );

        /***************************************************/

        jpeg_finish_decompress( &info );    
        fclose( file );
        //printf("Image read\n");  
        return new ImageObject(h, w, numChannels, path, imagedata);
        //return Image;
    };

    /***************************************************
        To write a jpg image file 
    ****************************************************/
    //void write_JPEG_file (char * filename, int quality){
    void LoadDataset::WriteJpegIntoFile(ImageObject &IMAGEDETAILS, std::vector<PixelObject *> &PIXELS){

        std::string name = "KMSeg_"+IMAGEDETAILS.ImageName;
        char *filename = &name[0];
        int quality    = 100;

        struct jpeg_compress_struct cinfo;    // Basic info for JPEG properties.
        struct jpeg_error_mgr jerr;           // In case of error.
        FILE * outfile;                       // Target file.
        JSAMPROW row_pointer[1];              // Pointer to JSAMPLE row[s].
        int row_stride;                       // Physical row width in image buffer.

        //## ALLOCATE AND INITIALIZE JPEG COMPRESSION OBJECT

        cinfo.err = jpeg_std_error(&jerr);
        jpeg_create_compress(&cinfo);

        //## OPEN FILE FOR DATA DESTINATION:

        if ((outfile = fopen(filename, "wb")) == NULL) {
            fprintf(stderr, "ERROR: can't open %s\n", filename);
            exit(1);
        }
        jpeg_stdio_dest(&cinfo, outfile);

        //## SET PARAMETERS FOR COMPRESSION:

        cinfo.image_width  = IMAGEDETAILS.Width;             // |-- Image width and height in pixels.
        cinfo.image_height = IMAGEDETAILS.Height;            // |
        cinfo.input_components = IMAGEDETAILS.NumChannels;   // Number of color components per pixel.
        cinfo.in_color_space = JCS_RGB;                      // Colorspace of input image as RGB.

        jpeg_set_defaults(&cinfo);
        jpeg_set_quality(&cinfo, quality, TRUE);


        //## CREATE IMAGE BUFFER TO WRITE FROM AND MODIFY THE IMAGE TO LOOK LIKE CHECKERBOARD:

        /********** To read a jpg image file ****************/
        unsigned char *image_buffer = NULL;
        image_buffer = (unsigned char*)malloc(cinfo.image_width*cinfo.image_height*cinfo.num_components);

        for (int i=0; i < PIXELS.size(); i++){
            image_buffer[(i*3)+0] = *PIXELS[i]->R; 
            image_buffer[(i*3)+1] = *PIXELS[i]->G; 
            image_buffer[(i*3)+2] = *PIXELS[i]->B;

            //image_buffer[(i*3)+0] =imagedata[(i*3)+0];
            //image_buffer[(i*3)+1] =imagedata[(i*3)+1];
            //image_buffer[(i*3)+2] =imagedata[(i*3)+2];

        }

        /***************************************************/
        

        //## START COMPRESSION:

        jpeg_start_compress(&cinfo, TRUE);
        row_stride = cinfo.image_width * 3;        // JSAMPLEs per row in image_buffer

        while (cinfo.next_scanline < cinfo.image_height){
            row_pointer[0] = &image_buffer[cinfo.next_scanline * row_stride];
            (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
        }
                // NOTE: jpeg_write_scanlines expects an array of pointers to scanlines.
                //       Here the array is only one element long, but you could pass
                //       more than one scanline at a time if that's more convenient.

        //## FINISH COMPRESSION AND CLOSE FILE:

        jpeg_finish_compress(&cinfo);
        fclose(outfile);
        jpeg_destroy_compress(&cinfo);

        printf("Image written\n");  
    };





}
