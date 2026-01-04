#include "Renderer/PpmImageWriter.h"

#include <cassert>

namespace aurora {

	// 1. https://www.cs.swarthmore.edu/~soni/cs35/f13/Labs/extras/01/ppm_info.html
	// 2. https://en.wikipedia.org/wiki/Netpbm
	// 3. https://netpbm.sourceforge.net/doc/ppm.html
	// 
	// Example:
	// 
	// P3
	// 4 4
	// 255
	//  0  0  0   100  0   0    0  0  0    255   0  255
	//  0  0  0    0  255 175   0  0  0     0    0   0
	//  0  0  0    0   0   0    0 15 175    0    0   0
	// 255 0 255   0   0   0    0  0  0    255  255 255
	// 
	// In the example, the first three lines are the image header,
	// which in our case are:
	// 
	// P3
	// 4 4
	// 255
	// 
	// The rest are the pixel data.

	PpmImageWriter::PpmImageWriter(const PpmImageProps& ppmProps)
		: ppmProps(ppmProps) {
	}
	PpmImageWriter::PpmImageWriter(const PpmImageProps& ppmProps, std::string_view fileName)
		: ppmProps(ppmProps), fileName(fileName) {
	}

	void PpmImageWriter::ChangeFileName(std::string_view fileName) {
		this->fileName = fileName;
	}

	void PpmImageWriter::WriteImageHeader(std::ostream& os, uint32_t imageWidth, uint32_t imageHeight) const {
		// Write the "PPM" header into the file named "ppmProps.fileName".
		// Create the file first if it doesn't exist.
		os << GetImageFormatMagicNumber() << "\n"
			<< imageWidth << " " << imageHeight << "\n"
			<< ppmProps.maxColorValue << "\n";
	}
	std::string_view PpmImageWriter::GetImageFormatMagicNumber() const {
		switch (ppmProps.ppmImageFormat) {
		case PpmImageFormat::ASCII:
			return "P3";
			break;
		case PpmImageFormat::BINARY:
			return "P6";
			break;
		default:
			assert(false && "Unsupported PPM Image Format provided!");
			return "\0";
			break;
		}
	}

	// PPM_AsciiImageWriter

	PpmAsciiImageWriter::PpmAsciiImageWriter(const PpmImageProps& ppmProps)
		: PpmImageWriter(ppmProps) {
		assert(ppmProps.ppmImageFormat == PpmImageFormat::ASCII &&
			   "Wrong image format! ASCII format is expected!");
	}
	PpmAsciiImageWriter::PpmAsciiImageWriter(const PpmImageProps& ppmProps, std::string_view fileName)
		: PpmImageWriter(ppmProps, fileName) {
		assert(ppmProps.ppmImageFormat == PpmImageFormat::ASCII &&
			   "Wrong image format! ASCII format is expected!");
	}

	std::ofstream PpmAsciiImageWriter::CreateOpenImageFile() {
		std::ofstream file{
			this->fileName,
			std::ios_base::out
		};
		return file;
	}

	void PpmAsciiImageWriter::WriteIntegerPixel(std::ostream& os, const numa::u64Vec3& pixel) {
		numa::u64Vec3 clampedPixel = numa::Clamp(pixel, numa::u64Vec3(0), numa::u64Vec3(255));
		os << clampedPixel.r << " " << clampedPixel.g << " " << clampedPixel.b << "\n";
	}
	void PpmAsciiImageWriter::WriteFloatPixel(std::ostream& os, const numa::dVec3& pixel) {
		numa::dVec3 clampedPixel = numa::Clamp(pixel, numa::dVec3(0.0f), numa::dVec3(1.0f));
		size_t r = static_cast<size_t>(clampedPixel.r * 255.0f);
		size_t g = static_cast<size_t>(clampedPixel.g * 255.0f);
		size_t b = static_cast<size_t>(clampedPixel.b * 255.0f);
		os << r << " " << g << " " << b << "\n";
	}

	// PPM_BinaryImageWriter

	PpmBinaryImageWriter::PpmBinaryImageWriter(const PpmImageProps& ppmProps)
		: PpmImageWriter(ppmProps) {
		assert(ppmProps.ppmImageFormat == PpmImageFormat::BINARY &&
			   "Wrong image format! BINARY format is expected!");
	}
	PpmBinaryImageWriter::PpmBinaryImageWriter(const PpmImageProps& ppmProps, std::string_view fileName)
		: PpmImageWriter(ppmProps, fileName) {
		assert(ppmProps.ppmImageFormat == PpmImageFormat::BINARY &&
			   "Wrong image format! BINARY format is expected!");
	}

	std::ofstream PpmBinaryImageWriter::CreateOpenImageFile() {
		std::ofstream file{
			this->fileName,
			std::ios_base::out | std::ios_base::binary
		};
		return file;
	}

	void PpmBinaryImageWriter::WriteIntegerPixel(std::ostream& os, const numa::u64Vec3& pixel) {
		WriteBinaryPixel(os, pixel);
	}
	void PpmBinaryImageWriter::WriteFloatPixel(std::ostream& os, const numa::dVec3& pixel) {
		numa::dVec3 clampedFloatPixel = numa::Clamp(pixel, numa::dVec3(0.0f), numa::dVec3(1.0f));
		numa::u64Vec3 intPixel{
			static_cast<uint64_t>(clampedFloatPixel.r * 255.0f),
			static_cast<uint64_t>(clampedFloatPixel.g * 255.0f),
			static_cast<uint64_t>(clampedFloatPixel.b * 255.0f),
		};
		WriteBinaryPixel(os, intPixel);
	}
	void PpmBinaryImageWriter::WriteBinaryPixel(std::ostream& os, const numa::u64Vec3& pixel) {
		uint32_t binaryPixel{0};
		binaryPixel |= static_cast<uint8_t>(pixel.r) << 8 * 0;
		binaryPixel |= static_cast<uint8_t>(pixel.g) << 8 * 1;
		binaryPixel |= static_cast<uint8_t>(pixel.b) << 8 * 2;
		os.write(reinterpret_cast<char*>(&binaryPixel), 3);
	}

}