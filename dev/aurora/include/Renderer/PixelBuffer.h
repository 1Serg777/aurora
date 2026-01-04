#pragma once

#include "Vec.hpp"

#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace aurora {

	template <typename PixelType>
	class PixelBuffer {
	public:
		using pixel_type = PixelType;
		using pixel_type_ref = pixel_type&;
		using pixel_type_cref = const pixel_type&;
		using pixel_type_ptr = pixel_type*;
		using pixel_type_cptr = const pixel_type*;

		using pixel_channel_type = typename pixel_type::component_type;
		using pixel_channel_type_ref = pixel_channel_type&;
		using pixel_channel_type_cref = const pixel_channel_type&;
		using pixel_channel_ptr = pixel_channel_type*;
		using pixel_channel_cptr = const pixel_channel_type*;

		PixelBuffer(uint32_t width, uint32_t height)
			: width(width), height(height) {
			pixelBuffer.resize(width * height);
		}

		// Can throw an 'out_of_range' exception
		void WritePixel(uint32_t rasterCoord_x, uint32_t rasterCoord_y, pixel_type_cref value) {
			CheckOutOfBoundCondition(rasterCoord_x, rasterCoord_y);
			size_t pixel_idx = GetPixelIdx(rasterCoord_x, rasterCoord_y);
			pixelBuffer[pixel_idx] = value;
		}

		void Fill(pixel_type_cref fillValue) {
			std::fill(pixelBuffer.begin(), pixelBuffer.end(), fillValue);
		}

		pixel_type_ref GetPixelValue(uint32_t rasterCoord_x, uint32_t rasterCoord_y) {
			CheckOutOfBoundCondition(rasterCoord_x, rasterCoord_y);
			size_t pixel_idx = GetPixelIdx(rasterCoord_x, rasterCoord_y);
			return pixelBuffer[pixel_idx];
		}
		pixel_type_cref GetPixelValue(uint32_t rasterCoord_x, uint32_t rasterCoord_y) const {
			CheckOutOfBoundCondition(rasterCoord_x, rasterCoord_y);
			size_t pixel_idx = GetPixelIdx(rasterCoord_x, rasterCoord_y);
			return pixelBuffer[pixel_idx];
		}

		uint32_t GetWidth() const {
			return width;
		}
		uint32_t GetHeight() const {
			return height;
		}

	private:
		// Will throw an 'out_of_range' exception if the coordinates are out of range
		void CheckOutOfBoundCondition(uint32_t rasterCoord_x, uint32_t rasterCoord_y) const {
			bool x_out_of_range = (rasterCoord_x < 0 || rasterCoord_x >= width);
			bool y_out_of_range = (rasterCoord_y < 0 || rasterCoord_y >= height);
			if (x_out_of_range || y_out_of_range) {
				throw std::out_of_range{"Out of range raster coordinates provided!"};
			}
		}

		size_t GetPixelIdx(uint32_t rasterCoord_x, uint32_t rasterCoord_y) const {
			size_t pixel_idx = static_cast<size_t>(rasterCoord_y) * width + rasterCoord_x;
			return pixel_idx;
		}

		std::vector<pixel_type> pixelBuffer;
		uint32_t width{};
		uint32_t height{};
	};

	using u8PixelBuffer = PixelBuffer<numa::u8Vec3>; // pixels are in the range [0, 255]

	using f32PixelBuffer = PixelBuffer<numa::Vec3>; // pixels are in the range [0, 1];
	using d64PixelBuffer = PixelBuffer<numa::dVec3>; // pixels are in the range [0, 1];

}