#ifndef __SUPERA_VOXEL_CXX__
#define __SUPERA_VOXEL_CXX__

#include "Voxel.h"
#include <iostream>
#include <cmath>
#include <sstream>

namespace supera {

  Voxel::Voxel(VoxelID_t id, float value)
  { _id = id; _value = value; }

  std::string Voxel::dump2cpp(const std::string &instanceName) const
  {
    std::stringstream ss;

    ss << "supera::Voxel " << instanceName << ";\n";
    ss << instanceName << ".set(static_cast<supera::VoxelID_t>(" << _id << "), " << _value << ");\n";

    return ss.str();
  }

  float VoxelSet::max() const
  {
    float val = kINVALID_FLOAT;
    for(auto const& vox : _voxel_v) {
      if(vox.value() > val) val = vox.value();
    }
    return val;
  }

  void VoxelSet::clear_invalid(bool clear_invalid_float, bool clear_nan, bool clear_inf)
  {
    if(!clear_invalid_float && !clear_nan && !clear_inf) return;
    std::vector<supera::Voxel> vox_v;
    vox_v.reserve(_voxel_v.size());
    for(auto const& vox : _voxel_v) {
      if (clear_inf && std::isinf(vox.value())) continue;
      if (clear_nan && std::isnan(vox.value())) continue;
      if (clear_invalid_float && vox.value() == supera::kINVALID_FLOAT) continue;
      vox_v.push_back(vox);
    }
    _voxel_v = std::move(vox_v);
  }


  float VoxelSet::min() const
  {
    float val = kINVALID_FLOAT;
    for(auto const& vox : _voxel_v) {
      if(vox.value() < val) val = vox.value();
    }
    return val;
  }

  void VoxelSet::threshold(float min, float max)
  {
    std::vector<supera::Voxel> vox_v;
    vox_v.reserve(_voxel_v.size());
    for(auto const& vox : _voxel_v) {
      if(vox.value() < min || vox.value() > max) continue;
      vox_v.push_back(vox);
    }
    _voxel_v = std::move(vox_v);
  }

  void VoxelSet::threshold_min(float min)
  {
    std::vector<supera::Voxel> vox_v;
    vox_v.reserve(_voxel_v.size());
    for(auto const& vox : _voxel_v) {
      if(vox.value() < min) continue;
      vox_v.push_back(vox);
    }
    _voxel_v = std::move(vox_v);
  }

  void VoxelSet::threshold_max(float max)
  {
    std::vector<supera::Voxel> vox_v;
    vox_v.reserve(_voxel_v.size());
    for(auto const& vox : _voxel_v) {
      if(vox.value() > max) continue;
      vox_v.push_back(vox);
    }
    _voxel_v = std::move(vox_v);
  }

  void VoxelSet::add(const Voxel& vox)
  {
    Voxel copy(vox);
    emplace(std::move(copy),true);
  }

  void VoxelSet::insert(const Voxel& vox)
  {
    Voxel copy(vox);
    emplace(std::move(copy),false);
  }

  const Voxel& VoxelSet::find(VoxelID_t id) const
  {
    if(_voxel_v.empty() ||
     id < _voxel_v.front().id() ||
     id > _voxel_v.back().id())
      return kINVALID_VOXEL;

    Voxel vox(id,0.);
    // Else do log(N) search
    auto iter = std::lower_bound(_voxel_v.begin(), _voxel_v.end(), vox);
    if( (*iter).id() == id ) return (*iter);
    else {
      //std::cout << "Returning invalid voxel since lower_bound had an id " << (*iter).id() << std::endl;
      return kINVALID_VOXEL;
    }
  }

  size_t VoxelSet::index(VoxelID_t id) const
  {
    if(_voxel_v.empty() ||
     id < _voxel_v.front().id() ||
     id > _voxel_v.back().id())
      return kINVALID_SIZE;

    Voxel vox(id,0.);
    // Else do log(N) search
    auto iter = std::lower_bound(_voxel_v.begin(), _voxel_v.end(), vox);
    if( (*iter).id() == id ) return (iter - _voxel_v.begin());
    else {
      //std::cout << "Returning invalid voxel since lower_bound had an id " << (*iter).id() << std::endl;
      return kINVALID_SIZE;
    }
  }

  void VoxelSet::emplace(Voxel&& vox, const bool add)
  {
    // In case it's empty or greater than the last one
    if (_voxel_v.empty() || _voxel_v.back() < vox) {
      _voxel_v.emplace_back(std::move(vox));
      return;
    }
    // In case it's smaller than the first one
    if (_voxel_v.front() > vox) {
      _voxel_v.emplace_back(std::move(vox));
      for (size_t idx = 0; (idx + 1) < _voxel_v.size(); ++idx) {
        auto& element1 = _voxel_v[ _voxel_v.size() - (idx + 1) ];
        auto& element2 = _voxel_v[ _voxel_v.size() - (idx + 2) ];
        std::swap( element1, element2 );
      }
      return;
    }

    // Else do log(N) search
    auto iter = std::lower_bound(_voxel_v.begin(), _voxel_v.end(), vox);

    // Cannot be the end
    if ( iter == _voxel_v.end() ) {
      std::cerr << "VoxelSet sorting logic error!" << std::endl;
      throw std::exception();
    }

    // If found, merge
    if ( vox.id() == (*iter).id() ) {
      if(add) (*iter) += vox.value();
      else (*iter).set(vox.id(),vox.value());
      return;
    }
    // Else insert @ appropriate place
    else {
      size_t target_ctr = _voxel_v.size() - (iter - _voxel_v.begin());
      _voxel_v.emplace_back(std::move(vox));
      for (size_t idx = 0; idx < target_ctr; ++idx) {
        auto& element1 = _voxel_v[ _voxel_v.size() - (idx + 1) ];
        auto& element2 = _voxel_v[ _voxel_v.size() - (idx + 2) ];
        std::swap( element1, element2 );
      }
    }
    return;
  }

