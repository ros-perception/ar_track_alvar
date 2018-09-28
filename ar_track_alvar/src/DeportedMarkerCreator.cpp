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
    int    marker_type;     // 0:MarkerData, 1:ArToolkit
    double posx, posy, posz;      // The position of marker center in the given units
    int    content_res;
    double margin_res;
    bool   array;
    unsigned char color;

    // MarkerData specific options
    MarkerData::MarkerContentType marker_data_content_type;

    State()
        : img(0),
          filename("marker"),
          minx(0), miny(0), maxx(0), maxy(0),
          prompt(false),
          units(96.0/2.54),      // cm assuming 96 dpi
          marker_side_len(9.0),  // 9 cm
          marker_type(0),
          posx(0), posy(0), posz(0),
          content_res(0),        // 0 uses default
          margin_res(0.0),       // 0.0 uses default (can be n*0.5)
          array(false),
          color(0),
          marker_data_content_type(MarkerData::MARKER_CONTENT_TYPE_NUMBER)
    {}
    ~State() {
        if (img) cvReleaseImage(&img);
    }

    void AddMarker(const char *id) {
        std::cout<<"ADDING MARKER "<<id<<std::endl;

        MarkerData md(marker_side_len, content_res, margin_res);
        int side_len = int(marker_side_len*units+0.5);
        if (img)
        {
            cvReleaseImage(&img);
            img = 0;
        }

        if (img == 0) {
            img = cvCreateImage(cvSize(side_len, side_len), IPL_DEPTH_8U, 1);
            filename.str("");
            filename<<"MarkerData";
        }

        if (marker_data_content_type == MarkerData::MARKER_CONTENT_TYPE_NUMBER)
        {
          int idi = atoi(id);
          md.SetContent(marker_data_content_type, idi, 0);
          std::cout<<"SetContent "<<id<<std::endl;
          if (filename.str().length()<64) filename<<"_"<<idi;

          Pose pose;
          pose.Reset();
          pose.SetTranslation(posx, -posy, posz);
          multi_marker.PointCloudAdd(idi, marker_side_len, pose);
        }
        else
        {
          md.SetContent(marker_data_content_type, 0, id);
          const char *p = id;
          int counter=0;
          filename<<"_";
          while(*p)
          {
            if (!isalnum(*p)) filename<<"_";
            else filename<<(char)tolower(*p);
            p++; counter++;
            if (counter > 8) break;
          }
        }

        md.ScaleMarkerToImage(img);
        cvResetImageROI(img);
        IplImage* color_img = cvCreateImage(cvGetSize(img),IPL_DEPTH_8U,3);
        cvCvtColor(img, color_img, CV_GRAY2RGB);
        col::change_color(color_img, color);
        img = color_img;
    }

    void Save() {
        if (img) {
            std::stringstream filenamexml;
            filenamexml<<filename.str()<<".xml";
            filename<<".png";
            std::cout<<"Saving: "<<filename.str()<<std::endl;
            cvSaveImage(filename.str().c_str(), img);

            std::cout<<"Saving: "<<filenamexml.str()<<std::endl;
            multi_marker.Save(filenamexml.str().c_str(), alvar::FILE_FORMAT_XML);

        }
    }
};

void print_tag()
{
  std::cout << std::endl;
  std::cout << "positive height" << std::endl;
  std::cout << "   ^   " << std::endl;
  std::cout << "   |   " << std::endl;
  std::cout << "#######" << std::endl;
  std::cout << "#  #  #" << std::endl;
  std::cout << "#  #  #" << std::endl;
  std::cout << "#     # --> positive side" << std::endl;
  std::cout << "#     #" << std::endl;
  std::cout << "#     #" << std::endl;
  std::cout << "#######" << std::endl;
  std::cout << std::endl;
}

