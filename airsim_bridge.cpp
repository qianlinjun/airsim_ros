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
  virtual void onInit()
  {
    ros::NodeHandle& nh = getNodeHandle();
    ros::NodeHandle& private_nh = getPrivateNodeHandle();//用来获取参数
    //AirSim无人机
    private_nh.getParam("airsim_ip", airsim_ip);
    private_nh.getParam("airsim_port", airsim_port);
    client = new msr::airlib::MultirotorRpcLibClient(airsim_ip, airsim_port);
    //相机参数
    private_nh.getParam("Tx",Tx);
    private_nh.getParam("Fx",Fx);
    private_nh.getParam("Fy",Fy);
    private_nh.getParam("cx",cx);
    private_nh.getParam("cy",cy);
    private_nh.getParam("colums",colums);
    private_nh.getParam("rows",rows);
    setCameraParams();
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
    pid.setParam(kP,kI,kD);
    
    private_nh.getParam("image_freq", image_freq);
    if(image_freq == 0)image_freq = 1;
    private_nh.getParam("CLCT_DATA", CLCT_DATA);
    private_nh.getParam("ManualMODE", ManualMODE);//是否手动控制
    private_nh.getParam("SAVE_PICTURE", SAVE_PICTURE);//是否手动控制
    //发布图像
    image_transport::ImageTransport it(nh);
    pub_cameraInfo = it.advertiseCamera("image_rect", 1);

    signal(SIGINT,quit);//响应ctrl + c
    
    //数字２ 3
    targetList[2].ground_vertical = 0;//地面
    targetList[3].ground_vertical = 0;//地面
    
    connect_publish_thread.start(&AirsimBridge::connect_publishAirSim, *this);//新线程
  }//类加载时首先调用onInit函数  onInit方法不能被阻塞，只用于初始化，如果需要执行循环任务，要将其放入线程中执行。
  
  
  
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
   

   //处理键盘事件 
    char c;
    if(ManualMODE)
    { 
      tcgetattr(kfd, &cooked); // 得到 termios 结构体保存，然后重新配置终端
      memcpy(&raw, &cooked, sizeof(struct termios));
      raw.c_lflag &=~ (ICANON | ECHO);
      raw.c_cc[VEOL] = 1;
      raw.c_cc[VEOF] = 2;
      tcsetattr(kfd, TCSANOW, &raw);
    }
   cout<<"Tx"<<Tx<<endl;
    
    

    //数字识别
    //训练
    //numRecong.trainmodel();
    
    //识别
    const char *model_file_name = "/home/exbot/catkin_ws/src/airsim_bridge/src/ImageProc/NumRecog/model/libsvm_hog_minist_model.model";
    numRecong.loadModel(model_file_name);


    ros::Rate r(image_freq);
    std::vector<ImageRequest> request = {ImageRequest(0, ImageType::DepthPerspective),//深度图(uint8)
                                         ImageRequest(0, ImageType::DepthPerspective, true),//深度图(float)
                                         ImageRequest(0, ImageType::Scene),//前视彩图
                                         ImageRequest(3, ImageType::Scene),//对地彩图
                                         ImageRequest(0, ImageType::DepthVis)};//深度图(显示)
    //接收windows数据
    while (ros::ok())
    {
      ros::Time start_hook_t = ros::Time::now(); //计算处理时间
      const std::vector<ImageResponse>& responses = client->simGetImages(request);   
      if (responses.size() > 0) {
           const ImageResponse& response_frontDepth　= responses[0];
           const ImageResponse& response_frontDepthReal　= responses[1];
           const ImageResponse& response_frontScene  = responses[2];
           const ImageResponse& response_groundScene = responses[3];
           const ImageResponse& response_depthVis = responses[4]; 
           cv::Mat cvMat_frontDepthVis = cv::imdecode(response_depthVis.image_data_uint8, cv::IMREAD_GRAYSCALE);

            ///cv::imshow("frontScene",cvMat_frontScene);
            //cv::imshow("groundScene",cvMat_groundScene);
            //cv::imshow("frontDepth",cvMat_frontDepth);
            //cv::imshow("cvMat_frontDepthReal",cvMat_frontDepthReal);//显示有问题
            //cv::imshow("cvMat_frontDepthVis",cvMat_frontDepthVis);//显示深度
            //cv::waitKey(10);
            
            
            //时间戳 保证深度图和相机信息以及tf同步
            uint32_t timestamp_s = uint32_t(response_frontDepth.time_stamp / 1000000000);
            uint32_t timestamp_ns = uint32_t(response_frontDepth.time_stamp % 1000000000);
            ros::Time timestamp(timestamp_s, timestamp_ns);   
            depthPublish(response_frontDepth,timestamp);//发布深度图和相机参数 到 image process制作点云
            tfPublish(getCameraPose(response_frontDepth), timestamp);//发布相机 tf坐标树 和点云制作octomap
          
          
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
            }else{
               if(M1_ON)
                  goMission1(response_frontScene,
                             response_frontDepthReal,
                             response_groundScene);  
            }

            ++frameNum;
            //if(frameNum%100==0)
             //ROS_INFO("received image responses size:%lu, total size:%zu",responses.size(),frameNum);
            ros::Time end_hook_t = ros::Time::now();
            //ROS_INFO_STREAM("process time "<< (((end_hook_t - start_hook_t).toSec())));
       }//if
       
       //r.sleep();
       //ros::spinOnce();
    }//while
  }//thread
  
  
  
  //转圈任务
  void goMission1(const ImageResponse& response_frontScene,
                  const ImageResponse& response_frontDepthReal,
                  const ImageResponse& response_groundScene)
  {        
        //获得前视和下视彩色图
       cv::Mat　cvMat_frontScene = cv::imdecode(response_frontScene.image_data_uint8, cv::IMREAD_COLOR);//前视图
       cv::Mat　cvMat_groundScene = cv::imdecode(response_groundScene.image_data_uint8, cv::IMREAD_COLOR);//下视图
       cv::Mat　cvMat_frontDepthReal.create(rows,colums , CV_32FC1);//获取深度信息
       double targetHeight= 3;
       double　realHeight　= -response_groundScene.camera_position.z();//飞机离地高度
       float* pData = (float*)cvMat_frontDepthReal.data;  
        for(size_t i=0;i < response_frontDepthReal.image_data_float.size(); ++i)
        {
             //cout<<*pData<<endl;
             if(response_frontDepthReal.image_data_float[i]>100)
                 *pData++ = 100;
             else
                 *pData++ = response_frontDepthReal.image_data_float[i];
        } 
          //保存图片
        if(SAVE_PICTURE)
          savePicture(cvMat_frontScene,cvMat_groundScene,cvMat_frontDepthReal,frameNum);
              
      //观察环境（图像以及自身位置）更新目标列表
      observeEnvironment(cvMat_frontScene,
                         cvMat_groundScene,
                         cvMat_frontDepthReal);
      //搜索数字位置
      if(targetList(targetNumber).findLoc)
      {
      　//飞到对应位置
       //如果在位置范围内执行相应策略
      }else{
        int ground_vertical = targetList(targetNumber).ground_vertical;
        if(ground_vertical==0)
        {
          //飞高
        }
        else if(ground_vertical==1)
        {
          //环视　平移
        }else{
        //不知道位置
        //策略
        }
      }     
      
      double error = targetHeight - realHeight;
      throttle = pid.getOutputByPos(error);
      //-0.00002, -0.00001
      client->moveByAngleThrottle(-0.002, -0.001,throttle,0,duration);
      cout<<"realHeight"<<realHeight<<"  "<<"throttle:"<<throttle<<endl;
  }
  
  
   //前视和下视彩色图
  void observeEnvironment(const cv::Mat& frontScene,
                          const cv::Mat& groundScene,
                          const vector<float>& depthData)
  {
      for (int i = 0; i < image_S.rows; i++)
      {
          uchar* data = image_S.ptr<uchar>(i);
          for (int j = 0; j < image_S.cols; j++)
          {
              if (data[j] >= 0 && data[j] <= 20)
                  data[j] = 0;
              else
                  data[j] = 255;
          }
      }
        //矩形识别
      cv::Mat resultImage1;
      
      bool = rectRecog.findSquares(cvMat_groundScene, resultImage1);
      if(ifFindSquares)
      {
        int number = numRecong.getNumber(resultImage1);
        cout<<"number:"<<number<<endl;
     }
  }
  
  
  //获取相机内参
  void setCameraParams()
  {
      msgCameraInfo.header.frame_id = "camera";
      msgCameraInfo.height = rows;
      msgCameraInfo.width = colums;
      //The distortion parameters, size depending on the distortion model.
      //For "plumb_bob", the 5 parameters are: (k1, k2, t1, t2, k3).
      msgCameraInfo.distortion_model = "plumb_bob";//畸变
      msgCameraInfo.D = {0.0, 0.0, 0.0, 0.0, 0.0};
      msgCameraInfo.K = {Fx,  0.0, cx, 
                       0.0, Fy,  cy, 
                       0.0, 0.0, 1};//投影矩阵(内参)
      msgCameraInfo.R = {1.0, 0.0, 0.0, 
                       0.0, 1.0, 0.0,
                       0.0, 0.0, 1.0};//旋转矩阵
      msgCameraInfo.P = {Fx,  0.0, cx,  Tx, 
                       0.0, Fy,  cy,  0.0, 
                       0.0, 0.0, 1.0, 0.0};//立体相机
      /*微软python获得点云示例代码中的投影矩阵
      projectionMatrix = np.array([[-0.501202762, 0.000000000, 0.000000000, 0.000000000],
                                [0.000000000, -0.501202762, 0.000000000, 0.000000000],
                                [0.000000000, 0.000000000, 10.00000000, 100.00000000],
                                [0.000000000, 0.000000000, -10.0000000, 0.000000000]])*/
      msgCameraInfo.binning_x = 0;
      msgCameraInfo.binning_y = 0;
  }
  
  //发布深度图
  void depthPublish(const ImageResponse& response_frontDepth, const ros::Time& timestamp)
  {
      //msg 头包括时间戳和坐标系戳
      std_msgs::Header header;
      header.stamp = timestamp;
      header.frame_id = "camera";           

      cv::Mat cvMat_frontDepth = cv::imdecode(response_frontDepth.image_data_uint8, cv::IMREAD_GRAYSCALE);//深度图(uint8)
      sensor_msgs::Image msgDepth;
      cv_bridge::CvImage(header, "32FC1", cvMat_frontDepth).toImageMsg(msgDepth);  
      msgCameraInfo.header.stamp = timestamp;
      pub_cameraInfo.publish(msgDepth,msgCameraInfo);
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
  void savePicture(const cv::Mat& frontScene,const cv::Mat& groundScene,const cv::Mat& depth,int framenum)
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
      sprintf(path0,"%sframe_%04d%s",prefix1,framenum,postfix);
      imwrite(path0,frontScene);
      
      char path1[255]={0};
      memset(path1,'\0',sizeof(char)*255);
      sprintf(path1,"%sframe_%04d%s",prefix2,framenum,postfix);
      imwrite(path1,groundScene);
      
      char path2[255]={0};
      memset(path2,'\0',sizeof(char)*255);
      sprintf(path2,"%sframe_%04d%s",prefix3,framenum,postfix);
      imwrite(path2,depth);
  }
  
  /*void callback(const std_msgs::Float64::ConstPtr& input)  {  
    std_msgs::Float64Ptr output(new std_msgs::Float64());  
    output->data = input->data + value_;  
    NODELET_DEBUG("Adding %f to get %f", value_, output->data);  
    pub.publish(output);  
  }*/





