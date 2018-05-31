#include <chrono>
#include <iostream>
#include <signal.h>
#include <termios.h>
#include <fcntl.h>

#include <ros/ros.h>
#include <pluginlib/class_list_macros.h>
#include <nodelet/nodelet.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/opencv.hpp>
#include <geometry_msgs/Pose.h>
#include <geometry_msgs/Vector3.h>
#include <tf/transform_broadcaster.h>
#include <ecl/threads.hpp>

#include "common/common_utils/StrictMode.hpp"

#include "airsim_bridge/ImageMosaicer.h"
#include "vehicles/multirotor/api/MultirotorRpcLibClient.hpp"
#include "vehicles/car/api/CarRpcLibClient.hpp"
#include "common/common_utils/FileSystem.hpp"

#include "HelperFunctions/QuatRotEuler.h"
#include "ImageProc/NumRecog/NumRecog.h"
#include "ImageProc/RectRecog/RectRecog.h"

#include "PID/PID.h"

//前后左右
#define KEYCODE_R 0x43 
#define KEYCODE_L 0x44
#define KEYCODE_U 0x41
#define KEYCODE_D 0x42

//上升下降
#define KEYCODE_W 0x77 
#define KEYCODE_S 0x73 


using namespace std;
using namespace msr::airlib;

//Short typedefs
typedef ImageCaptureBase::ImageRequest ImageRequest;
typedef ImageCaptureBase::ImageResponse ImageResponse;
typedef ImageCaptureBase::ImageType ImageType;

 
namespace airsim_bridge
{
  struct termios cooked, raw;
  int kfd = 0;
  
