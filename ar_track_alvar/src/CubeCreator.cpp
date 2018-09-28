#include "ar_track_alvar/MultiMarker.h"
#include "ar_track_alvar/Color.h"
#include "highgui.h"
using namespace std;
using namespace alvar;

struct State {
    IplImage *img;
    stringstream filename;
    double minx, miny, maxx, maxy; // top-left and bottom-right in pixel units
    MultiMarker multi_marker;

    // General options
    bool   prompt;
    double units;           // how many pixels per one unit
    double marker_side_len; // marker side len in current units
    double cube_side_len; // marker side len in current units
    int    marker_type;     // 0:MarkerData, 1:ArToolkit
    int    marker_id;
    double posx, posy;      // The position of marker center in the given units
    double transx, transy, transz;
    int    rot[9];
    double    content_res;
    double margin_res;
    bool   array;
    unsigned char color;

    // MarkerData specific options
    MarkerData::MarkerContentType marker_data_content_type;

    State()
        : img(0),
          prompt(false),
          units(96.0/2.54),      // cm assuming 96 dpi
          marker_side_len(9.0),  // 9 cm
          cube_side_len(9.0),   //9cm
          marker_type(0),
          marker_id(0),
          posx(0), posy(0),
          transx(0), transy(0), transz(0),
          content_res(0),        // 0 uses default
          margin_res(0.0),       // 0.0 uses default (can be n*0.5)
          array(false),
          color(0),
          marker_data_content_type(MarkerData::MARKER_CONTENT_TYPE_NUMBER)
    {}
    ~State() {
        if (img) cvReleaseImage(&img);
    }

    IplImage *rotateImage2(const IplImage *src, float angleDegrees)
    {
        // Create a map_matrix, where the left 2x2 matrix
        // is the transform and the right 2x1 is the dimensions.
        float m[6];
        CvMat M = cvMat(2, 3, CV_32F, m);
        int w = src->width;
        int h = src->height;
        float angleRadians = angleDegrees * ((float)CV_PI / 180.0f);
        m[0] = (float)(cos(angleRadians));
        m[1] = (float)(sin(angleRadians));
        m[3] = -m[1];
        m[4] = m[0];
        m[2] = w*0.5f;
        m[5] = h*0.5f;

        // Make a spare image for the result
        CvSize sizeRotated;
        sizeRotated.width = cvRound(h);
        sizeRotated.height = cvRound(w);

        // Rotate
        IplImage *imageRotated = cvCreateImage(sizeRotated,
            src->depth, src->nChannels);

        // Transform the image
        cvGetQuadrangleSubPix(src, imageRotated, &M);

        return imageRotated;
    }

    void AddMarker(const char *id) {
        std::cout<<"ADDING MARKER "<<id<<std::endl;

        MarkerData md(marker_side_len, content_res, margin_res);
        int side_len = int(marker_side_len*units+0.5);
        if (img == 0)
        {
            img = cvCreateImage(cvSize(side_len, side_len), IPL_DEPTH_8U, 1);
            filename.str("");
            filename<<"MarkerData";
            minx = (posx*units) - (marker_side_len*units/2.0);
            miny = (posy*units) - (marker_side_len*units/2.0);
            maxx = (posx*units) + (marker_side_len*units/2.0);
            maxy = (posy*units) + (marker_side_len*units/2.0);
        }
        else
        {
          double new_minx = (posx*units) - (marker_side_len*units/2.0);
          double new_miny = (posy*units) - (marker_side_len*units/2.0);
          double new_maxx = (posx*units) + (marker_side_len*units/2.0);
          double new_maxy = (posy*units) + (marker_side_len*units/2.0);
          if (minx < new_minx) new_minx = minx;
          if (miny < new_miny) new_miny = miny;
          if (maxx > new_maxx) new_maxx = maxx;
          if (maxy > new_maxy) new_maxy = maxy;
          IplImage *new_img = cvCreateImage(cvSize(int(new_maxx-new_minx+0.5), int(new_maxy-new_miny+0.5)), IPL_DEPTH_8U, 1);
          cvSet(new_img, cvScalar(255));
          CvRect roi = cvRect(int(minx-new_minx+0.5), int(miny-new_miny+0.5), img->width, img->height);
          cvSetImageROI(new_img, roi);
          cvCopy(img, new_img);
          cvReleaseImage(&img);
          img = new_img;
          roi.x = int((posx*units) - (marker_side_len*units/2.0) - new_minx + 0.5);
          roi.y = int((posy*units) - (marker_side_len*units/2.0) - new_miny + 0.5);
          roi.width = int(marker_side_len*units+0.5); roi.height = int(marker_side_len*units+0.5);
          cvSetImageROI(img, roi);
          minx = new_minx; miny = new_miny;
          maxx = new_maxx; maxy = new_maxy;
        }

        int idi = atoi(id);
        md.SetContent(marker_data_content_type, idi, 0);
        if (filename.str().length()<64) filename<<"_"<<idi;

        Pose pose;
        pose.Reset();
        pose.SetTranslation(0, 0, 0);
        multi_marker.PointCloudAdd(idi, marker_side_len, pose);
        multi_marker.PointCloudRotate(idi, rot);
        multi_marker.PointCloudTranslate(idi, transx, transy, transz);

        md.ScaleMarkerToImage(img);
        cvResetImageROI(img);
    }

