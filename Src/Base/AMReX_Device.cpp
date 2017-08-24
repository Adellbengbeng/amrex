
#include <AMReX_Device.H>
#include <AMReX_ParallelDescriptor.H>

bool amrex::Device::in_device_launch_region = false;

#ifdef CUDA
int amrex::Device::cuda_device_id = 0;
#endif

#ifdef CUDA
extern "C" {
void getNumDeviceUsed(int* ndev){
    *ndev = amrex::ParallelDescriptor::get_num_devices_used();
}
}
#endif