  //ctrl + c触发
  void quit(int sig){
    //log_data_before_shutting_down();
    //connect_publish_thread.join(); 
    // client_mutex.lock();
    
    //client->armDisarm(false);
    //client->reset();
    //client->enableApiControl(false); 
    cout<<"quit";
    tcsetattr(kfd, TCSANOW, &cooked);//在程序结束时在恢复原来的配置
    ros::shutdown();
    exit(0);
    //abort();
    //exit_out = true; 
   // std::cout << "killing the main thread" << std::endl;
    //std::ofstream myfile;
   // myfile.open("/home/wcui/catkin_ws/blah.txt", std::ofstream::app);
   // myfile << "killing the main thread" << std::endl;
   // myfile.close(); 
    // client_mutex.unlock();
}

class AirsimBridge : public nodelet::Nodelet 
{
public:
  AirsimBridge()
  {
  }
  ~AirsimBridge()
  {
    connect_publish_thread.join();
  }
  
private:  
  //类加载时首先调用onInit函数  onInit方法不能被阻塞，只用于初始化，
  //如果需要执行循环任务，要将其放入线程中执行。
  virtual void onInit()
  {
    ros::NodeHandle& nh = getNodeHandle();
    ros::NodeHandle& private_nh = getPrivateNodeHandle();//for read parameters
  
    //pub = private_nh.advertise<std_msgs::Float64>("out", 10);  
    //sub = private_nh.subscribe("in", 10, &Plus::callback, this); 

    //AirSim
    private_nh.getParam("airsim_ip", airsim_ip);
    private_nh.getParam("airsim_port", airsim_port);
    client = new msr::airlib::MultirotorRpcLibClient(airsim_ip, airsim_port);
    
    
    //控制参数
    private_nh.getParam("pitch", pitch);
    private_nh.getParam("roll", roll);
    private_nh.getParam("throttle", throttle);
    private_nh.getParam("yaw_rate", yaw_rate);
    private_nh.getParam("duration", duration);
    //pid
    private_nh.getParam("kP", kP);
    private_nh.getParam("kI", kI);
    private_nh.getParam("kD", kD);
    
    
    private_nh.getParam("image_freq", image_freq);
    // private_nh.getParam("topic_depthImage", topic_depthImage);
    private_nh.getParam("CLCT_DATA", CLCT_DATA);
    private_nh.getParam("ManualMODE", ManualMODE);

    //发布图像
    image_transport::ImageTransport it(nh);
    pub_cameraInfo = it.advertiseCamera("image_rect", 1);
 
    if(image_freq == 0)
        image_freq = 1;
        
    getCameraParams(private_nh);
    
    signal(SIGINT,quit);
    
    pid.setParam(kP,kI,kD);
    connect_publish_thread.start(&AirsimBridge::connect_publishAirSim, *this);
  }//onInit
  
  
  
  
  //新线程函数 包括阻塞(无人机起飞等)及耗时（循环）操作
  void connect_publishAirSim()
  {
     ROS_INFO("Airsim_brdige started! Connecting...");
     client->confirmConnection();
     client->enableApiControl(true);
     client->armDisarm(true);
     //std::cout << "Press Enter to takeoff" << std::endl; std::cin.get();
     float takeoffTimeout = 5; 
     client->takeoff();
     //std::this_thread::sleep_for(std::chrono::duration<double>(3));
     //client->hover();
    //const float speed = 0.2f;
    //const float size = 0.5f; 
    //const float duration = size / speed;
   
   
   
   //处理键盘事件 
    char c;
    if(ManualMODE)
    { 
      // get the console in raw mode 
      tcgetattr(kfd, &cooked); // 得到 termios 结构体保存，然后重新配置终端
      memcpy(&raw, &cooked, sizeof(struct termios));
      raw.c_lflag &=~ (ICANON | ECHO);
      raw.c_cc[VEOL] = 1;
      raw.c_cc[VEOF] = 2;
      tcsetattr(kfd, TCSANOW, &raw);
    }


    //数字识别
    //训练
    //numRecong.trainmodel();
    
    
    //识别
    const char *model_file_name = "/home/exbot/catkin_ws/src/airsim_bridge/src/ImageProc/NumRecog/model/libsvm_hog_minist_model.model";
    numRecong.loadModel(model_file_name);

  
    targetNumber = 2;
    ros::Rate r(image_freq);
    size_t frameNum=0;
    
    //接收windows数据
    while (ros::ok())
    {
      ros::Time start_hook_t = ros::Time::now(); //计算处理时间
      std::vector<ImageRequest> request = {ImageRequest(0, ImageType::DepthPerspective),
                                           ImageRequest(0, ImageType::Scene),
                                           ImageRequest(3, ImageType::Scene)};
      const std::vector<ImageResponse>& responses = client->simGetImages(request);
      
      if(frameNum%100==0)
        ROS_INFO("received image responses size:%lu, total size:%zu",responses.size(),frameNum);
      if (responses.size() > 0) {
           const ImageResponse& response_frontDepth = responses[0];
           const ImageResponse& response_frontScene = responses[1];
           const ImageResponse& response_groundScene = responses[2];
           realHeight = -response_groundScene.camera_position.z();//飞机离地高度

            //时间戳 保证深度图和相机信息以及tf同步
            uint32_t timestamp_s = uint32_t(response_frontScene.time_stamp / 1000000000);
            uint32_t timestamp_ns = uint32_t(response_frontScene.time_stamp % 1000000000);
            ros::Time timestamp(timestamp_s, timestamp_ns);
            //msg 头包括时间戳和坐标系戳
            std_msgs::Header header;
            header.stamp = timestamp;
            header.frame_id = "camera";           

            //发布深度图和相机参数 到 image process制作点云
            cvMat_frontDepth = cv::imdecode(response_frontDepth.image_data_uint8, cv::IMREAD_GRAYSCALE);
            //cv::imshow("frontDepth",cvMat_frontDepth);
            //cv::waitKey(1);
            cv_bridge::CvImage(header, "32FC1", cvMat_frontDepth).toImageMsg(msgDepth);  
            msgCameraInfo.header.stamp = timestamp;
            msgCameraInfo.height = response_frontDepth.height;
            msgCameraInfo.width = response_frontDepth.width;
            pub_cameraInfo.publish(msgDepth,msgCameraInfo);
            
            //获得前视和下视彩色图
            cvMat_frontScene = cv::imdecode(response_frontScene.image_data_uint8, cv::IMREAD_COLOR  );//前视图
            cvMat_groundScene = cv::imdecode(response_groundScene.image_data_uint8, cv::IMREAD_COLOR  );//下视图
            /*v::imshow("frontScene",cvMat_frontScene);
            cv::imshow("groundScene",cvMat_groundScene);
            cv::waitKey(1);*/
            
            //发布相机 tf坐标树 和点云制作octomap
            tfPublish(getCameraPose(response_frontDepth), timestamp);
          
          
          
          //手动控制模式并保存图像
          if(ManualMODE)
          {
            if(read(kfd, &c, 1) < 0)
            {
              perror("read():");
              exit(-1);
            }

            switch(c)
            {
              case KEYCODE_L:
                client->moveByAngleThrottle(0, -roll, throttle, 0, duration);
                break;
              case KEYCODE_R:
                client->moveByAngleThrottle(0, roll, throttle, 0, duration);
                break;
              case KEYCODE_U:
                client->moveByAngleThrottle(-pitch, 0, throttle, 0, duration);
                break;
              case KEYCODE_D:
                client->moveByAngleThrottle(pitch, 0, throttle, 0, duration);
                break;
              case KEYCODE_W:
                client->moveByAngleThrottle(0.0001, 0, 0.65, 0, duration);
                break;
              case KEYCODE_S:
                client->moveByAngleThrottle(0.0001, 0, 0.55, 0, duration);
                break;
              //default:
                //cout<<"hello"<<endl;//printf("value: %c = 0x%02X = %d\n", c, c, c);
            }       
            //保存图片
            if(frameNum % 2 == 0)
              savePicture(cvMat_frontScene,cvMat_groundScene,cvMat_frontDepth,frameNum,-response_frontDepth.camera_position.z()); 
          }else{
             if(M1_ON)
                ProcM1();
             else
                ProcM2();   
          }

          ++frameNum;
       }//if
      
        
       //计算程序运行时间
       ros::Time end_hook_t = ros::Time::now();
       //ROS_INFO_STREAM("decode "<< (((end_hook_t - start_hook_t).toSec()*1e9)));
       
       r.sleep();
       //ros::spinOnce();
    }//while
  }//thread
  
  
  typedef struct Target
  {
    int targetNumber;//目标数字
    int ground_vertical;//数字地面(0)还是垂直(1)
    
  }Target;
  
