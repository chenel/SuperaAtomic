/**
 * \file ImageMeta3D.h
 *
 * \ingroup base
 *
 * \brief Class def header for a class supera::ImageMeta3D
 * 
 * @author kazuhiro
 */

/** \addtogroup base
    @{*/
#ifndef __IMAGEMETA3D_H__
#define __IMAGEMETA3D_H__

#include "supera/base/SuperaType.h"
#include "supera/base/BBox.h"
#include <array>

namespace supera {

  /**
     \class ImageMeta3D
     @brief Meta data for defining voxels (ID, size, position) and voxelized volume (coordinate, size)
  */
  class ImageMeta3D : public BBox3D{
  public:
    /// Default ctor
    ImageMeta3D();
    /// Default dtor
    ~ImageMeta3D() = default;

    ImageMeta3D(const ImageMeta3D&)  = default;
    ImageMeta3D(ImageMeta3D&&)  = default;
    ImageMeta3D& operator=(const ImageMeta3D&) = default;
    ImageMeta3D& operator=(ImageMeta3D&&) = default;

    /// Update voxel count
    void update(size_t xnum,size_t ynum,size_t znum);
    /// Define dimensions
    inline void set(double xmin, double ymin, double zmin,
		    double xmax, double ymax, double zmax,
		    size_t xnum,size_t ynum,size_t znum)
    { BBox3D::update(xmin,ymin,zmin,xmax,ymax,zmax);
      ImageMeta3D::update(xnum,ynum,znum);
    }
    /// Clear method
    void clear();
    /// Uniry == operator
    inline bool operator ==(const ImageMeta3D& rhs) const
    { return ((BBox3D)(rhs) == (BBox3D)(*this) &&
              _xlen == rhs._xlen && _ylen == rhs._ylen && _zlen == rhs._zlen &&
              _xnum == rhs._xnum && _ynum == rhs._ynum && _znum == rhs._znum); }
    /// Uniry != operator
    inline bool operator !=(const ImageMeta3D& rhs) const
    { return !((*this) == rhs); }
    /// Checker if the meta parameters are set properly or not
    inline bool valid() const { return _valid; }
    /// Returns size
    inline VoxelID_t size() const { return _num_element; }
    /// Given a position, returns voxel ID
    inline VoxelID_t id(const Point3D& pt) const
    { return id(pt.x, pt.y, pt.z); }
    /// Given a position, returns voxel ID
    VoxelID_t id(const double x, const double y, const double z) const;
    /// Give the i_x, i_y, i_z indexes (indexs in 3 axes), find the total 1D index:

    VoxelID_t index(const size_t i_x, const size_t i_y, const size_t i_z) const;

    /// Given a voxel id, returns a neighbor voxel ID with (x,y,z) shift
    VoxelID_t shift(const VoxelID_t origin_id,
                    const int shift_x, const int shift_y, const int shift_z) const;
    /// Return value of invalid voxel ID:
    VoxelID_t invalid_voxel_id() const{return kINVALID_VOXELID;}
    /// Given a valid voxel ID, returns a position array
    Point3D position(VoxelID_t id) const;
    /// Given a valid voxel ID, returns X position
    double pos_x(VoxelID_t id) const;
    /// Given a valid voxel ID, returns Y position
    double pos_y(VoxelID_t id) const;
    /// Given a valid voxel ID, returns Z position
    double pos_z(VoxelID_t id) const;
    /// Returns voxel count along x-axis
    inline size_t num_voxel_x()  const { return _xnum; }
    /// Returns voxel count along y-axis
    inline size_t num_voxel_y()  const { return _ynum; }
    /// Returns voxel count along z-axis
    inline size_t num_voxel_z()  const { return _znum; }
    /// Returns voxel size along x-axis;
    inline double size_voxel_x() const { return _xlen; }
    /// Returns voxel size along y-axis;
    inline double size_voxel_y() const { return _ylen; }
    /// Returns voxel size along z-axis;
    inline double size_voxel_z() const { return _zlen; }
    /// text dumper
    std::string  dump() const;

    /// dump to text that could be used to create this instance of ImageMeta3D from C++ code
    std::string dump2cpp(const std::string & instanceName="meta") const;


    // Find x index that corresponds to a specified index
    size_t id_to_x_index(VoxelID_t id) const;
    // Find y index that corresponds to a specified index
    size_t id_to_y_index(VoxelID_t id) const;
    // Find z index that corresponds to a specified index
    size_t id_to_z_index(VoxelID_t id) const;
    // Find xyz index that corresponds to a specified index
    void id_to_xyz_index(VoxelID_t id, size_t& x, size_t& y, size_t& z) const;

  private:

    bool   _valid; ///< Boolean set to true only if voxel parameters are properly set
    size_t _num_element; ///< Total number of voxel elements

    double _xlen; ///< X voxel size in [cm]
    double _ylen; ///< Y voxel size in [cm]
    double _zlen; ///< Z voxel size in [cm]

    size_t _xnum; ///< Number of voxels along X
    size_t _ynum; ///< Number of voxels along Y
    size_t _znum; ///< Number of voxels along Z

  };
}
#endif
