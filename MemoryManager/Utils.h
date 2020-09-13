/**
* Some utilities I find useful for my projects.
*
* Macros:
*	- ARRAY_SIZE(arr) -> int
*		- Returns length of static defined array
*	- ASSERT(test, expected) -> void
*		- Debug equivalence assertion
*	- BASH(cmd) -> char*
*		- Denotes bash cmd to std::system
*	- VARNAME(var) -> char*
*		- Stringifies variable name
*	- TIMERSTART(id) -> void
*		- Starts a timer with unique id
*	- TIMEREND(id) -> void
*		- Ends timer with id and prints time taken
*
* Functions:
*
*
* Classes/Structs:
*	- Array<T>
*		- Methods: print, sort, reduce, size
*	- Vec3d
*		- Methods: Mag, Dot, Cross, AngleBetween
*				   ProjOnto, ProjOntoScalarComponent
*/

#ifndef _UTILS_H
#define _UTILS_H

// Standard includes
#include <iostream>
#include <chrono>
#include <string>

// Macros
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

#define ASSERT(test, expected)\
std::cout << "Assert: " << #test << " same as " << #expected << "\n";\
std::cout << "Was: " << test << ", Expected: " << expected << "\n";\
std::cout << "Equivalence: " << (test == expected ? "true" : "false") << "\n\n"

#define BASH(cmd) "bash -c " cmd

#define VARNAME(var) #var

#define TIMERSTART(id) auto _t_start_##id = std::chrono::high_resolution_clock::now();\
std::cout << "Timer" #id " start." << std::endl

#define TIMEREND(id) auto _t_end_##id = std::chrono::high_resolution_clock::now();\
std::cout << "Timer" #id ": " << std::chrono::duration_cast<std::chrono::milliseconds>(_t_end_##id - _t_start_##id).count() << "ms\n"

namespace util { // start util namespace

	template <class T>
	class Array
	{
	private:
		T* m_buffer;
		int m_size;

	public:
		Array(int size)
		{
			m_size = size;
			m_buffer = (T*)malloc(size * sizeof(T));
		}

		Array(const Array<T>& other)
		{
			m_size = other.m_size;
			m_buffer = (T*)malloc(other.m_size * sizeof(T));
			if (m_buffer) memcpy(m_buffer, other.m_buffer, m_size * sizeof(T));
		}

		Array(Array<T>&& other) noexcept
		{
			m_size = other.m_size;
			m_buffer = other.m_buffer;

			other.m_size = 0;
			other.m_buffer = nullptr;
		}

		~Array()
		{
			free(m_buffer);
		}

		void print(const char* delim = ", ")
		{
			if (!m_buffer) return;
			for (int i = 0; i < m_size - 1; i++)
			{
				std::cout << *(m_buffer + i) << delim;
			}
			std::cout << *(m_buffer + m_size - 1) << std::endl;
		}

		T& operator[] (size_t index)
		{
			if (index >= m_size) __debugbreak();
			return *(m_buffer + index);
		}

		T* operator& () { return m_buffer; }

		// Sorts in place
		void sort()
		{
			for (int j = 1; j < m_size; j++)
			{
				T key = *(m_buffer + j);
				int i = j - 1;
				while (i >= 0 && *(m_buffer + i) > key)
				{
					*(m_buffer + i + 1) = *(m_buffer + i);
					i--;
				}
				*(m_buffer + i + 1) = key;
			}
		}

		Array reduce(bool(*f)(T))
		{
			int count = 0;
			for (int i = 0; i < m_size; i++)
				if (f(*(m_buffer + i))) count++;

			Array temp(count);
			int j = 0;
			for (int i = 0; i < m_size; i++)
				if (f(*(m_buffer + i)))
				{
					*(temp.m_buffer + j) = *(m_buffer + i);
					j++;
				}
			return temp;
		}

		size_t size()
		{
			return m_size;
		}
	};


	struct Vec3d
	{
		float x, y, z;

		Vec3d()
			: x(0), y(0), z(0) {}

		Vec3d(float _x, float _y, float _z)
			: x(_x), y(_y), z(_z) {}

		Vec3d(const Vec3d& other)
			: x(other.x), y(other.y), z(other.z) {}

		Vec3d(Vec3d&& other) noexcept
			: x((float&&)other.x), y((float&&)other.y), z((float&&)other.z) {}


		float Mag() const { return std::sqrt(x * x + y * y + z * z); }

		Vec3d operator + (const Vec3d& rhs) { return Vec3d(x + rhs.x, y + rhs.y, z + rhs.z); }
		Vec3d operator - (const Vec3d& rhs) { return Vec3d(x - rhs.x, y - rhs.y, z - rhs.z); }
		Vec3d operator * (float scale) { return Vec3d(x * scale, y * scale, z * scale); }
		Vec3d operator / (float scale) { return Vec3d(x / scale, y / scale, z / scale); }

		float Dot(const Vec3d& rhs) const
		{
			return x * rhs.x + y * rhs.y + z * rhs.z;
		}

		Vec3d Cross(const Vec3d& rhs) const
		{
			float cx = y * rhs.z - z * rhs.y; // AyBz - AzBy
			float cy = z * rhs.x - x * rhs.z; // AzBx - AxBz
			float cz = x * rhs.y - y * rhs.x; // AxBy - AyBx
			return Vec3d(cx, cy, cz);
		}



		Vec3d Unit() const
		{
			float mag = this->Mag();
			return Vec3d(x / mag, y / mag, z / mag);
		}

		// a dot b = |a||b|cos(angle) => angle = arccos( (a dot b) / (|a||b|) );
		float AngleBetween(const Vec3d& rhs) const
		{
			float dot = this->Dot(rhs);
			return std::acos(dot / (Mag() * rhs.Mag()));
		}

		Vec3d ProjOnto(const Vec3d& rhs) const
		{
			float dot = this->Dot(rhs);
			float rhsMag = rhs.Mag();
			Vec3d rhsUnit = rhs.Unit();
			return  rhsUnit * (dot / rhsMag);
		}

		// Proj(left) onto right
		float ProjOntoScalarComp(const Vec3d& rhs) const
		{
			float dot = this->Dot(rhs);
			return dot / rhs.Mag();
		}

		friend std::ostream& operator<<(std::ostream& os, const Vec3d& v);

	};

	std::ostream& operator<<(std::ostream& os, const Vec3d& v)
	{
		os << v.x << "i, " << v.y << "j, " << v.z << "k";
		return os;
	}

	bool Between(float n, float lower, float upper)
	{
		return (n >= lower && n <= upper);
	}

} // end util namespace

#endif // !_UTILS_H
