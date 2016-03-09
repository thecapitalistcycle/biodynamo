#ifndef PHYSICS_COLLISION_CHECK_H_
#define PHYSICS_COLLISION_CHECK_H_

#include <array>
#include <memory>

namespace cx3d {
namespace physics {

class PhysicalCylinder;

class CollisionCheck {
 public:
  static double howMuchCanWeMove(const std::array<double, 3>& A, const std::array<double, 3>& B,
                                 const std::array<double, 3>& C, const std::array<double, 3>& D,
                                 const std::array<double, 3>& E, double dim);

  static void addPhysicalBondIfCrossing(const std::array<double, 3>& A, const std::array<double, 3>& B,
                                        const std::array<double, 3>& C, const std::shared_ptr<PhysicalCylinder>& moving,
                                        const std::shared_ptr<PhysicalCylinder>& still);

 private:
  CollisionCheck() = delete;
};

}  // namespace physics
}  // namespace cx3d

#endif  // PHYSICS_COLLISION_CHECK_H_
