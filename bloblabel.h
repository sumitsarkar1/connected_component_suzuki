#ifndef BLOBLABEL_H
#define BLOBLABEL_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "iostream"
#include "list"

using namespace std;
using namespace cv;

class bloblabel
{
public:
    bloblabel();
    //~bloblabel();
    int blobcount() const;
    void labelimage(IplImage*);
    void filter_blobs_greaterthan(int);
    void filter_blobs_lesserthan(int);
    CvRect getboundingbox(int&);

private:
    struct blob
    {
        int label_of_blob;
        int number_of_pixels;
        CvPoint TL,TR,BL,BR;
    };
    list<blob> bloblist;
    list<blob>::iterator blobiterator;
    void increase_pix_count(int&);
    void boundingbox(int&,CvPoint&,CvPoint&,CvPoint&,CvPoint&);

};

#endif // BLOBLABEL_H
