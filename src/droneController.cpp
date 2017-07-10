#include <cstdio>
#include "ros/ros.h"
#include "geometry_msgs/TwistStamped.h"
#include "geometry_msgs/PoseStamped.h"
#include <mavros_msgs/CommandBool.h>
#include <mavros_msgs/SetMode.h>
#include <mavros_msgs/State.h>

mavros_msgs::State current_state;



void state_cb(const mavros_msgs::State::ConstPtr& msg){
      current_state = *msg;
  }
  

int main(int argc, char **argv){

	ros::init(argc, argv,"controller");

	ros::NodeHandle n;
ros::Subscriber state_sub = n.subscribe<mavros_msgs::State>
              ("mavros/state", 10, state_cb);
	ros::Publisher local_pos_pub = n.advertise<geometry_msgs::PoseStamped>
				("/mavros/setpoint_position/local", 1000);
	//ros::Publisher accel_pub = n.advertise<geometry_msgs::TwistStamped>("/mavros/setpoint_velocity/cmd_vel",1000);
		
ros::ServiceClient arming_client = n.serviceClient<mavros_msgs::CommandBool>
              ("mavros/cmd/arming");
ros::ServiceClient set_mode_client = n.serviceClient<mavros_msgs::SetMode>
("mavros/set_mode");


ros::Rate rate(10);

	int count =0;

	while(ros::ok() && current_state.connected){
		ros::spinOnce();
		rate.sleep();
	}
	
	geometry_msgs::PoseStamped pose;
	
	pose.pose.position.x= -5;
	pose.pose.position.y= 3;
	pose.pose.position.z= 2;
	for(int i=100;ros::ok()&& i>0; --i){
		local_pos_pub.publish(pose);
		ros::spinOnce();
		rate.sleep();
	}
	  mavros_msgs::SetMode offb_set_mode;
      offb_set_mode.request.custom_mode = "OFFBOARD";
  
      mavros_msgs::CommandBool arm_cmd;
      arm_cmd.request.value = true;

     ros::Time last_request = ros::Time::now();


	while(ros::ok()){
		  if( current_state.mode != "OFFBOARD" &&
              (ros::Time::now() - last_request > ros::Duration(5.0))){
              if( set_mode_client.call(offb_set_mode) &&
                  offb_set_mode.response.success){
                  ROS_INFO("Offboard enabled");
              }
              last_request = ros::Time::now();
          } else {
              if( !current_state.armed &&
                  (ros::Time::now() - last_request > ros::Duration(5.0))){
                  if( arming_client.call(arm_cmd) &&
                      arm_cmd.response.success){
                      ROS_INFO("Vehicle armed");
                  }
                  last_request = ros::Time::now();
              }
          }

		//geometry_msgs::TwistStamped msg;
		//msg.twist.linear.x +=count;
		//ROS_INFO("x velocity : %lf\n",msg.twist.linear.x);
	      pose.pose.position.x=-5;
      pose.pose.position.y=3;
      pose.pose.position.z=2;
	
		local_pos_pub.publish(pose);
		ros::spinOnce();
		rate.sleep();
		++count;	
	}
	return 0;
}
