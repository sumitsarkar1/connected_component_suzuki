#include "bloblabel.h"

struct nbh
{
    CvPoint nbhpoints[8];
}nbhpts;

inline nbh *getnbh(int &row,int &col)
{
    nbhpts.nbhpoints[0].x=col+1;nbhpts.nbhpoints[0].y=row;
    nbhpts.nbhpoints[1].x=col+1;nbhpts.nbhpoints[1].y=row+1;
    nbhpts.nbhpoints[2].x=col  ;nbhpts.nbhpoints[2].y=row+1;
    nbhpts.nbhpoints[3].x=col-1;nbhpts.nbhpoints[3].y=row+1;
    nbhpts.nbhpoints[4].x=col-1;nbhpts.nbhpoints[4].y=row;
    nbhpts.nbhpoints[5].x=col-1;nbhpts.nbhpoints[5].y=row-1;
    nbhpts.nbhpoints[6].x=col  ;nbhpts.nbhpoints[6].y=row-1;
    nbhpts.nbhpoints[7].x=col+1;nbhpts.nbhpoints[7].y=row-1;

    return &(nbhpts);
}

bloblabel::bloblabel()
{
}

void bloblabel::labelimage(IplImage *im)
{
    //deleting list of blobs for each new frame
    //..this clears the list of previous frame
    bloblist.clear();

    IplImage *image = cvCreateImage(cvSize(im->width+2,im->height+2),8,1);

    int *labelmatrix = new int[image->width*image->height];
    int labelmatrix_w=image->width;

    bool white=false;

    for(int row = 0;row<image->height;++row)
    {
        for(int col=0;col<image->width;++col)
        {
            *(labelmatrix + row*labelmatrix_w + col)=0;
            if( (row>0) && (row<image->height-1) && (col>0) && (col<image->width-1) )
            {
                *(image->imageData + image->widthStep*row + col)= *(im->imageData + im->widthStep*(row-1) + col-1 );
                if( (!white) && ((unsigned char)*(im->imageData + im->widthStep*(row-1) + col-1 )==255) )
                    white=true;
            }
            else
                *(image->imageData + image->widthStep*row + col)=0;
        }
    }

    if(white)
    {

    int S_row=0;
    int S_col=0;
    int T_row=-1;
    int T_col=-1;

    int index;

    CvPoint prevs_contour_pt;
    CvPoint ptTL , ptTR , ptDL , ptDR;
    CvPoint maxrow,minrow,maxcol,mincol;

    int C=0;

    for(int row=1;row<image->height-1;++row)
    {
        for(int col=1;col<image->width-1;++col)
        {
            //check for white pix
            if((unsigned char)*(image->imageData + image->widthStep*row + col)==255)
            {
                //3 cases can take place for each point
                //...whichever case applies pix_case_done is switched to TRUE
                //...else it is false
                //..hence once a pt visits a case it cant visit other case
                bool pix_case_done=false;

                //if its external contour pt we need its bounding box;
                bool external_contour_startin_pt = false;

                int current_pix_label=*(labelmatrix+labelmatrix_w*row+col);
                unsigned char above_pix_gray_val=(unsigned char)*(image->imageData+image->widthStep
                                     *(row-1)+col);

                //CHECK FOR EXTERNAL CONTOUR
                if( (current_pix_label==0) && (above_pix_gray_val==0) && (!pix_case_done) )
                {
                    pix_case_done=true;
                    //external contour of new component

                    //increase label by 1
                     ++C;

                    //assign label which is increased by 1 already
                    *(labelmatrix+labelmatrix_w*row+col)=C;

                    //execute contour tracing

                    //current pix has no previous contour pt
                    prevs_contour_pt.x=-1;
                    prevs_contour_pt.y=-1;

                    //its the first pix of contour
                    external_contour_startin_pt=true;

                    //current pix is starting pt of extrnl contour
                    int search_start_index =7;

                    //variables needed for contour trace
                    bool contour_trace_done = false;
                    bool next_contour_pt_done = false;
                    int next_contour_index=0;

                    int row_contour=row;
                    int col_contour=col;
                    int S_row=row;
                    int S_col=col;

                    //initialising for storing max and min for bounding box
                    maxrow.x=0;
                    maxrow.y=0;
                    maxcol.x=0;
                    maxcol.y=0;
                    minrow.x=2000;
                    minrow.y=2000;
                    mincol.x=2000;
                    mincol.y=2000;

                    while(!contour_trace_done)
                    {

                        //get nbh pts of the current pix;
                        nbh *ptr2nbhpts = getnbh(row_contour,col_contour);

                        //find index of prevs contour pt
                        //this will be NOT done for first contour pt
                        if(prevs_contour_pt.x!=-1)
                        {
                             bool search_4_prevs_pt_done = false;
                             index=0;
                             while(!search_4_prevs_pt_done)
                             {
                                // std::cout<<"loop 1 external"<<std::endl;
                                 if( (prevs_contour_pt.x==ptr2nbhpts->nbhpoints[index].x) && (prevs_contour_pt.y==ptr2nbhpts->nbhpoints[index].y) )
                                     search_4_prevs_pt_done=true;
                                 else
                                     ++index;

                             }

                             //add 2 to it and adjust with mod 8
                             search_start_index=index+2;
                             if(search_start_index>7)
                                 search_start_index=search_start_index%8;

                        }

                        //this is starting pt of nxt white pixel search and is stored in search_start_index
                        //if pt is starting pt of contour then search_start_index is already = 7
                        //...and the previous if is not evaluated coz prevs_contour_pt
                        //...contains negative value

                        //search for next white pix in clock wise directn
                        index=search_start_index;
                        next_contour_pt_done = false;
                        int check_4_isolated_pt=0;

                        int r,c;
                        while( (check_4_isolated_pt<=8) && (!next_contour_pt_done) )
                        {
                          //  std::cout<<"in loop 2 external"<<std::endl;
                           r=ptr2nbhpts->nbhpoints[index].y;
                           c=ptr2nbhpts->nbhpoints[index].x;

                            if((unsigned char)*(image->imageData+image->widthStep*r+c)==255)
                                next_contour_pt_done=true;
                            else
                            {
                                *(labelmatrix+labelmatrix_w*r+c)=-1;
                                ++check_4_isolated_pt;
                                ++index;
                                if(index>7)
                                    index=index%8;
                            }
                        }

                       next_contour_index=index;

                        //two cases can occur
                        //1...no white pix...i.e...isolated pt(check_4_isolated_pt=8)
                        //2...white pix found(next_contour_pt_done=TRUE)

                        if(check_4_isolated_pt==9)
                        {
                            contour_trace_done=true;
                        }

                        //next part is meaningful if only pt is not isolated
                        //...i.e. next white pix found
                        if(check_4_isolated_pt!=9)
                        {
                            //store next contour pt in T if pt is
                            //...first pt in contour
                            if(prevs_contour_pt.x==-1)
                            {
                                T_row=ptr2nbhpts->nbhpoints[next_contour_index].y;
                                T_col=ptr2nbhpts->nbhpoints[next_contour_index].x;
                            }
                            //check wether the current pt is S
                            //..and the next contour pt found is
                            //..T or not
                            if( (row_contour==S_row) && (col_contour==S_col) && (ptr2nbhpts->nbhpoints[next_contour_index].y==T_row) && (ptr2nbhpts->nbhpoints[next_contour_index].x==T_col) && (prevs_contour_pt.x!=-1) )
                                contour_trace_done=true;
                            else
                            {
                                //assign label to all contour pts
                                *(labelmatrix+labelmatrix_w*ptr2nbhpts->nbhpoints[next_contour_index].y + ptr2nbhpts->nbhpoints[next_contour_index].x)=C;

                                //make prevs contour pt as the current contour pt
                                prevs_contour_pt.x=col_contour;
                                prevs_contour_pt.y=row_contour;

                                //store vals of max min
                                if(row_contour>maxrow.y)
                                {
                                    maxrow.y=row_contour;
                                    maxrow.x=col_contour;
                                }

                                if(row_contour<minrow.y)
                                {
                                    minrow.y=row_contour;
                                    minrow.x=col_contour;
                                }

                                if(col_contour>maxcol.x)
                                {
                                    maxcol.x=col_contour;
                                    maxcol.y=row_contour;
                                }

                                if(col_contour<mincol.x)
                                {
                                    mincol.x=col_contour;
                                    mincol.y=row_contour;
                                }
                                //make next pts to be analysed in the contour
                                col_contour=ptr2nbhpts->nbhpoints[next_contour_index].x;
                                row_contour=ptr2nbhpts->nbhpoints[next_contour_index].y;
                            }
                        }

                    }


                }


                unsigned char below_pix_gray_val=(unsigned char)*(image->imageData+image->widthStep*(row+1) + col);
                unsigned char below_pix_label=*(labelmatrix+labelmatrix_w*(row+1)+col);
                current_pix_label=*(labelmatrix+labelmatrix_w*row+col);


                //CHECK FOR INTERNAL CONTOUR
                if( (below_pix_gray_val==0) && (below_pix_label==0) )
                {
                    pix_case_done=true;
                    //newly encountered internal contour

                    int label_for_internal_contour=0;

                    //two cases arise
                    //1...current pix already labelled
                    if(current_pix_label>0)
                    {
                        label_for_internal_contour = current_pix_label;
                        //current pix is also on external contour
                    }

                    //2...current pix NOT labelled
                    if(current_pix_label==0)
                    {
                        //left neighbour of current pix must b labeld
                        int left_neighbour = *(labelmatrix+labelmatrix_w*row + col-1);
                        *(labelmatrix+labelmatrix_w*row+col)=left_neighbour;
                        label_for_internal_contour = left_neighbour;
                    }

                    //in either case we trace contour


                    //execute contour tracing

                    //current pix has no previous contour pt
                    prevs_contour_pt.x=-1;
                    prevs_contour_pt.y=-1;

                    //current pix is starting pt of internal contour
                    int search_start_index =3;

                    //variables needed for contour trace
                    bool contour_trace_done = false;
                    bool next_contour_pt_done = false;
                    int next_contour_index = 0;

                    int row_contour=row;
                    int col_contour=col;
                    S_row=row;
                    S_col=col;

                    while(!contour_trace_done)
                    {

                        //get nbh pts of the current pix;
                        nbh *ptr2nbhpts = getnbh(row_contour,col_contour);

                        //find index of prevs contour pt
                        //this will be NOT done for first contour pt
                        if(prevs_contour_pt.x!=-1)
                        {
                             bool search_4_prevs_pt_done = false;
                             index=0;
                             while(!search_4_prevs_pt_done)
                             {
                                 //std::cout<<"loop 1 internal"<<std::endl;
                                 if( (prevs_contour_pt.x==ptr2nbhpts->nbhpoints[index].x) && (prevs_contour_pt.y==ptr2nbhpts->nbhpoints[index].y) )
                                     search_4_prevs_pt_done=true;
                                 else
                                     ++index;

                             }

                             //add 2 to it and adjust with mod 8
                             search_start_index=index+2;
                             if(search_start_index>7)
                                 search_start_index=search_start_index%8;

                        }

                        //this is starting pt of nxt white pixel search and is stored in search_start_index
                        //if pt is starting pt of contour then search_start_index is already = 7
                        //...and the previous if is not evaluated coz prevs_contour_pt
                        //...contains negative value

                        //search for next white pix in clock wise directn
                        index=search_start_index;
                        next_contour_pt_done = false;
                      //  int check_4_isolated_pt=0;

                        int r,c;
                        while(!next_contour_pt_done)
                        {
                            //std::cout<<"in loop 2 internal"<<std::endl;
                            r=ptr2nbhpts->nbhpoints[index].y;
                            c=ptr2nbhpts->nbhpoints[index].x;

                            if((unsigned char)*(image->imageData+image->widthStep*r+c)==255)
                                next_contour_pt_done=true;
                            else
                            {
                                *(labelmatrix+labelmatrix_w*r+c)=-1;
                                ++index;
                                if(index>7)
                                    index=index%8;
                            }
                        }

                        next_contour_index=index;

                        //only one case can occur
                        // XXXXX ..WONT OCCUR 1...no white pix...i.e...isolated pt(check_4_isolated_pt=8)
                        //2...white pix found(next_contour_pt_done=TRUE)

                           //store next contour pt in T if pt is
                            //...first pt in contour
                            if(prevs_contour_pt.x==-1)
                            {
                                T_row=ptr2nbhpts->nbhpoints[next_contour_index].y;
                                T_col=ptr2nbhpts->nbhpoints[next_contour_index].x;
                            }
                            //check wether the current pt is S
                            //..and the next contour pt found is
                            //..T or not
                            if( (row_contour==S_row) && (col_contour==S_col) && (ptr2nbhpts->nbhpoints[next_contour_index].y==T_row) && (ptr2nbhpts->nbhpoints[next_contour_index].x==T_col) && (prevs_contour_pt.x!=-1) )
                                contour_trace_done=true;
                            else
                            {
                                //assign label to all contour pts
                                *(labelmatrix+labelmatrix_w*ptr2nbhpts->nbhpoints[next_contour_index].y + ptr2nbhpts->nbhpoints[next_contour_index].x)=label_for_internal_contour;

                                prevs_contour_pt.x=col_contour;
                                prevs_contour_pt.y=row_contour;

                                col_contour=ptr2nbhpts->nbhpoints[next_contour_index].x;
                                row_contour=ptr2nbhpts->nbhpoints[next_contour_index].y;
                            }

                    }

               }

                //IF NEITHER OF THE CASE
                //current pix is not on contour
                if( (current_pix_label==0) && (!pix_case_done) )
                {
                    pix_case_done=true;
                    //current pix is not a contour point
                    //left neighbour of the pix must be labeld
                    //left nbh pix label givn to present pix label
                    *(labelmatrix+labelmatrix_w*row+col)=*(labelmatrix+labelmatrix_w*row+col-1);
                }

             //   std::cout<<" ALL CASES COVERED"<<std::endl;

                current_pix_label = *(labelmatrix+labelmatrix_w*row+col);

                //if label of current pix is greater than numberofblobs till
                //...now recorded then it means new blob has been found


                if( current_pix_label>(signed)bloblist.size())
                {

                    //create a new blob structure
                    //...set label of blob to crrent pix label
                    //...set number of white pix =1
                    //add the blob detail to new list
                    blob newblob;
                    newblob.label_of_blob=current_pix_label;
                    newblob.number_of_pixels=1;
                    bloblist.push_back(newblob);

                }
                else //pix is just white pix to any of the already labeld blob
                {
                    this->increase_pix_count(current_pix_label);
                }

                if(external_contour_startin_pt)
                {
                    if(mincol.x==2000)
                        ptTL.x=0;
                    else
                        ptTL.x=mincol.x-1;

                    if(minrow.y==2000)
                        ptTL.y=0;
                    else
                        ptTL.y=minrow.y-1;

                    ptTR.x=maxcol.x-1;

                    if(minrow.y==2000)
                        ptTR.y=0;
                    else
                        ptTR.y=minrow.y;

                    if(mincol.x==2000)
                        ptDL.x=0;
                    else
                        ptDL.x=mincol.x-1;

                    ptDL.y=maxrow.y-1;

                    ptDR.x=maxcol.x-1;
                    ptDR.y=maxrow.y-1;

                    this->boundingbox(C,ptTL,ptTR,ptDL,ptDR);
                   // bloblist2.boundingbox(C,ptTL,ptTR,ptDL,ptDR);

                }
            }
        }
    }

    }// WHITE PIXEL LOOP CHECK END HERE

     cvReleaseImage(&image);
     delete labelmatrix;

 }

