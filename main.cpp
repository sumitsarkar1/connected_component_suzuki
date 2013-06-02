#include <iostream>
#include <vector>
#include <math.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <fstream>

//#include "bloblabel.h"

using namespace std;
using namespace cv;

int main()
{
    VideoCapture webcam;
    webcam.open(0);

    FILE *file = fopen("C:\\matrix.txt","w");
    Mat frame = imread("D:\\suzuki2.png");

    //webcam >> frame;

    Mat frameg = Mat(frame.rows,frame.cols,CV_8UC1);


    int ROW = frame.rows;
    int COL = frame.cols;

    double duration = 0;
    double total_duration = 0;


    int *labelMatrix = new int[ROW*COL];

    int mask[4];
    int prev_c,prev_r,next_c;


    int *n_label   = new int[COL*ROW/4];
    int *t_label   = new int[COL*ROW/4];
    int *rl_table  = new int[COL*ROW/4];
    int *regions   = new int[COL*ROW/2];

    int r;
    int c;

    int iterations = 0;
    while(iterations < 100)
    {
        int provisional_lbl_m = 1;
        int count_of_regions=0;

        cvtColor(frame,frameg,CV_BGR2GRAY);
        threshold(frameg,frameg,50,255,CV_THRESH_BINARY);

        duration = static_cast<double>(getTickCount());
        bool atleast_white  = false;

        for(r=0;r<ROW;r++)
        {
            for(c=0;c<COL;c++)
            {
                labelMatrix[COL*r +c] = 0;

                if(*(frameg.data + COL*r + c)&&255)
                {
                    atleast_white = true;

                    prev_c = c-1;
                    prev_r = r-1;
                    next_c = c+1;

                    //regular pixels
                    if(prev_r>=0 && prev_c>=0 && next_c<COL)
                    {
                        mask[0]=labelMatrix[COL*prev_r + prev_c];
                        mask[1]=labelMatrix[COL*prev_r + c];
                        mask[2]=labelMatrix[COL*prev_r + next_c];
                        mask[3]=labelMatrix[COL*r + prev_c];

                        if(!(mask[0]|mask[1]|mask[2]|mask[3]))
                        {
                            labelMatrix[COL*r+c]= provisional_lbl_m;

                            //creating S(m) = {m}
                            rl_table[provisional_lbl_m] = provisional_lbl_m;
                            n_label[provisional_lbl_m] = -1;
                            t_label[provisional_lbl_m] = provisional_lbl_m;

                            provisional_lbl_m++;
                        }
                        else//mask contains one or more object pixels other than current pix
                        {
                            int J = 0;//to be assigned label;
                            bool assigned = false;

                            for(int index=0;index<4;index++)
                            {
                                /*we can assign the present object pixel any arbitrary number in the mask.
                                 *so the need to find the minimum is eliminated. Hence we
                                 *assign the first positive number as the label. once assigned
                                 *the label is stored in 'J' and the rest of the object pixels
                                 * are compared to resolve the equivalence.
                                */
                                if(!assigned && mask[index]>0)
                                {
                                    int pos = index;

                                    switch(pos)
                                    {
                                    case 0:
                                        labelMatrix[COL*r +c]=J=
                                                labelMatrix[COL*prev_r+prev_c];
                                        break;

                                    case 1:
                                        labelMatrix[COL*r +c]=J=
                                                labelMatrix[COL*prev_r +c];
                                        break;

                                    case 2:
                                        labelMatrix[COL*r +c]=J=
                                                labelMatrix[COL*prev_r +next_c];
                                        break;

                                    case 3:
                                        labelMatrix[COL*r +c]=J=
                                                labelMatrix[COL*r +prev_c];
                                    }

                                    assigned = true;
                                    continue;
                                }

                                //resolve labels now by checking with other object pix;
                                int K = mask[index];
                                if( K>0 && K!=J )
                                {
                                    int u = rl_table[J]; int v = rl_table[K];

                                    if(u != v )
                                    {
                                        //this assumes 'u < v'. so swap if opposite
                                        if(u > v)
                                        {
                                            int dummy = v;
                                            v = u;
                                            u = dummy;
                                        }

                                        //swap(u,v);

                                        int i = v;
                                        while(i!=-1)
                                        {
                                            rl_table[i] = u;
                                            i = n_label[i];
                                        }

                                        n_label[t_label[u]] = v;
                                        t_label[u] = t_label[v];
                                    }
                                }


                            }
                        }

                        continue;

                    }


                    //upper most row
                    if(prev_r<0 && prev_c>=0 && next_c<COL)
                    {
                        mask[0]=0;
                        mask[1]=0;
                        mask[2]=0;
                        mask[3]= labelMatrix[COL*r + prev_c];

                        if(!(mask[0]|mask[1]|mask[2]|mask[3]))
                        {
                            labelMatrix[COL*r+c]= provisional_lbl_m;

                            //creating S(m) = {m}
                            rl_table[provisional_lbl_m] = provisional_lbl_m;
                            n_label[provisional_lbl_m] = -1;
                            t_label[provisional_lbl_m] = provisional_lbl_m;

                            provisional_lbl_m++;
                        }
                        else//mask contains one or more object pixels other than current pix
                        {
                            int J = 0;//to be assigned label;
                            bool assigned = false;

                            for(int index=0;index<4;index++)
                            {
                                /*we can assign the present object pixel any arbitrary number in the mask.
                                 *so the need to find the minimum is eliminated. Hence we
                                 *assign the first positive number as the label. once assigned
                                 *the label is stored in 'J' and the rest of the object pixels
                                 * are compared to resolve the equivalence.
                                */
                                if(!assigned && mask[index]>0)
                                {
                                    int pos = index;

                                    switch(pos)
                                    {
                                    case 0:
                                        labelMatrix[COL*r +c]=J=
                                                labelMatrix[COL*prev_r+prev_c];
                                        break;

                                    case 1:
                                        labelMatrix[COL*r +c]=J=
                                                labelMatrix[COL*prev_r +c];
                                        break;

                                    case 2:
                                        labelMatrix[COL*r +c]=J=
                                                labelMatrix[COL*prev_r +next_c];
                                        break;

                                    case 3:
                                        labelMatrix[COL*r +c]=J=
                                                labelMatrix[COL*r +prev_c];
                                    }

                                    assigned = true;
                                    continue;
                                }

                                //resolve labels now by checking with other object pix;
                                int K = mask[index];
                                if( K>0 && K!=J )
                                {
                                    int u = rl_table[J]; int v = rl_table[K];

                                    if(u != v )
                                    {
                                        //this assumes 'u < v'. so swap if opposite
                                        if(u > v)
                                        {
                                            int dummy = v;
                                            v = u;
                                            u = dummy;
                                        }

                                        //swap(u,v);

                                        int i = v;
                                        while(i!=-1)
                                        {
                                            rl_table[i] = u;
                                            i = n_label[i];
                                        }

                                        n_label[t_label[u]] = v;
                                        t_label[u] = t_label[v];
                                    }
                                }


                            }
                        }

                        continue;
                    }

                    //left most column
                    if(prev_c<0 && prev_r>=0 && next_c<COL)
                    {
                        mask[0]=0;
                        mask[1]=labelMatrix[COL*prev_r + c];
                        mask[2]=labelMatrix[COL*prev_r + next_c];
                        mask[3]=0;

                        if(!(mask[0]|mask[1]|mask[2]|mask[3]))
                        {
                            labelMatrix[COL*r+c]= provisional_lbl_m;

                            //creating S(m) = {m}
                            rl_table[provisional_lbl_m] = provisional_lbl_m;
                            n_label[provisional_lbl_m] = -1;
                            t_label[provisional_lbl_m] = provisional_lbl_m;

                            provisional_lbl_m++;
                        }
                        else//mask contains one or more object pixels other than current pix
                        {
                            int J = 0;//to be assigned label;
                            bool assigned = false;

                            for(int index=0;index<4;index++)
                            {
                                /*we can assign the present object pixel any arbitrary number in the mask.
                                 *so the need to find the minimum is eliminated. Hence we
                                 *assign the first positive number as the label. once assigned
                                 *the label is stored in 'J' and the rest of the object pixels
                                 * are compared to resolve the equivalence.
                                */
                                if(!assigned && mask[index]>0)
                                {
                                    int pos = index;

                                    switch(pos)
                                    {
                                    case 0:
                                        labelMatrix[COL*r +c]=J=
                                                labelMatrix[COL*prev_r+prev_c];
                                        break;

                                    case 1:
                                        labelMatrix[COL*r +c]=J=
                                                labelMatrix[COL*prev_r +c];
                                        break;

                                    case 2:
                                        labelMatrix[COL*r +c]=J=
                                                labelMatrix[COL*prev_r +next_c];
                                        break;

                                    case 3:
                                        labelMatrix[COL*r +c]=J=
                                                labelMatrix[COL*r +prev_c];
                                    }

                                    assigned = true;
                                    continue;
                                }

                                //resolve labels now by checking with other object pix;
                                int K = mask[index];
                                if( K>0 && K!=J )
                                {
                                    int u = rl_table[J]; int v = rl_table[K];

                                    if(u != v )
                                    {
                                        //this assumes 'u < v'. so swap if opposite
                                        if(u > v)
                                        {
                                            int dummy = v;
                                            v = u;
                                            u = dummy;
                                        }

                                        //swap(u,v);

                                        int i = v;
                                        while(i!=-1)
                                        {
                                            rl_table[i] = u;
                                            i = n_label[i];
                                        }

                                        n_label[t_label[u]] = v;
                                        t_label[u] = t_label[v];
                                    }
                                }


                            }
                        }

                        continue;

                    }

                    //right most column
                    if(prev_r>=0 && prev_c>0 && next_c==COL)
                    {
                        mask[0]=labelMatrix[COL*prev_r + prev_c];
                        mask[1]=labelMatrix[COL*prev_r + c];
                        mask[2]=0;
                        mask[3]=labelMatrix[COL*r + prev_c];

                        if(!(mask[0]|mask[1]|mask[2]|mask[3]))
                        {
                            labelMatrix[COL*r+c]= provisional_lbl_m;

                            //creating S(m) = {m}
                            rl_table[provisional_lbl_m] = provisional_lbl_m;
                            n_label[provisional_lbl_m] = -1;
                            t_label[provisional_lbl_m] = provisional_lbl_m;

                            provisional_lbl_m++;
                        }
                        else//mask contains one or more object pixels other than current pix
                        {
                            int J = 0;//to be assigned label;
                            bool assigned = false;

                            for(int index=0;index<4;index++)
                            {
                                /*we can assign the present object pixel any arbitrary number in the mask.
                                 *so the need to find the minimum is eliminated. Hence we
                                 *assign the first positive number as the label. once assigned
                                 *the label is stored in 'J' and the rest of the object pixels
                                 * are compared to resolve the equivalence.
                                */
                                if(!assigned && mask[index]>0)
                                {
                                    int pos = index;

                                    switch(pos)
                                    {
                                    case 0:
                                        labelMatrix[COL*r +c]=J=
                                                labelMatrix[COL*prev_r+prev_c];
                                        break;

                                    case 1:
                                        labelMatrix[COL*r +c]=J=
                                                labelMatrix[COL*prev_r +c];
                                        break;

                                    case 2:
                                        labelMatrix[COL*r +c]=J=
                                                labelMatrix[COL*prev_r +next_c];
                                        break;

                                    case 3:
                                        labelMatrix[COL*r +c]=J=
                                                labelMatrix[COL*r +prev_c];
                                    }

                                    assigned = true;
                                    continue;
                                }

                                //resolve labels now by checking with other object pix;
                                int K = mask[index];
                                if( K>0 && K!=J )
                                {
                                    int u = rl_table[J]; int v = rl_table[K];

                                    if(u != v )
                                    {
                                        //this assumes 'u < v'. so swap if opposite
                                        if(u > v)
                                        {
                                            int dummy = v;
                                            v = u;
                                            u = dummy;
                                        }

                                        //swap(u,v);

                                        int i = v;
                                        while(i!=-1)
                                        {
                                            rl_table[i] = u;
                                            i = n_label[i];
                                        }

                                        n_label[t_label[u]] = v;
                                        t_label[u] = t_label[v];
                                    }
                                }


                            }
                        }

                        continue;

                    }

                    //top left corner pixel
                    if(prev_r<0 && prev_c<0)
                    {
                        mask[0]=0;
                        mask[1]=0;
                        mask[2]=0;
                        mask[3]=0;

                        if(!(mask[0]|mask[1]|mask[2]|mask[3]))
                        {
                            labelMatrix[COL*r+c]= provisional_lbl_m;

                            //creating S(m) = {m}
                            rl_table[provisional_lbl_m] = provisional_lbl_m;
                            n_label[provisional_lbl_m] = -1;
                            t_label[provisional_lbl_m] = provisional_lbl_m;

                            provisional_lbl_m++;
                        }
                        else//mask contains one or more object pixels other than current pix
                        {
                            int J = 0;//to be assigned label;
                            bool assigned = false;

                            for(int index=0;index<4;index++)
                            {
                                /*we can assign the present object pixel any arbitrary number in the mask.
                                 *so the need to find the minimum is eliminated. Hence we
                                 *assign the first positive number as the label. once assigned
                                 *the label is stored in 'J' and the rest of the object pixels
                                 * are compared to resolve the equivalence.
                                */
                                if(!assigned && mask[index]>0)
                                {
                                    int pos = index;

                                    switch(pos)
                                    {
                                    case 0:
                                        labelMatrix[COL*r +c]=J=
                                                labelMatrix[COL*prev_r+prev_c];
                                        break;

                                    case 1:
                                        labelMatrix[COL*r +c]=J=
                                                labelMatrix[COL*prev_r +c];
                                        break;

                                    case 2:
                                        labelMatrix[COL*r +c]=J=
                                                labelMatrix[COL*prev_r +next_c];
                                        break;

                                    case 3:
                                        labelMatrix[COL*r +c]=J=
                                                labelMatrix[COL*r +prev_c];
                                    }

                                    assigned = true;
                                    continue;
                                }

                                //resolve labels now by checking with other object pix;
                                int K = mask[index];
                                if( K>0 && K!=J )
                                {
                                    int u = rl_table[J]; int v = rl_table[K];

                                    if(u != v )
                                    {
                                        //this assumes 'u < v'. so swap if opposite
                                        if(u > v)
                                        {
                                            int dummy = v;
                                            v = u;
                                            u = dummy;
                                        }

                                        //swap(u,v);

                                        int i = v;
                                        while(i!=-1)
                                        {
                                            rl_table[i] = u;
                                            i = n_label[i];
                                        }

                                        n_label[t_label[u]] = v;
                                        t_label[u] = t_label[v];
                                    }
                                }


                            }
                        }

                        continue;
                    }

                    //top right corner pixel
                    if(next_c == COL && prev_r<0 )
                    {
                        mask[0]=0;
                        mask[1]=0;
                        mask[2]=0;
                        mask[3]= labelMatrix[COL*r + prev_c];

                        if(!(mask[0]|mask[1]|mask[2]|mask[3]))
                        {
                            labelMatrix[COL*r+c]= provisional_lbl_m;

                            //creating S(m) = {m}
                            rl_table[provisional_lbl_m] = provisional_lbl_m;
                            n_label[provisional_lbl_m] = -1;
                            t_label[provisional_lbl_m] = provisional_lbl_m;

                            provisional_lbl_m++;
                        }
                        else//mask contains one or more object pixels other than current pix
                        {
                            int J = 0;//to be assigned label;
                            bool assigned = false;

                            for(int index=0;index<4;index++)
                            {
                                /*we can assign the present object pixel any arbitrary number in the mask.
                                 *so the need to find the minimum is eliminated. Hence we
                                 *assign the first positive number as the label. once assigned
                                 *the label is stored in 'J' and the rest of the object pixels
                                 * are compared to resolve the equivalence.
                                */
                                if(!assigned && mask[index]>0)
                                {
                                    int pos = index;

                                    switch(pos)
                                    {
                                    case 0:
                                        labelMatrix[COL*r +c]=J=
                                                labelMatrix[COL*prev_r+prev_c];
                                        break;

                                    case 1:
                                        labelMatrix[COL*r +c]=J=
                                                labelMatrix[COL*prev_r +c];
                                        break;

                                    case 2:
                                        labelMatrix[COL*r +c]=J=
                                                labelMatrix[COL*prev_r +next_c];
                                        break;

                                    case 3:
                                        labelMatrix[COL*r +c]=J=
                                                labelMatrix[COL*r +prev_c];
                                    }

                                    assigned = true;
                                    continue;
                                }

                                //resolve labels now by checking with other object pix;
                                int K = mask[index];
                                if( K>0 && K!=J )
                                {
                                    int u = rl_table[J]; int v = rl_table[K];

                                    if(u != v )
                                    {
                                        //this assumes 'u < v'. so swap if opposite
                                        if(u > v)
                                        {
                                            int dummy = v;
                                            v = u;
                                            u = dummy;
                                        }

                                        //swap(u,v);

                                        int i = v;
                                        while(i!=-1)
                                        {
                                            rl_table[i] = u;
                                            i = n_label[i];
                                        }

                                        n_label[t_label[u]] = v;
                                        t_label[u] = t_label[v];
                                    }
                                }


                            }
                        }

                        continue;
                    }

                }

            }
        }

        if(provisional_lbl_m>1)
        {
            regions[count_of_regions] = rl_table[1];
            count_of_regions++;

            for(int ind = 2;ind<provisional_lbl_m;ind++ )
            {
                bool match = false;

                for(int k = 0;k<count_of_regions;k++)
                    if(rl_table[ind] == regions[k])
                    {
                        match = true;
                        break;
                    }
                if(!match)
                {
                    regions[count_of_regions] = rl_table[ind];
                    count_of_regions++;
                }
            }

            for(r =0;r<ROW;r++)
            {
                for(c=0;c<COL;c++)
                {
                    int lbl = labelMatrix[COL*r + c];
                    if(!lbl)
                        labelMatrix[COL*r + c] = rl_table[lbl];
                }
            }
        }

        duration = static_cast<double>(getTickCount()) - duration;

        total_duration += duration;

        iterations++;

        imshow("SUZUKI",frameg);
        waitKey(30);
    }


    /*duration = static_cast<double>(getTickCount());
    bloblabel myblobcount;
    IplImage image = frameg;
    myblobcount.labelimage(&image);

    duration = static_cast<double>(getTickCount()) - duration;

    duration /= getTickFrequency();


    cout<<endl;
    cout<<"contour tracing = "<<myblobcount.blobcount()<<endl;
    cout<<"time elapsed = "<<duration<<" ms"<<endl;*/
    total_duration /= getTickFrequency();

    cout<<"average time = "<<(total_duration/iterations)*1000<<endl;

    delete n_label;delete t_label;delete rl_table;delete labelMatrix;

    return 0;
}
