#ifndef AMREX_LayoutData__H_
#define AMREX_LayoutData__H_

#include "AMReX_FabArrayBase.H"
#include "AMReX_MFIter.H"

namespace amrex
{
  /// a one-thingy-per-box distributed object
  template <class T>
  class LayoutData: public FabArrayBase
  {
  public:

    ///
    LayoutData() = default;

    LayoutData(const BoxArray            & a_grids, 
               const DistributionMapping & a_dm)
      {
        define(a_grids, a_dm);
      }

    void define(const BoxArray           & a_grids,
                const DistributionMapping & a_dm)
      {
        FabArrayBase::define(a_grids, a_dm, 1, 0);

        m_data.resize(local_size());
      }

    LayoutData (const LayoutData& rhs) = delete;
    LayoutData& operator= (const LayoutData& rhs) = delete;
    LayoutData& operator= (LayoutData&& rhs) = delete;

    LayoutData (LayoutData&& rhs) = default;

    T& operator[](const MFIter& a_mfi)
      {
        BL_ASSERT(a_mfi.LocalIndex() < m_data.size());
        BL_ASSERT(DistributionMap() == a_mfi.DistributionMap());
        return (m_data[a_mfi.LocalIndex()]);
      }


    const T& operator[](const MFIter& a_mfi) const
      {
        BL_ASSERT(a_mfi.LocalIndex() < m_data.size());
        BL_ASSERT(DistributionMap() == a_mfi.DistributionMap());
        return (m_data[a_mfi.LocalIndex()]);
      }

    T& operator[](const int & a_box_index)
      {
        int local_index = this->localindex(a_box_index);
        BL_ASSERT(local_index < m_data.size());
        return (m_data[local_index]);
      }


    const T& operator[](const int & a_box_index) const
      {
        int local_index = this->localindex(a_box_index);
        BL_ASSERT(local_index < m_data.size());
        return (m_data[local_index]);
      }

  private:
    Array<T> m_data;
        
  };
}
#endif
