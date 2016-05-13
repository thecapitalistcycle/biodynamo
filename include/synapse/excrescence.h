#ifndef SYNAPSE_EXCRESCENCE_H_
#define SYNAPSE_EXCRESCENCE_H_

#include <array>
#include <memory>
#include <local_biology/neurite_element.h>

#include "sim_state_serializable.h"

namespace cx3d {

namespace local_biology {
class NeuriteElement;
}  // namespace local_biology

namespace physics {
class PhysicalObject;
}  // namespace physics

namespace simulation {
class ECM;
}  // namespace simualtion

namespace synapse {

using local_biology::NeuriteElement;

class Excrescence : public SimStateSerializable {
 public:
  using UPtr = std::unique_ptr<Excrescence>;

  enum Type {
    kSpine = 0,
    kBouton = 1,
    kSomaticSpine = 2,
    kShaft = 3
  };

  static void setECM(const std::shared_ptr<simulation::ECM>& ecm) {
    ecm_ = ecm;
  }

  Excrescence();

  Excrescence(Excrescence::Type type);

  Excrescence(const std::shared_ptr<physics::PhysicalObject> po, const std::array<double, 2>& origin, double length,
              Excrescence::Type type);

  virtual ~Excrescence();

  virtual StringBuilder& simStateToJson(StringBuilder& sb) const override;

  /** returns the absolute coord of the point where this element is attached on the PO.*/
  virtual std::array<double, 3> getProximalEnd() const;

  /** returns the absolute coord of the point where this element ends.*/
  virtual std::array<double, 3> getDistalEnd() const;

  virtual bool synapseWith(Excrescence* other, bool create_physical_bond) = 0;

  virtual bool synapseWithSoma(Excrescence* other_excrescence, bool create_phyiscal_bond) = 0;

  virtual bool synapseWithShaft(NeuriteElement* other_ne, double max_dis,
                                int nr_segments, bool create_phyiscal_bond) = 0;

  // getters and setters
  virtual Excrescence* getEx() const;

  virtual void setEx(Excrescence* ex);

  virtual double getLength() const;

  virtual void setLength(double length);

  virtual std::shared_ptr<physics::PhysicalObject> getPo() const;

  virtual void setPo(const std::shared_ptr<physics::PhysicalObject>& po);

  virtual std::array<double, 2> getPositionOnPO() const;

  virtual void setPositionOnPO(const std::array<double, 2>& position);

  virtual int getType() const;

  virtual void setType(int type);

 protected:
  static std::shared_ptr<simulation::ECM> ecm_;

  /** the physical object it is attached to.*/
  std::shared_ptr<physics::PhysicalObject> po_;

  /** the other structure with which it forms a synapse.*/
  Excrescence* ex_ = nullptr;

  /** The position in polar coordinates on the Physical Object where it's origin is.*/
  std::array<double, 2> position_on_po_;

  double length_ = 1.0;

  /** spine or bouton*/
  int type_ = 0;  //fixme change to Excrescence::Type

  /** The neurite Element to which the shaft synapse is made */
  NeuriteElement* ne_shaft_;

 private:
  Excrescence(const Excrescence&) = delete;
  Excrescence& operator=(const Excrescence&) = delete;
};

}  // namespace synapse
}  // namespace cx3d

#endif  // SYNAPSE_EXCRESCENCE_H_
