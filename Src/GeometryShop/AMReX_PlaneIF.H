/*
 *       {_       {__       {__{_______              {__      {__
 *      {_ __     {_ {__   {___{__    {__             {__   {__  
 *     {_  {__    {__ {__ { {__{__    {__     {__      {__ {__   
 *    {__   {__   {__  {__  {__{_ {__       {_   {__     {__     
 *   {______ {__  {__   {_  {__{__  {__    {_____ {__  {__ {__   
 *  {__       {__ {__       {__{__    {__  {_         {__   {__  
 * {__         {__{__       {__{__      {__  {____   {__      {__
 *
 */

#ifndef _PLANEIF_H_
#define _PLANEIF_H_

#include "AMReX_RealVect.H"
#include "AMReX_BaseIF.H"

namespace amrex
{
  ///
  /**
     This implicit function specifies a plane.
  */
  class PlaneIF: public BaseIF
  {
  public:
    ///
    /**
       Constructor specifying plane normal (a_normal), a point on the plance
       (a_point), and whether the domain is the same side as the normal
       (a_inside).
    */
    PlaneIF(const RealVect& a_normal,
            const RealVect& a_point,
            const bool&     a_inside);

    /// Destructor
    virtual ~PlaneIF()
    {
    }

    ///
    /**
       Return the value of the function at a_point.  When delineating a domain,
       the level set value=0 represents the boundary and value<0 is inside the
       fluid.
    */
    virtual Real value(const RealVect& a_point) const override;


    ///
    /**
       Return a newly allocated derived class.  The responsibility
       for deleting the memory is left to the calling function.
    */
    virtual BaseIF* newImplicitFunction() const override;

  protected:
    RealVect m_normal;
    RealVect m_point;
    bool     m_inside;
  };
}

#endif
