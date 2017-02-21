#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

//#include <opencv2/gpu/device/utility.hpp>

#include <opencv2/video/tracking.hpp>
//#include <opencv2/videoio.hpp>

#include <iostream>
#include <cstring>

#include <ctype.h>
//#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/mat.hpp>

using namespace std;
using namespace cv;

static const std::string OPENCV_WINDOW = "Image window";

int loop_count=0;

Rect prev_rect;



Mat objectHistogram;
Mat globalHistogram;
 
void getObjectHistogram(Mat &frame, Rect object_region) 
{
    const int channels[] = { 0, 1 };     //this means we are going to consider B and G 
    const int histSize[] = { 64, 64 };   //64 X 64
    float range[] = { 0, 256 };			//histogram max value
    const float *ranges[] = { range, range };
 
    // Histogram in object region
    Mat objectROI = frame(object_region);
    calcHist(&objectROI, 1, channels, noArray(), objectHistogram, 2, histSize, ranges, true, false);
    
    
    // A priori color distribution with cumulative histogram
    calcHist(&frame, 1, channels, noArray(), globalHistogram, 2, histSize, ranges, true, true);
    
    
    // Boosting: Divide conditional probabilities in object area by a priori probabilities of colors
    for (int y = 0; y < objectHistogram.rows; y++) {
        for (int x = 0; x < objectHistogram.cols; x++) {
            objectHistogram.at<float>(y, x) /= globalHistogram.at<float>(y, x);
        }
    }
    normalize(objectHistogram, objectHistogram, 0, 255, NORM_MINMAX);
}

void backProjection(const Mat &frame, const Mat &histogram, Mat &bp) {
    const int channels[] = { 0, 1 };
    float range[] = { 0, 256 };
    const float *ranges[] = { range, range };
    calcBackProject(&frame, 1, channels, objectHistogram, bp, ranges);   //express received data as color 
}
 
 

class ImageConverter
{
  ros::NodeHandle nh_;
  image_transport::ImageTransport it_;
  image_transport::Subscriber image_sub_;
  image_transport::Publisher image_pub_;
  
public:
  ImageConverter()
    : it_(nh_)
  {
    // Subscrive to input video feed and publish output video feed
    image_sub_ = it_.subscribe("/dcu/depth_camera/image_raw", 1, 
      &ImageConverter::imageCb, this);
    image_pub_ = it_.advertise("/image_converter/output_video", 1);

    cv::namedWindow(OPENCV_WINDOW);
  }

  ~ImageConverter()
  {
    cv::destroyWindow(OPENCV_WINDOW);
  }

  void imageCb(const sensor_msgs::ImageConstPtr& msg)
  {
    cv_bridge::CvImagePtr cv_ptr;
    try
    {
      cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
    }
    catch (cv_bridge::Exception& e)
    {
      ROS_ERROR("cv_bridge exception: %s", e.what());
      return;
    }

    // Draw an example circle on the video stream
    //if (cv_ptr->image.rows > 60 && cv_ptr->image.cols > 60)
     // cv::circle(cv_ptr->image, cv::Point(50, 50), 10, CV_RGB(255,0,0));

//from webnautes.tistory.com/945

 namedWindow("찾을 색범위 설정", CV_WINDOW_AUTOSIZE); 
 
    //트랙바에서 사용되는 변수 초기화 
     int LowH = 164;
     int HighH = 179;
		
	int LowH2 = 0;
	int HighH2 = 15;
	 					// to find red, we need two ranges.
      int LowS = 150; 
     int HighS = 255;
 
      int LowV = 0;
     int HighV = 255;
 
    
    //트랙바 생성 
     cvCreateTrackbar("LowH", "찾을 색범위 설정", &LowH, 179); //Hue (0 - 179)
     cvCreateTrackbar("HighH", "찾을 색범위 설정", &HighH, 179);
 
      cvCreateTrackbar("LowS", "찾을 색범위 설정", &LowS, 255); //Saturation (0 - 255)
     cvCreateTrackbar("HighS", "찾을 색범위 설정", &HighS, 255);
 
      cvCreateTrackbar("LowV", "찾을 색범위 설정", &LowV, 255); //Value (0 - 255)
     cvCreateTrackbar("HighV", "찾을 색범위 설정", &HighV, 255);
 
 
    Rect trackingWindow(0, 0, 30, 30);
  
    //int i = 0;
    
 
 
    Mat bp;


        
  		 loop_count++; 

        Mat img_input, img_hsv, img_binary,img_binary2,img_tmp;
		
        img_input = cv_ptr->image;
 //		img_input.copyTo(img_tmp);
        //카메라로부터 이미지를 가져옴 


        //HSV로 변환
        cvtColor(img_input, img_hsv, COLOR_BGR2HSV); 
         
            //지정한 HSV 범위를 이용하여 영상을 이진화
        inRange(img_hsv, Scalar(LowH, LowS, LowV), Scalar(HighH, HighS, HighV), img_binary); 

            
 		inRange(img_hsv, Scalar(LowH2,LowS, LowV),Scalar(HighH2,HighS,HighV),img_binary2);
        
		bitwise_or(img_binary,img_binary2,img_binary);				// combine two ranges.
		//morphological opening 작은 점들을 제거 
        erode(img_binary, img_binary, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
        dilate( img_binary, img_binary, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 
 
 
        //morphological closing 영역의 구멍 메우기 
        dilate( img_binary, img_binary, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 
        erode(img_binary, img_binary, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
                  
        imshow("HSV ",img_hsv);
        
        if ( loop_count < 100 )
        {
			int left = 10;
			int top = 10;
			int width=400;
			int height=450;
 
            rectangle( img_input, Point(left,top), Point(left+width,top+height),  
                        Scalar(0,0,255),1 );  
            
            Rect object_region( left, top, width, height);
            
            getObjectHistogram(img_hsv, object_region);
            prev_rect = object_region;
        }
        else{                      
 			if(prev_rect.size().height==0)
			{
				loop_count=0;	
			}
			else{
            	backProjection(img_hsv, objectHistogram, bp);
            	bitwise_and(bp, img_binary, bp);  		//bp =  bp&& img_binary
            	RotatedRect rect = CamShift(bp, prev_rect, cvTermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 20, 1));
 
            	ellipse(img_input, rect, CV_RGB(0,255,0), 1, CV_AA); 
			}
        }
        
        imshow("이진화 영상", img_binary); 
		imshow("원본 영상", img_input); 
		image_pub_.publish(cv_ptr->toImageMsg()); 
        //ESC키 누르면 프로그램 종료
        if (waitKey(1) == 27) 
            return;
    }

//till here.
	
    // Update GUI Window
    //cv::imshow(OPENCV_WINDOW, cv_ptr->image);
    //cv::waitKey(3);
    
    // Output modified video stream
  //  image_pub_.publish(cv_ptr->toImageMsg());
 
};

int main(int argc, char** argv)
{
  ros::init(argc, argv, "image_converter");
  ImageConverter ic;
  ros::spin();
  return 0;
}
