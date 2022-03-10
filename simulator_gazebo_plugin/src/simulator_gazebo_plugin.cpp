#include "simulator_gazebo_plugin.hpp"

#include <gazebo/physics/physics.hh>
#include <iostream>
#include <gazebo_ros/node.hpp>

namespace gokart_gazebo_plugin
{

GokartGazeboPlugin::GokartGazeboPlugin()
: robot_namespace_{""},
  last_sim_time_{0},
  last_update_time_{0},
  update_period_ms_{8},
  desired_steering_angle{0},
  desired_velocity{0}
{
}

void GokartGazeboPlugin::Load(gazebo::physics::ModelPtr model, sdf::ElementPtr sdf)
{
  // Get model and world references
  model_ = model;
  world_ = model_->GetWorld();
  auto physicsEngine = world_->Physics();
  physicsEngine->SetParam("friction_model", std::string{"cone_model"});

  // Get robot namespace if one exists
  if (sdf->HasElement("robotNamespace")) {
    robot_namespace_ = sdf->GetElement("robotNamespace")->Get<std::string>() + "/";
  }

  // Set up ROS node and subscribers and publishers
  ros_node_ = gazebo_ros::Node::Get(sdf);
  RCLCPP_INFO(ros_node_->get_logger(), "Loading Boldbot Gazebo Plugin");

  control_command_sub_ = ros_node_->create_subscription<ControlCommand>(
    "/control_cmd",
    1,
    [ = ](ControlCommand::SharedPtr msg) {
      desired_steering_angle = msg->steering_angle;
      desired_velocity = msg->velocity;
    }
  );

  std::string fl_steering_joint_name = "front_left_steering_joint";
  std::string fr_steering_joint_name = "front_right_steering_joint";
  std::string rl_motor_joint_name = "back_left_wheel_joint";
  std::string rr_motor_joint_name = "back_right_wheel_joint";

  front_left_steering.SetJoint(fl_steering_joint_name, 
    10.0, 
    0.0, 
    0.0,
    model_->GetJoint(fl_steering_joint_name)
  );

  front_right_steering.SetJoint(fr_steering_joint_name, 
    10.0, 
    0.0, 
    0.0,
    model_->GetJoint(fr_steering_joint_name)
  );

  rear_left_motor.SetJoint(rl_motor_joint_name, 
    10.0, 
    0.0, 
    0.0,
    model_->GetJoint(rl_motor_joint_name)
  );

  rear_right_motor.SetJoint(rr_motor_joint_name, 
    10.0, 
    0.0, 
    0.0,
    model_->GetJoint(rr_motor_joint_name)
  );

  //RCLCPP_DEBUG(ros_node_->get_logger(), "Got joints:");

  // Hook into simulation update loop
  update_connection_ = gazebo::event::Events::ConnectWorldUpdateBegin(
    std::bind(&GokartGazeboPlugin::Update, this));
}

void GokartGazeboPlugin::Update()
{
  auto cur_time = world_->SimTime();
  if (last_sim_time_ == 0) {
    last_sim_time_ = cur_time;
    last_update_time_ = cur_time;
    return;
  }

  auto dt = (cur_time - last_sim_time_).Double();

  // Compute pid for speed

  auto err_rear_left = rear_left_motor.joint_->GetVelocity(0) - desired_velocity; // need to chceck if id of rotation axis is 0
  auto err_rear_right = rear_right_motor.joint_->GetVelocity(0) - desired_velocity;

  auto force_rear_left = rear_left_motor.pid.Update(err_rear_left, dt);
  auto force_rear_right = rear_right_motor.pid.Update(err_rear_right, dt);

  rear_left_motor.joint_->SetForce(0, force_rear_left); // need to chceck if id of rotation axis is 0
  rear_right_motor.joint_->SetForce(0, force_rear_right);

  // Compute pid for steering

  last_sim_time_ = cur_time;
}

GZ_REGISTER_MODEL_PLUGIN(GokartGazeboPlugin)

}  // namespace boldbot_gazebo_plugin