  //转圈任务
  void ProcM1()
  {
      //矩形识别
      cv::Mat resultImage1;
      
      bool ifFindSquares = rectRecog.findSquares(cvMat_groundScene, resultImage1);
      if(ifFindSquares)
      {
        //int number = numRecong.getNumber(resultImage1);
        //cout<<"number:"<<number<<endl;
      }

      
      targetHeight= 10;
      double error = targetHeight - realHeight;
      throttle = pid.getOutputByPos(error);
      //-0.00002, -0.00001
      client->moveByAngleThrottle(0,0,throttle,0,duration);
      cout<<"realHeight"<<realHeight<<"  "<<"throttle:"<<throttle<<endl;
      
      
  }
  
  void ProcM2()
  {      /*cv::Mat imageHSV,image_S,image_V;
      cv::cvtColor(cvMat_groundScene, imageHSV, CV_BGR2HSV);     //将image转到HSV空间
      
      image_S.create(imageHSV.size(), imageHSV.depth());   //定义与imageHSV同尺寸和深度的图像image_H
      image_V.create(imageHSV.size(), imageHSV.depth());
      int ch1[] = { 1, 0 };//将imageHSV的S层(1)复制到image_S(0)
      int ch2[] = { 2, 0 };//将imageHSV的V层(2)复制到image_V(0)
      cv::mixChannels(&imageHSV, 1, &image_S, 1, ch1, 1);
      cv::mixChannels(&imageHSV, 1, &image_V, 1, ch2, 1); 
      imshow("image_S",image_S);
      imshow("image_V",image_V);
      cv::waitKey(20);*/
  }
  
