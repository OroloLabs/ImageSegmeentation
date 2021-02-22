// Load jpegs and storing dataa
#include <stdio.h>
#include <jpeglib.h>

#include <string>
#include <vector>

namespace KMseg{

    //struct ImageObject;
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

    struct PixelObject{
        int cluster;
        unsigned char *R, *G, *B;
        PixelObject( unsigned char* red, unsigned char* green, unsigned char* blue )
            :R(red), G(green), B(blue) {cluster = 99;} // random larger number
    };

    class LoadDataset{
        //LoadDataset();
        //~LoadDataset();
        public:
        void WriteJpegIntoFile(ImageObject &IMAGEDETAILS, std::vector<PixelObject *> &PIXELS);
        ImageObject *ReadJpegIntoImageObject( const char *path );
    };
}
