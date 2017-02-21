#include "ros/ros.h"
//#include "foobar/Copymove.h"
//#include "foobar/key.h"
//#include "turtlesim/Pose.h"
//#include "std_msgs/String.h"
#include "geometry_msgs/PoseStamped.h"
#include "geometry_msgs/Pose.h"
#include "geometry_msgs/Point.h"
float x , y;
/*
bool CopyMove(
			//foobar::key::Request &req,
			//	foobar::key::Response &res)
			turtlesim::Pose::Request &req,
			turtlesim::Pose::Response &res)
{
	switch(req.input){
		case 0:
			x -=5;
			res.x = x;
			break;
		case 1:
			x+=5;
			res.x = x;
			break;
		case 2:
			y+=5;
			res.y = y;
			break;
		case 3:
			y-=5;
			res.y = y;
			break;
	}	
	//ROS_INFO("request: %d", req.input);
	ROS_INFO("answer = %f %f",req.x, req.y);
	return true;	
}
*/
void poseCallback(const geometry_msgs::PoseStamped::ConstPtr& req)
{
	geometry_msgs::Pose req2= (req->pose);
	geometry_msgs::Point req3 = (req2.position);
	ROS_INFO("Location : %f %f %f ", req3.x, req3.y, req3.z);
}
int main(int argc, char **argv)
{
	ros::init(argc, argv, "CopyMovelistener");
	ros::NodeHandle n;
	
	ros::Subscriber sub = n.subscribe("/mavros/local_position/pose", 1000,poseCallback);
	ros::spin();

	return 0;
}