    void AddCube()
    {
        if (img)
        {
            cvReleaseImage(&img);
            img = 0;
        }

        for(unsigned int face = 0; face < 6; face++)
        {
          std::cout<<"  marker id ["<< marker_id <<"]: "; std::flush(std::cout);

          if(face == 0)
          {
            assign_rot(0,0,0, 0,0,0, 0,0,0);
            transx = 0; transy = 0; transz = 0;
            posx = 0;
            posy = cube_side_len;
          }
          else if(face == 1)
          {
            assign_rot(0,0,-1, 0,1,0, 1,0,0);
            transx = cube_side_len/2.; transy = 0; transz = -cube_side_len/2.;
            posx = cube_side_len;
            posy = cube_side_len;
          }
          else if(face == 2)
          {
            assign_rot(-1,0,0, 0,1,0, 0,0,-1);
            transx = 0; transy = 0; transz = -cube_side_len;
            posx = 2.*cube_side_len;
            posy = cube_side_len;
          }
          else if(face == 3)
          {
            assign_rot(0,0,1, 0,1,0, -1,0,0);
            transx = -cube_side_len/2.; transy = 0; transz = -cube_side_len/2.;
            posx = 3.*cube_side_len;
            posy = cube_side_len;
          }
          else if(face == 4)
          {
            assign_rot(1,0,0, 0,0,-1, 0,1,0);
            transx = 0; transy = cube_side_len/2.; transz = -cube_side_len/2.;
            posx = 0;
            posy = 0;
          }
          else if(face == 5)
          {
            assign_rot(1,0,0, 0,0,1, 0,-1,0);
            transx = 0; transy = -cube_side_len/2.; transz = -cube_side_len/2.;
            posx = 0;
            posy = 2.*cube_side_len;
          }
          std::stringstream ss;
          ss<<marker_id;
          AddMarker(ss.str().c_str());
          marker_id++;
        }
    }

    void Save()
    {
        if (img)
        {
          IplImage* color_img = cvCreateImage(cvGetSize(img),IPL_DEPTH_8U,3);
          cvCvtColor(img, color_img, CV_GRAY2RGB);
          col::change_color(color_img, color);
          img = color_img;
          img = rotateImage2(img, 90);

          std::stringstream filenamexml;
          filenamexml<<filename.str()<<".xml";
          filename<<".png";
          std::cout<<"Saving: "<<filename.str()<<std::endl;
          cvSaveImage(filename.str().c_str(), img);

          std::cout<<"Saving: "<<filenamexml.str()<<std::endl;
          multi_marker.Save(filenamexml.str().c_str(), alvar::FILE_FORMAT_XML);
        }
    }

    void assign_rot(int xx, int xy, int xz, int yx, int yy, int yz, int zx, int zy, int zz)
    {
      rot[0] = xx;
      rot[1] = xy;
      rot[2] = xz;
      rot[3] = yx;
      rot[4] = yy;
      rot[5] = yz;
      rot[6] = zx;
      rot[7] = zy;
      rot[8] = zz;
    }
};

