#include "ros/ros.h"
#include "foobar/key.h"
#include <cstdlib>
#include <ctime>

int main(int argc, char **argv){
	ros::init(argc,argv, "copymove_client");
	ros::NodeHandle n;
	
	ros::ServiceClient client = n.serviceClient<foobar::key>("CopyMove");
	foobar::key srv;
	while(1){
		srand((unsigned int)time(NULL));
	srv.request.input = rand()%4;
		if(client.call(srv)){
			ROS_INFO("req : %d",srv.request.input);
		}	
		else{
			ROS_ERROR("???");
			return 1;
		}
	}
//	ros::spin();
	return 0;
}
