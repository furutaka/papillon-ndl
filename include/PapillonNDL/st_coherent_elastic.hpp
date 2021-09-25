/*
 * Papillon Nuclear Data Library
 * Copyright 2021, Hunter Belanger
 *
 * hunter.belanger@gmail.com
 *
 * This file is part of the Papillon Nuclear Data Library (PapillonNDL).
 *
 * PapillonNDL is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PapillonNDL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PapillonNDL. If not, see <https://www.gnu.org/licenses/>.
 *
 * */
#ifndef PAPILLON_NDL_ST_COHERENT_ELASTIC_H
#define PAPILLON_NDL_ST_COHERENT_ELASTIC_H

/**
 * @file
 * @author Hunter Belanger
 */

#include <PapillonNDL/ace.hpp>
#include <PapillonNDL/angle_energy.hpp>
#include <PapillonNDL/region_1d.hpp>
#include <algorithm>
#include <optional>

namespace pndl {

/**
 * @brief Holds the Coherent Elastic scattering data for a single nuclide
 *        at a single temperature.
 */
class STCoherentElastic : public AngleEnergy {
 public:
  /**
   * @param ace ACE file which contains thermal scattering law.
   */
  STCoherentElastic(const ACE& ace);
  ~STCoherentElastic() = default;

  /**
   * @brief Evaluates the coherent elastic scattering cross section
   *        at energy E.
   * @param E Incident energy at which to evaluate the cross section in MeV.
   */
  double xs(double E) const {
    if (bragg_edges_.size() == 0) return 0.;

    if (E > bragg_edges_.front() && E < bragg_edges_.back()) {
      // Get index for lower bragg edge
      auto Eit = std::lower_bound(bragg_edges_.begin(), bragg_edges_.end(), E);
      size_t l = std::distance(bragg_edges_.begin(), Eit) - 1;
      return structure_factor_sum_[l] / E;
    } else if (E < bragg_edges_.front()) {
      return 0.;
    } else {
      return structure_factor_sum_.back() / E;
    }
  }

  AngleEnergyPacket sample_angle_energy(
      double E_in, std::function<double()> /*rng*/) const override final {
    if (bragg_edges_.size() == 0) return {1., 0.};

    // Get Bragg edge of scatter
    double Ei = 0.;
    if (E_in > bragg_edges_.front() && E_in < bragg_edges_.back()) {
      // Get index for lower bragg edge
      auto Eit =
          std::lower_bound(bragg_edges_.begin(), bragg_edges_.end(), E_in);
      Eit--;
      Ei = *Eit;
    } else if (E_in < bragg_edges_.front()) {
      Ei = 0.;
    } else {
      Ei = bragg_edges_.back();
    }

    double mu = 1. - (2. * Ei / E_in);

    return {mu, E_in};
  }

  std::optional<double> angle_pdf(double /*E_in*/,
                                  double /*mu*/) const override final {
    return std::nullopt;
  }

  std::optional<double> pdf(double /*E_in*/, double /*mu*/,
                            double /*E_out*/) const override final {
    return std::nullopt;
  }

  /**
   * @brief Returns the vector of Bragg edges.
   */
  const std::vector<double>& bragg_edges() const { return bragg_edges_; }

  /**
   * @brief Returns the vector of the sum of structure factors.
   */
  const std::vector<double>& structure_factor_sum() const {
    return structure_factor_sum_;
  }

 private:
  std::vector<double> bragg_edges_;
  std::vector<double> structure_factor_sum_;
};

}  // namespace pndl

#endif
