//
// Created by kangd on 02.10.17.
//

#ifndef RAISIM_RAICONTACTOBJECT_HPP
#define RAISIM_RAICONTACTOBJECT_HPP

#include "Eigen/Geometry"
#include "raisim/object/Object.hpp"
#include "raisim/math.hpp"

namespace raisim {

enum GyroscopicMode {
  IMPLICIT_GYROSCOPIC_FORCE_BODY,     // implicit body model (stable, more computation)
  IMPLICIT_GYROSCOPIC_FORCE_WORLD,    // implicit world model (stable, more computation)
  EXPLICIT_GYROSCOPIC_FORCE,          // explicit model (unstable, less computation)
  NO_GYROSCOPIC_FORCE
};

/// this class is only for inheritance
class SingleBodyObject : public Object {

 public:
  explicit SingleBodyObject(ObjectType singleBodyObjectType);

  Eigen::Vector4d getQuaternion() {
    Eigen::Vector4d quat;
    memcpy(quat.data(), bodyQuaternion_.ptr(), 4 * sizeof(double));
    return quat;
  };

  void getQuaternion(Vec<4>& quat) {
    quat = bodyQuaternion_;
  }

  Eigen::Matrix3d getRotationMatrix() {
    Eigen::Matrix3d rot;
    memcpy(rot.data(), bodyRotationMatrix_.ptr(), 9 * sizeof(double));
    return rot;
  };

  void getRotationMatrix(Mat<3,3>& rotation) {
    rotation = bodyRotationMatrix_;
  }

  Eigen::Vector3d getPosition() {
    Eigen::Vector3d pos;
    memcpy(pos.data(), bodyPosition_.ptr(), 3 * sizeof(double));
    return pos;
  };

  Eigen::Vector3d getComPosition() {
    Eigen::Vector3d pos;
    memcpy(pos.data(), comPosition_.ptr(), 3 * sizeof(double));
    return pos;
  };

  Eigen::Vector3d getLinearVelocity() {
    Eigen::Vector3d vel;
    memcpy(vel.data(), linVel_.ptr(), 3 * sizeof(double));
    return vel;
  };

  Eigen::Vector3d getAngularVelocity(){
    Eigen::Vector3d vel;
    memcpy(vel.data(), angVel_.ptr(), 3 * sizeof(double));
    return vel;
  };

  void getPosition_W(size_t localIdx, Vec<3>& pos_w) final {
    pos_w = bodyPosition_;
  }

  void getOrientation_W(size_t localIdx, Mat<3,3>& rot) final {
    rot = bodyRotationMatrix_;
  }

  double getKineticEnergy();
  double getPotentialEnergy(const Vec<3> &gravity);
  double getEnergy(const Vec<3> &gravity);
  const Eigen::Vector3d getLinearMomentum();
  double getMass(size_t localIdx) final;

  Eigen::Matrix3d getInertiaMatrix_B() {
    Eigen::Matrix3d iner;
    memcpy(iner.data(), inertia_b_.ptr(), 9 * sizeof(double));
    return iner;
  }

  Eigen::Matrix3d getInertiaMatrix_W() {
    Eigen::Matrix3d iner;
    memcpy(iner.data(), inertia_w_.ptr(), 9 * sizeof(double));
    return iner;
  }

  ObjectType getObjectType() final;

  dGeomID getCollisionObject() const;

  GyroscopicMode getGyroscopicMode() const;
  virtual void setPosition(const Eigen::Vector3d &originPosition);
  virtual void setPosition(double x, double y, double z);
  virtual void setOrientation(const Eigen::Quaterniond &quaternion);
  virtual void setOrientation(double w, double x, double y, double z);
  virtual void setOrientation(const Eigen::Matrix3d &rotationMatrix);
  virtual void setPose(const Eigen::Vector3d &originPosition, const Eigen::Quaterniond &quaternion);
  virtual void setPose(const Eigen::Vector3d &originPosition, const Eigen::Matrix3d &rotationMatrix);
  void setVelocity(const Eigen::Vector3d &linearVelocity, const Eigen::Vector3d &angularVelocity);
  void setVelocity(double dx, double dy, double dz, double wx, double wy, double wz);
  void setExternalForce(size_t localIdx, const Vec<3>& force) final;
  void setExternalTorque(size_t localIdx, const Vec<3>& torque) final;
  void setGyroscopicMode(GyroscopicMode gyroscopicMode);
  void getPosition_W(size_t localIdx, const Vec<3>& pos_b, Vec<3>& pos_w) final;
  void getPosition_W(Vec<3>& pos_w);
  void getVelocity_W(size_t localIdx, Vec<3>& vel_w) final { vel_w = linVel_; }

  void preContactSolverUpdate1(const Vec<3> &gravity, double dt) final;
  void preContactSolverUpdate2(const Vec<3> &gravity, double dt) final;
  void integrate(double dt) final;
  void getContactPointVel(size_t pointId, Vec<3> &vel) final;

  virtual void updateCollision() override;

  void setLinearDamping(double damping);
  void setAngularDamping(Vec<3> damping);
  void setBodyType(BodyType type);

  CollisionGroup getCollisionGroup();
  CollisionGroup getCollisionMask();

 protected:

  virtual void destroyCollisionBodies(dSpaceID id);
  void addContactPointVel(size_t pointId, Vec<3>& vel) final;
  void subContactPointVel(size_t pointId, Vec<3>& vel) final;
  void updateGenVelWithImpulse(size_t pointId, const Vec<3>& imp) final;
  void updateTimeStep(double dt) final {};
  void updateTimeStepIfNecessary(double dt) final {};

  dGeomID collisionObject_;

  // object type
  ObjectType singleBodyObjectType_;

  /// NOTE
  /// each single body object has 3 frame
  /// 1. body frame (coincides with graphical object)
  /// 2. com frame
  /// 3. bullet frame

  // body pose
  /// body frame does NOT coincides with com frame
  Vec<4> bodyQuaternion_;
  Mat<3, 3> bodyRotationMatrix_;
  Vec<3> bodyPosition_;

  // center of mass pose
  /// com frame orientation always coincides with world frame
  Vec<3> comPosition_;
  Vec<3> body2com_;     // w.r.t body frame

  // bullet pose
  /// bullet frame orientation always coincides with body frame
  Vec<3> colPosition_;
  Vec<3> col2com_;     // w.r.t bullet frame

  // object velocity
  /// com velocity and angular velocity w.r.t com
  Vec<3> linVel_, oldLinVel_;
  Vec<3> angVel_;
  Vec<6> genVel_;

  // physical properties
  Mat<3, 3> inertia_b_;
  Mat<3, 3> inertia_w_;
  Mat<3, 3> inverseInertia_b_;
  Mat<3, 3> inverseInertia_w_;

  // forces
  GyroscopicMode gyroscopicMode_ = IMPLICIT_GYROSCOPIC_FORCE_BODY;
  Vec<3> h_f_, h_tau_;
  Vec<3> taustar_f_, taustar_tau_;
  Vec<3> tau_f_, tau_tau;    // external forces

  // -r (vector from contact point to c.o.m)
  std::vector<Vec<3>> contact2com_;
  
  double mass_, inverseMass_;
  double linearDamping_=0;
  Vec<3> angularDamping_;

  std::vector <raisim::Mat<6,3>> MinvJT_;
  std::vector <raisim::Mat<3,6>> J_;
};

} // rai_simulator

#endif //RAISIM_RAICONTACTOBJECT_HPP
