//
// Created by marius on 16.04.16.
//

#include "auto_trax_controller/controller_processor.h"

namespace auto_trax {

ControllerProcessor::ControllerProcessor(const ros::NodeHandle& nodehandle,
                                         const ParameterBag& params_bag):
    nh_(nodehandle),
    parameter_(params_bag),
    pid_(parameter_.pid_bag),
    setpoint_(parameter_.setpoint) {
  ROS_DEBUG("Auto_trax PID Processor started!");

  // Create Subscriber for the set point and plant state
  sub_set_point_= nh_.subscribe(parameter_.subscribed_rostopic_setpoint,
                                parameter_.queue_size_subscriber_setpoint,
                                &ControllerProcessor::CallbackSetPoint, this);
  sub_plant_state_= nh_.subscribe(parameter_.subscribed_rostopic_plantstate,
                                  parameter_.queue_size_subscriber_plantstate,
                                  &ControllerProcessor::CallbackPlantState,
                                  this);

  // Create Publisher for control effort
  pub_control_effort_ = nh_.advertise<std_msgs::Float64>(
      parameter_.pub_rostopic_control_effort,
      parameter_.queue_size_pub_control_effort);

  // Service for applying the controller output
  ros::service::waitForService(parameter_.output_service_name);
  client_ = nh_.serviceClient<auto_trax_io::IOSetpoint>(
      parameter_.output_service_name);
}

ControllerProcessor::~ControllerProcessor(){
}

void ControllerProcessor::CallbackSetPoint(const std_msgs::Float64& setpoint_msg) {
  ROS_DEBUG("Set Point received!");
  setpoint_ = setpoint_msg.data;
}

void ControllerProcessor::CallbackPlantState(const std_msgs::Float64& state_msg) {
  ROS_DEBUG("Plant State received!");

  float plant_state = state_msg.data;
  double control_effort = pid_.GetControlEffort(setpoint_, plant_state);

  // Publish the stabilizing control effort
  std_msgs::Float64 control_msg;
  control_msg.data = control_effort;
  pub_control_effort_.publish(control_msg);

  std::cout << "Control_effort: " << control_effort << std::endl;

  float io_setpoint = static_cast<float>(control_effort);
  auto_trax_io::IOSetpoint srv;
  srv.request.setpoint = io_setpoint;

  if (client_.call(srv)) {
    ROS_INFO("IO Setpoint: %d | %f \n", srv.response.success,
             srv.request.setpoint);
  } else {
    ROS_INFO("Failed to call service!!");
  }
}

} // namespace auto_trax
