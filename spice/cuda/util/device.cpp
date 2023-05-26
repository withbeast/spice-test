#include "device.h"

#include <spice/cuda/util/error.h>
#include <spice/util/assert.h>
#include <spice/util/type_traits.h>

#include <iostream>
#include <vector>


using namespace spice::util;


namespace spice::cuda::util
{
nonstd::span<device> device::devices()
{
	static std::array<device, max_devices> _devices{ 0, 1,2,3};
	static size_ const n = [] {
		int_ i = 4;
		// success_or_throw( cudaGetDeviceCount( &i ) );
		spice_assert(
		    spice::util::narrow<uint_>( i ) <= _devices.size(),
		    "spice does not support more than 8 gpus per node." );
		return i;
	}();

	return nonstd::span( _devices.data(), n );
}

device & device::devices( size_ i )
{
	spice_assert( i < devices().size() );
	return devices()[i];
}

device & device::active()
{
	int_ d;
	success_or_throw( cudaGetDevice( &d ) );

	return devices( d );
}

device const device::cpu = device( cudaCpuDeviceId );
device const device::none = device( -2 );


device::operator int_() const { return _id; }


cudaDeviceProp device::props() const
{
	cudaDeviceProp props{};
	success_or_throw( cudaGetDeviceProperties( &props, _id ) );

	return props;
}

void device::set() { success_or_throw( cudaSetDevice( _id ) ); }
void device::synchronize()
{
	auto & x = active();
	set();
	success_or_throw( cudaDeviceSynchronize() );
	x.set();
}


device::device( int_ id ) noexcept
    : _id( id )
{
}
} // namespace spice::cuda::util