  //获取相机内参
  void getCameraParams(const ros::NodeHandle& nh)
  {
      double Tx, Fx, Fy, cx, cy, width, height;
      nh.getParam("Tx",Tx);
      nh.getParam("Fx",Fx);
      nh.getParam("Fy",Fy);
      nh.getParam("cx",cx);
      nh.getParam("cy",cy);
      msgCameraInfo.header.frame_id = "camera";
      //The distortion parameters, size depending on the distortion model.
      //For "plumb_bob", the 5 parameters are: (k1, k2, t1, t2, k3).
      msgCameraInfo.distortion_model = "plumb_bob";//畸变
      msgCameraInfo.D = {0.0, 0.0, 0.0, 0.0, 0.0};
      //投影矩阵(内参)
      msgCameraInfo.K = {Fx,  0.0, cx, 
                       0.0, Fy,  cy, 
                       0.0, 0.0, 1};
      //旋转矩阵
      msgCameraInfo.R = {1.0, 0.0, 0.0, 
                       0.0, 1.0, 0.0,
                       0.0, 0.0, 1.0};
      //立体相机
      msgCameraInfo.P = {Fx,  0.0, cx,  Tx, 
                       0.0, Fy,  cy,  0.0, 
                       0.0, 0.0, 1.0, 0.0};
      /*微软python获得点云示例代码中的投影矩阵
      projectionMatrix = np.array([[-0.501202762, 0.000000000, 0.000000000, 0.000000000],
                                [0.000000000, -0.501202762, 0.000000000, 0.000000000],
                                [0.000000000, 0.000000000, 10.00000000, 100.00000000],
                                [0.000000000, 0.000000000, -10.0000000, 0.000000000]])*/
      msgCameraInfo.binning_x = 0;
      msgCameraInfo.binning_y = 0;
  }
  
  
  //获得相机位姿
  geometry_msgs::Pose getCameraPose(const ImageResponse& response_frontDepth)
  {
      geometry_msgs::Pose pose;
      pose.position.x = response_frontDepth.camera_position.x();
      pose.position.y = response_frontDepth.camera_position.y();
      pose.position.z = response_frontDepth.camera_position.z();
      //ROS_INFO("x:%lf,y:%lf,z:%lf",pose.position.x,pose.position.y,pose.position.z);


      //AirSimz轴朝下,所以需要转换一下 rpy:roll pitch yaw
      geometry_msgs::Quaternion rect_orientation;
      rect_orientation.x = response_frontDepth.camera_orientation.x();
      rect_orientation.y = response_frontDepth.camera_orientation.y();
      rect_orientation.z = response_frontDepth.camera_orientation.z();
      rect_orientation.w = response_frontDepth.camera_orientation.w();

      geometry_msgs::Vector3 rpy =  quat2rpy(rect_orientation);
      rpy.y = -rpy.y;
      rpy.z = -rpy.z + M_PI/2.0;
      //geometry_msgs::Quaternion q_body2cam = setQuat(0.5, -0.5, 0.5, -0.5);
      rect_orientation = rpy2quat(rpy);

      pose.orientation.x = rect_orientation.x;
      pose.orientation.y = rect_orientation.y;
      pose.orientation.z = rect_orientation.z;
      pose.orientation.w = rect_orientation.w;
      return pose;
  }
  //发布相机位姿
  void tfPublish(geometry_msgs::Pose CamPose, const ros::Time& timestamp)
  {
    static tf::TransformBroadcaster br;
    tf::Transform transformQuad, transformCamera;
    
    //the point of camera origin in world coordinate 
    transformCamera.setOrigin(tf::Vector3(CamPose.position.y,
                                          CamPose.position.x,
                                          -CamPose.position.z));
    transformCamera.setRotation(tf::Quaternion(CamPose.orientation.x, 
                                               CamPose.orientation.y, 
                                               CamPose.orientation.z,
                                               CamPose.orientation.w));
    br.sendTransform(tf::StampedTransform(transformCamera, timestamp, "world", "camera"));
  } 
  
  
  //储存数据
  void savePicture(const cv::Mat& frontScene,const cv::Mat& groundScene,const cv::Mat& depth,int framenum,const float height)
  {
      std::vector<int> compression_params;
      compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION); //PNG格式图片的压缩级别    
      compression_params.push_back(0); 
    
