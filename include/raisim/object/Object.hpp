//
// Created by kangd on 10.10.17.
//

#ifndef RAISIM_OBJECT_HPP
#define RAISIM_OBJECT_HPP

#include <memory>
#include <vector>
#include "raisim/math.hpp"
#include "raisim/contact/PerObjectContactList.hpp"
#include "raisim/contact/BisectionContactSolver.hpp"
#include "raisim/Materials.hpp"
#include "raisim/configure.hpp"

#include "ode/ode.h"

namespace raisim {

class World;
class Visualizer;

namespace contact {
class BisectionContactSolver;
class Single3DContactProblem;
}

class Object {
  friend class raisim::World;
  friend class raisim::Visualizer;
  friend class raisim::contact::BisectionContactSolver;

 public:
  explicit Object();
  virtual ~Object() = default;

 public:
  virtual void clearPerObjectContact();

  void addContactToPerObjectContact(contact::Contact &contact);
  void setIndexInWorld(size_t indexInWorld_);

  size_t getIndexInWorld() const;
  const std::vector<contact::Contact> &getContacts() const;
  virtual void updateCollision() = 0;
  virtual void preContactSolverUpdate1(const Vec<3> &gravity, double dt) = 0;
  virtual void preContactSolverUpdate2(const Vec<3> &gravity, double dt) = 0;
  virtual void integrate(double dt) = 0;
  virtual void setExternalForce(size_t localIdx, const Vec<3>& force) = 0;
  virtual void setExternalTorque(size_t localIdx, const Vec<3>& torque) = 0;
  virtual double getMass(size_t localIdx) = 0;
  virtual ObjectType getObjectType() = 0;
  virtual void getPosition_W(size_t localIdx, Vec<3>& pos_w) = 0;
  virtual void getVelocity_W(size_t localIdx, Vec<3>& vel_w) = 0;
  virtual void getOrientation_W(size_t localIdx, Mat<3,3>& rot) = 0;
  virtual void getPosition_W(size_t localIdx, const Vec<3>& pos_b, Vec<3>& pos_w) = 0;
  std::vector<contact::Contact> &getContacts();
  virtual BodyType getBodyType(size_t localIdx) const { return bodyType_; };
  virtual BodyType getBodyType() const { return bodyType_; };
  virtual void getContactPointVel(size_t pointId, Vec<3> &vel) = 0;
  void setName(const std::string& name) { name_ = name; }
  const std::string& getName() const { return name_; }

 protected:
  std::vector<std::pair<std::vector <raisim::Mat<3, 3>>, raisim::Vec<3>>> &getDelassusAndTauStar();
  std::vector<double> &getImpactVel();
  contact::PerObjectContactList &getPerObjectContact();
  virtual void destroyCollisionBodies(dSpaceID id) = 0;
  virtual void addContactPointVel(size_t pointId, Vec<3>& vel) = 0;
  virtual void subContactPointVel(size_t pointId, Vec<3>& vel) = 0;
  virtual void updateTimeStep(double dt) = 0;
  virtual void updateTimeStepIfNecessary(double dt) = 0;
  virtual void updateGenVelWithImpulse(size_t pointId, const Vec<3>& imp) = 0;
  virtual void appendJointLimits(std::vector<contact::Single3DContactProblem, AlignedAllocator<contact::Single3DContactProblem, 32>>& problem) {};
  virtual void enforceJointLimits(contact::Single3DContactProblem& problem) {};

  contact::PerObjectContactList perObjectContact_;

  size_t indexInWorld_;
  BodyType bodyType_ = BodyType::DYNAMIC;
  bool useDel_ = true;
  bool visualizeFramesAndCom_ = true;
  std::string name_;
  std::vector<std::string> localNames_;
};

} // raisim

#endif //RAISIM_COLLISIONOBJECT_HPP