int bloblabel::blobcount() const
{
    return bloblist.size();
}

void bloblabel::increase_pix_count(int &l)
{
        blobiterator = bloblist.begin();
        while((*blobiterator).label_of_blob!=l)
            ++blobiterator;
        ++(*blobiterator).number_of_pixels;
}

void bloblabel::boundingbox(int &l, CvPoint &TL, CvPoint &TR, CvPoint &BL, CvPoint &BR)
{
        blobiterator=bloblist.begin();
        while((*blobiterator).label_of_blob!=l)
            ++blobiterator;

        (*blobiterator).TL=TL;
        (*blobiterator).TR=TR;
        (*blobiterator).BL=BL;
        (*blobiterator).BR=BR;
}

CvRect bloblabel::getboundingbox(int &l)
{
    if(l>(signed)bloblist.size())
    {
        CvRect voidbox;
        voidbox.height=0;
        voidbox.width=0;
        voidbox.x=0;
        voidbox.y=0;

        return voidbox;
    }
    else
    {

        blobiterator=bloblist.begin();
        int count = 1;

        while(count!=l)
        {
            ++count;
            ++blobiterator;
        }
        CvRect box;

        box.x=(*blobiterator).TL.x;
        box.y=(*blobiterator).TL.y;
        box.height= (*blobiterator).BL.y - (*blobiterator).TL.y + 1;
        box.width=(*blobiterator).TR.x - (*blobiterator).TL.x + 1;

        return box;
    }
}

void bloblabel::filter_blobs_greaterthan(int t)
{
    blobiterator = bloblist.begin();
    while(blobiterator!=bloblist.end())
    {
        if((*blobiterator).number_of_pixels>t)
            blobiterator=bloblist.erase(blobiterator);
        else
            ++blobiterator;
    }
}

void bloblabel::filter_blobs_lesserthan(int t)
{
    blobiterator = bloblist.begin();
    while(blobiterator!=bloblist.end())
    {
        if((*blobiterator).number_of_pixels<t)
            blobiterator=bloblist.erase(blobiterator);
        else
            ++blobiterator;
    }
}






