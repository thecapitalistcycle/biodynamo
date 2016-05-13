#include "local_biology/soma_element.h"

#include "matrix.h"
#include "sim_state_serialization_util.h"

#include "local_biology/local_biology_module.h"

#include "simulation/ecm.h"
#include "physics/physical_object.h"
#include "physics/physical_sphere.h"

namespace cx3d {
namespace local_biology {

SomaElement::SomaElement()
    : CellElement() {
  ecm_->addSomaElement(this);
}

SomaElement::~SomaElement() {
  ecm_->removeSomaElement(this);
}

StringBuilder& SomaElement::simStateToJson(StringBuilder& sb) const {
  CellElement::simStateToJson(sb);

  SimStateSerializationUtil::keyValue(sb, "physical", physical_);
  SimStateSerializationUtil::removeLastChar(sb);

  sb.append("}");
  return sb;
}

SomaElement::UPtr SomaElement::divide(double volume_ratio, double phi, double theta) {
  auto new_soma = UPtr(new SomaElement());
  auto pc = physical_->divide(volume_ratio, phi, theta);
  new_soma->setPhysical(pc);   // this method also sets the callback

  // Copy of the local biological modules:
  for (auto m : local_biology_modules_) {
    if (m->isCopiedWhenSomaDivides()) {
      new_soma->addLocalBiologyModule(m->getCopy());
    }
  }
  return new_soma;
}

void SomaElement::run() {
  runLocalBiologyModules();
}

NeuriteElement* SomaElement::extendNewNeurite() {
  return extendNewNeurite(Param::kNeuriteDefaultDiameter);
}

NeuriteElement* SomaElement::extendNewNeurite(double diameter) {
  //andreas thinks this gives a better distribution based on some friends of mine.
  double phi = (ecm_->getRandomDouble1() - 0.5f) * 2 * Param::kPi;
  double theta = ecm_->asin(ecm_->getRandomDouble1() * 2 - 1) + Param::kPi / 2;

  return extendNewNeurite(diameter, phi, theta);
}

NeuriteElement* SomaElement::extendNewNeurite(const std::array<double, 3>& direction) {
  // we do this cause transform is for 2 points in space and not for a direction:
  auto dir = Matrix::add(direction, physical_->getMassLocation());
  auto angles = physical_->transformCoordinatesGlobalToPolar(dir);
  return extendNewNeurite(Param::kNeuriteDefaultDiameter, angles[1], angles[2]);
}

NeuriteElement* SomaElement::extendNewNeurite(double diameter, const std::array<double, 3>& direction) {
  // we do this cause transform is for 2 points in space and not for a direction:
  auto dir = Matrix::add(direction, physical_->getMassLocation());
  auto angles = physical_->transformCoordinatesGlobalToPolar(dir);
  return extendNewNeurite(diameter, angles[1], angles[2]);
}

NeuriteElement* SomaElement::extendNewNeurite(double diameter, double phi, double theta) {
  // creating the new NeuriteElement and PhysicalCylinder, linking them
  double length = Param::kNeuriteDefaultActualLength;
  auto ne = NeuriteElement::UPtr { new NeuriteElement() };
  auto pc = physical_->addNewPhysicalCylinder(length, phi, theta);
  ne->setPhysical(pc);
  // setting diameter for new branch
  pc->setDiameter(diameter, true);
  // setting ref for Cell
  ne->setCell(getCell());
  // copy of the biological modules
  for (auto module : local_biology_modules_) {
    if (module->isCopiedWhenNeuriteExtendsFromSoma())
      ne->addLocalBiologyModule(module->getCopy());
  }
  auto ne_raw = ne.get();
  cell_->addNeuriteElement(std::move(ne));
  return ne_raw;
}

std::shared_ptr<physics::PhysicalObject> SomaElement::getPhysical() const {
  return physical_;
}

void SomaElement::setPhysical(const std::shared_ptr<physics::PhysicalObject>& po) {
  physical_ = std::static_pointer_cast<physics::PhysicalSphere>(po);
  physical_->setSomaElement(this);
}

std::shared_ptr<physics::PhysicalSphere> SomaElement::getPhysicalSphere() const {
  return physical_;
}

void SomaElement::setPhysicalSphere(const std::shared_ptr<physics::PhysicalSphere>& ps) {
  physical_ = ps;
  physical_->setSomaElement(this);
}

std::list<NeuriteElement*> SomaElement::getNeuriteList() const {
  std::list<NeuriteElement*> neurites;
  auto cylinders = physical_->getDaughters();
  for (auto pc : cylinders) {
    neurites.push_back(pc->getNeuriteElement());
  }
  return neurites;
}

bool SomaElement::isANeuriteElement() const {
  return false;
}

bool SomaElement::isASomaElement() const {
  return true;
}

}  // namespace local_biology
}  // namespace cx3d
