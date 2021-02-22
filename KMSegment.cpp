#include <iostream>
#include <string>
#include <vector>

// Generate Centriods
#include <cstdlib>
#include <limits>
#include <random>

// Euclid Distance
#include <math.h>         // std::sqrt()
#include <cmath>          // std::abs()
#include "LoadDataset.h" 

// load folder
#include <dirent.h>
#include <sys/types.h>


namespace KMseg{
    /***************************************************
        Global varibles 
    ****************************************************/
    
    /*
    struct PixelObject{
        int cluster;
        unsigned char *R, *G, *B;
        PixelObject( unsigned char* red, unsigned char* green, unsigned char* blue )
            :R(red), G(green), B(blue) {cluster = 100;}
    };
    */

    ImageObject *OriginalImageDetails;            // image object data
    LoadDataset RAWDATA;                         // functions to load image
    std::vector<PixelObject *> PixelImage;
    std::vector<PixelObject *> CentroidStack;
    /***************************************************
       Initiate data - load image and prep for segmentation 
    ****************************************************/
    void Load(const char *filename){
        OriginalImageDetails = RAWDATA.ReadJpegIntoImageObject( filename );
        for(int i= 0; i < OriginalImageDetails ->Data.size(); i+=3){           
            PixelObject *pixel = new PixelObject(&OriginalImageDetails->Data[i+0], 
                                                 &OriginalImageDetails->Data[i+1], 
                                                 &OriginalImageDetails->Data[i+2]);
            pixel->cluster = 100; // random
            PixelImage.push_back( pixel );
        }
    }

    /***************************************************
        writign image and printing pixels 
    ****************************************************/
    void PRINT(int x){
        if( x == 1 ){
            for (auto & pixel : PixelImage){
                std::cout << (int)*pixel->R << (int)*pixel->G << (int)*pixel->B <<std::endl;
                //std::cout << pixel->R << pixel->G << pixel->B <<std::endl;
            }
        } 
        if( x == 2 ){
            for (auto & pixel : CentroidStack)
                std::cout << (double)*pixel->R << pixel->G << pixel->B <<std::endl;
        }
        if( x == 3 ){
            RAWDATA.WriteJpegIntoFile(*OriginalImageDetails, PixelImage);
        }
        if( x == 4 ){
            for (auto & centroid : CentroidStack){
                std::cout << "\n\n>> Centriod "<< centroid->cluster << " : "<< std::endl;
                for (auto & pixel : PixelImage)
                    if (centroid->cluster == pixel->cluster){
                        std::cout << " |" << (int)*pixel->R; 
                        std::cout << " "  << (int)*pixel->G; 
                        std::cout << " "  << (int)*pixel->R;
                    }   
            }
        }
        
    };

    /***************************************************
        Generate Centriods 
    ****************************************************/
    void GenarateCentriods(const int _k_){
        std::random_device rd;                  // obtain a random number from hardware
        std::mt19937 eng(rd());                 // seed the generator
        std::uniform_int_distribution<> distr(0, PixelImage.size()-1);  // define range of pixel to chose
        for(int i= 0; i < _k_ ; i++){           // pick _k_ random pixels to become centriods
            int choice = distr(eng);           
            PixelObject *NewCentroid = new PixelObject(PixelImage[choice]->R, 
                                                       PixelImage[choice]->G,
                                                       PixelImage[choice]->B);
            NewCentroid->cluster = i;            // centroid cluster flag
            CentroidStack.push_back( NewCentroid );
        }
    };

    /***************************************************
        EuclidDistance
    ****************************************************/
    int EuclidDistance(PixelObject *centroid, PixelObject *image){
        int value = 0;
        value += std::pow((*centroid->R - *image->R), 2);
        value += std::pow((*centroid->G - *image->G), 2);
        value += std::pow((*centroid->B - *image->B), 2);
        return (int)sqrt(std::abs( value ));
    };