  bool VoxelSet::operator==(const VoxelSet &rhs) const
  {
    // obviously not the same if they're not the same size
    if (size() != rhs.size())
      return false;

    // because VoxelSets are sorted by nature, we can just compare them element by element
    for (std::size_t idx = 0; idx < size(); idx++)
    {
      if (_voxel_v[idx] != rhs._voxel_v[idx])
        return false;
    }
    return true;
  }

  std::string VoxelSet::dump2cpp(const std::string &instanceName) const
  {
    std::stringstream ss;

    ss << "supera::VoxelSet " << instanceName << ";\n";
    ss << instanceName << ".id(" << _id;
    if (_id > std::numeric_limits<int>::max())  // can happen if it's, for example, kINVALID_INSTANCE
      ss << "ul";
    ss << ");\n";

    ss << instanceName << ".reserve(" << _voxel_v.size() << ");\n";
    for (std::size_t idx = 0; idx < _voxel_v.size(); idx++)
    {
      const supera::Voxel & vox = _voxel_v[idx];
      std::string voxInstance = instanceName + "_vox" + std::to_string(idx);
      ss << vox.dump2cpp(voxInstance);
      ss << instanceName << ".emplace(std::move(" << voxInstance << "), false);\n";
    }

    return ss.str();
  }

  //
  // VoxelSetArray
  //
  float VoxelSetArray::sum() const
  { float res=0.; for(auto const& vox_v : _voxel_vv) res+=vox_v.sum(); return res;}

  float VoxelSetArray::mean() const
  {
    size_t vox_ctr = 0;
    for(auto const& vox_v : _voxel_vv) vox_ctr += vox_v.size();
      return (vox_ctr<1 ? 0. : this->sum() / (float)vox_ctr);
  }

  float VoxelSetArray::max() const
  {
    float val = std::numeric_limits<float>::min();
    float ival = 0.;
    for(auto const& vox_v : _voxel_vv) {
      ival = vox_v.max();
      val = (val < ival ? ival : val);
    }
    return val;
  }

  float VoxelSetArray::min() const
  {
    float val = std::numeric_limits<float>::max();
    float ival = 0.;
    for(auto const& vox_v : _voxel_vv) {
      ival = vox_v.min();
      val = (val > ival ? ival : val);
    }
    return val;
  }

  const supera::VoxelSet& VoxelSetArray::voxel_set(InstanceID_t id) const
  {
    if(id >= _voxel_vv.size()) {
      std::cerr << "VoxelSetArray has no VoxelSet with InstanceID_t " << id << std::endl;
      throw std::exception();
    }
    return _voxel_vv[id];
  }

  void VoxelSetArray::emplace(std::vector<supera::VoxelSet>&& voxel_vv)
  {
    _voxel_vv = std::move(voxel_vv);
    for(size_t id=0; id<_voxel_vv.size(); ++id)
      _voxel_vv[id].id(id);
  }

  inline void VoxelSetArray::insert(const std::vector<supera::VoxelSet>& voxel_vv)
  {
    _voxel_vv = voxel_vv;
    for(size_t id=0; id<_voxel_vv.size(); ++id)
      _voxel_vv[id].id(id);
  }

  void VoxelSetArray::emplace(supera::VoxelSet&& voxel_v)
  {
    if(voxel_v.id() == supera::kINVALID_INSTANCEID) {
      _voxel_vv.emplace_back(std::move(voxel_v));
      _voxel_vv.back().id(_voxel_vv.size()-1);
      return;
    }
    if(voxel_v.id() >= _voxel_vv.size()) {
      size_t orig_size = _voxel_vv.size();
      _voxel_vv.resize(voxel_v.id()+1);
      for(size_t id=orig_size; id<_voxel_vv.size(); ++id)
        _voxel_vv[id].id(id);
    }
    _voxel_vv[voxel_v.id()] = std::move(voxel_v);
  }

  void VoxelSetArray::insert(const supera::VoxelSet& voxel_v)
  {
    if(voxel_v.id() == supera::kINVALID_INSTANCEID) {
      _voxel_vv.push_back(voxel_v);
      _voxel_vv.back().id(_voxel_vv.size()-1);
      return;
    }
    if(voxel_v.id() >= _voxel_vv.size()) {
      size_t orig_size = _voxel_vv.size();
      _voxel_vv.resize(voxel_v.id()+1);
      for(size_t id=orig_size; id<_voxel_vv.size(); ++id)
        _voxel_vv.at(id).id(id);
    }
    _voxel_vv.at(voxel_v.id()) = voxel_v;
  }

  supera::VoxelSet& VoxelSetArray::writeable_voxel_set(const InstanceID_t id)
  {
    if(id >= _voxel_vv.size()) {
      std::cerr << "VoxelSetArray has no VoxelSet with InstanceID_t " << id << std::endl;
      throw std::exception();
    }
    return _voxel_vv[id];
  }

};

#endif

