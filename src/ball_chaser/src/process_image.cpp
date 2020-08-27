#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO_DONE: Request a service and pass the velocities to it to drive the robot
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    // Call the safe_move service and pass the requested joint angles
    if (!client.call(srv))
        ROS_ERROR("Failed to call service safe_move");
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{
    int white_pixel = 255;

    int countLeft = 0;
    int countMiddle = 0;
    int countRight = 0;

    // TODO_Done: Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera
    
    bool chase_ball = false;

    int left = img.step * 0.20;
    int right = img.step - left;

    ROS_INFO_STREAM("In image callback");
     
     
    	ROS_INFO_STREAM(left);
	ROS_INFO_STREAM(right);
     
     
	for (int i = 0; i < img.height * img.step; i+=3) {
	
	  if (img.data[i + 0] == white_pixel && img.data[i + 1] == white_pixel && img.data[i + 2] == white_pixel) {
	    chase_ball = true;
	    
	    int currentPosition = i % img.step;    	
	    
	    if (currentPosition <= left) {
	      countLeft++;
	    }
	    else if (currentPosition >= right) {
	      countRight++;
	    }
	    else {
	      countMiddle++;
	    }	 
	    
	    break;
	  }
	}
	
	
    
    
    if (chase_ball) {
	  
       ROS_INFO_STREAM("Chase ball!");
    
    	ROS_INFO_STREAM(countLeft);
	ROS_INFO_STREAM(countMiddle);
	ROS_INFO_STREAM(countRight);
    
    
    	if (countLeft > countRight && countLeft > countMiddle) {
    	  // move left
    	  ROS_INFO_STREAM("LEFT");
    	  drive_robot(0.5, 1);
    	}
    	else if (countRight > countMiddle) {
    	  // move right
     	  ROS_INFO_STREAM("RIGHT");
    	  drive_robot(0.5, -1);
    	}
    	else {
    	  // move straight
    	  ROS_INFO_STREAM("MIDDLE");
    	  drive_robot(1, 0);
    	}
    
    }
    else {
    	// stop
    	  ROS_INFO_STREAM("Stop");
    	  drive_robot(0, 0);   
    }
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
