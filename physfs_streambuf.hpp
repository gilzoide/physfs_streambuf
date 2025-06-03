/** @file physfs_streambuf.hpp
 * A std::basic_streambuf implementation based on PhysicsFS files.
 */

/*
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * For more information, please refer to <http://unlicense.org/>
 */
#ifndef __PHYSFS_STREAMBUF_HPP__
#define __PHYSFS_STREAMBUF_HPP__

#include <cstddef>
#include <streambuf>

#include <physfs.h>

#ifndef PHYSFS_STREAMBUF_DEFAULT_BUFFER_SIZE
	#define PHYSFS_STREAMBUF_DEFAULT_BUFFER_SIZE 1024
#endif

template<typename CharT, typename Traits = std::char_traits<CharT>>
class physfs_basic_streambuf : public std::basic_streambuf<CharT, Traits> {
public:
	using char_type = CharT;
	using traits_type = Traits;
	using int_type = typename traits_type::int_type;
	using pos_type = typename traits_type::pos_type;
	using off_type = typename traits_type::off_type;

	physfs_basic_streambuf() : file(nullptr) {}
	physfs_basic_streambuf(PHYSFS_File *file, std::streamsize buffer_size = PHYSFS_STREAMBUF_DEFAULT_BUFFER_SIZE)
		: file(file)
		, buffer_size(buffer_size)
	{
	}
	physfs_basic_streambuf(const char *filename, std::ios::openmode mode, std::streamsize buffer_size = PHYSFS_STREAMBUF_DEFAULT_BUFFER_SIZE)
		: buffer_size(buffer_size)
	{
		switch (mode) {
			case std::ios::app:
				file = PHYSFS_openAppend(filename);
				break;

			case std::ios::in:
				file = PHYSFS_openRead(filename);
				break;

			case std::ios::out:
				file = PHYSFS_openWrite(filename);
				break;

			default:
				file = nullptr;
				break;
		}

		if (file) {
			buffer = new char_type[buffer_size];
			this->setp(buffer, buffer + buffer_size);
		}
	}
	physfs_basic_streambuf(const std::string& filename, std::ios::openmode mode, std::streamsize buffer_size = PHYSFS_STREAMBUF_DEFAULT_BUFFER_SIZE)
		: physfs_basic_streambuf(filename.c_str(), mode, buffer_size)
	{
	}

	virtual ~physfs_basic_streambuf() {
		if (file) {
			PHYSFS_close(file);
		}
		if (buffer) {
			delete buffer;
		}
	}

protected:
	int_type underflow() override {
		if (!file) {
			return traits_type::eof();
		}

		PHYSFS_sint64 read_bytes = PHYSFS_readBytes(file, buffer, buffer_size * sizeof(char_type));
		if (read_bytes < sizeof(char_type)) {
			return traits_type::eof();
		}

		PHYSFS_sint64 read_chars = read_bytes / sizeof(char_type);
		this->setg(buffer, buffer, buffer + read_chars);
		return traits_type::to_int_type(buffer[0]);
	}

	std::streamsize showmanyc() override {
		if (!file) {
			return -1;
		}

		PHYSFS_sint64 current_pos = PHYSFS_tell(file);
		if (current_pos >= 0) {
			PHYSFS_sint64 file_length = PHYSFS_fileLength(file);
			if (file_length >= 0) {
				std::streamsize buffered_size = this->egptr() - this->gptr();
				return file_length - (buffered_size + current_pos);
			}
		}
		return 0;
	}

	int_type overflow(int_type i = traits_type::eof()) override {
		if (!file) {
			return traits_type::eof();
		}

		if (sync() == -1) {
			return traits_type::eof();
		}
		return this->sputc(traits_type::to_char_type(i));
	}

	int sync() override {
		if (!file) {
			return -1;
		}

		std::ptrdiff_t chars_to_write = this->pptr() - this->pbase();
		if (chars_to_write > 0) {
			std::ptrdiff_t bytes_to_write = chars_to_write * sizeof(char_type);
			PHYSFS_sint64 bytes_written = PHYSFS_writeBytes(file, this->pbase(), bytes_to_write);
			if (bytes_written < bytes_to_write) {
				return -1;
			}
			this->setp(buffer, buffer + buffer_size);
		}
		return 0;
	}

private:
	PHYSFS_File *file;
	char_type *buffer;
	std::streamsize buffer_size;
};

using physfs_streambuf = physfs_basic_streambuf<char>;

#endif  // __PHYSFS_STREAMBUF_HPP__