private:
   //Airsim参数
   MultirotorRpcLibClient* client;
   string  airsim_ip;//服务端地址
   int     airsim_port;//端口
   int     image_freq=1;//获取图像频率
   string  topic_depthImage="";//发布话题
   size_t frameNum=0;//帧数
   
   //相机参数
   double Tx, Fx, Fy, cx, cy;
   
   //无人机参数
   float pitch, roll, throttle, yaw_rate, duration;
   //pid
   PID pid;
   double kP, kI, kD, kN;
   
   //图像相关
   //sensor_msgs::ImagePtr msgDepth;
   //发布深度图 生成点云
   image_transport::CameraPublisher pub_cameraInfo;
   
   sensor_msgs::CameraInfo msgCameraInfo;//相机内参
   
   //数字识别
   NumRecog numRecong;
   RectRecog rectRecog;
   int targetNumber = 2;//目标数字
   
   
   typedef struct Target
   {
      int ground_vertical = -1;//未知(-1)数字地面(0)还是垂直(1)
      bool findLoc = bool;//是否发现目标位置
      geometry_msgs::Vector3 location;//目标大概位置
   }Target;
   std::vector<Target> targetList(11);//0-10 方便写代码
   
   //新建线程进行阻塞操作
   ecl::Thread connect_publish_thread;
   
   
   bool CLCT_DATA;//决定是否显示处理时间
   bool ManualMODE;//手动模式控制无人机并保存摄像头数据
   bool SAVE_PICTURE;//是否保存图片
   
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
