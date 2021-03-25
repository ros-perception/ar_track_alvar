#include "ar_track_alvar/CvTestbed.h"

CvTestbed::CvTestbed() {
	cap=NULL;
	running=false;
	videocallback=NULL;
	keycallback=NULL;
	images.clear();
}

CvTestbed::~CvTestbed() {
	for (size_t i=0; i<images.size(); i++) {
		if (images[i].release_at_exit) {
			cvReleaseImage(&(images[i].ipl));
		}
	}
	images.clear();
}

void CvTestbed::default_videocallback(IplImage *image) {
	// TODO: Skip frames if we are too slow? Impossible using OpenCV?
	/*
	static bool semaphore=false;
	if (semaphore) return;
	semaphore = true;
	*/

	if (CvTestbed::Instance().videocallback) {
		CvTestbed::Instance().videocallback(image);
	}
	CvTestbed::Instance().ShowVisibleImages();

	//semaphore = false;
}

void CvTestbed::WaitKeys() {
	running=true;
	static bool pause = false;
	while (running) {
		if (cap) {
			IplImage *frame = cap->captureImage();
			if (frame) {
				default_videocallback(frame);
				if (wintitle.size() > 0) {
#if CV_VERSION_MAJOR < 4
					cvShowImage(wintitle.c_str(), frame);
#else
					cv::imshow(wintitle.c_str(), cv::cvarrToMat(frame));
#endif
				}
			}
		}
		int key;
#ifdef WIN32
		if( (key = cvWaitKey(1)) >= 0 ) {
#else
#if CV_VERSION_MAJOR < 4
		if( (key = cvWaitKey(20)) >= 0 ) {
#else
		if( (key = cv::waitKey(20)) >= 0 ) {
#endif
#endif
			if (keycallback) {
				key = keycallback(key);
			}
			// If 'keycallback' didn't handle key - Use default handling
			// By default 'C' for calibration
			if (key == 'C') {
			    if (cap) {
                    cap->showSettingsDialog();
			    }
			}
			// By default '0'-'9' toggles visible images
			else if ((key >= '0') && (key <= '9')) {
				int index=key-'0';
				ToggleImageVisible(index);
			}
			else if(key == 'p') {
				pause = !pause;
			}
			else if (key > 0) {
				running=false;
			}
		}
	}
}

void CvTestbed::ShowVisibleImages() {
	for (size_t i=0; i<images.size(); i++) {
		if (images[i].visible) {
#if CV_VERSION_MAJOR < 4
			cvShowImage(images[i].title.c_str(), images[i].ipl);
#else
			cv::imshow(images[i].title.c_str(), cv::cvarrToMat(images[i].ipl));
#endif
		}
	}
}

CvTestbed& CvTestbed::Instance() {
	static CvTestbed obj;
	return obj;
}

void CvTestbed::SetVideoCallback(void (*_videocallback)(IplImage *image)) {
	videocallback=_videocallback;
}

void CvTestbed::SetKeyCallback(int (*_keycallback)(int key)) {
	keycallback=_keycallback;
}

bool CvTestbed::StartVideo(Capture *_cap, const char *_wintitle) {
	bool clean=false;
    cap = _cap;
    if (cap == NULL) {
		CaptureFactory::CaptureDeviceVector vec = CaptureFactory::instance()->enumerateDevices();
		if (vec.size() < 1) return false;
		cap = CaptureFactory::instance()->createCapture(vec[0]);
		if (!cap->start()) {
			delete cap;
			return false;
		}
		clean=true;
	}
    if (_wintitle) {
        wintitle = _wintitle;
#if CV_VERSION_MAJOR < 4
        cvNamedWindow(_wintitle, 1);
#else
        cv::namedWindow(_wintitle, 1);
#endif
    }
	WaitKeys(); // Call the main loop
	if (clean) {
		cap->stop();
		delete cap;
	}
	return true;
}

size_t CvTestbed::SetImage(const char *title, IplImage *ipl, bool release_at_exit /* =false */) {
	size_t index = GetImageIndex(title);
	if (index == -1) {
		// If the title wasn't found create new
		Image i(ipl, title, false, release_at_exit);
		images.push_back(i);
		return (images.size()-1);
	}
	// If the title was found replace the image
	if (images[index].release_at_exit) {
		cvReleaseImage(&(images[index].ipl));
	}
	images[index].ipl = ipl;
	images[index].release_at_exit = release_at_exit;
	return index;
}

IplImage *CvTestbed::CreateImage(const char *title, CvSize size, int depth, int channels ) {
	IplImage *ipl=cvCreateImage(size, depth, channels);
	if (!ipl) return NULL;
	SetImage(title, ipl, true);
	return ipl;
}

IplImage *CvTestbed::CreateImageWithProto(const char *title, IplImage *proto, int depth /* =0 */, int channels /* =0 */) {
	if (depth == 0) depth = proto->depth;
	if (channels == 0) channels = proto->nChannels;
	IplImage *ipl= cvCreateImage(cvSize(proto->width, proto->height), depth, channels);
	if (!ipl) return NULL;
	ipl->origin = proto->origin;
	SetImage(title, ipl, true);
	return ipl;
}

IplImage *CvTestbed::GetImage(size_t index) {
	if (index < 0) return NULL;
	if (index >= images.size()) return NULL;
	return images[index].ipl;
}

size_t CvTestbed::GetImageIndex(const char *title) {
	std::string s(title);
	for (size_t i=0; i<images.size(); i++) {
		if (s.compare(images[i].title) == 0) {
			return i;
		}
	}
	return (size_t)-1;
}

IplImage *CvTestbed::GetImage(const char *title) {
	return GetImage(GetImageIndex(title));
}

bool CvTestbed::ToggleImageVisible(size_t index, int flags) {
	if (index >= images.size()) return false;
	if (images[index].visible == false) {
		images[index].visible=true;
#if CV_VERSION_MAJOR < 4
		cvNamedWindow(images[index].title.c_str(), flags);
#else
		cv::namedWindow(images[index].title.c_str(), flags);
#endif
		return true;
	}
	else {
		images[index].visible=false;
#if CV_VERSION_MAJOR < 4
		cvDestroyWindow(images[index].title.c_str());
#else
		cv::destroyWindow(images[index].title.c_str());
#endif
		return false;
	}
}