int main(int argc, char *argv[])
{
    State st;
    try {
        if (argc < 2) st.prompt = true;
        for (int i=1; i<argc; i++)
        {
            if (strcmp(argv[i],"-u") == 0)
                st.units = atof(argv[++i]);
            else if (strcmp(argv[i],"-uin") == 0)
                st.units = (96.0);
            else if (strcmp(argv[i],"-ucm") == 0)
                st.units = (96.0/2.54);
            else if (strcmp(argv[i],"-sm") == 0)
                st.marker_side_len = atof(argv[++i]);
            else if (strcmp(argv[i],"-sc") == 0)
                st.cube_side_len = atof(argv[++i]);
            else if (strcmp(argv[i],"-r") == 0)
                st.content_res = atoi(argv[++i]);
            else if (strcmp(argv[i],"-m") == 0)
                {st.margin_res = atof(argv[++i]); std::cout << st.margin_res << std::endl;}
            else if (strcmp(argv[i],"-p") == 0)
                st.prompt = true;
            else if (strcmp(argv[i],"-c") == 0)
            {
                col::color_t tmp_color = col::get_color(argv[++i][0]);
                st.color = col::get_color(tmp_color);
            }
            else
            {
                std::string s = argv[i];
                if (s.length() > 0) st.marker_id=atoi(s.c_str());
                st.AddCube();
                st.Save();
            }
        }

        // Output usage message
        if (st.prompt)
        {
            std::string filename(argv[0]);
            filename = filename.substr(filename.find_last_of('\\') + 1);
            std::cout << "#=============#" << std::endl;
            std::cout << "= CubeCreator =" << std::endl;
            std::cout << "#=============#" << std::endl;
            std::cout << std::endl;
            std::cout << "Description:" << std::endl;
            std::cout << "  This is a utility to generate pattern with markers for to create cubes." << std::endl;
            std::cout << "  This will generate both the PNG and XML files where you run this utility." << std::endl;
            std::cout << "  Once the PNG is printed, do not cut all markers independently, but only the entire pattern." << std::endl;
            std::cout << std::endl;
            std::cout << "Options usage:" << std::endl;
            std::cout << "  " << filename << " [options] argument" << std::endl;
            std::cout << std::endl;
            std::cout << "    65535             marker with number 65535" << std::endl;
            std::cout << "    -u 96             use units corresponding to 1.0 unit per 96 pixels" << std::endl;
            std::cout << "    -uin              use inches as units (assuming 96 dpi)" << std::endl;
            std::cout << "    -ucm              use cm's as units (assuming 96 dpi) <default>" << std::endl;
            std::cout << "    -sm 5.0           use marker size 5.0x5.0 units (default 9.0x9.0)" << std::endl;
            std::cout << "    -sc 5.0           use cube size 5.0x5.0 units (default 9.0x9.0)" << std::endl;
            std::cout << "    -r 5              marker content resolution -- 0 uses default" << std::endl;
            std::cout << "    -m 1.0            marker margin resolution -- 0 uses default" << std::endl;
            std::cout << "    -p                prompt marker placements interactively from the user" << std::endl;
            std::cout << "    -c B              cube color :  B:blue R:red G:green P:pink S:sky Y:yellow -- default black" << std::endl;
            std::cout << std::endl;

            // Interactive stuff here
            st.marker_type = 0;
            st.marker_data_content_type = MarkerData::MARKER_CONTENT_TYPE_NUMBER;
            std::cout<<"\nPrompt marker placements interactively"<<std::endl;
            std::cout<<"  units: "<<st.units/96.0*2.54<<" cm "<<st.units/96.0<<" inches"<<std::endl;
            std::cout<<"  marker side: "<<st.marker_side_len<<" units"<<std::endl;
            std::cout << std::endl;
            std::cout << std::endl;

            bool loop=true;
            std::string s;

            while(loop)
            {
                std::cout<<"  marker id (use -1 to end and ENTER to automaticaly increase) ["<<st.marker_id<<"]: "; std::flush(std::cout);
                std::getline(std::cin, s); if (s.length() > 0) st.marker_id=atoi(s.c_str());
                if (st.marker_id < 0) break;

                std::cout<<"  Marker size (cm): "; std::flush(std::cout);
                std::getline(std::cin, s); if (s.length() > 0) st.marker_side_len = atof(s.c_str());

                st.cube_side_len = st.marker_side_len;
                std::cout<<"  Cube size (cm): "; std::flush(std::cout);
                std::getline(std::cin, s);
                if (s.length() > 0)
                    st.cube_side_len = atof(s.c_str());

                std::cout<<"  Colors :" << std::endl;
                std::cout<<"  B - blue" << std::endl;
                std::cout<<"  R - red" << std::endl;
                std::cout<<"  G - green" << std::endl;
                std::cout<<"  P - pink" << std::endl;
                std::cout<<"  S - sky" << std::endl;
                std::cout<<"  Y - yellow" << std::endl;
                std::cout<<"  default - black :" << std::endl;
                std::cout<<"  Marker color : "; std::flush(std::cout);
                std::getline(std::cin, s); if (s.length() <= 0) s = " ";
                col::color_t tmp_color = col::get_color(s[0]);
                st.color = col::get_color(tmp_color);

                st.AddCube();
                st.Save();
                st.multi_marker = MultiMarker();
            }
        }

        return 0;
    }
    catch (const std::exception &e) {
        std::cout << "Exception: " << e.what() << endl;
    }
    catch (...) {
        std::cout << "Exception: unknown" << std::endl;
    }
}
