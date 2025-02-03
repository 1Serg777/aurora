#pragma once

#include "PixelBuffer.h"

#include <cstdint>
#include <fstream>
#include <string>
#include <string_view>
#include <type_traits>

namespace aurora
{
	enum class PpmImageFormat
	{
		ASCII,
		BINARY
	};

	struct PpmImageProps
	{
		uint32_t maxColorValue{ 255 };
		PpmImageFormat ppmImageFormat{ PpmImageFormat::ASCII };
	};

	class PpmImageWriter
	{
	public:

		PpmImageWriter(const PpmImageProps& ppmProps);
		virtual ~PpmImageWriter() = default;

		template<
			typename PixelBufferType,
			std::enable_if_t<std::is_integral_v<typename PixelBufferType::pixel_channel_type>, bool> = true>
		void WritePixels(const PixelBufferType& pixelBuffer)
		{
			// Integer pixels

			std::ofstream file = CreateOpenImageFile();

			WriteImageHeader(file, pixelBuffer.GetWidth(), pixelBuffer.GetHeight());

			for (uint32_t y = 0; y < pixelBuffer.GetHeight(); y++)
			{
				for (uint32_t x = 0; x < pixelBuffer.GetWidth(); x++)
				{
					WriteIntegerPixel(file, pixelBuffer.GetPixelValue(x, y));
				}
			}
		}

		template<
			typename PixelBufferType,
			std::enable_if_t<std::is_floating_point_v<typename PixelBufferType::pixel_channel_type>, bool> = true>
		void WritePixels(const PixelBufferType& pixelBuffer)
		{
			// Floating point pixels

			std::ofstream file = CreateOpenImageFile();

			WriteImageHeader(file, pixelBuffer.GetWidth(), pixelBuffer.GetHeight());

			for (uint32_t y = 0; y < pixelBuffer.GetHeight(); y++)
			{
				for (uint32_t x = 0; x < pixelBuffer.GetWidth(); x++)
				{
					WriteFloatPixel(file, numa::dVec3{ pixelBuffer.GetPixelValue(x, y) });
				}
			}
		}

		void ChangeFileName(std::string_view fileName);

	protected:

		virtual std::ofstream CreateOpenImageFile() = 0;

		virtual void WriteIntegerPixel(std::ostream& os, const numa::i64Vec3& pixel) = 0;
		virtual void WriteFloatPixel(std::ostream& os, const numa::dVec3& pixel) = 0;

		void WriteImageHeader(std::ostream& os, uint32_t imageWidth, uint32_t imageHeight) const;

		std::string_view GetImageFormatMagicNumber() const;

		std::string fileName;
		PpmImageProps ppmProps{};
	};

	class PpmAsciiImageWriter : public PpmImageWriter
	{
	public:

		PpmAsciiImageWriter(const PpmImageProps& ppmProps);
		virtual ~PpmAsciiImageWriter() = default;

	protected:

		std::ofstream CreateOpenImageFile() override;

		void WriteIntegerPixel(std::ostream& os, const numa::i64Vec3& pixel) override;
		void WriteFloatPixel(std::ostream& os, const numa::dVec3& pixel) override;
	};

	class PpmBinaryImageWriter : public PpmImageWriter
	{
	public:

		PpmBinaryImageWriter(const PpmImageProps& ppmProps);
		virtual ~PpmBinaryImageWriter() = default;

	protected:

		std::ofstream CreateOpenImageFile() override;

		void WriteIntegerPixel(std::ostream& os, const numa::i64Vec3& pixel) override;
		void WriteFloatPixel(std::ostream& os, const numa::dVec3& pixel) override;

		void WriteBinaryPixel(std::ostream& os, const numa::i64Vec3& pixel);
	};

	PpmImageWriter* CreateImageWriter(const PpmImageProps& ppmImageProps);
	void DeleteImageWriter(PpmImageWriter* imageWriter);
}