    /***************************************************
        Colour image (segmentation colouring)
    ****************************************************/
    void ColourImage(void){
        // Detetmine histograms in associtaed with a cluster and upadate its values
        for (auto & centroid : CentroidStack){
            std::cout << std::endl;
            std::cout << std::endl;
            for (auto & pixel : PixelImage){
                if (centroid->cluster == pixel->cluster){
					std::cout << centroid->cluster  <<" ";
                    *pixel->R = *centroid->R;
                    *pixel->G = *centroid->G;
                    *pixel->R = *centroid->B;
                }
                if (pixel->cluster == 99)
					std::cout << centroid->cluster  <<" ";
            }
		}
        PRINT(3);
    
    }

    /***************************************************
        Cluster 
    ****************************************************/
    void Cluster(void){
        /* assging images to clusters, by determing closests centroid to it */
        int shortest_distances, current_distances;
        for (auto & pixel : PixelImage){
            shortest_distances =  (int)sqrt(std::abs( std::pow(255+255+255,2) ));
            for (auto & centroid : CentroidStack){
                current_distances = EuclidDistance( centroid, pixel );

                // we check if there is centriod closer than first centriod
                if (current_distances <= shortest_distances){
                    shortest_distances = current_distances;    // updating shortest path
                    pixel->cluster = centroid->cluster;        // assign image to clusters

                }
            }
        }
    };


    /***************************************************
        Update Centriods 
    ****************************************************/

    void LlyodsAlgorithm(int epochs, int k) {
        /* UpdateCentriods */

        GenarateCentriods(k); // Randomly initialise centroids
        for (int i = 0; i <= epochs; ++i) {
            std::cout << "Iteration: " << i <<" of "<< epochs <<  std::endl;

            Cluster();
            int average = 1;
            // Detetmine pixels associtaed with a cluster and update its values
            for (auto & centroid : CentroidStack){
                for (auto & pixel : PixelImage){
                    if (centroid->cluster == pixel->cluster){
                        *centroid->R += *pixel->R;
                        *centroid->G += *pixel->G;
                        *centroid->B += *pixel->B;
                        average++;
                    }
                }
                // updating the centroids
                if(average > 0){ // if cluster is not empty
                    //*centroid->R /= average;
                    //*centroid->G /= average;
                    //*centroid->B /= average;
                    //*centroid->R = (*centroid->R/average);
                    //*centroid->G = (*centroid->G/average);
                    //*centroid->B = (*centroid->B/average);
                    average = 1;
                }
            }   

        }

    };



    void ExcuteDataset(std::string dataset){
        std::vector<std::string> imagename;
        DIR *dr; struct dirent *en;
        dr = opendir(dataset.c_str()); //open all directory
        if (!dr) {std::cout << "folder not found" <<std::endl;}
        else {
            //getting all the images in the folder
            while ((en = readdir(dr)) != NULL){
                std::cout<<en->d_name <<std::endl;
                //imagename.push_back(dataset +"/"+en->d_name);
            }
            closedir(dr);

            for(int i= 0; i < imagename.size(); i++){
                int k = 32; //16 bit colour paletts
                int iterations = 50;
                KMseg::Load(imagename[i].c_str());
                KMseg::LlyodsAlgorithm(iterations,k);
                KMseg::ColourImage();

                //delete[] OriginalImageDetails;            // image object data
                //delete[] RAWDATA;                         // functions to load image
                PixelImage.clear();
                CentroidStack.clear();

            }
        }


    };

}

int main (void){
    int k = 32; //16 bit colour paletts
    int iterations = 50;

    KMseg::Load("2092.jpg");
    //KMseg::Load("3096.jpg");
    //KMseg::Load("8023.jpg");
    //KMseg::Load("12084.jpg");

    //KMseg::LlyodsAlgorithm(iterations,k);
    //KMseg::GenarateCentriods(k); KMseg::Cluster();

    //KMseg::ColourImage();
    KMseg::ExcuteDataset("RICH");

    return 0;
}