int main(int argc, char *argv[])
{
    State st;
    try {
        if (argc < 2) st.prompt = true;
        for (int i=1; i<argc; i++) {
            if (strcmp(argv[i],"-1") == 0)
                st.marker_data_content_type = MarkerData::MARKER_CONTENT_TYPE_STRING;
            else if (strcmp(argv[i],"-2") == 0)
                st.marker_data_content_type = MarkerData::MARKER_CONTENT_TYPE_FILE;
            else if (strcmp(argv[i],"-3") == 0)
                st.marker_data_content_type = MarkerData::MARKER_CONTENT_TYPE_HTTP;
            else if (strcmp(argv[i],"-u") == 0)
                st.units = atof(argv[++i]);
            else if (strcmp(argv[i],"-uin") == 0)
                st.units = (96.0);
            else if (strcmp(argv[i],"-ucm") == 0)
                st.units = (96.0/2.54);
            else if (strcmp(argv[i],"-s") == 0)
                st.marker_side_len = atof(argv[++i]);
            else if (strcmp(argv[i],"-r") == 0)
                st.content_res = atoi(argv[++i]);
            else if (strcmp(argv[i],"-m") == 0)
                st.margin_res = atof(argv[++i]);
            else if (strcmp(argv[i],"-p") == 0)
                st.prompt = true;
            else if (strcmp(argv[i],"-xyz") == 0) {
                st.posx = atof(argv[++i]);
                st.posy = atof(argv[++i]);
                st.posz = atof(argv[++i]);
            }
            else if (strcmp(argv[i],"-c") == 0)
            {
                col::color_t tmp_color = col::get_color(argv[++i][0]);
                st.color = col::get_color(tmp_color);
            }
            else
            {
              st.AddMarker(argv[i]);
              st.Save();
            }
        }

        // Output usage message
        if (st.prompt)
        {
            std::string filename(argv[0]);
            filename = filename.substr(filename.find_last_of('\\') + 1);
            std::cout << "#=======================#" << std::endl;
            std::cout << "= DeportedMarkerCreator =" << std::endl;
            std::cout << "#=======================#" << std::endl;
            std::cout << std::endl;
            std::cout << "Description:" << std::endl;
            std::cout << "  This is a utility to generate deported markers." << std::endl;
            std::cout << "  This will generate both the PNG and XML files where you run this utility." << std::endl;
            std::cout << std::endl;
            std::cout << "Options usage:" << std::endl;
            std::cout << "  " << filename << " [options] argument" << std::endl;
            std::cout << std::endl;
            std::cout << "    65535             marker with number 65535" << std::endl;
            std::cout << "    -1 \"hello world\"  marker with string" << std::endl;
            std::cout << "    -2 catalog.xml    marker with file reference" << std::endl;
            std::cout << "    -3 www.vtt.fi     marker with URL" << std::endl;
            std::cout << "    -u 96             use units corresponding to 1.0 unit per 96 pixels" << std::endl;
            std::cout << "    -uin              use inches as units (assuming 96 dpi)" << std::endl;
            std::cout << "    -ucm              use cm's as units (assuming 96 dpi) <default>" << std::endl;
            std::cout << "    -s 5.0            use marker size 5.0x5.0 units (default 9.0x9.0)" << std::endl;
            std::cout << "    -r 5              marker content resolution -- 0 uses default" << std::endl;
            std::cout << "    -m 2.0            marker margin resolution -- 0 uses default" << std::endl;
            std::cout << "    -p                prompt marker placements interactively from the user" << std::endl;
            std::cout << "    -c B              marker color :  B:blue R:red G:green P:pink S:sky Y:yellow -- default black" << std::endl;
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
            int marker_id=0;
            double posx=0.0, posy=0.0, posz=0.0;

            while(loop)
            {
                std::stringstream ss;

                std::cout<<"  marker id (use -1 to end and ENTER to automaticaly increase) ["<<marker_id<<"]: "; std::flush(std::cout);
                std::getline(std::cin, s); if (s.length() > 0) marker_id=atoi(s.c_str());
                if (marker_id < 0) break;

                std::cout<<"  Marker size (cm): "; std::flush(std::cout);
                std::getline(std::cin, s); if (s.length() > 0) st.marker_side_len = atof(s.c_str());

                print_tag();
                std::cout<<"  Marker side translation : "; std::flush(std::cout);
                std::getline(std::cin, s); if (s.length() > 0) posx=atof(s.c_str());
                std::cout<<"  Marker height translation : "; std::flush(std::cout);
                std::getline(std::cin, s); if (s.length() > 0) posy=atof(s.c_str());
                std::cout<<"  Marker depth translation : "; std::flush(std::cout);
                std::getline(std::cin, s); if (s.length() > 0) posz=atof(s.c_str());
                st.posx=-posx;
                st.posy=posy;
                st.posz=-posz;
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

                ss<<marker_id;
                st.AddMarker(ss.str().c_str());
                st.Save();
                marker_id++;
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
