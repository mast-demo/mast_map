/** \file 
 */
#include <ros/ros.h>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/opencv.hpp>
#include <geometry_msgs/PoseWithCovarianceStamped.h>
#include "tf/tf.h"
#include <image_transport/image_transport.h>

int image_count = 0;
std::string filename;
geometry_msgs::PoseWithCovarianceStamped pose;
void savePose(void)
{
  FILE *fp;
  char str[256];
  sprintf(str, "%s_pose%04i.txt",filename.c_str(),image_count);
  fp=fopen(str, "w");
  if(!fp) {
    ROS_ERROR("failed to open %s\n",str);
    return;
  }
  tf::Quaternion q;
  tf::quaternionMsgToTF(pose.pose.pose.orientation, q);
  tf::Quaternion qrot = tf::createQuaternionFromRPY(0.0,M_PI/2.0,0.0);
  tf::Matrix3x3 m(q*qrot);
  fprintf(fp,
      "[camera]\n"
      "focal_length = 1.0\n"
      "pixel_aspect = 1\n"
      "principal_point = 0.5 0.5\n"
      "rotation = %f %f %f %f %f %f %f %f %f\n"
      "translation = %f %f %f\n"
      "\n"
      "[view]\n"
      "id = %i\n"
      "name = image%04i\n",
      /*
         m[0].getX(),m[0].getY(),m[0].getZ(),
         m[1].getX(),m[1].getY(),m[1].getZ(),
         m[2].getX(),m[2].getY(),m[2].getZ(),
       */
      m[0].getX(),m[1].getX(),m[2].getX(),
      m[0].getY(),m[1].getY(),m[2].getY(),
      m[0].getZ(),m[1].getZ(),m[2].getZ(),
      pose.pose.pose.position.x,
      pose.pose.pose.position.y,
      pose.pose.pose.position.z,
      image_count,image_count);
  fclose(fp);
}
void imageCallback(const sensor_msgs::ImageConstPtr& msg)
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
    char str[256];
    sprintf(str, "%s_image%04i.png",filename.c_str(),image_count);
    ROS_ASSERT( cv::imwrite( str,  cv_ptr->image ) );
    savePose();
    image_count++;
}
void poseCallback(const geometry_msgs::PoseWithCovarianceStamped::ConstPtr& msg)
{
  pose = *msg;
}
/***
  subscribe to image and pose messages 
  played back from a bag file and save them as images and 
  corresponding poses
 */
int main(int argc, char **argv)
{
  ros::init(argc, argv, "com");
  ros::NodeHandle n;
  ros::NodeHandle nh("~");
  std::string imageTopic, poseTopic;
  nh.param<std::string>("imageTopic", imageTopic, "camera/image");
  nh.param<std::string>("poseTopic", poseTopic, "pose");
  nh.param<std::string>("filename", filename, "agent");
  image_transport::ImageTransport it(n);
  image_transport::Subscriber sub = it.subscribe(imageTopic, 1, imageCallback);
  ros::Subscriber poseSub = n.subscribe(poseTopic, 1, poseCallback);
  image_count = 0;
  ros::spin();
  return 0;
}