      char prefix1[]="/home/exbot/Pictures/frontScene/";
      char prefix2[]="/home/exbot/Pictures/groundScene/";
      char prefix3[]="/home/exbot/Pictures/depth/";
      char postfix[]=".png";  
      
      char path0[255]={0};
      memset(path0,'\0',sizeof(char)*255);
      sprintf(path0,"%sframe_%04d_%f%s",prefix1,framenum,height,postfix);
      imwrite(path0,frontScene);
      
      char path1[255]={0};
      memset(path1,'\0',sizeof(char)*255);
      sprintf(path1,"%sframe_%04d_%f%s",prefix2,framenum,height,postfix);
      imwrite(path1,groundScene);
      
      char path2[255]={0};
      memset(path2,'\0',sizeof(char)*255);
      sprintf(path2,"%sframe_%04d_%f%s",prefix3,framenum,height,postfix);
      imwrite(path2,depth);
  }
  
  /*void callback(const std_msgs::Float64::ConstPtr& input)  {  
    std_msgs::Float64Ptr output(new std_msgs::Float64());  
    output->data = input->data + value_;  
    NODELET_DEBUG("Adding %f to get %f", value_, output->data);  
    pub.publish(output);  
  }*/


   //Airsim参数
   MultirotorRpcLibClient* client;
   string  airsim_ip;//服务端地址
   int     airsim_port;//端口
   int     image_freq=1;//获取图像频率
   string  topic_depthImage="";//发布话题
   
   //无人机参数
   float pitch, roll, throttle, yaw_rate, duration;
   //pid
   PID pid;
   double kP, kI, kD, kN;
   double targetHeight,realHeight; 
   
   //图像相关
   //sensor_msgs::ImagePtr msgDepth;
   cv::Mat cvMat_frontScene;//前视图
   cv::Mat cvMat_groundScene;//下视图
   cv::Mat cvMat_frontDepth;//深度图
   
   //发布深度图 生成点云
   image_transport::CameraPublisher pub_cameraInfo;
   sensor_msgs::Image msgDepth;
   sensor_msgs::CameraInfo msgCameraInfo;//相机内参
   //ros::Subscriber sub;
   
   //数字识别
   NumRecog numRecong;
   RectRecog rectRecog;
   int targetNumber;//目标数字
   
   //新建线程进行阻塞操作
   ecl::Thread connect_publish_thread;
   
   
   bool CLCT_DATA;//决定是否显示处理时间
   bool ManualMODE;//手动模式控制无人机并保存摄像头数据
   
   bool M1_ON = true;//正在进行任务1
   
};//class	
}//namespace
//导出插件
PLUGINLIB_EXPORT_CLASS(airsim_bridge::AirsimBridge, nodelet::Nodelet)
  
  
  
 /*response 结构体信息
vector<uint8_t> image_data_uint8;
vector<float> image_data_float;
int camera_id = -1; //should be filled later
Vector3r camera_position = Vector3r::Zero();
Quaternionr camera_orientation = Quaternionr::Identity();
TTimePoint time_stamp = 0;(uint64_t)
std::string message;
bool pixels_as_float = false;
bool compress = true;
int width = 0, height = 0;
ImageType image_type;
*/
