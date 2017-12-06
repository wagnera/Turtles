#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
#include <tf/transform_broadcaster.h>

int main(int argc, char* argv[])
{
  ros::init(argc, argv, "map_imagery_publisher");
  ros::NodeHandle nh;
  static tf::TransformBroadcaster br;
  std::string image_filename;
  image_transport::ImageTransport it(nh);
  image_transport::Publisher pub = it.advertise("camera/image", 1);
  cv::Mat image;

  while (!nh.getParam("map_image_publisher/image_filename", image_filename))
  {
     ROS_WARN("Failed to get filename");
     ros::Duration(1).sleep(); 
  }
  image = cv::imread(image_filename);
  cv::waitKey(30);
  if (image.empty())
  {
    ROS_WARN("Failed to read image");
    ros::shutdown();
  }
  std_msgs::Header camera_Header;
  camera_Header.frame_id="camera";
  sensor_msgs::ImagePtr msg = cv_bridge::CvImage(camera_Header, "bgr8", image).toImageMsg();
  tf::Transform transform;
  transform.setOrigin( tf::Vector3(0.0, 0.0, 0.0) );
  tf::Quaternion q;
  q.setRPY(0, 0, 0);
  transform.setRotation(q);

  ros::Rate loop_rate(2);
  while (nh.ok()) {
    pub.publish(msg);
    br.sendTransform(tf::StampedTransform(transform, ros::Time::now(), "map", "camera"));
    ros::spinOnce();
    loop_rate.sleep();
  }
}